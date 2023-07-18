#pragma once

#include <cstring>

#include "common.h"

namespace linpipe::kbelik::utils {

void load_n_byte_uint(const byte* ptr, uint64_t& value, size_t size_of_value_in_bytes) {
  memcpy(&value, ptr, size_of_value_in_bytes);
}

void write_n_byte_uint(uint64_t value, byte* ptr, size_t size_of_value_in_bytes) {
  memcpy(ptr, &value, size_of_value_in_bytes);
}

} // linpipe::kbelik::utils
