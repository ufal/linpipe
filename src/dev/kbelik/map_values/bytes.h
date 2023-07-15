#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/byte_serializer_deserializer.h"

namespace linpipe::kbelik::map_values {

template<typename SizeType>
class Bytes {
 public:
  using Type = vector<byte>;
  size_t length(const byte* ptr) const;
  size_t length(const Type& val) const;
  void deserialize(const byte* ptr, Type& value) const;
  void serialize(const Type& value, vector<byte>& data) const;
};

template<typename SizeType>
size_t Bytes<SizeType>::length(const byte* ptr) const {
  SizeType sz;
  memcpy((byte*)&sz, ptr, sizeof(SizeType));
  return (size_t)sz;
}

template<typename SizeType>
size_t Bytes<SizeType>::length(const Bytes<SizeType>::Type& value) const {
  return value.size() + sizeof(SizeType);
}

template<typename SizeType>
void Bytes<SizeType>::deserialize(const byte* ptr, Bytes<SizeType>::Type& value) const {
  SizeType vals_cnt = Bytes<SizeType>::length(ptr) - sizeof(SizeType);
  value.resize(vals_cnt);
  memcpy(value.data(), ptr + sizeof(SizeType), vals_cnt);
}

template<typename SizeType>
void Bytes<SizeType>::serialize(const Bytes<SizeType>::Type& value, vector<byte>& data) const {
  SizeType bytes_cnt = Bytes<SizeType>::length(value);
  data.resize(bytes_cnt);
  memcpy(data.data(), (byte*)&bytes_cnt, sizeof(SizeType));
  memcpy(data.data() + sizeof(SizeType), value.data(), bytes_cnt - sizeof(SizeType));
}

} // namespace linpipe::kbelik::map_values
