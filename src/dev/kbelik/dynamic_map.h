#pragma once

#include <cstring>
#include <filesystem>
#include <map>

#include "common.h"

#include "dev/kbelik/byte_serializer_deserializer.h"
#include "dev/kbelik/map_type.h"

namespace linpipe::kbelik {

template<typename MapKey, typename Value>
class DynamicMap{
 public:
  DynamicMap(MapKey& mk, Value& mv) : mk(mk), mv(mv) {}

  bool find(typename MapKey::Type key, typename Value::Type& value) const;
  void add(typename MapKey::Type key, const typename Value::Type& value);
  void erase(typename MapKey::Type key);
  void insert(DynamicMap<MapKey, Value> m2);

  map<typename MapKey::Type, typename Value::Type> get_values() const;

  size_t length() const;

  void save_map(ostream& os, MapType type);

  map<typename MapKey::Type, typename Value::Type> values;

 private:
  MapKey& mk;
  Value& mv;

  void write_type(ostream& os, MapType type);
  void write_keys_and_values(ostream& os);
  void memcpy_two(byte* dest, const byte* first, const byte* second, 
                  size_t first_count, size_t second_count);

  vector<size_t> value_prefix_sums();
  int const key_size = 8;
};

template<typename MapKey, typename Value>
bool DynamicMap<MapKey, Value>::find(typename MapKey::Type key, typename Value::Type& value) const {
  auto search =  values.find(key);
  if (search == values.end())
    return false;
  value = (*search).second;
  return true;
}

template<typename MapKey, typename Value>
void DynamicMap<MapKey, Value>::add(typename MapKey::Type key, const typename Value::Type& value) {
  values.insert({key, value});
}

template<typename MapKey, typename Value>
void DynamicMap<MapKey, Value>::erase(typename MapKey::Type key) {
  values.erase(key);
}

template<typename MapKey, typename Value>
void DynamicMap<MapKey, Value>::insert(DynamicMap<MapKey, Value> m2) {
  auto vals = m2.get_values();
  values.insert(vals.begin(), vals.end());
}

template<typename MapKey, typename Value>
map<typename MapKey::Type, typename Value::Type> DynamicMap<MapKey, Value>::get_values() const {
  return values;
}

template<typename MapKey, typename Value>
size_t DynamicMap<MapKey, Value>::length() const {
  return values.size();
}

template<typename MapKey, typename Value>
void DynamicMap<MapKey, Value>::save_map(ostream& os, MapType type) {
  write_type(os, type);
  write_keys_and_values(os);
}

template<typename MapKey, typename Value>
void DynamicMap<MapKey, Value>::write_type(ostream& os, MapType type) {
  os.write((char*)&type, sizeof(type));
}

template<typename MapKey, typename Value>
void DynamicMap<MapKey, Value>::write_keys_and_values(ostream& os) {
  auto size_sums = value_prefix_sums();
  using address_type = uint32_t;
  size_t address_size = sizeof(address_type);
  size_t key_address_size = key_size + address_size;
  size_t index_size = values.size() *  key_address_size;
  vector<byte> to_stream(size_sums.back() + index_size + sizeof(size_t), (byte)0);
  byte* to_stream_ptr = to_stream.data();
  int idx = 0;
  vector<byte> data;

  memcpy(to_stream_ptr, (byte*)&index_size, sizeof(size_t));
  to_stream_ptr += sizeof(size_t);

  for (auto& p: values) {
    // key  offset -> data
    data.resize(key_address_size);
    address_type offset = size_sums[idx] + index_size - key_address_size * idx;
    vector<byte> serialized_key;
    //mk.serialize(p.first, serialized_key);
    uint64_t key_as_uint = mk.convert_to_uint(p.first);
    memcpy_two(data.data(), (byte*)&key_as_uint, 
               (byte*)&offset,
               mk.length(), address_size);

    // data -> to_stream
    memcpy(to_stream_ptr + idx * key_address_size, 
           data.data(),
           key_address_size);

    // data (contains value) -> to_stream
    data.clear();
    mv.serialize(p.second, data);
    memcpy(to_stream_ptr + index_size + size_sums[idx++], 
           data.data(), 
           data.size());
  }
  os.write((char*)to_stream.data(), to_stream.size());
}

template<typename MapKey, typename Value>
void DynamicMap<MapKey, Value>::memcpy_two(byte* dest, const byte* first, 
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

template<typename MapKey, typename Value>
vector<size_t> DynamicMap<MapKey, Value>::value_prefix_sums() {
  vector<size_t> ps(values.size() + 1, 0);
  int idx = 1;
  for (auto p: values) {
    ps[idx] = mv.length(p.second) + ps[idx - 1];
    idx++;
  }
  return ps;
}

} // namespace linpipe::kbelik
