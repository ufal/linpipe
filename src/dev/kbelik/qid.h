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

} // linpipe::kbelik