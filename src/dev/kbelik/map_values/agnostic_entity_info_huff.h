// AgnosticEntityInfoH allows for reading and writing claims into memory.
// Expects claims to be string -> string map.
#pragma once

#include <cmath>
#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/huffman.h"
#include "dev/kbelik/map_values/bits.h"
#include "dev/kbelik/map_values/bytes_vli.h"
#include "dev/kbelik/map_values/typed_value.h"
#include "dev/kbelik/map_values/vli.h"

#include "dev/kbelik/agnostic_entity_info.h"
#include "dev/kbelik/named_entity_mapper.h"
#include "dev/kbelik/typed_value.h"
#include "dev/kbelik/utils.h"

namespace linpipe::kbelik::map_values {

class AgnosticEntityInfoH {
 public:
  using Type = linpipe::kbelik::AgnosticEntityInfo;

  AgnosticEntityInfoH() = delete;
  AgnosticEntityInfoH(linpipe::kbelik::HuffmanTree& huffman, linpipe::kbelik::NamedEntityMapper& nem) : 
    huffman(huffman), nem(nem), tv(TypedValue(huffman)) { }

  size_t length(const byte* ptr) const;
  size_t length(const Type& value) const;
   
  void serialize(const Type& value, vector<byte>& data) const;
  void deserialize(const byte*& ptr_whole, Type& value) const;
  linpipe::kbelik::HuffmanTree& huffman;
  linpipe::kbelik::NamedEntityMapper& nem;

 private:

  Bits bits;
  BytesVLI bytes_vli;
  TypedValue tv;
  VLI vli;

  void encodeAEIP(const AEIProperties& aeip, vector<byte>& encoded) const;
  void decodeAEIP(const byte*& ptr_whole, AEIProperties& aeip) const;

  void encodeNE(const vector<string>& value, vector<byte>& encoded) const;
  void decodeNE(const byte*& ptr, vector<string>& ne) const;
};

} // namespace linpipe::kbelik::map_values
