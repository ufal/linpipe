#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/map_values/vli.h"

namespace linpipe::kbelik::map_values {

size_t VLI::length(const byte* ptr) const {
  size_t count = 1;
  for (byte b = get_byte(ptr, 0); ((b >> 7)&(byte)1) == (byte)1; b = get_byte(ptr, count++));
  return count;
}

size_t VLI::length(const VLI::Type& value) const {
  Type valuecopy = value;
  int msb = 0;
  size_t res = 0;
  while (valuecopy) {
    valuecopy >>= 1;
    msb++;
    if (msb == 7) {
      res++;
      msb -= 7;
    }
  }
  if (msb != 0)
    res++;
  return max((size_t)1, res);
}

void VLI::deserialize(const byte* ptr, VLI::Type& value) const {
  value = 0;
  //cout << length(ptr) << '\n';
  for (size_t i = 0; i < length(ptr); ++i) {
    Type b = (Type)get_byte(ptr, i);
    b &= 0x7F;
    value |= b << (7 * i);
  }
}

void VLI::serialize(const VLI::Type& value, vector<byte>& data) const {
  size_t bytes_cnt = VLI::length(value);
  data.resize(bytes_cnt);
  VLI::Type valuecopy = value;
  for (size_t i = 0; i < bytes_cnt; ++i) {
    byte b = (byte)valuecopy;
    b |= (byte)0x80;
    if (i + 1 == bytes_cnt)
      b &= (byte)0x7F;
    data[i] = b;
    valuecopy >>= 7;
  }
}

byte VLI::get_byte(const byte* ptr, size_t offset) const {
  byte res;
  memcpy(&res, ptr + offset, 1);
  return res;
}

} // namespace linpipe::kbelik::map_values
