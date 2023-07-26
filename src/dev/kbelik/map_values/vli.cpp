#include <cstring>
#include <filesystem>
#include <fstream>

#include "dev/kbelik/map_values/vli.h"

namespace linpipe::kbelik::map_values {

size_t VLI::length(const byte* ptr) const {
  size_t count = 1;
  for (byte b = get_byte(ptr++); ((b >> 7)&(byte)1) == (byte)1; b = get_byte(ptr++), count++);
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

void VLI::deserialize(const byte*& ptr, VLI::Type& value) const {
  value = 0;
  size_t n_of_bytes = length(ptr);
  for (size_t i = 0; i < n_of_bytes; ++i) {
    Type b = (Type)get_byte(ptr);
    ptr++;
    b &= 0x7F;
    value |= b << (7 * i);
  }
}

void VLI::deserialize(ifstream& ifs, VLI::Type& value) {
  value = 0;
  int i = 0;
  byte b = get_byte(ifs);
  while (true){
    Type casted = (Type)b;
    casted &= 0x7F;
    value |= casted << (7 * i);
    if (!(((b >> 7)&(byte)1) == (byte)1))
      break;
    i++;
    b = get_byte(ifs);
  }
}

void VLI::serialize(const VLI::Type& value, vector<byte>& data) const {
  size_t bytes_cnt = VLI::length(value);
  //data.resize(bytes_cnt);
  VLI::Type valuecopy = value;
  for (size_t i = 0; i < bytes_cnt; ++i) {
    byte b = (byte)valuecopy;
    b |= (byte)0x80;
    if (i + 1 == bytes_cnt)
      b &= (byte)0x7F;
    data.push_back(b);
    valuecopy >>= 7;
  }
}

byte VLI::get_byte(const byte* ptr) const {
  byte res;
  memcpy(&res, ptr, 1);
  return res;
}

byte VLI::get_byte(ifstream& ifs) {
  byte res;
  ifs.read((char*)&res, sizeof(res));
  return res;
}

} // namespace linpipe::kbelik::map_values
