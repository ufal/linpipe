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
#include "utils/arguments.h"

namespace linpipe::formats {

Conll::Conll(const string description) {
  Arguments args;
  args.parse_format(_args, description);

  int i = 1;
  while(true) { // see how many columns requested
    unordered_map<string, string>::const_iterator it = _args.find(to_string(i));
    if (it == _args.end()) break; // no more columns
    _types.push_back(it->second);
    i++;
  }
}

bool Conll::load(Document& document, istream& input, const string source_path) {
  if (input.eof())
    return false;

  // Create layers.
  vector<unique_ptr<Layer>> layers;
  for (string type : _types) {
    layers.push_back(Layer::create(type));
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
      if (cols.size() != _types.size()) {
        throw LinpipeError{"Number of columns does not match number of columns in format description on line '", line, "'"};
      }
      for (size_t i = 0; i < _types.size(); i++) {
        if (_types[i] == "tokens") {
          dynamic_cast<layers::Tokens*>(layers[i].get())->tokens.push_back(cols[i]);
        }
      }
    }
  }

  for (size_t i = 0; i < _types.size(); i++) {
    document.add_layer(move(layers[i]));
  }

  document.set_source_path(source_path);

  return true;
}

void Conll::save(Document& document, ostream& output) {
  // Peek in first layer to find out the number of tokens.
  size_t n = 0; // number of token lines
  const vector<unique_ptr<Layer>>& layers = document.layers();
  if (layers.size()) {
    if (layers[0]->name() == "tokens") {
      n = dynamic_cast<layers::Tokens*>(layers[0].get())->tokens.size();
    }
  }

  // Print the lines
  for (size_t i = 0; i < n; i++) {  // token lines
    for (size_t j = 0; j < _types.size(); j++) {  // columns
      if (_types[j] == "tokens") {
        auto& layer = document.get_layer<layers::Tokens>(_types[j]);
        output << layer.tokens[i];
        if (j != n-1) output << "\t";
      }
    }
    output << endl;
  }
}

} // namespace linpipe::formats
