#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

namespace linpipe::kbelik::map_values {

class Int4 {
 public:
  using Type = int32_t;
  static inline size_t length(const byte* ptr);
  static inline size_t length(const Type& val);
  static inline void deserialize(const byte* ptr, Type& value);
  static inline void serialize(const Type& value, vector<byte>& data);
};

size_t Int4::length(const byte* /*ptr*/) {
  return 4;
}

size_t Int4::length(const Int4::Type& /*value*/) {
  return 4;
}

void Int4::deserialize(const byte* ptr, Int4::Type& value) {
  memcpy(&value, ptr, Int4::length(ptr));
}

void Int4::serialize(const Int4::Type& value, vector<byte>& data) {
  size_t bytes_cnt = Int4::length(value);
  data.resize(bytes_cnt);
  memcpy(data.data(), &value, bytes_cnt);
}

} // namespace linpipe::kbelik::map_values
