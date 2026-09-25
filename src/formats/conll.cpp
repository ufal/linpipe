// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "formats/conll.h"
#include "layers/layer.h"
#include "layers/lemmas.h"
#include "layers/spans.h"
#include "layers/token_layer.h"
#include "lib/json.h"
#include "utils/arguments.h"
#include "utils/split.h"

namespace linpipe::formats {

Conll::Conll(const std::string description) {
  Arguments args;
  args.parse_format(args_, description);

  int i = 1;
  while(true) { // see how many columns requested
    std::unordered_map<std::string, std::string>::const_iterator it = args_.find(std::to_string(i));
    if (it == args_.end()) break; // no more columns

    // split column description into name and type
    if (size_t index = it->second.find(':'); index != std::string::npos) {
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
    std::unordered_map<std::string, std::string>::const_iterator it = args_.find(std::to_string(i+1) + "_encoding");
    if (it != args_.end()) {
      encodings_[i] = it->second;
    }
  }

}

std::unique_ptr<Document> Conll::load(std::istream& input, const std::string source_path) {
  if (input.eof())
    return nullptr;

  auto document = std::make_unique<Document>();

  // Create layers.
  for (size_t i = 0; i < types_.size(); i++) {
    document->add_layer(Layer::create(types_[i], names_[i]));
    // Document may have changed the name of the added layer to unique name.
    names_[i] = document->get_layer().name();
  }

  // Read content.
  std::string line;
  unsigned ntokens = 0;
  while (getline(input, line)) {
    if (line.empty()) { // end of sentence
      for (size_t i = 0; i < types_.size(); i++) {
        if (types_[i] == "token_layer") {
          document->get_layer<layers::TokenLayer>(names_[i]).sentences.push_back(ntokens);
        }
      }
    }
    else { // line with cols
      std::vector<std::string_view> cols;
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
        if (types_[i] == "token_layer") {
          document->get_layer<layers::TokenLayer>(names_[i]).tokens.emplace_back(cols[i]);
        }
      }
      ntokens += 1;
    }
  }

  document->set_source_path(source_path);

  return document;
}

void Conll::save(Document& document, std::ostream& output) {
  // Peek in first layer to find out the number of tokens.
  size_t n = 0; // number of token lines
  const std::vector<std::unique_ptr<Layer>>& layers = document.layers();
  if (layers.size()) {
    if (layers[0]->type() == "token_layer") {
      n = dynamic_cast<layers::TokenLayer*>(layers[0].get())->token_view()->size();
    }
  }

  // Preprocess the columns that need preprocessing,
  // e.g. encoding named entities.
  std::vector<std::vector<std::string>> encoded_columns(layers.size());
  for (size_t i = 0; i < encoded_columns.size(); i++) {
    if (types_[i] == "spans") { // encode spans
      encoded_columns[i].resize(n);
      document.get_layer<layers::Spans>(names_[i]).encode(encoded_columns[i],
                                                          linpipe::layers::SpanEncoding::create(encodings_[i]));
    }
  }

  // Print the lines.
  std::vector<std::unique_ptr<TokenView>> token_views(types_.size());
  for (size_t i = 0; i < types_.size(); i++) {
    if (types_[i] == "token_layer") {
      token_views[i] = document.get_layer<layers::TokenLayer>(names_[i]).token_view();
    }
  }

  size_t sentence_index = 0;
  for (size_t i = 0; i < n; i++) {  // token lines
    bool sentence_printed = false;
    for (size_t j = 0; j < types_.size(); j++) {  // columns
      if (types_[j] == "lemmas") {
        auto& layer = document.get_layer<layers::Lemmas>(names_[j]);
        output << layer.lemmas[i];
      }

      if (types_[j] == "token_layer") {
        auto& layer = document.get_layer<layers::TokenLayer>(names_[j]);

        // Print end of sentence.
        if (layer.sentences[sentence_index] == i && !sentence_printed) {
          output << std::endl;
          sentence_index += 1;
          sentence_printed = true;
        }

        // Print token.
        output << token_views[i]->text(i);
      }

      if (types_[j] == "spans") {
        output << encoded_columns[j][i];
      }

      // Print delimiter.
      if (j != types_.size() - 1) output << "\t";
    }
    output << std::endl;
  }
}

} // namespace linpipe::formats
