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

namespace linpipe::layers {

class Token {
 public:
  Token() = default;
  explicit Token(std::string text, IndexSpan index_span = {}) : index_span(index_span), text(std::move(text)) {}
  explicit Token(IndexSpan index_span) : index_span(index_span) {}

  IndexSpan index_span;
  std::string text;
};

void to_json(Json& json, const Token& token);
void from_json(const Json& json, Token& token);

} // namespace linpipe::layers
