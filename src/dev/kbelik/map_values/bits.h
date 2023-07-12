#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/map_values/bytes_vli.h"

#include "dev/kbelik/byte_serializer_deserializer.h"

namespace linpipe::kbelik::map_values {

class Bits {
 public:
  using Type = vector<bool>;
  size_t length(const byte* ptr);
  size_t length(const Type& val, ByteSerializerDeserializers* bsds=nullptr);
  void deserialize(const byte* ptr, Type& value, ByteSerializerDeserializers* bsds=nullptr);
  void serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds=nullptr);
 private:
  BytesVLI bytes_vli;
};

} // namespace linpipe::kbelik::map_values
