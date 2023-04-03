// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <filesystem>
#include "common.h"
//#include "utils/search.cpp"
//#include "utils/benchmark_search.cpp"
#include "utils/mmap.cpp"
//#include "dev/kbelik/persistent_map.cpp"

using namespace linpipe;

int main(int /*argc*/, char* /*argv*/[]) {
  //iostream::sync_with_stdio(false);

  /*
  benchmark_small();
  benchmark_big();

  benchmark_small_missing();
  benchmark_big_missing();

  load_qids();
  benchmark_qids();
  benchmark_qids_missing();

  */

  auto path = filesystem::path("utils/qids.txt");
  MMAP index(path);
  //index.read(0, 100);
  //cout << "int read\n";
  //index.read_int_like(0, 100);
  return 0;
}
