// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"
#include "lib/doctest.h"
#include "lib/httplib.h"

namespace linpipe {

TEST_CASE("httplib::Client http" * doctest::may_fail(true)) {
  httplib::Client req("http://huggingface.co");
  auto res = req.Get("/");
  REQUIRE_MESSAGE(res, "http request failed: ", httplib::to_string(res.error()));
  CHECK(res->status == 301);
}

TEST_CASE("httplib::Client https" * doctest::may_fail(true)) {
  httplib::Client req("https://huggingface.co");
  auto res = req.Get("/");
  REQUIRE_MESSAGE(res, "https request failed: ", httplib::to_string(res.error()));
  CHECK(res->status == 200);
}

} // namespace linpipe
