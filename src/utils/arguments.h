// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_map>

#include "common.h"

namespace linpipe {

class Arguments {
 public:
  void parse_operations(std::vector<std::string>& descriptions, const std::string description);
  void parse_arguments(std::unordered_map<std::string, std::string>& args, std::vector<std::string>& kwargs, const std::string description);
  void parse_format(std::unordered_map<std::string, std::string>& args, const std::string description);
 private:
  size_t find_next_operation_(const std::string description, size_t offset);
};

} // namespace linpipe
