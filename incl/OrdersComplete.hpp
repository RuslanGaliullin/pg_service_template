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
#include <vector>
#include "Orders.hpp"
namespace bds_service {
struct Complete {
  int courier_id;
  int order_id;
  std::chrono::time_point<std::chrono::system_clock> complete_time;
};
Complete Parse(const userver::formats::json::Value& json,
               userver::formats::parse::To<Complete>);

userver::formats::json::Value Serialize(
    const Complete& data,
    userver::formats::serialize::To<userver::formats::json::Value>);
void AppendOrdersComplete(userver::components::ComponentList& component_list);
}  // namespace bds_service
