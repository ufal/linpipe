// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "core/selector.h"
#include "lib/doctest/doctest.h"

namespace linpipe {

TEST_CASE("Selector::matches") {
  CHECK(Selector{"name", "text"}.matches("text"));
  CHECK_FALSE(Selector{"name", "text"}.matches("other"));
  CHECK_FALSE(Selector{"name", "fr"}.matches("french"));

  CHECK(Selector{"language", "fr"}.matches("fr"));
  CHECK(Selector{"language", "FRA"}.matches("fr"));
  CHECK(Selector{"language", "French"}.matches("fr"));
  CHECK_FALSE(Selector{"language", "French"}.matches("de"));

  CHECK_FALSE(Selector{"name", "text"}.matches("text_extra"));
  CHECK(Selector{"name", "text"}.matches("text_extra", true));
  CHECK(Selector{"name", "text"}.matches("text", true));
  CHECK_FALSE(Selector{"name", "text"}.matches("tex", true));
  CHECK_FALSE(Selector{"name", "text"}.matches("other_text", true));
  CHECK(Selector{"name", ""}.matches("anything", true));

  CHECK(Selector{"language", "fr"}.matches("fr_extra", true));
  CHECK(Selector{"language", "French"}.matches("fr_extra", true));
  CHECK_FALSE(Selector{"language", "French"}.matches("fr_extra"));
  CHECK_FALSE(Selector{"language", "French"}.matches("de_extra", true));

  CHECK(Selector{"name", "udpipe"}.matches("UDPipe"));
  CHECK(Selector{"name", "UDPipe"}.matches("udpipe"));
  CHECK(Selector{"name", "udpipe"}.matches("UDPipe_2", true));
  CHECK_FALSE(Selector{"name", "udpipe"}.matches("UDPipe_2"));
  CHECK_FALSE(Selector{"name", "udpipe"}.matches("UDPip", true));

  CHECK(Selector{"language", "french"}.matches("fr"));
  CHECK(Selector{"language", "fra"}.matches("fr"));
  CHECK(Selector{"language", "French"}.matches("FR"));
  CHECK(Selector{"Language", "French"}.matches("fr"));
  CHECK(Selector{"LANGUAGE", "french"}.matches("FR_extra", true));

  CHECK(Selector{"name", "Čeština"}.matches("čEŠTINA"));
  CHECK(Selector{"name", "čeŠ"}.matches("Čeština", true));
  CHECK_FALSE(Selector{"name", "Čeština"}.matches("Cestina"));
  CHECK(Selector{"name", "k"}.matches("K_extra", true));  // Kelvin sign is 3 bytes in UTF-8
  CHECK(Selector{"name", "K"}.matches("k_extra", true));
}

TEST_CASE("parse_selectors") {
  auto selectors = parse_selectors("plain_text:language=fr");
  REQUIRE(selectors.size() == 2);
  CHECK(selectors[0].key == "");
  CHECK(selectors[0].value == "plain_text");
  CHECK(selectors[1].key == "language");
  CHECK(selectors[1].value == "fr");

  CHECK_THROWS_AS(parse_selectors("=fr"), LinPipeError);
}

} // namespace linpipe
