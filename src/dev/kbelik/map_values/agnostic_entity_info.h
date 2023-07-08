// AgnosticEntityInfo allows for reading and writing claims into memory.
// Expects claims to be string -> string map.
#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/map_values/simple_json.h"

#include "dev/kbelik/agnostic_kbelik.h"
#include "dev/kbelik/byte_serializer_deserializer.h"

namespace linpipe::kbelik::map_values {

class AgnosticEntityInfo {
 public:
  using Type = linpipe::kbelik::AgnosticEntityInfo;

  static inline size_t length(const byte* ptr);
  static inline size_t length(const Type& value, ByteSerializerDeserializers* bsds = nullptr);
   
  static inline void deserialize(const byte* ptr, Type& value), ByteSerializerDeserializers* bsds = nullptr;
  static inline void serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds = nullptr);
};

size_t AgnosticEntityInfo::length(const byte* ptr) {
  return SimpleJson::length(ptr);
}

size_t AgnosticEntityInfo::length(const AgnosticEntityInfo::Type& value, ByteSerializerDeserializers* /*bsds*/) {
  Json j = value.claims;
  return SimpleJson::length(j);
}

void AgnosticEntityInfo::deserialize(const byte* ptr, AgnosticEntityInfo::Type& value, ByteSerializerDeserializers* /*bsds*/) {
  Json j;
  SimpleJson::deserialize(ptr, j);
  auto aei = Type();
  aei.claims = j;
  value = aei;
}

void AgnosticEntityInfo::serialize(const AgnosticEntityInfo::Type& value, vector<byte>& data, ByteSerializerDeserializers* /*bsds*/) {
  Json j = value.claims;
  SimpleJson::serialize(j, data);
}

} // namespace linpipe::kbelik::map_values
