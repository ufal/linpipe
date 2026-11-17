// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/plain_text_layer.h"
#include "utils/json_utils.h"

namespace linpipe {

void PlainTextLayer::from_json(Json& json) {
  Layer::from_json(json);
  text = json_extract<std::string>(json, "text");
}

Json PlainTextLayer::to_json() const {
  Json json = Layer::to_json();
  json["text"] = text;
  return json;
}

std::string PlainTextLayer::to_html() const {
  return {};
}

} // namespace linpipe
