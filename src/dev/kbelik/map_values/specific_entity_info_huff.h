// SpecificEntityInfoH allows for reading and writing claims into memory.
// Expects claims to be string -> string map.
#pragma once

#include "common.h"

#include "dev/kbelik/map_values/bytes_vli.h"
#include "dev/kbelik/map_values/vli.h"

#include "dev/kbelik/specific_entity_info.h"

namespace linpipe::kbelik::map_values {

class SpecificEntityInfoH {
 public:
  SpecificEntityInfoH() = delete;
  SpecificEntityInfoH(HuffmanTree& huffman) : huffman(huffman) {};
  using Type = linpipe::kbelik::SpecificEntityInfo;

  size_t length(const byte* ptr) const;
  size_t length(const Type& value) const;
   
  void serialize(const Type& value, vector<byte>& data) const;
  void deserialize(const byte*& ptr_whole, Type& value) const;

  HuffmanTree& huffman;
 private:

  BytesVLI bytes_vli;
  VLI vli;

  void encode_aliases(const unordered_set<string>& aliases, vector<byte>& encoded) const;
  void decode_aliases(const byte* ptr_whole, unordered_set<string>& aliases) const;
};


} // namespace linpipe::kbelik::map_values
