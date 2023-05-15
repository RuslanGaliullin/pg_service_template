#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "../incl/Couriers.hpp"
#include "../incl/CouriersId.hpp"
#include "../incl/Orders.hpp"
#include "../incl/OrdersComplete.hpp"
#include "../incl/OrdersId.hpp"
#include "../incl/CouriersMetaInfo.hpp"

int main(int argc, char* argv[]) {
  auto component_list = userver::components::MinimalServerComponentList()
                            .Append<userver::server::handlers::Ping>()
                            .Append<userver::components::TestsuiteSupport>()
                            .Append<userver::components::HttpClient>()
                            .Append<userver::server::handlers::TestsControl>();

  bds_service::AppendCouriers(component_list);
  bds_service::AppendOrdersId(component_list);
  bds_service::AppendOrders(component_list);
  bds_service::AppendOrdersComplete(component_list);
  bds_service::AppendCouriersId(component_list);
  bds_service::AppendCouriersMetaInfo(component_list);

  component_list.Append<userver::components::Postgres>("postgres-db-1");
  component_list.Append<userver::clients::dns::Component>();
  return userver::utils::DaemonMain(argc, argv, component_list);
}
