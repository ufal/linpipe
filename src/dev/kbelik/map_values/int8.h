#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

namespace linpipe::kbelik::map_values {

class Int8 {
 public:
  using Type = int64_t;
  static inline size_t length(const byte* ptr);
  static inline size_t length(const Type& val);
  static inline void deserialize(const byte* ptr, Type& value);
  static inline void serialize(const Type& value, vector<byte>& data);
};

size_t Int8::length(const byte* /*ptr*/) {
  return 8;
}

size_t Int8::length(const Int8::Type& /*value*/) {
  return 8;
}

void Int8::deserialize(const byte* ptr, Int8::Type& value) {
  memcpy(&value, ptr, Int8::length(ptr));
}

void Int8::serialize(const Int8::Type& value, vector<byte>& data) {
  size_t bytes_cnt = Int8::length(value);
  data.resize(bytes_cnt);
  memcpy(data.data(), &value, bytes_cnt);
}

} // namespace linpipe::kbelik::map_values
