#include <cstring>
#include <filesystem>

#include "dev/kbelik/map_values/bytes_vli.h"

namespace linpipe::kbelik::map_values {

size_t BytesVLI::length(const byte* ptr) const {
  uint64_t data_sz;
  const byte* ptr_copy = ptr;
  vli.deserialize(ptr_copy, data_sz);
  return vli.length(ptr) + (size_t)data_sz;
}

size_t BytesVLI::length(const BytesVLI::Type& value) const {
  return vli.length(value.size()) + value.size();
}

void BytesVLI::deserialize(const byte*& ptr, BytesVLI::Type& value) const {
  uint64_t data_sz;
  vli.deserialize(ptr, data_sz);
  value.resize(data_sz);
  memcpy(value.data(), ptr, data_sz);
  ptr += data_sz;
}

void BytesVLI::serialize(const BytesVLI::Type& value, vector<byte>& data) const {
  uint64_t data_sz = value.size();
  //size_t vli_sz = vli.length(data_sz);
  vli.serialize(data_sz, data);
  size_t old_size = data.size();
  data.resize(data_sz + old_size);
  memcpy(data.data() + old_size, value.data(), data_sz);
}

} // namespace linpipe::kbelik::map_values
