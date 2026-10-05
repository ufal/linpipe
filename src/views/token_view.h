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
#include "view.h"

namespace linpipe {

class TokenView : public View {
 public:
  virtual ~TokenView() = default;

  virtual size_t size() const = 0;
  virtual const std::string_view text(size_t i) const = 0;
};

class TokenViewSlice : public TokenView {
 public:
  TokenViewSlice(const TokenView& base, layers::IndexSpan span) : base_(base), span_(span) {
    if (span.begin < 0 || span.begin > span.end || static_cast<size_t>(span.end) > base.size())
      throw LinpipeError("TokenViewSlice::TokenViewSlice: Span out of token range");
  }

  size_t size() const override { return span_.size(); }
  const std::string_view text(size_t i) const override { return base_.text(span_.begin + i); }

 private:
  const TokenView& base_;
  layers::IndexSpan span_;
};

} // namespace linpipe
