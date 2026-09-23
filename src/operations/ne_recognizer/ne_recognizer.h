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
#include "models/model_manager.h"
#include "operations/implementation.h"

namespace linpipe::operations {

class NERecognizer : public Implementation {
 public:
  virtual ~NERecognizer() {};
  virtual void recognize(ModelManager* model_manager, const std::vector<std::string>& tokens, std::vector<std::pair<unsigned, unsigned>>& spans) = 0;

 protected:
  NERecognizer(const std::string type, std::vector<std::string> model_names) : Implementation(type, model_names) {};
};

} // namespace linpipe::operations
