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
#include "layers/tokens.h"
#include "lib/json.h"

namespace linpipe::formats {

Conll::Conll(const string description) : Format("conll") {
  _string_helper.split(_descriptions, description, ":");
  _descriptions.erase(_descriptions.begin()); // remove leading "conll"
}

bool Conll::load(Document& document, istream& input, const string source_path) {
  if (input.eof())
    return false;

  // Create layers.
  vector<unique_ptr<Layer>> layers;
  for (string description : _descriptions) {
    layers.push_back(Layer::create(description));
  }

  // Read content.
  string line;
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
        if (_descriptions[i] == "tokens") {
          dynamic_cast<layers::Tokens*>(layers[i].get())->tokens.push_back(cols[i]);
        }
      }
    }
  }

  for (size_t i = 0; i < _descriptions.size(); i++) {
    document.add_layer(move(layers[i]));
  }

  document.set_source_path(source_path);

  return true;
}

void Conll::save(Document& /*document*/, ostream& /*output*/) {
  // TODO
}

} // namespace linpipe::formats
