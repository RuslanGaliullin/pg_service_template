#include "../incl/Couriers.hpp"
namespace bds_service {

namespace {
class Couriers final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "handler-couriers";

  Couriers(const userver::components::ComponentConfig& config,
           const userver::components::ComponentContext& component_context)
      : HttpHandlerJsonBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& json,
      userver::server::request::RequestContext&) const override {
    userver::formats::json::ValueBuilder response;
    switch (request.GetMethod()) {
      case userver::server::http::HttpMethod::kGet:
        return GetValue(json, request);
      case userver::server::http::HttpMethod::kPost:
        return PostValue(json, request);
      default:
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{
                fmt::format("Unsupported method {}", request.GetMethod())});
    }
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;

 private:
  userver::formats::json::Value GetValue(
      const userver::formats::json::Value&,
      const userver::server::http::HttpRequest& request) const;

  userver::formats::json::Value PostValue(
      const userver::formats::json::Value& json,
      const userver::server::http::HttpRequest& request) const;
};
userver::formats::json::Value Couriers::GetValue(
    const userver::formats::json::Value&,
    const userver::server::http::HttpRequest& request) const {
  int limit = 1;
  int offset = 0;
  if (request.HasArg("limit")) {
    limit = std::stoi(request.GetArg("limit"));
  }
  if (request.HasArg("offset")) {
    offset = std::stoi(request.GetArg("offset"));
  }
  auto result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "SELECT * from bds_schema.couriers LIMIT $1 OFFSET $2", limit, offset);
  std::vector<Courier> all_curiers;
  LOG_INFO() << "there are " << all_curiers.size() << " rows in shit\n";
  for (size_t i = 0; i < result.Size(); i++) {
    Courier current;
    auto row = result[i];
    current = row.As<Courier>(userver::storages::postgres::kRowTag);
    all_curiers.push_back(current);
  }

  return userver::formats::json::ValueBuilder(all_curiers).ExtractValue();
}

userver::formats::json::Value Couriers::PostValue(
    const userver::formats::json::Value& json,
    const userver::server::http::HttpRequest& request) const {
  auto all_curiers = json["couriers"].As<std::vector<Courier>>();
  LOG_INFO() << "in table " << all_curiers.size();
  userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
      "sample_transaction_insert",
      userver::storages::postgres::ClusterHostType::kMaster, {});

  for (auto& curier : all_curiers) {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO bds_schema.couriers(courier_id, regions, "
        "courier_type, working_hours) VALUES(DEFAULT, $1, $2, $3) "
        "ON CONFLICT (courier_id) "
        "DO NOTHING "
        "RETURNING couriers.courier_id",
        curier.regions, curier.courier_type, curier.working_hours);
    if (!result.RowsAffected()) {
      transaction.Rollback();
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{fmt::format("Bad request")});
      return {};
    } else {
      LOG_INFO() << "inserted smth in db";
    }
    curier.courier_id = result.AsSingleRow<int>();
  }

  transaction.Commit();
  request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);

  // Если нужен возврат всех
  // auto to_read = pg_cluster_->Execute(
  //     userver::storages::postgres::ClusterHostType::kMaster,
  //     "SELECT * from bds_schema.couriers");
  // auto output =
  // to_read.AsContainer<std::vector<Courier>>(userver::storages::postgres::kRowTag);

  return userver::formats::json::ValueBuilder(all_curiers).ExtractValue();
}
}  // namespace
Courier Parse(const userver::formats::json::Value& json,
              userver::formats::parse::To<Courier>) {
  std::vector<std::string> times;
  for (auto& i : json["working_hours"].As<std::vector<std::string>>({})) {
    times.push_back(i.substr(0, 5));
    times.push_back(i.substr(6));
  }

  auto value = Courier{json["courier_id"].As<int>(1),
                       json["regions"].As<std::vector<int>>(1),
                       json["courier_type"].As<std::string>(""), times};

  return value;
}

userver::formats::json::Value Serialize(
    const Courier& data,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["courier_id"] = data.courier_id;
  builder["regions"] = data.regions;
  builder["courier_type"] = data.courier_type;
  std::vector<std::string> times;
  for (size_t i = 0; i < data.working_hours.size(); i += 2) {
    times.push_back(data.working_hours[i] + "-" + data.working_hours[i + 1]);
  }
  builder["working_hours"] = std::move(times);
  // if(data.completed_time.has_value()){
  //   builder["completed_time"] = data.completed_time;
  // }
  return builder.ExtractValue();
}
void AppendCouriers(userver::components::ComponentList& component_list) {
  component_list.Append<Couriers>();
}
}  // namespace bds_service