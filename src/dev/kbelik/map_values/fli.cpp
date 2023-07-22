// Fixed length integer
#include "common.h"

#include "dev/kbelik/map_values/fli.h"

#include "dev/kbelik/utils/memcpy.h"

namespace linpipe::kbelik::map_values {

size_t FLI::length(const byte* /*ptr*/) const {
  return _length;
}

size_t FLI::length(const FLI::Type& /*value*/) const {
  return _length;
}

void FLI::deserialize(const byte*& ptr, Type& value) {
  value = 0;
  linpipe::kbelik::utils::load_n_byte_uint(ptr, value, _length);
  ptr += _length;
}

void FLI::serialize(const Type& value, vector<byte>& data) {
  size_t old_size = data.size();
  data.resize(old_size + _length);
  linpipe::kbelik::utils::write_n_byte_uint(value, data.data() + old_size, _length);
}

} // namespace linpipe::kbelik::map_keys
