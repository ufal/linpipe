#pragma once

#include "common.h"

namespace linpipe::kbelik::map_keys {

template<class T>
class MapKey {
 public:
  constexpr int length();
  uint64_t convert_to_uint(const T& id);
};
  
} // linpipe::kbelik::map_keys