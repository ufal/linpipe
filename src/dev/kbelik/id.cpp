#include <charconv>

#include "dev/kbelik/id.h"

namespace linpipe::kbelik {

ID::ID() {}

ID::ID(string id) {
  _id = id;
  _is_qid = _is_qid_parsable(id);
}

bool ID::is_qid() const {
  return _is_qid;
}

uint64_t ID::qid() const {
  if (_id[0] != 'Q')
    return false;

  uint64_t qid;
  auto result = from_chars(_id.data() + 1, _id.data() + _id.length(), qid);
  return result.ec == errc() ? qid : 0;
}

string ID::str() const {
  return _id;
}

bool ID::_is_qid_parsable(string& id) {
  if (id[0] != 'Q')
    return false;

  uint64_t qid;
  auto result = from_chars(id.data() + 1, id.data() + id.length(), qid);
  return result.ec == errc();
}

} // linpipe::kbelik
