// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "formats/conll.h"
#include "layers/layer.h"
#include "layers/lemmas.h"
#include "layers/spans.h"
#include "layers/tokens.h"
#include "lib/json.h"
#include "utils/arguments.h"
#include "utils/split.h"

namespace linpipe::formats {

Conll::Conll(const string description) {
  Arguments args;
  args.parse_format(args_, description);

  int i = 1;
  while(true) { // see how many columns requested
    unordered_map<string, string>::const_iterator it = args_.find(to_string(i));
    if (it == args_.end()) break; // no more columns

    // split column description into name and type
    if (size_t index = it->second.find(':'); index != string::npos) {
      names_.emplace_back(it->second, 0, index);
      types_.emplace_back(it->second, index + 1);
    }
    else { // if without ':', assume the description is a type
      names_.emplace_back();
      types_.emplace_back(it->second);
    }

    i++;
  }

  encodings_.resize(names_.size());
  for (size_t i = 0; i < encodings_.size(); i++) {
    unordered_map<string, string>::const_iterator it = args_.find(to_string(i+1) + "_encoding");
    if (it != args_.end()) {
      encodings_[i] = it->second;
    }
  }

}

unique_ptr<Document> Conll::load(istream& input, const string source_path) {
  if (input.eof())
    return nullptr;

  auto document = make_unique<Document>();

  // Create layers.
  for (size_t i = 0; i < types_.size(); i++) {
    document->add_layer(Layer::create(types_[i], names_[i]));
    // Document may have changed the name of the added layer to unique name.
    names_[i] = document->get_layer().name();
  }

  // Read content.
  string line;
  unsigned ntokens = 0;
  while (getline(input, line)) {
    if (line.empty()) { // end of sentence
      for (size_t i = 0; i < types_.size(); i++) {
        if (types_[i] == "tokens") {
          document->get_layer<layers::Tokens>(names_[i]).sentences.push_back(ntokens);
        }
      }
    }
    else { // line with cols
      vector<string_view> cols;
      if (split(line, '\t', cols) != types_.size())
        throw LinpipeError{"Conll::load: Number of columns does not match number of columns in format description on line '", line, "'"};

      for (size_t i = 0; i < types_.size(); i++) {
        if (types_[i] == "lemmas") {
          document->get_layer<layers::Lemmas>(names_[i]).lemmas.emplace_back(cols[i]);
        }
        if (types_[i] == "spans") {
          document->get_layer<layers::Spans>(names_[i]).decode(cols[i],
                                                               ntokens,
                                                               linpipe::layers::SpanEncoding::create(encodings_[i]));
        }
        if (types_[i] == "tokens") {
          document->get_layer<layers::Tokens>(names_[i]).tokens.emplace_back(cols[i]);
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

  // Preprocess the columns that need preprocessing,
  // e.g. encoding named entities.
  vector<vector<string>> encoded_columns(layers.size());
  for (size_t i = 0; i < encoded_columns.size(); i++) {
    if (types_[i] == "spans") { // encode spans
      encoded_columns[i].resize(n);
      document.get_layer<layers::Spans>(names_[i]).encode(encoded_columns[i],
                                                          linpipe::layers::SpanEncoding::create(encodings_[i]));
    }
  }

  // Print the lines.
  size_t sentence_index = 0;
  for (size_t i = 0; i < n; i++) {  // token lines
    bool sentence_printed = false;
    for (size_t j = 0; j < types_.size(); j++) {  // columns
      if (types_[j] == "lemmas") {
        auto& layer = document.get_layer<layers::Lemmas>(names_[j]);
        output << layer.lemmas[i];
      }

      if (types_[j] == "tokens") {
        auto& layer = document.get_layer<layers::Tokens>(names_[j]);

        // Print end of sentence.
        if (layer.sentences[sentence_index] == i && !sentence_printed) {
          output << endl;
          sentence_index += 1;
          sentence_printed = true;
        }

        // Print token.
        output << layer.tokens[i];
      }

      if (types_[j] == "spans") {
        output << encoded_columns[j][i];
      }

      // Print delimiter.
      if (j != n-1) output << "\t";
    }
    output << endl;
  }
}

} // namespace linpipe::formats
