#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/id.h"

#include "dev/kbelik/map_values/vli.h"
#include "dev/kbelik/map_values/int8.h"

#include "dev/kbelik/byte_serializer_deserializer.h"

namespace linpipe::kbelik::map_values {

class ID {
 public:
  using Type = linpipe::kbelik::ID;
  size_t length(const byte* ptr) const;
  size_t length(const Type& val) const;
  void deserialize(const byte*& ptr, Type& value) const;
  void serialize(const Type& value, vector<byte>& data) const;
 private:
  VLI vli;
  Int8 int8;
};

} // namespace linpipe::kbelik::map_values
