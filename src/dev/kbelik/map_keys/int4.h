// For testing purposes.
#pragma once

#include "common.h"

#include "dev/kbelik/map_keys/map_key.h"

#include "dev/kbelik/id.h"

namespace linpipe::kbelik::map_keys {

class IntMapKey4 : public MapKey<int> {
 public:
  constexpr int length() const { return 4; }
  uint64_t convert_to_uint(const int& id) const { return id; }
};

  
} // linpipe::kbelik::map_keys