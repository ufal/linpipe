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
  void parse_operations(std::vector<std::string>& descriptions, std::string_view description);
  void parse_arguments(std::unordered_map<std::string, std::string>& args, std::vector<std::string>& kwargs, std::string_view description);
  void parse_format(std::unordered_map<std::string, std::string>& args, std::string_view description);
  static void tokenize(std::vector<std::string>& tokens, std::string_view description);
  static std::string join(const std::vector<std::string>& tokens);

 private:
  static bool is_operation_(std::string_view token);
};

} // namespace linpipe
