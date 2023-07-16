#pragma once

#include <cstring>

#include "common.h"

namespace linpipe::kbelik::utils {

void load_n_byte_int(const byte* ptr, int64_t& value, int size_of_value_in_bytes) {
  memcpy(&value, ptr, size_of_value_in_bytes);
}

void write_n_byte_int(int64_t value, byte* ptr, int size_of_value_in_bytes) {
  memcpy(ptr, &value, size_of_value_in_bytes);
}

} // linpipe::kbelik::utils
