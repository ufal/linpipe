#include <cstring>
#include <filesystem>
#include <cmath>

#include "dev/kbelik/map_values/bits.h"

#include "dev/kbelik/byte_serializer_deserializer.h"

namespace linpipe::kbelik::map_values {

size_t Bits::length(const byte* ptr) const {
  return bytes_vli.length(ptr);
}

size_t Bits::length(const Bits::Type& value) const {
  vector<byte> mock((int)ceil(value.size() / 8.0) + 1);
  return bytes_vli.length(mock);
}

void Bits::deserialize(const byte*& ptr, Bits::Type& value) const {
  vector<byte> bytes;
  bytes_vli.deserialize(ptr, bytes);

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

void Bits::serialize(const Bits::Type& value, vector<byte>& data) const {
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

  bytes_vli.serialize(bytes, data);
}

} // namespace linpipe::kbelik::map_values
