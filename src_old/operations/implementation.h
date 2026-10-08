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

namespace linpipe::operations {

class Implementation {
 // Abstract implementation ancestor.
 // TODO: Maybe implementations should have their own directory.

 public:
  virtual ~Implementation() = default;
  std::vector<std::string>& model_names();
  std::string& type();

 protected:
  Implementation(std::string type, std::vector<std::string> model_names)
      : type_(std::move(type)), model_names_(std::move(model_names)) {}

  std::string type_;
  std::vector<std::string> model_names_;
};

} // namespace linpipe::operations
