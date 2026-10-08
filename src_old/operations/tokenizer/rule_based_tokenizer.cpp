// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <limits>

#include "operations/tokenizer/rule_based_tokenizer.h"

namespace linpipe::operations {

namespace {

bool is_space(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

} // namespace

void RuleBasedTokenizer::tokenize(ModelManager* /*model_manager*/, const std::string& text,
                                  std::vector<layers::Token>& tokens, std::vector<layers::IndexSpan>& /*sentences*/) {
  // Token spans are ints, so the text must fit into their range.
  if (text.size() > static_cast<size_t>(std::numeric_limits<int>::max()))
    throw LinpipeError("RuleBasedTokenizer::tokenize: Text too long");

  // Every maximal run of non-whitespace characters is a token, stored
  // as a span into the text without a copy of its text.
  size_t i = 0;
  while (i < text.size()) {
    while (i < text.size() && is_space(text[i])) i++;
    size_t begin = i;
    while (i < text.size() && !is_space(text[i])) i++;
    if (i > begin)
      tokens.emplace_back(layers::IndexSpan(static_cast<int>(begin), static_cast<int>(i)));
  }
}

} // namespace linpipe::operations
