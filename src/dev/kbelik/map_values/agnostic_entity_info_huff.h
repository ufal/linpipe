// AgnosticEntityInfoH allows for reading and writing claims into memory.
// Expects claims to be string -> string map.
#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/map_values/simple_json.h"
#include "dev/kbelik/huffman.h"

#include "dev/kbelik/agnostic_kbelik.h"

namespace linpipe::kbelik::map_values {

class AgnosticEntityInfoH {
 public:
  using Type = linpipe::kbelik::AgnosticEntityInfo;

  static inline size_t length(const byte* ptr);
  static inline size_t length(const Type& value, HuffmanTree& huff);
   
  static inline void deserialize(const byte* ptr, Type& value, HuffmanTree& huff);
  static inline void serialize(const Type& value, vector<byte>& data, HuffmanTree& huff);
};

size_t AgnosticEntityInfoH::length(const byte* ptr) {
  return SimpleJson::length(ptr);
}

size_t AgnosticEntityInfoH::length(const AgnosticEntityInfoH::Type& value, HuffmanTree& huff) {
}

void AgnosticEntityInfoH::deserialize(const byte* ptr, AgnosticEntityInfoH::Type& value, HuffmanTree& huff) {
}

void AgnosticEntityInfoH::serialize(const AgnosticEntityInfoH::Type& value, vector<byte>& data, HuffmanTree& huff) {
}

} // namespace linpipe::kbelik::map_values
