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
  std::ostringstream o;
  CHECK_THROWS_WITH_AS(download("example.com/file", o), doctest::Contains("Cannot parse"), LinpipeError);
  CHECK_THROWS_WITH_AS(download("://example.com", o), doctest::Contains("Cannot parse"), LinpipeError);
  CHECK_THROWS_WITH_AS(download("http:///file", o), doctest::Contains("Missing host"), LinpipeError);
  CHECK(o.str().empty());
}

TEST_CASE("download rejects unsupported protocols") {
  std::ostringstream o;
  CHECK_THROWS_WITH_AS(download("ftp://example.com/file", o), doctest::Contains("Unsupported network protocol 'ftp'"), LinpipeError);
  CHECK_THROWS_WITH_AS(download("file:///etc/passwd", o), doctest::Contains("Unsupported network protocol"), LinpipeError);
  CHECK_THROWS_WITH_AS(download("HTTP://example.com/", o), doctest::Contains("Unsupported network protocol"), LinpipeError);
  CHECK(o.str().empty());
}

#ifndef CPPHTTPLIB_SSL_ENABLED
TEST_CASE("download rejects https without SSL support") {
  std::ostringstream o;
  CHECK_THROWS(download("https://example.com/", o));
}
#endif

TEST_CASE("download reports connection failures") {
  // Nothing listens on port 1 of the loopback interface, so the connection is refused immediately.
  std::ostringstream o;
  CHECK_THROWS_WITH_AS(download("http://127.0.0.1:1/file", o), doctest::Contains("Cannot download URL"), LinpipeError);
  CHECK(o.str().empty());
}

} // namespace linpipe
