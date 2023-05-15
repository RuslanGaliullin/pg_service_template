#include "../incl/CouriersId.hpp"

namespace bds_service {

namespace {
class CouriersId final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "handler-couriers-id";

  CouriersId(const userver::components::ComponentConfig& config,
             const userver::components::ComponentContext& component_context)
      : HttpHandlerJsonBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value&,
      userver::server::request::RequestContext&) const override {
    
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT * from bds_schema.couriers where courier_id = $1",
        std::stoi(request.GetPathArg("courier_id")));
    if (result.Size() > 0) {
      return userver::formats::json::ValueBuilder(
                 result[0].As<Courier>(userver::storages::postgres::kRowTag))
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
void AppendCouriersId(userver::components::ComponentList& component_list) {
  component_list.Append<CouriersId>();
}
}  // namespace bds_service

