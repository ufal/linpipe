// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/token_layer.h"
#include "lib/json/json.h"
#include "utils/json_utils.h"

namespace linpipe::layers {

TokenLayerSentenceView::TokenLayerSentenceView(const std::vector<IndexSpan>& sentences, size_t ntokens) {
  int end = 0;  // end of the covered tokens
  for (const auto& sentence : sentences) {
    if (sentence.begin < end || sentence.begin > sentence.end || static_cast<size_t>(sentence.end) > ntokens)
      throw LinpipeError("TokenLayerSentenceView: Sentences must be ordered, non-overlapping spans of tokens");
    if (sentence.begin > end)
      spans_.emplace_back(end, sentence.begin);  // tokens between sentences
    if (!sentence.empty())
      spans_.push_back(sentence);
    end = sentence.end;
  }
  if (static_cast<size_t>(end) < ntokens)
    spans_.emplace_back(end, static_cast<int>(ntokens));  // tokens after the last sentence
}

void TokenLayer::from_json(const Json& json) {
  json_assert_object("TokenLayer::from_json", json);

  json_get_string("TokenLayer::from_json", json, "type", type_);
  json_get_string("TokenLayer::from_json", json, "name", name_);

  if (!json.contains("tokens") || !json["tokens"].is_array())
    throw LinpipeError("TokenLayer::from_json: Missing or non-array 'tokens'");
  tokens = json["tokens"].get<std::vector<Token>>();

  std::vector<IndexSpan> new_sentences;
  if (json.contains("sentences")) {
    const Json& sentences_json = json["sentences"];
    if (!sentences_json.is_array())
      throw LinpipeError("TokenLayer::from_json: Non-array 'sentences'");

    long long previous_end = 0;
    for (const Json& sentence : sentences_json) {
      if (!sentence.is_array() || sentence.size() != 2 || !sentence[0].is_number_integer() || !sentence[1].is_number_integer())
        throw LinpipeError("TokenLayer::from_json: Each sentence must be an array of two integers");
      long long begin = sentence[0].get<long long>(), end = sentence[1].get<long long>();
      if (begin < previous_end || begin > end || end > static_cast<long long>(tokens.size()))
        throw LinpipeError("TokenLayer::from_json: Sentences must be ordered, non-overlapping spans of tokens");
      new_sentences.emplace_back(static_cast<int>(begin), static_cast<int>(end));
      previous_end = end;
    }
  }
  sentences = std::move(new_sentences);
}

Json TokenLayer::to_json() {
  Json sentences_json = Json::array();
  for (auto& sentence : sentences)
    sentences_json.push_back(Json::array({sentence.begin, sentence.end}));

  return {
    {"type", type_},
    {"name", name_},
    {"tokens", tokens},
    {"sentences", sentences_json},
  };
}

std::string TokenLayer::to_html() {
  return {};
}

} // namespace linpipe::layers
