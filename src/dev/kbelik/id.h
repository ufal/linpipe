#pragma once

#include "common.h"

namespace linpipe::kbelik {

class ID {
 public:
  ID(string id);

  bool is_qid() const;
  int64_t qid() const; // Maybe can return -1 on a not-a-qid?
  string str() const;

 private:
  string _id;
  bool _is_qid;
  bool _is_qid_parsable(string& id);
};

inline bool operator==(const ID& a, const ID& b) {
  return a.str() == b.str();
}

inline bool operator!=(const ID& a, const ID& b) {
  return !(a == b);
}

} // linpipe::kbelik