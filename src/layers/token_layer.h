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
#include "layers/layer.h"
#include "layers/plain_text.h"
#include "layers/token.h"
#include "views/token_view.h"

namespace linpipe::layers {

class TokenLayerTokenView : public TokenView {
  public:
    explicit TokenLayerTokenView(const std::vector<Token>& tokens, const PlainText* plain_text = nullptr) : tokens_(tokens), plain_text(plain_text) {}

    size_t size() const override { return tokens_.size(); }
    const std::string_view text(size_t i) const override {
      const Token& token = tokens_[i];
      if (!token.text.empty() || !plain_text || token.index_span.empty())
        return token.text;
      std::string_view source = plain_text->text;
      const IndexSpan& span = token.index_span;
      if (span.begin < 0 || static_cast<size_t>(span.end) > source.size())
        throw LinpipeError("TokenLayerTokenView::text: Token span out of bounds");
      return source.substr(span.begin, span.size());
    }

  private:
    const std::vector<Token>& tokens_;
    const PlainText* plain_text = nullptr;
};

class TokenLayer : public Layer {
  public:
    TokenLayer(const std::string& name = {}, const PlainText* plain_text = nullptr) : Layer("token_layer", name.empty() ? "token_layer" : name), plain_text(plain_text) {}

    virtual void from_json(const Json& json) override;
    virtual Json to_json() override;
    virtual std::string to_html() override;

    std::unique_ptr<TokenView> token_view() const { return std::make_unique<TokenLayerTokenView>(tokens, plain_text); }

    std::vector<Token> tokens;
    std::vector<IndexSpan> sentences;

    const PlainText* plain_text = nullptr;
};

} // namespace linpipe::layers
