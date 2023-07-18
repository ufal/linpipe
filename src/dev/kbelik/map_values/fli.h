// Fixed length integer
#pragma once

#include "common.h"

namespace linpipe::kbelik::map_values {

class FLI {
 public:
  using Type = uint64_t;

  FLI() = delete;
  FLI(size_t length) : _length(length) {}

  size_t length(const byte* ptr) const;
  size_t length(const Type& value) const;

  void deserialize(const byte* ptr, Type& value);
  void serialize(const Type& value, vector<byte>& data);
 private:
  size_t _length;
};

} // namespace linpipe::kbelik::map_keys
