// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
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
  void parse_operations(vector<string_view>& descriptions, const string_view description);
  void parse_arguments(unordered_map<string, string>& args, vector<string>& kwargs, const string_view description);
 private:
  size_t _find_next_operation(const string_view description, size_t offset);
};

} // namespace linpipe
