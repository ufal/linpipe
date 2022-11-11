// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "formats/conll.h"
#include "layers/layer.h"
#include "layers/tokenized_text.h"
#include "lib/json.h"

namespace linpipe::formats {

Conll::Conll(const string description) : Format("conll") {
  _string_helper.split(_descriptions, description, ":");
  _descriptions.erase(_descriptions.begin()); // remove leading "conll"
}

bool Conll::load(Document& document, istream& input, const string source_path) {
  // Create layers.
  vector<unique_ptr<Layer>> layers;
  for (string description : _descriptions) {
    layers.push_back(Layer::create(description));
  }

  // Read content.
  string line;
  vector<vector<string>> content(_descriptions.size());
  while (getline(input, line)) {
    if (line.empty()) { // end of sentence
      // TODO
    }
    else { // line with cols
      vector<string> cols;
      _string_helper.split(cols, line, "\t");
      if (cols.size() != _descriptions.size()) {
        throw LinpipeError{"Number of columns does not match number of columns in format description on line '", line, "'"};
      }
      for (size_t i = 0; i < _descriptions.size(); i++) {
        if (_descriptions[i] == "tokenized_text") {
          content[i].push_back(cols[i]);
        }
      }
    }
  }

  // Fill layers with JSON created from content and add to document.
  for (size_t i = 0; i < _descriptions.size(); i++) {
    Json json;
    if (_descriptions[i] == "tokenized_text") {
      json["tokens"] = content[i];
    }
    layers[i]->from_json(json);
    document.add_layer(move(layers[i]));
  }

  document.set_source_path(source_path);

  return false;
}

void Conll::save(Document& /*document*/, ostream& /*output*/) {
  // TODO
}

} // namespace linpipe::formats
