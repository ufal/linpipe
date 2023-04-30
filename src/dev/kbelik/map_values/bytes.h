#pragma once

#include <filesystem>

#include "common.h"

namespace linpipe::kbelik::map_values {

template<typename SizeType>
class Bytes {
 public:
  using Type = vector<byte>;
  static size_t length(const byte* ptr);
  static size_t length(const Type& val);
  static void deserialize(const byte* ptr, Type& value);
  static void serialize(const Type& value, vector<byte>& data);
};

} // namespace linpipe::kbelik::map_values
