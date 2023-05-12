#include "hello.hpp"

#include <fmt/format.h>

#include <userver/server/handlers/http_handler_base.hpp>

namespace service_template {

namespace {

class Hello final : public userver::server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "handler-hello";

  using HttpHandlerBase::HttpHandlerBase;

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest &request,
      userver::server::request::RequestContext &) const override {
    return service_template::SayHelloTo(request.GetMethod());
  }
};
class Orders final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-orders";

  // Orders(const userver::components::ComponentConfig& config,
  //       const userver::components::ComponentContext& component_context)
  //     : HttpHandlerBase(config, component_context),
  //       pg_cluster_(
  //           component_context
  //               .FindComponent<userver::components::Postgres>("postgres-db-1")
  //               .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    const auto& name = request.GetArg("name");
    auto user_type = UserType::kFirstTime;
    return service_template::SayHelloTo("big est.GetMethod()");
  }

};
} // namespace

std::string SayHelloTo(std::string_view name) {
  if (name.empty()) {
    name = "unknown user";
  }

  return fmt::format("Hello, {}!\n", name);
}

void AppendHello(userver::components::ComponentList &component_list) {
  component_list.Append<Hello>();
}

} // namespace service_template
