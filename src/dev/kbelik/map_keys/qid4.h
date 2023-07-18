#pragma once

#include "common.h"

#include "dev/kbelik/map_keys/map_key.h"

#include "dev/kbelik/id.h"

namespace linpipe::kbelik::map_keys {

class QID4 : public MapKey<ID> {
 public:
  constexpr int length() const { return 4; }
  uint64_t convert_to_uint(const ID& id) const { return id.qid(); }
};

  
} // linpipe::kbelik::map_keys