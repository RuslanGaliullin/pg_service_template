#include "../incl/CouriersMetaInfo.hpp"

namespace bds_service {

namespace {
class CouriersMetaInfo final
    : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "handler-couriers-meta-info";

  CouriersMetaInfo(
      const userver::components::ComponentConfig& config,
      const userver::components::ComponentContext& component_context)
      : HttpHandlerJsonBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& C,
      userver::server::request::RequestContext&) const override {
    if (!request.HasArg("startDate")) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      return {};
    }
    if (!request.HasArg("endDate")) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      return {};
    }

    auto start_date = userver::utils::datetime::Date(
        userver::utils::datetime::DateFromRFC3339String(
            request.GetArg("startDate")));
    auto end_date = userver::utils::datetime::Date(
        userver::utils::datetime::DateFromRFC3339String(
            request.GetArg("endDate")));

    auto start_date_point = start_date.GetSysDays();
    auto end_date_point = end_date.GetSysDays();

    // const std::time_t t_c =
    // std::chrono::system_clock::to_time_t(start_date.GetSysDays()); LOG_INFO()
    // << "parsed start and end dates like " <<
    // userver::utils::datetime::ToString(start_date) << " "<<
    // userver::utils::datetime::ToString(end_date); LOG_INFO() << "parsed start
    // and end dates in chrono style like " <<
    // std::put_time(std::localtime(&t_c), "%F %T.\n");

    auto courier_id = std::stoi(request.GetPathArg("courier_id"));
    auto result_earnings = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT coalesce(SUM(ord.cost), 0) AS cost, COUNT(*) AS cnt FROM "
        "bds_schema.completed_orders AS hist "
        "JOIN bds_schema.orders AS ord ON (hist.order_id=ord.order_id) "
        "WHERE hist.courier_id = $1 AND hist.complete_date >= $2 AND "
        "hist.complete_date < $3 ",
        courier_id, start_date_point, end_date_point);

    auto result_type = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT * FROM bds_schema.couriers "
        "WHERE couriers.courier_id = $1 ",
        courier_id);

    if (result_type.Size() == 0) {
      return {};
    }

    int earnings = 0;
    int total_cost = result_earnings.Front()["cost"].As<int>();
    int total_count = result_earnings.Front()["cnt"].As<int>();
    auto found_courier =
        result_type.AsSingleRow<Courier>(userver::storages::postgres::kRowTag);

    double rating = 0;
    auto hours = std::chrono::duration_cast<std::chrono::hours>(
        end_date_point - start_date_point);

    if (hours.count() <= 0) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      // throw userver::server::handlers::ClientError(
      //       userver::server::handlers::ExternalBody{
      //           fmt::format("Incorrect start adn end dates {}, {}",
      //           userver::utils::datetime::ToString(start_date),
      //           userver::utils::datetime::ToString(end_date))});
      return {};
    }

    if (found_courier.courier_type == "FOOT") {
      earnings = total_cost * 2;
      rating = 3.0 * total_count / hours.count();
    } else if (found_courier.courier_type == "BIKE") {
      earnings = total_cost * 3;
      rating = 2.0 * total_count / hours.count();
    } else if (found_courier.courier_type == "AUTO") {
      earnings = total_cost * 4;
      rating = 1.0 * total_count / hours.count();
    }

    userver::formats::json::ValueBuilder builder(found_courier);
    if (rating > 0) {
      builder["rating"] = rating;
    }
    if (earnings > 0) {
      builder["earnings"] = earnings;
    }
    return builder.ExtractValue();
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};
}  // namespace
void AppendCouriersMetaInfo(
    userver::components::ComponentList& component_list) {
  component_list.Append<CouriersMetaInfo>();
}
}  // namespace bds_service

