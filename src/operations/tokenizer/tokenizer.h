// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "layers/index_span.h"
#include "layers/token.h"
#include "models/model_manager.h"
#include "operations/implementation.h"

namespace linpipe::operations {

class Tokenizer : public Implementation {
 public:
  virtual ~Tokenizer() {};
  // Tokenizes the text, appending to the given (normally empty) vectors:
  // - tokens: tokens anchored in the text by their index spans;
  // - sentences: sentences as half-open spans of indices into tokens,
  //   ordered and non-overlapping. A tokenizer that does no sentence
  //   segmentation may leave it empty; SentenceView then treats all
  //   tokens as a single sentence.
  virtual void tokenize(ModelManager* model_manager, const std::string& text,
                        std::vector<layers::Token>& tokens, std::vector<layers::IndexSpan>& sentences) = 0;

 protected:
  Tokenizer(const std::string type, std::vector<std::string> model_names) : Implementation(type, model_names) {};
};

} // namespace linpipe::operations
