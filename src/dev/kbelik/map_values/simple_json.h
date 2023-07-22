#pragma once

#include <cstring>
#include <filesystem>

#include "common.h"
#include "lib/json.h"

#include "dev/kbelik/map_values/vli.h"

#include "dev/kbelik/byte_serializer_deserializer.h"

namespace linpipe::kbelik::map_values {

class SimpleJson {
 public:
  using Type = Json;
  size_t length(const byte* ptr) const;
  size_t length(const Type& val) const;
  void deserialize(const byte*& ptr, Type& value) const;
  void serialize(const Type& value, vector<byte>& data) const;
  // Can serialize maps:
  // The deserialization is not implemented because not all Jsons can be
  // deserialized into nlohman jsons.
  void serialize(const map<string, string>& value, vector<byte>& data) const;
 private:
  VLI vli;
};

} // namespace linpipe::kbelik::map_values
