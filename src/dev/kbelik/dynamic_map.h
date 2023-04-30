#pragma once

#include <filesystem>
#include <map>

#include "common.h"

namespace linpipe::kbelik {

template<typename Key, typename Value>
class DynamicMap{
 public:
  DynamicMap<Key, Value>(int32_t id); 

  bool find(Key key, typename Value::Type& value) const;
  void add(Key key, const typename Value::Type& value);
  size_t length() const;
  void erase(Key key);

  void save_map(ostream& os);

 private:
  map<Key, typename Value::Type> values;

  int32_t id;

  void write_id(ostream& os);
  void write_keys_and_values(ostream& os);
  void memcpy_two(byte* dest, const byte* first, const byte* second, 
                  size_t first_count, size_t second_count);

  vector<size_t> value_prefix_sums();

  static void build(map<Key, Value>* data, filesystem::path path); // ??
};
} // namespace linpipe::kbelik
