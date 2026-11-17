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
#include "core/view.h"

namespace linpipe {

class PlainTextView : public TypedView<PlainTextView> {
 public:
  static constexpr std::string_view TYPE = "plain_text";

  virtual std::string_view text() const = 0;

 protected:
  explicit PlainTextView(const Layer& layer) : TypedView(layer) {}
};

} // namespace linpipe
