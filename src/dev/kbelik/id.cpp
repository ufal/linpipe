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

int64_t ID::qid() const {
  if (!is_qid())
    return -1;
  return stoll(_id.substr(1));
}

string ID::str() const {
  return _id;
}

bool ID::_is_qid_parsable(string& id) {
  if (id[0] != 'Q')
    return false;

  try {
    // this is really ugly but I don't know who to do it differently.
    auto tmp = stoll(id.substr(1));
    tmp++;
  } catch (const invalid_argument& /*e*/) {
    return false;
  } catch (const out_of_range& /*e*/) {
    return false;
  }
  return true;
}

} // linpipe::kbelik