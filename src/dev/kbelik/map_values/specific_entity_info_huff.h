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

  BytesVLI::serialize(result, data);  
}

void SpecificEntityInfoH::deserialize(const byte* ptr_whole, Type& value, ByteSerializerDeserializers* bsds) {

}

} // namespace linpipe::kbelik::map_values
