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

void Text::from_json(const Json& json) {
  _json_checker.json_has_string("Text::from_json", json, "text");
  text = json["text"];

  _json_checker.json_has_string("Text::from_json", json, "type");
  _type = json["type"];

  if (json.contains("name") && json.at("name").is_string()) {
    _name = json["name"];
  }

}

Json Text::to_json() {
  return {
    {"text", text},
    {"name", _name},
    {"type", _type},
  };
}

string Text::to_html() {
  return string();
}

} // namespace linpipe::layers
