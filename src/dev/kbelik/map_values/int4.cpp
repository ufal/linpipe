#include "common.h"

#include <string.h>

#include "dev/kbelik/map_values/int4.h"

namespace linpipe::kbelik::map_values {

size_t Int4::length(const byte* ptr) {
  return 4;
}

void Int4::deserialize(const byte* ptr, Int4::Type& value) {
  memcpy(&value, ptr, Int4::length(ptr));
}

void Int4::serialize(const Int4::Type& value, vector<byte>& data) {
  size_t bytes_cnt = Int4::length(NULL);
  data.resize(bytes_cnt);
  memcpy(data.data(), &value, bytes_cnt);
}

} // namespace linpipe::kbelik::map_values
