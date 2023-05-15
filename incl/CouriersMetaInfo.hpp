#pragma once

#include <fmt/format.h>
#include <boost/algorithm/string/join.hpp>
#include <string>
#include <string_view>
#include <userver/clients/dns/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/formats/json.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/parse/common.hpp>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/parse/time_of_day.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/serialize/time_of_day.hpp>
#include <userver/logging/level.hpp>
#include <userver/logging/log.hpp>
#include <userver/logging/log_filepath.hpp>
#include <userver/logging/log_helper.hpp>
#include <userver/logging/logger.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/exceptions.hpp>
#include <userver/storages/postgres/io/pg_types.hpp>
#include <userver/storages/postgres/result_set.hpp>
#include <userver/utils/assert.hpp>
#include <userver/utils/time_of_day.hpp>
#include <userver/utils/datetime/date.hpp>
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
