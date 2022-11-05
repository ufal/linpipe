// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/text.h"
#include "lib/json.h"

namespace linpipe::layers {

void TokenizedText::from_json(const Json& json) {
  _json_checker.json_has_array("PlainText::from_json", json, "tokenized_text");
  text = json["tokenized_text"];
}

void Text::to_json(Json& json) {
  json = Json::object();
  json["tokenized_text"] = tokens;
}

void Text::to_html(string& /*html*/) {
}

} // namespace linpipe::layers
