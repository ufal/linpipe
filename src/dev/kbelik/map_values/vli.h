#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

namespace linpipe::kbelik::map_values {

class VLI {
 public:
  using Type = uint64_t;
  static inline size_t length(const byte* ptr);
  static inline size_t length(const Type& val);
  static inline void deserialize(const byte* ptr, Type& value);
  static inline void serialize(const Type& value, vector<byte>& data);
 private:
  static inline byte get_byte(const byte* ptr, size_t offset);
};

size_t VLI::length(const byte* ptr) {
  size_t count = 1;
  for (byte b = get_byte(ptr, 0); ((b >> 7)&(byte)1) == (byte)1; b = get_byte(ptr, count++));
  return count;
}

size_t VLI::length(const VLI::Type& value) {
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
  return max((Type)1, res);
}

void VLI::deserialize(const byte* ptr, VLI::Type& value) {
  value = 0;
  //cout << length(ptr) << '\n';
  for (size_t i = 0; i < length(ptr); ++i) {
    Type b = (Type)get_byte(ptr, i);
    b &= 0x7F;
    value |= b << (7 * i);
  }
}

void VLI::serialize(const VLI::Type& value, vector<byte>& data) {
  size_t bytes_cnt = VLI::length(value);
  data.resize(0);
  data.resize(bytes_cnt);
  VLI::Type valuecopy = value;
  for (size_t i = 0; i < bytes_cnt; ++i) {
    byte b = (byte)valuecopy;
    if (i + 1 == bytes_cnt)
      b &= (byte)0x7F;
    data[i] = b;
    valuecopy >>= 7;
  }
}

byte VLI::get_byte(const byte* ptr, size_t offset) {
  byte res;
  memcpy(&res, ptr + offset, 1);
  return res;
}

} // namespace linpipe::kbelik::map_values
