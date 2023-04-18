// Abstract class that should be implemented by all values used by persistent
// map.
#pragma once

#include "common.h"

namespace linpipe {
namespace kbelik {
  class MapValue {
    public:
      virtual void deserialize(byte* data) = 0; // data points to the start of data in memory
      virtual vector<byte> serialize() = 0;
      static size_t get_size(byte* data);
  };
}
}
