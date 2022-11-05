// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <unordered_map>

#include "lib/doctest.h"
#include "utils/arguments.h"

namespace linpipe {

TEST_CASE("Arguments::parse_operations") {
  Arguments args;
  vector<string> parsed;
  vector<string> gold;

  SUBCASE("parses single operation name") {
    gold.push_back(" -load");
    CHECK_NOTHROW(args.parse_operations(parsed, " -load"));
    CHECK(parsed == gold);
  }

  SUBCASE("parses single operation with argument") {
    gold.push_back(" -load --format=text");
    CHECK_NOTHROW(args.parse_operations(parsed, " -load --format=text"));
    CHECK(parsed == gold);
  }

  SUBCASE("parses two operations") {
    gold.push_back(" -load");
    gold.push_back(" -save");
    CHECK_NOTHROW(args.parse_operations(parsed, " -load -save"));
    CHECK(parsed == gold);
  }

  SUBCASE("parses two operations with arguments") {
    gold.push_back(" -load --format=text");
    gold.push_back(" -save --format=text");
    CHECK_NOTHROW(args.parse_operations(parsed, " -load --format=text -save --format=text"));
    CHECK(parsed == gold);
  }
}

TEST_CASE("Arguments::parse_arguments") {
  Arguments parser;
  unordered_map<string, string> args;
  vector<string> kwargs;
  unordered_map<string, string> gold_args;
  vector<string> gold_kwargs;

  SUBCASE("parses 1 kwarg in 1 operation") {
    gold_kwargs.push_back("test.in");
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " -load test.in"));
    CHECK(args.empty());
    CHECK(gold_kwargs == kwargs);
  }

  SUBCASE("parses 1 arg in 1 operation") {
    gold_args["format"] = "text";
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " -load --format text"));
    CHECK(gold_args["format"] == args["format"]);
    CHECK(kwargs.empty());
  }

  SUBCASE("parses 1 arg and 1 kwargs in 1 operation") {
    gold_args["format"] = "lif";
    gold_kwargs.push_back("dummy");
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " -load --format lif dummy"));
    CHECK(gold_args["format"] == args["format"]);
    CHECK(gold_kwargs == kwargs);
  }
}

} // namespace linpipe
