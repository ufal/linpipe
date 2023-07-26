#pragma once

#include <fstream>

#include "common.h"

namespace linpipe::kbelik::map_values {

class VLI {
 public:
  using Type = uint64_t;
  size_t length(const byte* ptr) const;
  size_t length(const Type& val) const;
  void deserialize(const byte*& ptr, Type& value) const;
  void deserialize(ifstream& ifs, Type& value);
  void serialize(const Type& value, vector<byte>& data) const;
 private:
  byte get_byte(const byte* ptr) const;
  byte get_byte(ifstream& ptr);
};

} // namespace linpipe::kbelik::map_values
