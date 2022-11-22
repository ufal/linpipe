// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"
#include "layers/spans.h"
#include "lib/json.h"

namespace linpipe::layers {

void Spans::from_json(const Json& json) {
  _json_checker.json_has_string("Spans::from_json", json, "token_layer");
  token_layer = json["token_layer"];

  _json_checker.json_has_array("Spans::from_json", json, "spans");
  spans = json["spans"].get<vector<pair<unsigned int, unsigned int>>>();

  if (json.contains("tags") && json.at("tags").is_array()) {
    tags = json["tags"].get<vector<string>>();
  }
}

Json Spans::to_json() {
  return {
    {"token_layer", token_layer},
    {"spans", spans},
    {"tags", tags},
  };
}

string Spans::to_html() {
  return string();
}

void Spans::decode(const vector<string>& /*encoded_tags*/, const SpanEncoding /*encoding*/) {
}

void Spans::encode(vector<string>& /*encoded_tags*/, const SpanEncoding /*encoding*/) {
}

} // namespace linpipe::layers
