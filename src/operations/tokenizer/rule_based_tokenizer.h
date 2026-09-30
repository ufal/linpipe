// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "operations/tokenizer/tokenizer.h"

namespace linpipe::operations {

// Rule-based tokenizer. Splits the text on ASCII whitespace; performs no
// sentence segmentation.
class RuleBasedTokenizer : public Tokenizer {
 public:
  RuleBasedTokenizer(std::vector<std::string> /*model_names*/) : Tokenizer("rule_based", {}) {}

  void tokenize(ModelManager* model_manager, const std::string& text,
                std::vector<layers::Token>& tokens, std::vector<layers::IndexSpan>& sentences) override;
};

} // namespace linpipe::operations
