#pragma once

#include "common.h"

namespace linpipe::kbelik::utils {

void load_n_byte_uint(const byte* ptr, uint64_t& value, size_t size_of_value_in_bytes);
void write_n_byte_uint(uint64_t value, byte* ptr, size_t size_of_value_in_bytes);

} // linpipe::kbelik::utils
