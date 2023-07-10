// SpecificEntityInfoH allows for reading and writing claims into memory.
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

#include "dev/kbelik/specific_entity_info.h"
#include "dev/kbelik/named_entity.h"
#include "dev/kbelik/typed_value.h"
#include "dev/kbelik/utils.h"

namespace linpipe::kbelik::map_values {

class SpecificEntityInfoH {
 public:
  using Type = linpipe::kbelik::SpecificEntityInfo;

  static inline size_t length(const byte* ptr);
  static inline size_t length(const Type& value, ByteSerializerDeserializers* bsds);
   
  static inline void serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds);
  static inline void deserialize(const byte* ptr_whole, Type& value, ByteSerializerDeserializers* bsds);

 private:
  static inline void encode_aliases(const unordered_set<string>& aliases, vector<byte>& encoded, ByteSerializerDeserializers* bsds);
  static inline void decode_aliases(const byte* ptr_whole, unordered_set<string>& aliases, ByteSerializerDeserializers* bsds);
};

size_t SpecificEntityInfoH::length(const byte* ptr) {
  return BytesVLI::length(ptr);
}

size_t SpecificEntityInfoH::length(const Type& value, ByteSerializerDeserializers* bsds) {
  vector<byte> serialized;
  serialize(value, serialized, bsds);
  return BytesVLI::length(serialized.data());
}

void SpecificEntityInfoH::serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds) {
  vector<byte> result;
  vector<byte> temp;

  // label
  vector<byte> encoded_label;
  bsds->huffman.encode(value.label, encoded_label);
  BytesVLI::serialize(encoded_label, temp);
  result.insert(result.end(), temp.begin(), temp.end());

  // text
  vector<byte> encoded_text;
  bsds->huffman.encode(value.text, encoded_text);
  BytesVLI::serialize(encoded_text, temp);
  result.insert(result.end(), temp.begin(), temp.end());

  // description
  vector<byte> encoded_desc;
  bsds->huffman.encode(value.description, encoded_desc);
  BytesVLI::serialize(encoded_desc, temp);
  result.insert(result.end(), temp.begin(), temp.end());

  encode_aliases(value.aliases, temp, bsds);
  result.insert(result.end(), temp.begin(), temp.end());

  BytesVLI::serialize(result, data);  
}

void SpecificEntityInfoH::deserialize(const byte* ptr_whole, Type& value, ByteSerializerDeserializers* bsds) {
  vector<byte> result;
  BytesVLI::deserialize(ptr_whole, result);

  auto ptr = result.data();

  // label
  vector<byte> decoded_label;
  BytesVLI::deserialize(ptr, decoded_label);
  ptr += BytesVLI::length(ptr);
  bsds->huffman.decode(decoded_label.data(), value.label);

  // text
  vector<byte> decoded_text;
  BytesVLI::deserialize(ptr, decoded_text);
  ptr += BytesVLI::length(ptr);
  bsds->huffman.decode(decoded_text.data(), value.text);

  // description
  vector<byte> decoded_desc;
  BytesVLI::deserialize(ptr, decoded_desc);
  ptr += BytesVLI::length(ptr);
  bsds->huffman.decode(decoded_desc.data(), value.description);

  decode_aliases(ptr, value.aliases, bsds);
}
  
void SpecificEntityInfoH::encode_aliases(const unordered_set<string>& aliases, vector<byte>& encoded, ByteSerializerDeserializers* bsds) {
  encoded.clear();

  size_t cnt = aliases.size();
  VLI::serialize(cnt, encoded);

  vector<byte> temp;
  for (auto al: aliases) {
    vector<byte> encoded_al;
    bsds->huffman.encode(al, encoded_al);
    BytesVLI::serialize(encoded_al, temp);
    encoded.insert(encoded.end(), temp.begin(), temp.end());
  }
}

void SpecificEntityInfoH::decode_aliases(const byte* ptr, unordered_set<string>& aliases, ByteSerializerDeserializers* bsds) {
  aliases.clear();

  size_t cnt;
  VLI::deserialize(ptr, cnt);
  ptr += VLI::length(cnt);

  for (size_t i = 0; i < cnt; ++i) {
    string al;
    vector<byte> bytes;
    BytesVLI::deserialize(ptr, bytes);
    bsds->huffman.decode(bytes.data(), al);
    ptr += BytesVLI::length(ptr);
    aliases.insert(al);
  }
}

} // namespace linpipe::kbelik::map_values
