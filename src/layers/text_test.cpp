// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/text.h"
#include "lib/doctest.h"
#include "lib/json.h"

namespace linpipe::layers {

TEST_CASE("Text::from_json") {
  Text text;
  CHECK_THROWS_AS(text.from_json(Json(42)), LinpipeError);
  CHECK_THROWS_AS(text.from_json(Json::object()), LinpipeError);
  CHECK_THROWS_AS(text.from_json(Json{{"text", 42}}), LinpipeError);
  CHECK_NOTHROW(text.from_json(Json{{"text", "raw text"}}));
}

} // namespace linpipe
