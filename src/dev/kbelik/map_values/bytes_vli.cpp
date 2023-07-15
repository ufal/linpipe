#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/map_values/bytes_vli.h"

#include "dev/kbelik/byte_serializer_deserializer.h"

namespace linpipe::kbelik::map_values {

size_t BytesVLI::length(const byte* ptr) const {
  size_t data_sz;
  vli.deserialize(ptr, data_sz);
  return vli.length(ptr) + data_sz;
}

size_t BytesVLI::length(const BytesVLI::Type& value) const {
  return vli.length(value.size()) + value.size();
}

void BytesVLI::deserialize(const byte* ptr, BytesVLI::Type& value) const {
  size_t vli_sz = vli.length(ptr);
  size_t data_sz;
  vli.deserialize(ptr, data_sz);
  value.resize(data_sz);
  memcpy(value.data(), ptr + vli_sz, data_sz);
}

void BytesVLI::serialize(const BytesVLI::Type& value, vector<byte>& data) const {
  size_t data_sz = value.size();
  size_t vli_sz = vli.length(data_sz);
  vli.serialize(data_sz, data);
  data.resize(vli_sz + data_sz);
  memcpy(data.data() + vli_sz, value.data(), data_sz);
}

} // namespace linpipe::kbelik::map_values
