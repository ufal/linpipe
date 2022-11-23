// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"
#include "layers/spans.h"
#include "lib/json.h"
#include "utils/json_utils.h"

namespace linpipe::layers {

SpanEncoding create(const string& type) {
  if (type == "BIO") return SpanEncoding::BIO;
  if (type == "IOB") return SpanEncoding::IOB;
  throw LinpipeError{"SpanEncoding::create: Unexpected SpanEncoding type '", type, "'"};
}

void Spans::from_json(const Json& json) {
  json_assert_object("Spans::from_json", json);

  json_get_string("Spans::from_json", json, "type", _type);
  json_get_string("Spans::from_json", json, "name", _name);

  json_get_string("Spans::from_json", json, "token_layer", token_layer);
  json_get_unsigned_pair_vector("Spans::from_json", json, "spans", spans);

  if (json.contains("tags"))
    json_get_string_vector("Spans::from_json", json, "tags", tags);
  else
    tags.clear();
}

Json Spans::to_json() {
  return {
    {"type", _type},
    {"name", _name},
    {"token_layer", token_layer},
    {"spans", spans},
    {"tags", tags},
  };
}

string Spans::to_html() {
  return string();
}

void Spans::decode(const vector<string>& /*encoded_tags*/, const SpanEncoding encoding) {

  if (encoding.type == SpanEncoding::BIO) {
    // TODO
    return;
  }

  if (encoding.type == SpanEncoding::IOB) {
    // TODO
    return;
  }

  throw LinpipeError{"Spans::decode: Unexpected SpanEncoding with value ", to_string(encoding.type)};
}

void Spans::encode(vector<string>& /*encoded_tags*/, const SpanEncoding /*encoding*/) {
}

} // namespace linpipe::layers
