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
#include "lib/hash_map.h"

namespace linpipe {

struct OperationDescription {
  std::string operation;
  std::vector<std::string> args;
  HashMap<std::string, std::string> kwargs;

  static std::vector<OperationDescription> parse_descriptions(std::span<const std::string_view> tokens);
  static std::vector<std::string> tokenize(std::string_view description);
};

} // namespace linpipe
