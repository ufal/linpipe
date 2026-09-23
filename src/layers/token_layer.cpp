// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/token_layer.h"
#include "lib/json.h"
#include "utils/json_utils.h"

namespace linpipe::layers {

void TokenLayer::from_json(const Json& json) {
  json_assert_object("Text::from_json", json);

  json_get_string("Text::from_json", json, "type", type_);
  json_get_string("Text::from_json", json, "name", name_);

  json_get_string_vector("Text::from_json", json, "tokens", tokens);

  if (json.contains("sentences"))
    json_get_unsigned_vector("Text::from_json", json, "sentences", sentences);
  else
    sentences.clear();
}

Json TokenLayer::to_json() {
  return {
    {"type", type_},
    {"name", name_},
    {"tokens", tokens},
    {"sentences", sentences},
  };
}

std::string TokenLayer::to_html() {
  return std::string();
}

} // namespace linpipe::layers
