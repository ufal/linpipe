// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/plain_text.h"
#include "lib/json.h"
#include "utils/json_utils.h"

namespace linpipe::layers {

void PlainText::from_json(const Json& json) {
  json_assert_object("PlainText::from_json", json);

  json_get_string("PlainText::from_json", json, "type", type_);
  json_get_string("PlainText::from_json", json, "name", name_);

  json_get_string("PlainText::from_json", json, "text", text);
}

Json PlainText::to_json() {
  return {
    {"text", text},
    {"name", name_},
    {"type", type_},
  };
}

string PlainText::to_html() {
  return string();
}

} // namespace linpipe::layers
