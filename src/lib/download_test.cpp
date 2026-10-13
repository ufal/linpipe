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
#include "lib/httplib/httplib.h"

namespace linpipe {

TEST_CASE("download rejects malformed URLs") {
  std::ostringstream os;
  CHECK_THROWS_WITH_AS(download_url("example.com/file", os), doctest::Contains("Cannot parse"), LinpipeError);
  CHECK_THROWS_WITH_AS(download_url("://example.com", os), doctest::Contains("Cannot parse"), LinpipeError);
  CHECK_THROWS_WITH_AS(download_url("http:///file", os), doctest::Contains("Missing host"), LinpipeError);
  CHECK(os.view().empty());
}

TEST_CASE("download rejects unsupported protocols") {
  std::ostringstream os;
  CHECK_THROWS_WITH_AS(download_url("ftp://example.com/file", os), doctest::Contains("Unsupported network protocol 'ftp'"), LinpipeError);
  CHECK_THROWS_WITH_AS(download_url("file:///etc/passwd", os), doctest::Contains("Unsupported network protocol"), LinpipeError);
  CHECK_THROWS_WITH_AS(download_url("HTTP://example.com/", os), doctest::Contains("Unsupported network protocol"), LinpipeError);
  CHECK(os.view().empty());
}

} // namespace linpipe
