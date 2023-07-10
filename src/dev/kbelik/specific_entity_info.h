#pragma once

#include "common.h"

#include <unordered_set>

#include "dev/kbelik/typed_value.h"
#include "dev/kbelik/named_entity.h"
#include "dev/kbelik/utils.h"

namespace linpipe::kbelik {

class SpecificEntityInfo {
 public:
  string label;
  unordered_set<string> aliases;
  string description;
  string text;

  SpecificEntityInfo();
  SpecificEntityInfo(Json& js);
};

inline bool operator==(const SpecificEntityInfo& a, const SpecificEntityInfo& b) {
  return a.label == b.label && \
  a.description == b.description && \
  a.text == b.text && \
  a.aliases == b.aliases;
}

inline bool operator!=(const SpecificEntityInfo& a, const SpecificEntityInfo& b) {
  return !(a == b);
}

} // namespace linpipe::kbelik