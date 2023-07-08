#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"
#include "lib/json.h"

#include "dev/kbelik/map_values/vli.h"

#include "dev/kbelik/byte_serializer_deserializer.h"

namespace linpipe::kbelik::map_values {

class SimpleJson {
 public:
  using Type = Json;
  static inline size_t length(const byte* ptr);
  static inline size_t length(const Type& val, ByteSerializerDeserializers* bsds=nullptr);
  static inline void deserialize(const byte* ptr, Type& value, ByteSerializerDeserializers* bsds=nullptr);
  static inline void serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds=nullptr);
  // Can serialize maps:
  // The deserialization is not implemented because not all Jsons can be
  // deserialized into nlohman jsons.
  static inline void serialize(const map<string, string>& value, vector<byte>& data,
   ByteSerializerDeserializers* bsds=nullptr);
};

size_t SimpleJson::length(const byte* ptr) {
  size_t json_length;
  VLI::deserialize(ptr, json_length);
  return json_length;
}

size_t SimpleJson::length(const SimpleJson::Type& value, ByteSerializerDeserializers* /*bsds*/) {
  vector<uint8_t> v_bson = Json::to_bson(value);
  size_t expected = VLI::length(v_bson.size());
  size_t with_length_prefix = VLI::length(expected + v_bson.size());
  return expected + v_bson.size() + (with_length_prefix > expected ? 1 : 0);
}

void SimpleJson::deserialize(const byte* ptr, SimpleJson::Type& value, ByteSerializerDeserializers* /*bsds*/) {
  size_t total_length;
  size_t vli_length;
  VLI::deserialize(ptr, total_length);
  vli_length = VLI::length(ptr);  
  vector<uint8_t> v_bson;
  v_bson.resize(total_length - vli_length);
  memcpy(v_bson.data(), ptr + vli_length, v_bson.size());
  value = Json::from_bson(v_bson);
}

void SimpleJson::serialize(const SimpleJson::Type& value, vector<byte>& data, ByteSerializerDeserializers* /*bsds*/) {
  size_t total_length = length(value);
  size_t vli_length;
  vli_length = VLI::length(total_length);  
  VLI::serialize(total_length, data);
  data.resize(total_length);
  vector<uint8_t> v_bson = Json::to_bson(value);
  memcpy(data.data() + vli_length, (byte*)v_bson.data(), total_length - vli_length);
}

void SimpleJson::serialize(const map<string, string>& value, vector<byte>& data, ByteSerializerDeserializers* /*bsds*/) {
  Json j = value;
  serialize(j, data);
}


} // namespace linpipe::kbelik::map_values
