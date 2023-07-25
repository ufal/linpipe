#include <cstring>
#include <filesystem>

#include "common.h"

namespace linpipe::kbelik::map_values {

size_t TypedValue::length(const byte* ptr) const {
  return bytes_vli.length(ptr);
}

size_t TypedValue::length(const Type& value) const {
  vector<byte> encoded_st, encoded_val;
  encode(value, encoded_st, encoded_val);

  return bytes_vli.length(encoded_st) + bytes_vli.length(encoded_val);
}

void TypedValue::deserialize(const byte*& ptr, Type& value) const {
  vector<byte> encoded_st, encoded_val, encoded;

  bytes_vli.deserialize(ptr, encoded);

  size_t st_encoded_length = bytes_vli.length(encoded.data());
  encoded_val = vector<byte>(encoded.begin() + st_encoded_length, encoded.end());

  const byte* encoded_ptr = encoded.data();
  bytes_vli.deserialize(encoded_ptr, encoded_st);

  decode(encoded_st, encoded_val, value);
}

void TypedValue::serialize(const Type& value, vector<byte>& data) const {
  vector<byte> encoded_st, encoded_val;
  encode(value, encoded_st, encoded_val);

  vector<byte> encoded;
  // Encodes subtype so that encoded is: subtype bytes count | subtype bytes
  bytes_vli.serialize(encoded_st, encoded);

  encoded.insert(encoded.end(), encoded_val.begin(), encoded_val.end());

  // The encoded result will look like: all bytes count | subtype bytes count | subtype bytes | val bytes
  // where the number of val bytes is {all bytes} - {subtype bytes}
  bytes_vli.serialize(encoded, data);
}

void TypedValue::encode(const Type& value, vector<byte>& encoded_st, vector<byte>& encoded_val) const{
  string st =value.get_type_string();
  string val = value.get_string();

  huffman.encode(st, encoded_st);
  huffman.encode(val, encoded_val);
}

void TypedValue::decode(vector<byte>& encoded_st, vector<byte>& encoded_val, Type& value) const{
  string st, val;

  huffman.decode(encoded_st.data(), st);
  huffman.decode(encoded_val.data(), val);

  value = Type(st, val);
}

} // namespace linpipe::kbelik::map_values
