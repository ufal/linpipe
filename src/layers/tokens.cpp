// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/tokens.h"
#include "lib/json.h"
#include "utils/json_utils.h"

namespace linpipe::layers {

void Tokens::from_json(const Json& json) {
  json_assert_object("Text::from_json", json);

  json_get_string("Text::from_json", json, "type", _type);
  json_get_string("Text::from_json", json, "name", _name);

  json_get_string_vector("Text::from_json", json, "tokens", tokens);

  if (json.contains("sentences"))
    json_get_unsigned_vector("Text::from_json", json, "sentences", sentences);
  else
    sentences.clear();
}

Json Tokens::to_json() {
  return {
    {"type", _type},
    {"name", _name},
    {"tokens", tokens},
    {"sentences", sentences},
  };
}

string Tokens::to_html() {
  return string();
}

} // namespace linpipe::layers
