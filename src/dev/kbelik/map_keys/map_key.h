#pragma once

#include "common.h"

namespace linpipe::kbelik::map_keys {

template<class T>
class MapKey {
 public:
  using Type = T;
  constexpr int length() const;
  uint64_t convert_to_uint(const T& id) const;
};
  
} // linpipe::kbelik::map_keys