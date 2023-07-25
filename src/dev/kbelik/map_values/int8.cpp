#include <cstring>
#include <filesystem>

#include "dev/kbelik/map_values/int8.h"

namespace linpipe::kbelik::map_values {

size_t Int8::length(const byte* /*ptr*/) const {
  return 8;
}

size_t Int8::length(const Int8::Type& /*value*/) const {
  return 8;
}

void Int8::deserialize(const byte*& ptr, Int8::Type& value) const {
  memcpy(&value, ptr, Int8::length(ptr));
  ptr += 8;
}

void Int8::serialize(const Int8::Type& value, vector<byte>& data) const {
  size_t bytes_cnt = Int8::length(value);
  size_t old_size = data.size();
  data.resize(data.size() + bytes_cnt);
  memcpy(data.data() + old_size, &value, bytes_cnt);
}

} // namespace linpipe::kbelik::map_values
