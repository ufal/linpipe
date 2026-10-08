// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/token.h"
#include "lib/json/json.h"
#include "utils/json_utils.h"

namespace linpipe::layers {

void to_json(Json& json, const Token& token) {
  if (token.index_span.empty()) {
    json = token.text;
    return;
  }

  json = {{"span", {token.index_span.begin, token.index_span.end}}};
  if (!token.text.empty())
    json["text"] = token.text;
}

void from_json(const Json& json, Token& token) {
  token = Token();

  if (json.is_string()) {
    token.text = json.get<std::string>();
    return;
  }

  json_assert_object("Token::from_json", json);

  if (json.contains("text"))
    json_get_string("Token::from_json", json, "text", token.text);

  if (json.contains("span")) {
    const Json& span = json.at("span");
    if (!span.is_array() || span.size() != 2 || !span[0].is_number_integer() || !span[1].is_number_integer())
      throw LinpipeError("Token::from_json: Token span must be an array of two integers");

    int begin = span[0].get<int>(), end = span[1].get<int>();
    if (begin < 0 || end < begin)
      throw LinpipeError("Token::from_json: Token span must satisfy 0 <= begin <= end");

    token.index_span = IndexSpan(begin, end);
  }

  if (token.text.empty() && token.index_span.empty())
    throw LinpipeError("Token::from_json: Token object has neither text nor span");
}

} // namespace linpipe::layers
