// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest.h"
#include "utils/string_helper.h"

namespace linpipe {

TEST_CASE("StringHelper::split") {
  StringHelper string_helper;
  vector<string> parsed;
  vector<string> gold;

  SUBCASE("splits empty string") {
    string_helper.split(parsed, "", ",");
    CHECK(parsed == gold);
  }

  SUBCASE("splits one token") {
    string_helper.split(parsed, "hello", ",");
    gold.push_back("hello");
    CHECK(parsed == gold);
  }

  SUBCASE("splits two tokens") {
    string_helper.split(parsed, "hello world", " ");
    gold.push_back("hello");
    gold.push_back("world");
    CHECK(parsed == gold);
  }

  SUBCASE("splits CoNLL-2003 format description") {
    string_helper.split(parsed, "1=name:type,2=:lemmas,2_default=_,3=:chunks,3_default=_,4=:named_entities,4_encoding=bio", ",");
    gold.push_back("1=name:type");
    gold.push_back("2=:lemmas");
    gold.push_back("2_default=_");
    gold.push_back("3=:chunks");
    gold.push_back("3_default=_");
    gold.push_back("4=:named_entities");
    gold.push_back("4_encoding=bio");
    CHECK(gold == parsed);
  }
}

TEST_CASE("StringHelper::is_number") {
  StringHelper sh;
  CHECK(sh.is_number("1") == true);
}

} // namespace linpipe
