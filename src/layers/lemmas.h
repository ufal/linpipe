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
#include "layers/layer.h"

namespace linpipe::layers {

class Lemmas : public Layer {
 public:
  Lemmas(std::string_view name = {}) : Layer("lemmas", name.empty() ? "lemmas" : name) {}

  void from_json(const Json& json) override;
  Json to_json() override;
  std::string to_html() override;

  std::string token_layer;
  std::vector<std::string> lemmas;
};

} // namespace linpipe::layers
