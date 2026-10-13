// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <sstream>

#include "doctest/doctest.h"
#include "lib/download.h"

namespace linpipe {

TEST_CASE("download rejects malformed URLs") {
  std::ostringstream oss;
  CHECK_THROWS_WITH_AS(download_url("example.com/file", oss), doctest::Contains("Cannot parse"), LinpipeError);
  CHECK_THROWS_WITH_AS(download_url("://example.com", oss), doctest::Contains("Cannot parse"), LinpipeError);
  CHECK_THROWS_WITH_AS(download_url("http:///file", oss), doctest::Contains("Missing host"), LinpipeError);
  CHECK(oss.view().empty());
}

TEST_CASE("download rejects unsupported protocols") {
  std::ostringstream oss;
  CHECK_THROWS_WITH_AS(download_url("ftp://example.com/file", oss), doctest::Contains("Unsupported network protocol 'ftp'"), LinpipeError);
  CHECK_THROWS_WITH_AS(download_url("file:///etc/passwd", oss), doctest::Contains("Unsupported network protocol"), LinpipeError);
  CHECK_THROWS_WITH_AS(download_url("HTTP://example.com/", oss), doctest::Contains("Unsupported network protocol"), LinpipeError);
  CHECK(oss.view().empty());
}

TEST_CASE("download from huggingface.co" * doctest::may_fail(true)) {
  std::ostringstream oss;
  REQUIRE_NOTHROW(download_url("https://huggingface.co/ufal/robeczech-base/raw/main/config.json", oss));
  CHECK(oss.view().starts_with("{"));
}

TEST_CASE("download from GitHub" * doctest::may_fail(true)) {
  std::ostringstream oss;
  REQUIRE_NOTHROW(download_url("https://raw.githubusercontent.com/ufal/linpipe/refs/heads/master/README.md", oss));
  CHECK(!oss.view().empty());
}

} // namespace linpipe
