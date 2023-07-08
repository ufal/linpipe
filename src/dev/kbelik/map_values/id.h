#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/id.h"

#include "dev/kbelik/map_values/vli.h"
#include "dev/kbelik/map_values/int8.h"

#include "dev/kbelik/byte_serializer_deserializer.h"

namespace linpipe::kbelik::map_values {

class ID {
 public:
  using Type = linpipe::kbelik::ID;
  static size_t length(const byte* ptr);
  static size_t length(const Type& val, ByteSerializerDeserializers* bsds=nullptr);
  static void deserialize(const byte* ptr, Type& value, ByteSerializerDeserializers* bsds=nullptr);
  static void serialize(const Type& value, vector<byte>& data, ByteSerializerDeserializers* bsds=nullptr);
};

size_t ID::length(const byte* ptr) {
  if ((int)*ptr & 1)  { 
    // The size is followed by the string representation
    size_t val;
    VLI::deserialize(ptr, val);
    return (val >> 1) + VLI::length(ptr);
  }
  // Else the size itself is the QID
  return 8;
}

size_t ID::length(const ID::Type& value, ByteSerializerDeserializers* /*bsds*/) {
  if (value.is_qid())
    return 8;
  string text = value.str();
  vector<byte> bytes(reinterpret_cast<const byte*>(text.data()), reinterpret_cast<const byte*>(text.data() + text.size()));
  size_t sz = (bytes.size() << 1) + 1;
  return VLI::length(sz) + bytes.size();
}

void ID::deserialize(const byte* ptr, ID::Type& value, ByteSerializerDeserializers* /*bsds*/) {
  if ((int)*ptr & 1)  { 
    size_t bytes_sz;
    VLI::deserialize(ptr, bytes_sz);
    bytes_sz >>= 1;
    ptr += VLI::length(ptr);
    vector<byte> bytes;
    bytes.resize(bytes_sz);
    memcpy(bytes.data(), ptr, bytes_sz);
    value = linpipe::kbelik::ID(string(reinterpret_cast<const char*>(bytes.data()), bytes.size()));
  }
  else {
    int64_t id_val;
    Int8::deserialize(ptr, id_val);
    id_val >>= 1;
    value = linpipe::kbelik::ID("Q" + to_string(id_val));
  }
}

void ID::serialize(const ID::Type& value, vector<byte>& data, ByteSerializerDeserializers* /*bsds*/) {
  if (value.is_qid()) {
    int64_t id_val = value.qid();
    Int8::serialize(id_val << 1, data);
  }
  else {
    string text = value.str();
    vector<byte> bytes(reinterpret_cast<const byte*>(text.data()), reinterpret_cast<const byte*>(text.data() + text.size()));
    size_t sz = (bytes.size() << 1) + 1;
    VLI::serialize(sz, data);
    vector<byte> temp;
    temp.resize(bytes.size());
    memcpy(temp.data(), bytes.data(), bytes.size());
    data.insert(data.end(), temp.begin(), temp.end());
  }
}

} // namespace linpipe::kbelik::map_values
