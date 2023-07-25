#pragma once

#include "common.h"

#include "dev/kbelik/huffman.h"
#include "dev/kbelik/map_values/bytes_vli.h"
#include "dev/kbelik/typed_value.h"

namespace linpipe::kbelik::map_values {

class TypedValue {
 public:
  using Type = linpipe::kbelik::TypedValue;

  TypedValue() = delete;
  TypedValue(HuffmanTree& huffman) : huffman(huffman) {};

  size_t length(const byte* ptr) const;
  size_t length(const Type& value) const;
   
  void deserialize(const byte*& ptr, Type& value) const;
  void serialize(const Type& value, vector<byte>& data) const;
 private:
  HuffmanTree& huffman;

  BytesVLI bytes_vli;

  void encode(const Type& value, vector<byte>& encoded_st, vector<byte>& encoded_val) const;
  void decode(vector<byte>& encoded_st, vector<byte>& encoded_val, Type& value) const;
};

} // namespace linpipe::kbelik::map_values
