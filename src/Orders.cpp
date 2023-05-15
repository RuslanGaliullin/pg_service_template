#include "../incl/Orders.hpp"

namespace bds_service {

namespace {

class Orders final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "handler-orders";

  Orders(const userver::components::ComponentConfig& config,
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

 private:
  userver::formats::json::Value GetValue(
      const userver::formats::json::Value& json,
      const userver::server::http::HttpRequest& request) const;

  userver::formats::json::Value PostValue(
      const userver::formats::json::Value& json,
      const userver::server::http::HttpRequest& request) const;

  userver::storages::postgres::ClusterPtr pg_cluster_;
};
userver::formats::json::Value Orders::GetValue(
    const userver::formats::json::Value& json,
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
      "SELECT * from bds_schema.orders LIMIT $1 OFFSET $2", limit, offset);
  std::vector<Order> all_orders;

  if (result.Size() == 0) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return {};
  }

  for (size_t i = 0; i < result.Size(); i++) {
    Order current;
    auto row = result[i];
    current = row.As<Order>(userver::storages::postgres::kRowTag);
    all_orders.push_back(current);
  }
  return userver::formats::json::ValueBuilder(all_orders).ExtractValue();
}
userver::formats::json::Value Orders::PostValue(
    const userver::formats::json::Value& json,
    const userver::server::http::HttpRequest& request) const {
  auto all_orders = json["orders"].As<std::vector<Order>>();

  userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
      "sample_transaction_insert_key_value",
      userver::storages::postgres::ClusterHostType::kMaster, {});

  for (auto& order : all_orders) {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO bds_schema.orders(order_id, weight, regions, cost, "
        "delivery_hours, completed_time) VALUES(DEFAULT, $1, $2, $3, $4, "
        "NULL) "
        "ON CONFLICT (order_id)"
        "DO NOTHING "
        "RETURNING orders.order_id",
        order.weight, order.regions, order.cost, order.delivery_hours);
    if (!result.RowsAffected()) {
      transaction.Rollback();
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      return {};
    }
    order.order_id = result.AsSingleRow<int>();
  }

  transaction.Commit();
  request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
  // Если нужен возврат всех
  // auto to_read = pg_cluster_->Execute(
  //     userver::storages::postgres::ClusterHostType::kMaster,
  //     "SELECT * from bds_schema.orders");
  // auto output =
  // to_read.AsContainer<std::vector<Order>>(userver::storages::postgres::kRowTag);

  return userver::formats::json::ValueBuilder(all_orders).ExtractValue();
}
}  // namespace
Order Parse(const userver::formats::json::Value& json,
            userver::formats::parse::To<Order>) {
  std::vector<std::string> times;
  for (auto& i : json["delivery_hours"].As<std::vector<std::string>>({})) {
    times.push_back(i.substr(0, 5));
    times.push_back(i.substr(6));
  }
  // LOG_INFO() << "i get time interav " << boost::algorithm::join(times, " ");

  auto value =
      Order{json["order_id"].As<int>(1), json["regions"].As<int>(1),
            json["weight"].As<int>(1),
            json["cost"].As<int>(1),  // return `1` if "field2" is missing
            times};
  if (json.HasMember("completed_time")) {
    value.completed_time =
        json["completed_time"]
            .As<std::chrono::time_point<std::chrono::system_clock>>();
  }

  return value;
}

userver::formats::json::Value Serialize(
    const Order& data,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["order_id"] = data.order_id;
  builder["regions"] = data.regions;
  builder["weight"] = data.weight;
  builder["cost"] = data.cost;
  std::vector<std::string> times;
  for (size_t i = 0; i < data.delivery_hours.size(); i += 2) {
    times.push_back(data.delivery_hours[i] + "-" + data.delivery_hours[i + 1]);
  }
  builder["delivery_hours"] = std::move(times);
  if (data.completed_time.has_value()) {
    builder["completed_time"] = data.completed_time;
  }
  return builder.ExtractValue();
}

void AppendOrders(userver::components::ComponentList& component_list) {
  component_list.Append<Orders>();
}
}  // namespace bds_service