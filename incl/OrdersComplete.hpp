#pragma once

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
