#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/map_values/int4.h"

#include "dev/kbelik/byte_serializer_deserializer.h"

namespace linpipe::kbelik::map_values {

size_t Int4::length(const byte* /*ptr*/) const {
  return 4;
}

size_t Int4::length(const Int4::Type& /*value*/) const {
  return 4;
}

void Int4::deserialize(const byte*& ptr, Int4::Type& value) const {
  memcpy(&value, ptr, 4);
  ptr += 4;
}

void Int4::serialize(const Int4::Type& value, vector<byte>& data) const {
  size_t old_size = data.size();
  data.resize(old_size + 4);
  memcpy(data.data() + old_size, &value, 4);
}

} // namespace linpipe::kbelik::map_values
