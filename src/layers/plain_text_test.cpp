// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/plain_text.h"
#include "lib/doctest.h"
#include "lib/json.h"

namespace linpipe {

TEST_CASE("PlainText::from_json") {
  layers::PlainText plain_text("plain_text");
  CHECK_THROWS_AS(plain_text.from_json(Json(42)), LinpipeError);
  CHECK_THROWS_AS(plain_text.from_json(Json::object()), LinpipeError);
  CHECK_THROWS_AS(plain_text.from_json(Json{{"text", 42}}), LinpipeError);
  CHECK_NOTHROW(plain_text.from_json(Json{{"text", "hello world"}, {"type", "plain_text"}, {"name", "plain_text"}}));
}

} // namespace linpipe
