// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "models/model_manager.h"
#include "operations/implementation.h"

namespace linpipe::operations {

class Tokenizer : public Implementation {
 public:
  virtual ~Tokenizer() {};
  virtual void tokenize(ModelManager* model_manager, const string& text, vector<string>& tokens) = 0;

 protected:
  Tokenizer(const string type, vector<string> model_names) : Implementation(type, model_names) {};
};

} // namespace linpipe::operations
