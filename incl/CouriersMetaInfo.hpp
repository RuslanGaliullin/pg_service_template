#pragma once

#include <userver/utils/datetime/date.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <iostream>
#include <userver/formats/parse/common_containers.hpp>
#include <vector>
#include <string>
#include <string_view>
#include <fmt/format.h>
#include <userver/clients/dns/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/http/http_method.hpp>
#include <userver/components/component_list.hpp>
#include <vector>
#include <chrono>
#include <iostream>
#include "Couriers.hpp"
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
namespace bds_service {

void AppendCouriersMetaInfo(userver::components::ComponentList& component_list);
}  // namespace bds_service

