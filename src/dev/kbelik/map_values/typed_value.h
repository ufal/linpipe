#pragma once

#include "common.h"

#include "dev/kbelik/byte_serializer_deserializer.h"

#include "dev/kbelik/map_values/bytes_vli.h"

#include "dev/kbelik/typed_value.h"

namespace linpipe::kbelik::map_values {

class TypedValue {
 public:
  using Type = linpipe::kbelik::TypedValue;

  size_t length(const byte* ptr);
  size_t length(const Type& value, ByteSerializerDeserializers* bsds);
   
  void deserialize(const byte* ptr, Type& value, ByteSerializerDeserializers* bsds);
  void serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds);
 private:
  BytesVLI bytes_vli;

  void encode(const Type& value, ByteSerializerDeserializers* bsds, vector<byte>& encoded_st, vector<byte>& encoded_val);
  void decode(vector<byte>& encoded_st, vector<byte>& encoded_val, ByteSerializerDeserializers* bsds, Type& value);
};

} // namespace linpipe::kbelik::map_values
