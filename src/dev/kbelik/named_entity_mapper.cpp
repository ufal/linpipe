#include "common.h"

#include <fstream>
#include <unordered_map>

#include "dev/kbelik/named_entity_mapper.h"

#include "lib/json.h"


namespace linpipe::kbelik {

string NamedEntityMapper::get_entity(uint16_t idx) const {
  return _int_to_ne.at(idx);
}

uint16_t NamedEntityMapper::get_entity_index(const string& named_entity) const {
  return _ne_to_int.at(named_entity);
}

void NamedEntityMapper::add_entity(string named_entity) {
  if (_ne_to_int.find(named_entity) == _ne_to_int.end()) {
    _int_to_ne[_int_to_ne.size()] = named_entity;
    _ne_to_int[named_entity] = _ne_to_int.size();
    _ne_to_count[named_entity] = 0;
  }
  _ne_to_count[named_entity]++;
}

void NamedEntityMapper::add_entities_from_json(Json& js) {
  Json& ne = js["named_entities"];
  for (auto e : ne["type"])
    add_entity(e);
}

vector<bool> NamedEntityMapper::ne_to_bools(const vector<string>& ne) const {
  uint16_t max_n = 0;
  for (auto &entity : ne)
    max_n = max(get_entity_index(entity), max_n);
  vector<bool> out(max_n + 1, false);
  for (auto &entity : ne)
    out[get_entity_index(entity)] = true;
  return out;
}

vector<string> NamedEntityMapper::bools_to_ne(const vector<bool>& vb) const {
  vector<string> out;
  for (size_t i = 0; i < vb.size(); i++)
    if (vb[i])
      out.push_back(get_entity(i));
  return out;
}

void NamedEntityMapper::sort() {
  _int_to_ne = unordered_map<uint16_t, string>();
  _ne_to_int = unordered_map<string, uint16_t>();

  for (auto it = _ne_to_count.rbegin(); it != _ne_to_count.rend(); ++it) {
    add_entity(it->first);
  }
}

void NamedEntityMapper::serialize(vector<byte>& to) const {
  vector<byte> temp;
  _vli.serialize(_int_to_ne.size(), temp);
  for (uint16_t i = 0; i < _int_to_ne.size(); ++i) 
    serialize_entity(i, temp);
  uint64_t sz = temp.size();
  _fli.serialize(sz, to);
  to.insert(to.end(), temp.begin(), temp.end());
}

void NamedEntityMapper::deserialize(const byte*& from) {
  from += _fli_size;
  _deserialize(from);
}

void NamedEntityMapper::deserialize(const vector<byte>& from) {
  const byte* in_ptr = const_cast<byte*>(from.data());
  deserialize(in_ptr);
}

size_t NamedEntityMapper::deserialize(ifstream& ifs) {
  vector<byte> size_of_rest_encoded;
  byte b;
  while (size_of_rest_encoded.size() < _fli_size && ifs.read((char*)&b, sizeof(b))) 
    size_of_rest_encoded.push_back(b);
  const byte* ptr = size_of_rest_encoded.data();
  uint64_t size_of_rest;
  _fli.deserialize(ptr, size_of_rest);
  vector<byte> encoded_nem;
  for (uint64_t i = 0; i < size_of_rest && ifs.read((char*)&b, sizeof(b)); ++i) 
    encoded_nem.push_back(b);
  ptr = encoded_nem.data();
  _deserialize(ptr);
  return _fli_size + (ptr - encoded_nem.data());
}

void NamedEntityMapper::serialize_entity(uint16_t idx, vector<byte>& to) const {
  string ne = _int_to_ne.at(idx);
  vector<byte> bytes(reinterpret_cast<const byte*>(ne.data()), reinterpret_cast<const byte*>(ne.data() + ne.size()));
  vector<byte> encoded_bytes;
  _bytes_vli.serialize(bytes, encoded_bytes);
  to.insert(to.end(), encoded_bytes.begin(), encoded_bytes.end());
}

void NamedEntityMapper::deserialize_entity(const byte*& from) {
  vector<byte> ne_decoded;
  _bytes_vli.deserialize(from, ne_decoded);
  string named_entity = string(reinterpret_cast<const char*>(ne_decoded.data()), ne_decoded.size());
  _int_to_ne[_int_to_ne.size()] = named_entity;
  _ne_to_int[named_entity] = _ne_to_int.size();
  _ne_to_count[named_entity] = 1;
}

void NamedEntityMapper::_deserialize(const byte*& from) {
  uint64_t ne_count;
  _vli.deserialize(from, ne_count);
  for (uint16_t i = 0; i < ne_count; ++i)
    deserialize_entity(from);
}

} // linpipe::kbelik