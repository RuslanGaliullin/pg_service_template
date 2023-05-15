#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "Couriers.hpp"
#include "CouriersId.hpp"
#include "Orders.hpp"
#include "OrdersAssign.hpp"
#include "OrdersComplete.hpp"
#include "OrdersId.hpp"

int main(int argc, char* argv[]) {
  auto component_list = userver::components::MinimalServerComponentList()
                            .Append<userver::server::handlers::Ping>()
                            .Append<userver::components::TestsuiteSupport>()
                            .Append<userver::components::HttpClient>()
                            .Append<userver::server::handlers::TestsControl>();

  pg_service_template::AppendCouriers(component_list);
  pg_service_template::AppendOrdersId(component_list);
  pg_service_template::AppendOrders(component_list);
  pg_service_template::AppendOrdersAssign(component_list);
  pg_service_template::AppendOrdersComplete(component_list);
  pg_service_template::AppendCouriersId(component_list);

  component_list.Append<userver::components::Postgres>("postgres-db-1");
  component_list.Append<userver::clients::dns::Component>();
  return userver::utils::DaemonMain(argc, argv, component_list);
}
