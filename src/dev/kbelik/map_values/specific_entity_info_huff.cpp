#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/map_values/specific_entity_info_huff.h"

#include "dev/kbelik/byte_serializer_deserializer.h"
#include "dev/kbelik/map_values/bits.h"
#include "dev/kbelik/map_values/bytes_vli.h"
#include "dev/kbelik/map_values/typed_value.h"
#include "dev/kbelik/map_values/vli.h"

#include "dev/kbelik/specific_entity_info.h"

namespace linpipe::kbelik::map_values {

size_t SpecificEntityInfoH::length(const byte* ptr) {
  return bytes_vli.length(ptr);
}

size_t SpecificEntityInfoH::length(const Type& value, ByteSerializerDeserializers* bsds) {
  vector<byte> serialized;
  serialize(value, serialized, bsds);
  return bytes_vli.length(serialized.data());
}

void SpecificEntityInfoH::serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds) {
  vector<byte> result;
  vector<byte> temp;

  // label
  vector<byte> encoded_label;
  bsds->huffman.encode(value.label, encoded_label);
  bytes_vli.serialize(encoded_label, temp);
  result.insert(result.end(), temp.begin(), temp.end());

  // text
  vector<byte> encoded_text;
  bsds->huffman.encode(value.text, encoded_text);
  bytes_vli.serialize(encoded_text, temp);
  result.insert(result.end(), temp.begin(), temp.end());

  // description
  vector<byte> encoded_desc;
  bsds->huffman.encode(value.description, encoded_desc);
  bytes_vli.serialize(encoded_desc, temp);
  result.insert(result.end(), temp.begin(), temp.end());

  encode_aliases(value.aliases, temp, bsds);
  result.insert(result.end(), temp.begin(), temp.end());

  bytes_vli.serialize(result, data);  
}

void SpecificEntityInfoH::deserialize(const byte* ptr_whole, Type& value, ByteSerializerDeserializers* bsds) {
  vector<byte> result;
  bytes_vli.deserialize(ptr_whole, result);

  auto ptr = result.data();

  // label
  vector<byte> decoded_label;
  bytes_vli.deserialize(ptr, decoded_label);
  ptr += bytes_vli.length(ptr);
  bsds->huffman.decode(decoded_label.data(), value.label);

  // text
  vector<byte> decoded_text;
  bytes_vli.deserialize(ptr, decoded_text);
  ptr += bytes_vli.length(ptr);
  bsds->huffman.decode(decoded_text.data(), value.text);

  // description
  vector<byte> decoded_desc;
  bytes_vli.deserialize(ptr, decoded_desc);
  ptr += bytes_vli.length(ptr);
  bsds->huffman.decode(decoded_desc.data(), value.description);

  decode_aliases(ptr, value.aliases, bsds);
}
  
void SpecificEntityInfoH::encode_aliases(const unordered_set<string>& aliases, vector<byte>& encoded, ByteSerializerDeserializers* bsds) {
  encoded.clear();

  size_t cnt = aliases.size();
  vli.serialize(cnt, encoded);

  vector<byte> temp;
  for (auto al: aliases) {
    vector<byte> encoded_al;
    bsds->huffman.encode(al, encoded_al);
    bytes_vli.serialize(encoded_al, temp);
    encoded.insert(encoded.end(), temp.begin(), temp.end());
  }
}

void SpecificEntityInfoH::decode_aliases(const byte* ptr, unordered_set<string>& aliases, ByteSerializerDeserializers* bsds) {
  aliases.clear();

  size_t cnt;
  vli.deserialize(ptr, cnt);
  ptr += vli.length(cnt);

  for (size_t i = 0; i < cnt; ++i) {
    string al;
    vector<byte> bytes;
    bytes_vli.deserialize(ptr, bytes);
    bsds->huffman.decode(bytes.data(), al);
    ptr += bytes_vli.length(ptr);
    aliases.insert(al);
  }
}

} // namespace linpipe::kbelik::map_values
