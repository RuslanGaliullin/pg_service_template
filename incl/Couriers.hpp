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

namespace bds_service {

struct Courier {
  int courier_id;
  std::vector<int> regions;
  std::string courier_type;
  std::vector<std::string> working_hours;
};

Courier Parse(const userver::formats::json::Value& json,
              userver::formats::parse::To<Courier>);

userver::formats::json::Value Serialize(
    const Courier& data,
    userver::formats::serialize::To<userver::formats::json::Value>);

void AppendCouriers(userver::components::ComponentList& component_list);

}  // namespace bds_service

