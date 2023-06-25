#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/map_values/bytes_vli.h"

namespace linpipe::kbelik::map_values {

class Bits {
 public:
  using Type = vector<bool>;
  static inline size_t length(const byte* ptr);
  static inline size_t length(const Type& val);
  static inline void deserialize(const byte* ptr, Type& value);
  static inline void serialize(const Type& value, vector<byte>& data);
};

size_t Bits::length(const byte* ptr) {
  return BytesVLI::length(ptr);
}

size_t Bits::length(const Bits::Type& value) {
  vector<byte> mock(ceil(value.size() / 8.0) + 1);
  return BytesVLI::length(mock);
}

void Bits::deserialize(const byte* ptr, Bits::Type& value) {
  vector<byte> bytes;
  BytesVLI::deserialize(ptr, bytes);

  size_t over_eight = (size_t)bytes[0];
  value.resize(0);
  for (size_t i = 1; i < bytes.size(); ++i) {
    byte mask = (byte)0b10000000;
    for (size_t j = 0; j < (size_t)((over_eight != 0 && i+1 == bytes.size()) ? over_eight : 8); ++j) {
      value.push_back((bytes[i] & mask) != (byte)0);
      mask >>= 1;
    }
  }
}

void Bits::serialize(const Bits::Type& value, vector<byte>& data) {
  vector<byte> bytes;

  byte over_eight = (byte)(value.size() - (value.size() / 8) * 8);
  bytes.push_back(over_eight);

  byte b = (byte)0;
  for (size_t i = 0; i < value.size(); ++i) {
    if (value[i])
      b |= (byte) 1;
    if (i % 8 == 7) {
      bytes.push_back(b);
      b = (byte)0;
    }
    b <<= 1;
  }
  if (value.size() % 8) {
    b <<= 8 - (int)over_eight - 1;
    bytes.push_back(b);
  }

  BytesVLI::serialize(bytes, data);
}

} // namespace linpipe::kbelik::map_values
