// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <memory>

#include "lib/doctest.h"
#include "operations/composite.h"
#include "operations/load.h"
#include "operations/operation.h"
#include "operations/save.h"

namespace linpipe {

TEST_CASE("Operation::create") {
  std::unique_ptr<Operation> op;

  SUBCASE("creates load operation") {
    CHECK_NOTHROW(op = Operation::create(" --load -format text test.in"));
    CHECK(dynamic_cast<operations::Load*>(op.get()) != nullptr);
  }

  SUBCASE("creates save operation") {
    CHECK_NOTHROW(op = Operation::create(" --save -format lif test.out"));
    CHECK(dynamic_cast<operations::Save*>(op.get()) != nullptr);
  }

  SUBCASE("creates composite operation from two operations") {
    CHECK_NOTHROW(op = Operation::create(" --load -format text test.in --save -format lif test.out"));
    CHECK(dynamic_cast<operations::Composite*>(op.get()) != nullptr);
  }

  SUBCASE("throws on unknown operation") {
    CHECK_THROWS_AS(Operation::create(" --unknown test.in"), LinpipeError);
  }

  SUBCASE("throws on operation name that only starts with a known name") {
    CHECK_THROWS_AS(Operation::create(" --loader test.in"), LinpipeError);
  }

  SUBCASE("throws on single-hyphen operation") {
    CHECK_THROWS_AS(Operation::create(" -load test.in"), LinpipeError);
  }
}

} // namespace linpipe
