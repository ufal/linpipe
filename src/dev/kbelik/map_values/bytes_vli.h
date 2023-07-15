#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/map_values/vli.h"

#include "dev/kbelik/byte_serializer_deserializer.h"

namespace linpipe::kbelik::map_values {

class BytesVLI {
 public:
  using Type = vector<byte>;
  size_t length(const byte* ptr) const;
  size_t length(const Type& val) const;
  void deserialize(const byte* ptr, Type& value) const;
  void serialize(const Type& value, vector<byte>& data) const;
 private:
  VLI vli;
};

} // namespace linpipe::kbelik::map_values
