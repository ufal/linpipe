// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <functional>

#include "common.h"

// A hash function for any string-like type convertible to std::string_view.
struct string_hash {
  using is_transparent = void;

  std::size_t operator()(std::string_view text) const noexcept {
    return std::hash<std::string_view>{}(text);
  }
};
