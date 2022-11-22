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

    // split column description into name and type
    if (it->second.find(":") != string::npos) {
      vector<string> tokens;
      _string_helper.split(tokens, it->second, ":");
      if (tokens.size() != 2) {
        throw LinpipeError{"Expected name:format in description of ", to_string(i), "-th column of --format description '", description, "'"};
      }
      _names.push_back(tokens[0]);
      _types.push_back(tokens[1]);
    }
    else { // if without ':', assume the description is a type
      _names.push_back({});
      _types.push_back(it->second);
    }

    i++;
  }
}

unique_ptr<Document> Conll::load(istream& input, const string source_path) {
  if (input.eof())
    return nullptr;

  auto document = make_unique<Document>();

  // Create layers.
  for (size_t i = 0; i < _types.size(); i++) {
    document->add_layer(Layer::create(_types[i], _names[i]));
    // Document may have changed the name of the added layer to unique name.
    _names[i] = document->get_layer().name();
  }

  // Read content.
  string line;
  size_t ntokens = 0;
  while (getline(input, line)) {
    if (line.empty()) { // end of sentence
      for (size_t i = 0; i < _types.size(); i++) {
        if (_types[i] == "tokens") {
          document->get_layer<layers::Tokens>(_names[i]).sentences.push_back(ntokens);
        }
      }
    }
    else { // line with cols
      vector<string> cols;
      _string_helper.split(cols, line, "\t");
      if (cols.size() != _types.size()) {
        throw LinpipeError{"Number of columns does not match number of columns in format description on line '", line, "'"};
      }
      for (size_t i = 0; i < _types.size(); i++) {
        if (_types[i] == "tokens") {
          document->get_layer<layers::Tokens>(_names[i]).tokens.push_back(cols[i]);
        }
      }
      ntokens += 1;
    }
  }

  document->set_source_path(source_path);

  return document;
}

void Conll::save(Document& document, ostream& output) {
  // Peek in first layer to find out the number of tokens.
  size_t n = 0; // number of token lines
  const vector<unique_ptr<Layer>>& layers = document.layers();
  if (layers.size()) {
    if (layers[0]->type() == "tokens") {
      n = dynamic_cast<layers::Tokens*>(layers[0].get())->tokens.size();
    }
  }

  // Print the lines.
  size_t sentence_index = 0;
  for (size_t i = 0; i < n; i++) {  // token lines
    bool sentence_printed = false;
    for (size_t j = 0; j < _types.size(); j++) {  // columns
      if (_types[j] == "tokens") {
        auto& layer = document.get_layer<layers::Tokens>(_names[j]);

        // Print end of sentence.
        if (layer.sentences[sentence_index] == i && !sentence_printed) {
          output << endl;
          sentence_index += 1;
          sentence_printed = true;
        }

        // Print token.
        output << layer.tokens[i];

        // Print delimiter.
        if (j != n-1) output << "\t";
      }
    }
    output << endl;
  }
}

} // namespace linpipe::formats
