// AgnosticEntityInfoH allows for reading and writing claims into memory.
// Expects claims to be string -> string map.
#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/huffman.h"
#include "dev/kbelik/map_values/bytes_vli.h"

#include "dev/kbelik/agnostic_kbelik.h"

namespace linpipe::kbelik::map_values {

class AgnosticEntityInfoH {
 public:
  using Type = linpipe::kbelik::AgnosticEntityInfo;

  static inline size_t length(const byte* ptr);
  static inline size_t length(const Type& value, HuffmanTree& huff);
   
  static inline void deserialize(const byte* ptr,  HuffmanTree& huff, Type& value);
  static inline void serialize(const Type& value, HuffmanTree& huff, vector<byte>& data);
 private:
  static inline void encode(const Type& value, HuffmanTree& huff, vector<byte>& encoded);
  static inline void decode(vector<byte>& encoded, HuffmanTree& huff, Type& value);
};

size_t AgnosticEntityInfoH::length(const byte* ptr) {
  return BytesVLI::length(ptr);
}

size_t AgnosticEntityInfoH::length(const Type& value, HuffmanTree& huff) {
  vector<byte> encoded;
  encode(value, huff, encoded);
  return BytesVLI::length(encoded);
}

void AgnosticEntityInfoH::deserialize(const byte* ptr, HuffmanTree& huff, Type& value) {
  vector<byte> compressed;
  BytesVLI::deserialize(ptr, compressed);
  decode(compressed, huff, value);
}

void AgnosticEntityInfoH::serialize(const Type& value, HuffmanTree& huff, vector<byte>& data) {
  vector<byte> encoded;
  encode(value, huff, encoded);
  Type v2;
  decode(encoded, huff, v2);
  BytesVLI::serialize(encoded, data);
}

void AgnosticEntityInfoH::encode(const Type& value, HuffmanTree& huff, vector<byte>& encoded){
  string aei = value.to_string_representation();
  huff.encode(aei, encoded);
}

void AgnosticEntityInfoH::decode(vector<byte>& encoded, HuffmanTree& huff, Type& value){
  string aei;
  huff.decode(encoded.data(), aei);
  value = Type(aei);
}

} // namespace linpipe::kbelik::map_values
