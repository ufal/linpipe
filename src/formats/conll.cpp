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

  for (auto& type : types_)
    if (type != "token_layer" && type != "lemmas" && type != "spans")
      throw LinpipeError{"Conll::Conll: Unsupported column type '", type, "' in format description '", description, "'"};

  encodings_.resize(names_.size());
  for (size_t i = 0; i < encodings_.size(); i++) {
    std::unordered_map<std::string, std::string>::const_iterator it = args_.find(std::to_string(i+1) + "_encoding");
    if (it != args_.end()) {
      encodings_[i] = it->second;
    }
    else if (types_[i] == "spans") {
      encodings_[i] = "BIO";  // default span encoding
    }

    if (types_[i] == "spans")
      layers::SpanEncoding::create(encodings_[i]);  // fail early on an unknown encoding
  }
}

std::unique_ptr<Document> Conll::load(std::istream& input, const std::string source_path) {
  if (input.eof())
    return nullptr;

  auto document = std::make_unique<Document>();

  // Create layers, one per column.
  std::vector<Layer*> columns(types_.size());
  for (size_t i = 0; i < types_.size(); i++) {
    columns[i] = &document->add_layer(Layer::create(types_[i], names_[i]));
    // Document may have changed the name of the added layer to unique name.
    names_[i] = columns[i]->name();
  }

  // Record a sentence boundary in all token layers, ignoring empty sentences.
  unsigned ntokens = 0;
  auto end_sentence = [&]() {
    for (size_t i = 0; i < types_.size(); i++)
      if (types_[i] == "token_layer") {
        auto& sentences = static_cast<layers::TokenLayer*>(columns[i])->sentences;
        if (ntokens > (sentences.empty() ? 0 : sentences.back()))
          sentences.push_back(ntokens);
      }
  };

  // Read content.
  std::string line;
  std::vector<std::string_view> cols;
  while (getline(input, line)) {
    if (!line.empty() && line.back() == '\r')
      line.pop_back();

    if (line.empty()) { // end of sentence
      end_sentence();
      continue;
    }

    // line with cols
    if (split(line, '\t', cols) != types_.size())
      throw LinpipeError{"Conll::load: Number of columns does not match number of columns in format description on line '", line, "'"};

    for (size_t i = 0; i < types_.size(); i++) {
      if (types_[i] == "lemmas") {
        static_cast<layers::Lemmas*>(columns[i])->lemmas.emplace_back(cols[i]);
      }
      if (types_[i] == "spans") {
        static_cast<layers::Spans*>(columns[i])->decode(cols[i], ntokens, layers::SpanEncoding::create(encodings_[i]));
      }
      if (types_[i] == "token_layer") {
        static_cast<layers::TokenLayer*>(columns[i])->tokens.emplace_back(std::string(cols[i]));
      }
    }
    ntokens += 1;
  }
  end_sentence(); // the last sentence need not be followed by an empty line

  document->set_source_path(source_path);

  return document;
}

void Conll::save(Document& document, std::ostream& output) {
  // Gather the printed values of all columns.
  std::vector<std::vector<std::string>> columns(types_.size());
  const std::vector<unsigned>* sentences = nullptr;  // taken from the first token layer

  for (size_t j = 0; j < types_.size(); j++) {
    if (types_[j] == "token_layer") {
      auto& layer = document.get_layer<layers::TokenLayer>(names_[j]);
      auto token_view = layer.token_view();
      columns[j].reserve(token_view->size());
      for (size_t i = 0; i < token_view->size(); i++)
        columns[j].emplace_back(token_view->text(i));
      if (!sentences)
        sentences = &layer.sentences;
    }
    if (types_[j] == "lemmas") {
      columns[j] = document.get_layer<layers::Lemmas>(names_[j]).lemmas;
    }
  }

  // Find out the number of token lines; all token and lemma columns must agree.
  size_t n = 0;
  bool n_known = false;
  for (size_t j = 0; j < types_.size(); j++) {
    if (types_[j] == "spans") continue;
    if (!n_known) {
      n = columns[j].size();
      n_known = true;
    }
    else if (columns[j].size() != n) {
      throw LinpipeError{"Conll::save: Column ", std::to_string(j + 1), " has ", std::to_string(columns[j].size()),
                         " values, but ", std::to_string(n), " were expected"};
    }
  }
  if (!n_known)
    throw LinpipeError{"Conll::save: At least one token_layer or lemmas column is required"};

  // Encode the spans, e.g., named entities.
  for (size_t j = 0; j < types_.size(); j++) {
    if (types_[j] == "spans") {
      auto& layer = document.get_layer<layers::Spans>(names_[j]);
      if (layer.tags.size() != layer.spans.size())
        throw LinpipeError{"Conll::save: Spans layer '", layer.name(), "' has different number of spans and tags"};
      for (auto& span : layer.spans)
        if (span.first > span.second || span.second >= n)
          throw LinpipeError{"Conll::save: Spans layer '", layer.name(), "' contains a span out of token range"};
      columns[j].resize(n);
      layer.encode(columns[j], layers::SpanEncoding::create(encodings_[j]));
    }
  }

  // Print the lines, with an empty line after every sentence.
  size_t sentence_index = 0;
  for (size_t i = 0; i < n; i++) {
    if (sentences && i > 0) {
      while (sentence_index < sentences->size() && (*sentences)[sentence_index] < i) sentence_index++;
      if (sentence_index < sentences->size() && (*sentences)[sentence_index] == i)
        output << '\n';
    }

    for (size_t j = 0; j < types_.size(); j++) {
      if (j) output << '\t';
      output << columns[j][i];
    }
    output << '\n';
  }
  if (n) output << '\n';
}

} // namespace linpipe::formats
