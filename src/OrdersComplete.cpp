#include "../incl/OrdersComplete.hpp"
namespace bds_service {

namespace {
class OrdersComplete final
    : public userver::server::handlers::HttpHandlerJsonBase {
public:
  static constexpr std::string_view kName = "handler-orders-complete";

  OrdersComplete(const userver::components::ComponentConfig &config,
                 const userver::components::ComponentContext &component_context)
      : HttpHandlerJsonBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest &request,
      const userver::formats::json::Value &json,
      userver::server::request::RequestContext &) const override {
    auto completions = json["complete_info"].As<std::vector<Complete>>();
    userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
        "post_completion",
        userver::storages::postgres::ClusterHostType::kMaster, {});

    std::vector<Order> result_list;
    for (auto &i : completions) {
      auto result_courier = pg_cluster_->Execute(
          userver::storages::postgres::ClusterHostType::kMaster,
          "SELECT * from bds_schema.couriers where courier_id = $1",
          i.courier_id);

      auto result_order = pg_cluster_->Execute(
          userver::storages::postgres::ClusterHostType::kMaster,
          "SELECT * from bds_schema.orders where order_id = $1", i.order_id);

      if (result_courier.Size() == 0 || result_order.Size() == 0) {
        transaction.Rollback();
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return {};
      }

      if (result_order.AsSingleRow<Order>(userver::storages::postgres::kRowTag)
              .completed_time.has_value()) {
        transaction.Rollback();
        request.SetResponseStatus(
            userver::server::http::HttpStatus::kBadRequest);
        return {};
      }

      auto result = pg_cluster_->Execute(
          userver::storages::postgres::ClusterHostType::kMaster,
          "UPDATE bds_schema.orders "
          "SET completed_time = $1 "
          "WHERE order_id = $2 "
          "RETURNING *",
          i.complete_time, i.order_id);

      if (!result.RowsAffected()) {
        transaction.Rollback();
        request.SetResponseStatus(
            userver::server::http::HttpStatus::kBadRequest);
        return {};
      }
      result_list.push_back(
          result.AsSingleRow<Order>(userver::storages::postgres::kRowTag));
      pg_cluster_->Execute(
          userver::storages::postgres::ClusterHostType::kMaster,
          "INSERT INTO bds_schema.completed_orders(courier_id, order_id, "
          "complete_date) "
          "VALUES ($1, $2, $3) ",
          i.courier_id, i.order_id, i.complete_time);
    }
    transaction.Commit();
    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ValueBuilder(result_list).ExtractValue();
  }
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

} // namespace
Complete Parse(const userver::formats::json::Value &json,
               userver::formats::parse::To<Complete>) {
  return Complete{
      json["courier_id"].As<int>(1),
      json["order_id"].As<int>(1),
      json["complete_time"]
          .As<std::chrono::time_point<std::chrono::system_clock>>(),
  };
}

userver::formats::json::Value
Serialize(const Complete &data,
          userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["courier_id"] = data.courier_id;
  builder["complete_time"] = data.complete_time;
  builder["order_id"] = data.order_id;
  return builder.ExtractValue();
}
void AppendOrdersComplete(userver::components::ComponentList &component_list) {
  component_list.Append<OrdersComplete>();
}
} // namespace bds_service
