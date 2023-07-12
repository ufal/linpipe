// SpecificEntityInfoH allows for reading and writing claims into memory.
// Expects claims to be string -> string map.
#pragma once

#include "common.h"

#include "dev/kbelik/byte_serializer_deserializer.h"

#include "dev/kbelik/map_values/bytes_vli.h"
#include "dev/kbelik/map_values/vli.h"

#include "dev/kbelik/specific_entity_info.h"

namespace linpipe::kbelik::map_values {

class SpecificEntityInfoH {
 public:
  using Type = linpipe::kbelik::SpecificEntityInfo;

  size_t length(const byte* ptr);
  size_t length(const Type& value, ByteSerializerDeserializers* bsds);
   
  void serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds);
  void deserialize(const byte* ptr_whole, Type& value, ByteSerializerDeserializers* bsds);

 private:
  BytesVLI bytes_vli;
  VLI vli;

  void encode_aliases(const unordered_set<string>& aliases, vector<byte>& encoded, ByteSerializerDeserializers* bsds);
  void decode_aliases(const byte* ptr_whole, unordered_set<string>& aliases, ByteSerializerDeserializers* bsds);
};


} // namespace linpipe::kbelik::map_values
