#pragma once

#include <filesystem>
#include <map>

#include "common.h"

namespace linpipe::kbelik {

template<typename Key, typename Value>
class DynamicMap{
 public:
  bool find(Key key, Value::Type& value) const;
  void add(Key key, const Value::Type& value);

  void save_map(ostream& os);

 private:
  map<Key, Value::Type> values;

  static void build(map<Key, Value>* data, filesystem::path path);
};
} // namespace linpipe::kbelik
