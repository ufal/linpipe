// AgnosticEntityInfoH allows for reading and writing claims into memory.
// Expects claims to be string -> string map.
#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/byte_serializer_deserializer.h"
#include "dev/kbelik/map_values/bits.h"
#include "dev/kbelik/map_values/bytes_vli.h"
#include "dev/kbelik/map_values/typed_value.h"
#include "dev/kbelik/map_values/vli.h"

#include "dev/kbelik/agnostic_entity_info.h"
#include "dev/kbelik/named_entity.h"
#include "dev/kbelik/typed_value.h"
#include "dev/kbelik/utils.h"

namespace linpipe::kbelik::map_values {

class AgnosticEntityInfoH {
 public:
  using Type = linpipe::kbelik::AgnosticEntityInfo;

  static inline size_t length(const byte* ptr);
  static inline size_t length(const Type& value, ByteSerializerDeserializers* bsds);
   
  static inline void serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds);
  static inline void deserialize(const byte* ptr_whole, Type& value, ByteSerializerDeserializers* bsds);
 private:
  static inline void encodeAEIP(const AEIProperties& aeip, vector<byte>& encoded, ByteSerializerDeserializers* bsds);
  static inline void decodeAEIP(const byte* ptr_whole, AEIProperties& aeip, ByteSerializerDeserializers* bsds);

  static inline void encodeNE(const vector<NamedEntity>& value, vector<byte>& encoded);
  static inline void decodeNE(const byte* ptr, vector<NamedEntity>& ne);
};

size_t AgnosticEntityInfoH::length(const byte* ptr) {
  return BytesVLI::length(ptr);
}

size_t AgnosticEntityInfoH::length(const Type& value, ByteSerializerDeserializers* bsds) {
  vector<byte> serialized;
  serialize(value, serialized, bsds);
  return BytesVLI::length(serialized.data());
}

void AgnosticEntityInfoH::serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds) {
  vector<byte> result;

  size_t claims_cnt = value.claims.size();
  vector<byte> cnt_encoded;
  VLI::serialize(claims_cnt, result);


  for (auto &[key, aeip]: value.claims) {
    vector<byte> key_encoded;
    vector<byte> key_huffed;
    bsds->huffman.encode(key, key_huffed);
    BytesVLI::serialize(key_huffed, key_encoded);

    vector<byte> aeip_encoded;
    encodeAEIP(aeip, aeip_encoded, bsds);

    result.insert(result.end(), key_encoded.begin(), key_encoded.end());
    result.insert(result.end(), aeip_encoded.begin(), aeip_encoded.end());
  }

  vector<byte> ne_encoded;
  encodeNE(value.named_entities, ne_encoded);
  result.insert(result.end(), ne_encoded.begin(), ne_encoded.end());

  vector<byte> fictional_encoded;
  int fictional = static_cast<int>(value.fictional);
  VLI::serialize(fictional, fictional_encoded);
   
  result.insert(result.end(), fictional_encoded.begin(), fictional_encoded.end());

  BytesVLI::serialize(result, data);
}

void AgnosticEntityInfoH::deserialize(const byte* ptr_whole, Type& value, ByteSerializerDeserializers* bsds) {
  vector<byte> result;
  BytesVLI::deserialize(ptr_whole, result);

  byte* ptr = result.data();

  size_t claims_cnt;
  VLI::deserialize(ptr, claims_cnt);
  ptr += VLI::length(ptr);

  unordered_map<string, AEIProperties> claims;

  for (size_t i = 0; i < claims_cnt; ++i) {
    string key;
    vector<byte> key_bytes;
    BytesVLI::deserialize(ptr, key_bytes);
    bsds->huffman.decode(key_bytes.data(), key);
    ptr += BytesVLI::length(ptr);

    AEIProperties aeip;
    decodeAEIP(ptr, aeip, bsds);
    ptr += BytesVLI::length(ptr);

    claims[key] = aeip;
  }

  vector<NamedEntity> ne;
  decodeNE(ptr, ne);
  ptr += Bits::length(ptr);

  size_t fictional_as_num;
  VLI::deserialize(ptr, fictional_as_num);
  Ternary fictional = static_cast<Ternary>(fictional_as_num);

  value.claims = claims;
  value.named_entities = ne;
  value.fictional = fictional;

}

void AgnosticEntityInfoH::encodeAEIP(const AEIProperties& aeip, vector<byte>& encoded, ByteSerializerDeserializers* bsds){
  vector<byte> tv_encoded;
  TypedValue::serialize(aeip.tv, tv_encoded, bsds);

  size_t optionals_cnt = aeip.optionals.size();
  vector<byte> cnt_encoded;
  VLI::serialize(optionals_cnt, cnt_encoded);

  vector<byte> result;

  result.insert(result.end(), tv_encoded.begin(), tv_encoded.end());
  result.insert(result.end(), cnt_encoded.begin(), cnt_encoded.end());

  for (auto &[key, val] : aeip.optionals) {
    vector<byte> key_encoded;
    vector<byte> key_huffed;
    bsds->huffman.encode(key, key_huffed);
    BytesVLI::serialize(key_huffed, key_encoded);

    vector<byte> val_encoded;
    TypedValue::serialize(val, val_encoded, bsds);
    result.insert(result.end(), key_encoded.begin(), key_encoded.end());
    result.insert(result.end(), val_encoded.begin(), val_encoded.end());
  }
  BytesVLI::serialize(result, encoded);
}

void AgnosticEntityInfoH::decodeAEIP(const byte* ptr_whole, AEIProperties& aeip, ByteSerializerDeserializers* bsds){
  vector<byte> result;
  BytesVLI::deserialize(ptr_whole, result);

  byte* ptr = result.data();

  TypedValue::deserialize(ptr, aeip.tv, bsds);
  ptr += TypedValue::length(ptr);

  size_t optionals_cnt;
  VLI::deserialize(ptr, optionals_cnt);
  ptr += VLI::length(ptr);

  unordered_map<string, linpipe::kbelik::TypedValue> optionals;
  for (size_t i = 0; i < optionals_cnt; ++i) {
    string key;
    vector<byte> key_bytes;
    BytesVLI::deserialize(ptr, key_bytes);
    ptr += BytesVLI::length(ptr);
    bsds->huffman.decode(key_bytes.data(), key);

    linpipe::kbelik::TypedValue tv;
    TypedValue::deserialize(ptr, tv, bsds);
    ptr += TypedValue::length(ptr);

    optionals[key] = tv;
  }
  aeip.optionals = optionals;
}

void AgnosticEntityInfoH::encodeNE(const vector<NamedEntity>& value, vector<byte>& encoded) {
  auto ne_as_bits = NamedEntityConverter::ne_to_bools(value);
  Bits::serialize(ne_as_bits, encoded);
}

void AgnosticEntityInfoH::decodeNE(const byte* ptr, vector<NamedEntity>& ne) {
  vector<bool> ne_as_bits;
  Bits::deserialize(ptr, ne_as_bits);
  ne = NamedEntityConverter::bools_to_ne(ne_as_bits);
}

} // namespace linpipe::kbelik::map_values
