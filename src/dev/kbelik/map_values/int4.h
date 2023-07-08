#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/byte_serializer_deserializer.h"

namespace linpipe::kbelik::map_values {

class Int4 {
 public:
  using Type = int32_t;
  static inline size_t length(const byte* ptr);
  static inline size_t length(const Type& val, ByteSerializerDeserializers* bsds=nullptr);
  static inline void deserialize(const byte* ptr, Type& value, ByteSerializerDeserializers* bsds=nullptr);
  static inline void serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds=nullptr);
};

size_t Int4::length(const byte* /*ptr*/) {
  return 4;
}

size_t Int4::length(const Int4::Type& /*value*/, ByteSerializerDeserializers* /*bsds*/) {
  return 4;
}

void Int4::deserialize(const byte* ptr, Int4::Type& value, ByteSerializerDeserializers* /*bsds*/) {
  memcpy(&value, ptr, Int4::length(ptr));
}

void Int4::serialize(const Int4::Type& value, vector<byte>& data, ByteSerializerDeserializers* /*bsds*/) {
  size_t bytes_cnt = Int4::length(value);
  data.resize(bytes_cnt);
  memcpy(data.data(), &value, bytes_cnt);
}

} // namespace linpipe::kbelik::map_values
