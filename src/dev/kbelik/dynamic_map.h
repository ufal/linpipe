#pragma once

#include <cstring>
#include <filesystem>
#include <map>

#include "common.h"

#include "dev/kbelik/map_type.h"

namespace linpipe::kbelik {

template<typename Key, typename Value>
class DynamicMap{
 public:
  bool find(Key key, typename Value::Type& value) const;
  void add(Key key, const typename Value::Type& value);
  void erase(Key key);
  void insert(DynamicMap<Key, Value> m2);

  map<Key, typename Value::Type> get_values() const;

  size_t length() const;

  void save_map(ostream& os, MapType type);

  map<Key, typename Value::Type> values;

 private:

  void write_type(ostream& os, MapType type);
  void write_keys_and_values(ostream& os);
  void memcpy_two(byte* dest, const byte* first, const byte* second, 
                  size_t first_count, size_t second_count);

  vector<size_t> value_prefix_sums();
};

template<typename Key, typename Value>
bool DynamicMap<Key, Value>::find(Key key, typename Value::Type& value) const {
  auto search =  values.find(key);
  if (search == values.end())
    return false;
  value = (*search).second;
  return true;
}

template<typename Key, typename Value>
void DynamicMap<Key, Value>::add(Key key, const typename Value::Type& value) {
  values.insert({key, value});
}

template<typename Key, typename Value>
void DynamicMap<Key, Value>::erase(Key key) {
  values.erase(key);
}

template<typename Key, typename Value>
void DynamicMap<Key, Value>::insert(DynamicMap<Key, Value> m2) {
  auto vals = m2.get_values();
  values.insert(vals.begin(), vals.end());
}

template<typename Key, typename Value>
map<Key, typename Value::Type> DynamicMap<Key, Value>::get_values() const {
  return values;
}

template<typename Key, typename Value>
size_t DynamicMap<Key, Value>::length() const {
  return values.size();
}

template<typename Key, typename Value>
void DynamicMap<Key, Value>::save_map(ostream& os, MapType type) {
  write_type(os, type);
  write_keys_and_values(os);
}

template<typename Key, typename Value>
void DynamicMap<Key, Value>::write_type(ostream& os, MapType type) {
  os.write((char*)&type, sizeof(type));
}

template<typename Key, typename Value>
void DynamicMap<Key, Value>::write_keys_and_values(ostream& os) {
  auto size_sums = value_prefix_sums();
  using address_type = uint32_t;
  size_t address_size = sizeof(address_type);
  size_t key_address_size = sizeof(Key) + address_size;
  size_t index_size = values.size() *  key_address_size;
  vector<byte> to_stream(size_sums.back() + index_size + sizeof(size_t), (byte)0);
  byte* to_stream_ptr = to_stream.data();
  int idx = 0;
  vector<byte> data;

  memcpy(to_stream_ptr, (byte*)&index_size, sizeof(size_t));
  to_stream_ptr += sizeof(size_t);

  for (auto p: values) {
    // key + offset -> data
    data.resize(key_address_size);
    address_type offset = size_sums[idx] + index_size - key_address_size * idx;
    memcpy_two(data.data(), (byte*)&p.first, 
               (byte*)&offset,
               sizeof(Key), address_size);

    // data -> to_stream
    memcpy(to_stream_ptr + idx * key_address_size, 
           data.data(),
           key_address_size);

    // data (contains value) -> to_stream
    Value::serialize(p.second, data);
    memcpy(to_stream_ptr + index_size + size_sums[idx++], 
           data.data(), 
           data.size());
  }
  os.write((char*)to_stream.data(), to_stream.size());
}

template<typename Key, typename Value>
void DynamicMap<Key, Value>::memcpy_two(byte* dest, const byte* first, 
                                         const byte* second, 
                                         size_t first_count, 
                                         size_t second_count) {
    memcpy(dest, 
           first, 
           first_count);
    memcpy(dest + first_count, 
           second, 
           second_count);
}

template<typename Key, typename Value>
vector<size_t> DynamicMap<Key, Value>::value_prefix_sums() {
  vector<size_t> ps(values.size() + 1, 0);
  int idx = 1;
  for (auto p: values) {
    ps[idx] = Value::length(p.second) + ps[idx - 1];
    idx++;
  }
  return ps;
}

} // namespace linpipe::kbelik
