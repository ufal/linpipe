// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "core/pipeline.h"
#include "lib/doctest.h"

namespace linpipe {

TEST_CASE("Pipeline::create") {
  CHECK_THROWS_AS(Pipeline(""), LinpipeError);
  CHECK_THROWS_WITH_AS(Pipeline(""), "Pipeline::create: Invalid description ''", LinpipeError);
}

} // namespace linpipe
