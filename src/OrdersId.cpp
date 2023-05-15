#include "../incl/OrdersId.hpp"

namespace bds_service {

namespace {
class OrdersId final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "handler-orders-id";

  OrdersId(const userver::components::ComponentConfig& config,
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
    // LOG_WARNING() << "id is " << std::stoi(request.GetPathArg("order_id"));

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT * from bds_schema.orders where order_id = $1",
        std::stoi(request.GetPathArg("order_id")));

    if (result.Size() == 1) {
      return userver::formats::json::ValueBuilder(
                 result[0].As<Order>(userver::storages::postgres::kRowTag))
          .ExtractValue();
    } else {
      request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
      return {};
    }
    return {};
  }
  userver::storages::postgres::ClusterPtr pg_cluster_;
};
}  // namespace
void AppendOrdersId(userver::components::ComponentList& component_list) {
  component_list.Append<OrdersId>();
}
}  // namespace bds_service
