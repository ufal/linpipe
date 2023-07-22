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

size_t SpecificEntityInfoH::length(const byte* ptr) const {
  return bytes_vli.length(ptr);
}

size_t SpecificEntityInfoH::length(const Type& value) const {
  vector<byte> serialized;
  serialize(value, serialized);
  return bytes_vli.length(serialized.data());
}

void SpecificEntityInfoH::serialize(const Type& value, vector<byte>& data) const {
  vector<byte> result;

  // label
  vector<byte> encoded_label;
  huffman.encode(value.label, encoded_label);
  bytes_vli.serialize(encoded_label, result);

  // text
  vector<byte> encoded_text;
  huffman.encode(value.text, encoded_text);
  bytes_vli.serialize(encoded_text, result);

  // description
  vector<byte> encoded_desc;
  huffman.encode(value.description, encoded_desc);
  bytes_vli.serialize(encoded_desc, result);

  encode_aliases(value.aliases, result);

  bytes_vli.serialize(result, data);  
}

void SpecificEntityInfoH::deserialize(const byte*& ptr_whole, Type& value) const {
  vector<byte> result;
  bytes_vli.deserialize(ptr_whole, result);

  const byte* ptr = result.data();

  // label
  vector<byte> decoded_label;
  bytes_vli.deserialize(ptr, decoded_label);
  huffman.decode(decoded_label.data(), value.label);

  // text
  vector<byte> decoded_text;
  bytes_vli.deserialize(ptr, decoded_text);
  huffman.decode(decoded_text.data(), value.text);

  // description
  vector<byte> decoded_desc;
  bytes_vli.deserialize(ptr, decoded_desc);
  huffman.decode(decoded_desc.data(), value.description);

  decode_aliases(ptr, value.aliases);
}
  
void SpecificEntityInfoH::encode_aliases(const unordered_set<string>& aliases, vector<byte>& encoded) const {
  size_t cnt = aliases.size();
  vli.serialize(cnt, encoded);

  vector<byte> temp;
  for (auto al: aliases) {
    vector<byte> encoded_al;
    huffman.encode(al, encoded_al);
    bytes_vli.serialize(encoded_al, temp);
    encoded.insert(encoded.end(), temp.begin(), temp.end());
  }
}

void SpecificEntityInfoH::decode_aliases(const byte* ptr, unordered_set<string>& aliases) const {
  aliases.clear();

  uint64_t cnt;
  vli.deserialize(ptr, cnt);

  for (size_t i = 0; i < cnt; ++i) {
    string al;
    vector<byte> bytes;
    bytes_vli.deserialize(ptr, bytes);
    huffman.decode(bytes.data(), al);
    aliases.insert(al);
  }
}

} // namespace linpipe::kbelik::map_values
