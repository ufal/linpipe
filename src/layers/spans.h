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

class SpanEncoding {
 public:
  enum {
    BIO = 0,
    IOB = 1,
  };
  int type;

  SpanEncoding(int type) : type(type) {}
  static SpanEncoding create(const std::string& type);

  static inline std::vector<std::string> types = {"BIO", "IOB"};
};

class Spans : public Layer {
 public:
  Spans(std::string_view name = {}) : Layer("spans", name.empty() ? "spans" : name) {}

  void from_json(const Json& json) override;
  Json to_json() override;
  std::string to_html() override;

  void decode(std::string_view encoded_tag, unsigned index, SpanEncoding encoding);
  void encode(std::vector<std::string>& encoded_tags, SpanEncoding encoding);

  std::string token_layer;
  std::vector<std::pair<unsigned, unsigned>> spans;
  std::vector<std::string> tags;
};

} // namespace linpipe::layers
