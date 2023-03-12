// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"
//#include "utils/search.cpp"
#include "utils/benchmark_search.cpp"

using namespace linpipe;

int main(int /*argc*/, char* /*argv*/[]) {
  //iostream::sync_with_stdio(false);

  benchmark_small();
  benchmark_big();

  benchmark_small_missing();
  benchmark_big_missing();

  load_qids();
  benchmark_qids();
  benchmark_qids_missing();

  return 0;
}
