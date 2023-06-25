#pragma once

#include "common.h"

namespace linpipe::kbelik {

enum class NamedEntity {
  PER, 
  ORG, 
  LOC, 
  EVENT, 
  BRAND,
  WORK_OF_ART, 
  MANUFACTURED
};

class NamedEntityConverter {
 public:
  static inline vector<bool> ne_to_bools(const vector<NamedEntity>& ne);
  static inline vector<NamedEntity> bools_to_ne(const vector<bool>& vb);
};

vector<bool> NamedEntityConverter::ne_to_bools(const vector<NamedEntity>& ne) {
  int max_n = 0;
  for (auto &entity : ne)
    max_n = max(static_cast<int>(entity), max_n);
  vector<bool> out(max_n + 1, false);
  for (auto &entity : ne)
    out[static_cast<int>(entity)] = true;
  return out;

}
vector<NamedEntity> NamedEntityConverter::bools_to_ne(const vector<bool>& vb) {
  vector<NamedEntity> out;
  for (size_t i = 0; i < vb.size(); i++)
    if (vb[i])
      out.push_back(static_cast<NamedEntity>(i));
  return out;
}

} // linpipe::kbelik