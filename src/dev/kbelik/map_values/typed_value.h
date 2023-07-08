#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/byte_serializer_deserializer.h"
#include "dev/kbelik/map_values/bytes_vli.h"

#include "dev/kbelik/typed_value.h"

namespace linpipe::kbelik::map_values {

class TypedValue {
 public:
  using Type = linpipe::kbelik::TypedValue;

  static inline size_t length(const byte* ptr);
  static inline size_t length(const Type& value, ByteSerializerDeserializers* bsds);
   
  static inline void deserialize(const byte* ptr, Type& value, ByteSerializerDeserializers* bsds);
  static inline void serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds);
 private:
  static inline void encode(const Type& value, ByteSerializerDeserializers* bsds, vector<byte>& encoded_st, vector<byte>& encoded_val);
  static inline void decode(vector<byte>& encoded_st, vector<byte>& encoded_val, ByteSerializerDeserializers* bsds, Type& value);
};

size_t TypedValue::length(const byte* ptr) {
  return BytesVLI::length(ptr);
}

size_t TypedValue::length(const Type& value, ByteSerializerDeserializers* bsds) {
  vector<byte> encoded_st, encoded_val;
  encode(value, bsds, encoded_st, encoded_val);

  return BytesVLI::length(encoded_st) + BytesVLI::length(encoded_val);
}

void TypedValue::deserialize(const byte* ptr, Type& value, ByteSerializerDeserializers* bsds) {
  vector<byte> encoded_st, encoded_val, encoded;

  BytesVLI::deserialize(ptr, encoded);

  size_t st_encoded_length = BytesVLI::length(encoded.data());
  encoded_val = vector<byte>(encoded.begin() + st_encoded_length, encoded.end());

  BytesVLI::deserialize(encoded.data(), encoded_st);

  decode(encoded_st, encoded_val, bsds, value);
}

void TypedValue::serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds) {
  vector<byte> encoded_st, encoded_val;
  encode(value, bsds, encoded_st, encoded_val);

  vector<byte> encoded;
  // Encodes subtype so that encoded is: subtype bytes count | subtype bytes
  BytesVLI::serialize(encoded_st, encoded);

  encoded.insert(encoded.end(), encoded_val.begin(), encoded_val.end());

  // The encoded result will look like: all bytes count | subtype bytes count | subtype bytes | val bytes
  // where the number of val bytes is {all bytes} - {subtype bytes}
  BytesVLI::serialize(encoded, data);
}

void TypedValue::encode(const Type& value, ByteSerializerDeserializers* bsds, vector<byte>& encoded_st, vector<byte>& encoded_val){
  auto [st, val] = value.get_as_string();

  bsds->huffman.encode(st, encoded_st);
  bsds->huffman.encode(val, encoded_val);
}

void TypedValue::decode(vector<byte>& encoded_st, vector<byte>& encoded_val, ByteSerializerDeserializers* bsds, Type& value){
  string st, val;

  bsds->huffman.decode(encoded_st.data(), st);
  bsds->huffman.decode(encoded_val.data(), val);

  value = Type(st, val);
}

} // namespace linpipe::kbelik::map_values
