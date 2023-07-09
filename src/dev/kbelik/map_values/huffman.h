#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/map_values/bytes_vli.h"

#include "dev/kbelik/byte_serializer_deserializer.h"
#include "dev/kbelik/huffman.h"

namespace linpipe::kbelik::map_values {

class HuffmanTree {
 public:
  using Type = linpipe::kbelik::HuffmanTree;
  static inline size_t length(const byte* ptr);
  static inline size_t length(const Type& val, ByteSerializerDeserializers* bsds=nullptr);
  static inline void deserialize(const byte* ptr, Type& value, ByteSerializerDeserializers* bsds=nullptr);
  static inline void serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds=nullptr);
};

size_t HuffmanTree::length(const byte* ptr) {
  return BytesVLI::length(ptr);
}

size_t HuffmanTree::length(const HuffmanTree::Type& value, ByteSerializerDeserializers* /*bsds*/) {
  vector<byte >serialized_huffman;
  value.serialize(serialized_huffman);
  return BytesVLI::length(serialized_huffman);
}

void HuffmanTree::deserialize(const byte* ptr, HuffmanTree::Type& value, ByteSerializerDeserializers* /*bsds*/) {
  vector<byte >serialized_huffman;
  BytesVLI::deserialize(ptr, serialized_huffman);
  value.deserialize(serialized_huffman);
}

void HuffmanTree::serialize(const HuffmanTree::Type& value, vector<byte>& data, ByteSerializerDeserializers* /*bsds*/) {
  vector<byte >serialized_huffman;
  value.serialize(serialized_huffman);
  BytesVLI::serialize(serialized_huffman, data);
}

} // namespace linpipe::kbelik::map_values
