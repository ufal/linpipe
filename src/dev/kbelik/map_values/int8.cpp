#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/map_values/int8.h"

namespace linpipe::kbelik::map_values {

size_t Int8::length(const byte* /*ptr*/) {
  return 8;
}

size_t Int8::length(const Int8::Type& /*value*/, ByteSerializerDeserializers* /*bsds*/) {
  return 8;
}

void Int8::deserialize(const byte* ptr, Int8::Type& value, ByteSerializerDeserializers* /*bsds*/) {
  memcpy(&value, ptr, Int8::length(ptr));
}

void Int8::serialize(const Int8::Type& value, vector<byte>& data, ByteSerializerDeserializers* /*bsds*/) {
  size_t bytes_cnt = Int8::length(value);
  data.resize(bytes_cnt);
  memcpy(data.data(), &value, bytes_cnt);
}

} // namespace linpipe::kbelik::map_values
