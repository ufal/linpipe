// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


#include "operations/tokenizer/rule_based_tokenizer.h"


namespace linpipe::operations {

void RuleBasedTokenizer::tokenize(ModelManager* /*model_manager*/, const string& text, vector<string>& tokens) {
  /* Generic rule-based tokenization. Splits on spaces.

    TODO: Split using regexp.

    Receives:
      model_manager: pointer to ModelManager - not used as RuleBasedTokenizer
        does not use any models.
      text: plain text string to tokenize.
      tokens: reference to vector of strings, assumed empty, to fill in with
        tokens.
  */

  size_t start = 0;
  size_t pos = 0;
  while (pos != string::npos && start < text.length()) {
    pos = text.find(" ", start);
    tokens.push_back(text.substr(start, pos-start));
    start = pos+1;
  }
};

} // namespace linpipe::operations
