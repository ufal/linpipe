#pragma once

#include "common.h"

#include "dev/kbelik/map_keys/map_key.h"

#include "dev/kbelik/id.h"

namespace linpipe::kbelik::map_keys {

class QIDMapKey8 : public MapKey<ID> {
 public:
  constexpr int length() { return 8; }
  uint64_t convert_to_uint(const ID& id) { return id.qid(); }
};

  
} // linpipe::kbelik::map_keys