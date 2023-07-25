#pragma once

#include "common.h"

namespace linpipe::kbelik::map_values {

class Int4 {
 public:
  using Type = int32_t;
  size_t length(const byte* ptr) const;
  size_t length(const Type& val) const;
  void deserialize(const byte*& ptr, Type& value) const;
  void serialize(const Type& value, vector<byte>& data) const;
};

} // namespace linpipe::kbelik::map_values
