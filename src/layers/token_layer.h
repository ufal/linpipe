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
#include "views/token_view.h"

namespace linpipe::layers {

class TokenLayerTokenView : public TokenView {
  public:
    explicit TokenLayerTokenView(const std::vector<std::string>& tokens) : tokens_(tokens) {}

    size_t size() const override { return tokens_.size(); }
    const std::string& text(size_t i) const override { return tokens_[i]; }

  private:
    const std::vector<std::string>& tokens_;
};

class TokenLayer : public Layer {
  public:
    TokenLayer(const std::string name={}) : Layer("token_layer", name.empty() ? "token_layer" : name) {};

    virtual void from_json(const Json& json) override;
    virtual Json to_json() override;
    virtual std::string to_html() override;

    std::unique_ptr<TokenView> token_view() const { return std::make_unique<TokenLayerTokenView>(tokens); }

    std::vector<std::string> tokens;
    std::vector<unsigned> sentences;
  private:
};

} // namespace linpipe::layers
