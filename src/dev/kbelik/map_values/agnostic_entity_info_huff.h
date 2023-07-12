// AgnosticEntityInfoH allows for reading and writing claims into memory.
// Expects claims to be string -> string map.
#pragma once

#include <cmath>
#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/byte_serializer_deserializer.h"
#include "dev/kbelik/map_values/bits.h"
#include "dev/kbelik/map_values/bytes_vli.h"
#include "dev/kbelik/map_values/typed_value.h"
#include "dev/kbelik/map_values/vli.h"

#include "dev/kbelik/agnostic_entity_info.h"
#include "dev/kbelik/named_entity.h"
#include "dev/kbelik/typed_value.h"
#include "dev/kbelik/utils.h"

namespace linpipe::kbelik::map_values {

class AgnosticEntityInfoH {
 public:
  using Type = linpipe::kbelik::AgnosticEntityInfo;

  size_t length(const byte* ptr);
  size_t length(const Type& value, ByteSerializerDeserializers* bsds);
   
  void serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds);
  void deserialize(const byte* ptr_whole, Type& value, ByteSerializerDeserializers* bsds);
 private:
  VLI vli;
  Bits bits;
  BytesVLI bytes_vli;

  void encodeAEIP(const AEIProperties& aeip, vector<byte>& encoded, ByteSerializerDeserializers* bsds);
  void decodeAEIP(const byte* ptr_whole, AEIProperties& aeip, ByteSerializerDeserializers* bsds);

  void encodeNE(const vector<NamedEntity>& value, vector<byte>& encoded);
  void decodeNE(const byte* ptr, vector<NamedEntity>& ne);
};

} // namespace linpipe::kbelik::map_values
