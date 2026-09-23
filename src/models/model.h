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

namespace linpipe {

class Model {
 public:
  virtual ~Model() {}
  const std::string& name();

  static std::unique_ptr<Model> create(std::string& name, std::istream& input);

 protected:
  Model(std::string name) : name_(name) {};
  std::string name_;
};

} // namespace linpipe
