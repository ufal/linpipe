#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/map_values/id.h"

#include "dev/kbelik/id.h"

namespace linpipe::kbelik::map_values {

size_t ID::length(const byte* ptr) const {
  if ((int)*ptr & 1)  { 
    // The size is followed by the string representation
    uint64_t val;
    vli.deserialize(ptr, val);
    return (val >> 1) + vli.length(ptr);
  }
  // Else the size itself is the QID
  return 8;
}

size_t ID::length(const ID::Type& value) const {
  if (value.is_qid())
    return 8;
  string text = value.str();
  vector<byte> bytes(reinterpret_cast<const byte*>(text.data()), reinterpret_cast<const byte*>(text.data() + text.size()));
  size_t sz = (bytes.size() << 1) + 1;
  return vli.length(sz) + bytes.size();
}

void ID::deserialize(const byte*& ptr, ID::Type& value) const {
  if ((int)*ptr & 1)  { 
    uint64_t bytes_sz;
    vli.deserialize(ptr, bytes_sz);
    bytes_sz >>= 1;
    vector<byte> bytes;
    bytes.resize(bytes_sz);
    memcpy(bytes.data(), ptr, bytes_sz);
    value = linpipe::kbelik::ID(string(reinterpret_cast<const char*>(bytes.data()), bytes.size()));
  }
  else {
    int64_t id_val;
    int8.deserialize(ptr, id_val);
    id_val >>= 1;
    value = linpipe::kbelik::ID("Q" + to_string(id_val));
  }
}

void ID::serialize(const ID::Type& value, vector<byte>& data) const {
  if (value.is_qid()) {
    int64_t id_val = value.qid();
    int8.serialize(id_val << 1, data);
  }
  else {
    string text = value.str();
    vector<byte> bytes(reinterpret_cast<const byte*>(text.data()), reinterpret_cast<const byte*>(text.data() + text.size()));
    uint64_t sz = (bytes.size() << 1) + 1;
    vli.serialize(sz, data);
    size_t old_size = data.size();
    data.resize(old_size + bytes.size());
    memcpy(data.data() + old_size, bytes.data(), bytes.size());
  }
}

} // namespace linpipe::kbelik::map_values
