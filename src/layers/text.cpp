// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/text.h"
#include "lib/json.h"
#include "utils/json_utils.h"

namespace linpipe::layers {

void Text::from_json(const Json& json) {
  json_assert_object("Text::from_json", json);

  json_get_string("Text::from_json", json, "type", type_);
  json_get_string("Text::from_json", json, "name", name_);

  json_get_string("Text::from_json", json, "text", text);
}

Json Text::to_json() {
  return {
    {"text", text},
    {"name", name_},
    {"type", type_},
  };
}

string Text::to_html() {
  return string();
}

} // namespace linpipe::layers
