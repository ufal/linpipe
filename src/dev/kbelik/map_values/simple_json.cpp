#include <cstring>
#include <filesystem>

#include "common.h"
#include "lib/json.h"

#include "dev/kbelik/map_values/simple_json.h"

#include "dev/kbelik/byte_serializer_deserializer.h"

namespace linpipe::kbelik::map_values {

size_t SimpleJson::length(const byte* ptr) const {
  size_t json_length;
  vli.deserialize(ptr, json_length);
  return json_length;
}

size_t SimpleJson::length(const SimpleJson::Type& value) const {
  vector<uint8_t> v_bson = Json::to_bson(value);
  size_t expected = vli.length(v_bson.size());
  size_t with_length_prefix = vli.length(expected + v_bson.size());
  return expected + v_bson.size() + (with_length_prefix > expected ? 1 : 0);
}

void SimpleJson::deserialize(const byte*& ptr, SimpleJson::Type& value) const {
  uint64_t total_length;
  size_t vli_length;
  vli_length = vli.length(ptr);  
  vli.deserialize(ptr, total_length);
  vector<uint8_t> v_bson;
  v_bson.resize(total_length - vli_length);
  memcpy(v_bson.data(), ptr, v_bson.size());
  ptr += v_bson.size();
  value = Json::from_bson(v_bson);
}

void SimpleJson::serialize(const SimpleJson::Type& value, vector<byte>& data) const {
  size_t total_length = length(value);
  size_t vli_length;
  vli_length = vli.length(total_length);  
  size_t old_size = data.size();
  vli.serialize(total_length, data);
  data.resize(old_size + total_length);
  vector<uint8_t> v_bson = Json::to_bson(value);
  memcpy(data.data() + vli_length + old_size, (byte*)v_bson.data(), total_length - vli_length);
}

void SimpleJson::serialize(const map<string, string>& value, vector<byte>& data) const {
  Json j = value;
  serialize(j, data);
}

} // namespace linpipe::kbelik::map_values
