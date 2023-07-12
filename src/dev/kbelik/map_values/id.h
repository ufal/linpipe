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
  size_t length(const byte* ptr);
  size_t length(const Type& val, ByteSerializerDeserializers* bsds=nullptr);
  void deserialize(const byte* ptr, Type& value, ByteSerializerDeserializers* bsds=nullptr);
  void serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds=nullptr);
 private:
  VLI vli;
  Int8 int8;
};

} // namespace linpipe::kbelik::map_values
