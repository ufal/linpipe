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

namespace linpipe::layers {

void Tokens::from_json(const Json& json) {
  _json_checker.json_has_array("Tokens::from_json", json, "tokens");

  tokens = json["tokens"].get<vector<string>>();
}

void Tokens::to_json(Json& json) {
  json["tokens"] = tokens;
}

void Tokens::to_html(string& /*html*/) {
}

} // namespace linpipe::layers
