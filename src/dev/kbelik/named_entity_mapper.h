#pragma once

#include <fstream>
#include <unordered_map>

#include "common.h"

#include "dev/kbelik/map_values/bytes_vli.h"
#include "dev/kbelik/map_values/vli.h"

namespace linpipe::kbelik {

class NamedEntityMapper {
 public:
  string get_entity(uint16_t idx) const;
  uint16_t get_entity_index(const string& named_entity) const;
  void add_entity(string named_entity);
  void add_entities_from_json(Json& js);

  void sort();

  vector<bool> ne_to_bools(const vector<string>& ne) const;
  vector<string> bools_to_ne(const vector<bool>& vb) const;

  void serialize(vector<byte>& to) const;

  void deserialize(const byte*& from);
  void deserialize(const vector<byte>& from);
  size_t deserialize(ifstream& ifs);

 private:
  unordered_map<uint16_t, string> _int_to_ne;
  unordered_map<string, uint16_t> _ne_to_int;
  map<string, uint64_t> _ne_to_count;

  map_values::BytesVLI _bytes_vli = map_values::BytesVLI();
  map_values::VLI _vli = map_values::VLI();

  void serialize_entity(uint16_t idx, vector<byte>& to) const;
  void deserialize_entity(const byte*& from);

  void _deserialize(const byte*& from);
};

} // linpipe::kbelik