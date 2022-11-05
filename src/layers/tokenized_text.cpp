// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/tokenized_text.h"
#include "lib/json.h"

namespace linpipe::layers {

void TokenizedText::from_json(const Json& json) {
  _json_checker.json_has_array("TokenizedText::from_json", json, "tokens");

  tokens = json["tokens"].get<vector<string>>();
}

void TokenizedText::to_json(Json& json) {
  json["tokenized_text"] = tokens;
}

void TokenizedText::to_html(string& /*html*/) {
}

} // namespace linpipe::layers
