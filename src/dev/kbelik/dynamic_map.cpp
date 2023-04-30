#include <string.h>

#include "common.h"

#include "dev/kbelik/dynamic_map.h"

namespace linpipe::kbelik {

template<typename Key, typename Value>
DynamicMap<Key, Value>::DynamicMap(int32_t id) {
  this->id = id;
}

template<typename Key, typename Value>
size_t DynamicMap<Key, Value>::length() const {
  return values.size();
}

template<typename Key, typename Value>
void DynamicMap<Key, Value>::erase(Key key) {
  values.erase(key);
}

template<typename Key, typename Value>
void DynamicMap<Key, Value>::add(Key key, const typename Value::Type& value) {
  values.insert({key, value});
}

template<typename Key, typename Value>
bool DynamicMap<Key, Value>::find(Key key, typename Value::Type& value) const {
  auto search =  values.find(key);
  if (search == values.end())
    return false;
  value = (*search).second;
  return true;
}

template<typename Key, typename Value>
void DynamicMap<Key, Value>::write_id(ostream& os) {
  cout << "idsize" << sizeof(id) << endl;
  cout << "id" << id << endl;
  os.write((char*)&id, sizeof(id));
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
  cout << "sizeof " <<sizeof(size_t) << endl;
  to_stream_ptr += sizeof(size_t);

  for (auto p: values) {
    cout << p.first << ' ' << p.second << ' ' << to_stream.size() << '\n';
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
  os.write(static_cast<char*>(static_cast<void*>(to_stream.data())), to_stream.size());
}

template<typename Key, typename Value>
void DynamicMap<Key, Value>::save_map(ostream& os) {
  write_id(os);
  write_keys_and_values(os);
}

} // namespace linpipe::kbelik
