#pragma once

#include "common.h"

#include "dev/kbelik/map_value_blueprint.h"

namespace linpipe::kbelik::map_values {

class MapValueBlueprint {
 public:
  using Type = buleprint;
  size_t length(const byte* ptr);
  size_t length(const Type& val);
  void deserialize(const byte* ptr, Type& value);
  void serialize(const Type& value, vector<byte>& data);
};

} // namespace linpipe::kbelik::map_values
