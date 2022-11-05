// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "formats/lif.h"
#include "lib/json.h"

namespace linpipe::formats {

bool Lif::load(Document& document, istream& input, const string source_path) {
  //Json json(input);
  // TODO process JSON and create layers
  document.set_source_path(source_path);
  return !input.eof();
}

void Lif::save(Document& document, ostream& output) {
  Json layers_json = Json::array();

  const vector<unique_ptr<Layer>>& layers = document.layers();
  for (size_t i = 0; i < layers.size(); i++) {
    Json layer_json = Json();
    layers[i]->to_json(layer_json);
    layers_json.push_back(layer_json);
  }

  Json json = Json::object();
  json["layers"] = layers_json;

  output << string(json.dump());
}

} // namespace linpipe::formats
