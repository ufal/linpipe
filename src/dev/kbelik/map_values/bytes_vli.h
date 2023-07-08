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
  static size_t length(const byte* ptr);
  static size_t length(const Type& val, ByteSerializerDeserializers* bsds=nullptr);
  static void deserialize(const byte* ptr, Type& value, ByteSerializerDeserializers* bsds=nullptr);
  static void serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds=nullptr);
};

size_t BytesVLI::length(const byte* ptr) {
  size_t data_sz;
  VLI::deserialize(ptr, data_sz);
  return VLI::length(ptr) + data_sz;
}

size_t BytesVLI::length(const BytesVLI::Type& value, ByteSerializerDeserializers* /* bsds */) {
  return VLI::length(value.size()) + value.size();
}

void BytesVLI::deserialize(const byte* ptr, BytesVLI::Type& value, ByteSerializerDeserializers* /* bsds */) {
  size_t vli_sz = VLI::length(ptr);
  size_t data_sz;
  VLI::deserialize(ptr, data_sz);
  value.resize(data_sz);
  memcpy(value.data(), ptr + vli_sz, data_sz);
}

void BytesVLI::serialize(const BytesVLI::Type& value, vector<byte>& data, ByteSerializerDeserializers* /* bsds */) {
  size_t data_sz = value.size();
  size_t vli_sz = VLI::length(data_sz);
  VLI::serialize(data_sz, data);
  data.resize(vli_sz + data_sz);
  memcpy(data.data() + vli_sz, value.data(), data_sz);

}

} // namespace linpipe::kbelik::map_values
