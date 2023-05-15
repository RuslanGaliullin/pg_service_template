
#include "../incl/OrdersAssign.hpp"
namespace bds_service {

namespace {
class OrdersAssign final
    : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "handler-orders-assign";

  OrdersAssign(const userver::components::ComponentConfig& config,
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
    return {};
  }
  userver::storages::postgres::ClusterPtr pg_cluster_;
};
}  // namespace
void AppendOrdersAssign(userver::components::ComponentList& component_list) {
  component_list.Append<OrdersAssign>();
}
}  // namespace bds_service