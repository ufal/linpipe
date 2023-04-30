#include <string.h>

#include "common.h"

#include "dev/kbelik/map_values/bytes.h"

namespace linpipe::kbelik::map_values {

template<typename SizeType>
size_t Bytes<SizeType>::length(const byte* ptr) {
  SizeType sz;
  memcpy((byte*)&sz, ptr, sizeof(SizeType));
  return (size_t)sz;
}

template<typename SizeType>
size_t Bytes<SizeType>::length(const Bytes<SizeType>::Type& value) {
  return value.size() + sizeof(SizeType);
}

template<typename SizeType>
void Bytes<SizeType>::deserialize(const byte* ptr, Bytes<SizeType>::Type& value) {
  SizeType vals_cnt = Bytes<SizeType>::length(ptr) - sizeof(SizeType);
  value.resize(vals_cnt);
  memcpy(value.data(), ptr + sizeof(SizeType), vals_cnt);
}

template<typename SizeType>
void Bytes<SizeType>::serialize(const Bytes<SizeType>::Type& value, vector<byte>& data) {
  SizeType bytes_cnt = Bytes<SizeType>::length(value);
  data.resize(bytes_cnt);
  memcpy(data.data(), (byte*)&bytes_cnt, sizeof(SizeType));
  memcpy(data.data() + sizeof(SizeType), value.data(), bytes_cnt - sizeof(SizeType));
}

} // namespace linpipe::kbelik::map_values
