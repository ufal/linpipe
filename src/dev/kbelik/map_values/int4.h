#pragma once

#include <filesystem>
#include "common.h"

namespace linpipe::kbelik::map_values {

class Int4 {
 public:
  using Type = int32_t;
  static size_t length(const byte* ptr);
  static void deserialize(const byte* ptr, Type& value);
  static void serialize(const Type& value, vector<byte>& data);
};

} // namespace linpipe::kbelik::map_values
