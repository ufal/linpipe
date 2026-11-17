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
#include "core/layer.h"

namespace linpipe {

class PlainTextLayer : public TypedLayer<PlainTextLayer> {
 public:
  static constexpr std::string_view TYPE = "plain_text";

  explicit PlainTextLayer(std::string_view name = {}) : TypedLayer(name.empty() ? TYPE : name) {}

  Json to_json() const override;
  std::string to_html() const override;

  std::string text;

 protected:
  void from_json(Json& json) override;

  static std::span<const ViewProvider> provided_views();
  friend TypedLayer;
};

} // namespace linpipe
