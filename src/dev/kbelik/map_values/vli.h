#pragma once

#include "common.h"

#include "dev/kbelik/byte_serializer_deserializer.h"

namespace linpipe::kbelik::map_values {

class VLI {
 public:
  using Type = uint64_t;
  size_t length(const byte* ptr) const;
  size_t length(const Type& val) const;
  void deserialize(const byte* ptr, Type& value) const;
  void serialize(const Type& value, vector<byte>& data) const;
 private:
  byte get_byte(const byte* ptr, size_t offset) const;
};

} // namespace linpipe::kbelik::map_values
