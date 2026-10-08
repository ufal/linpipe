// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"

using namespace linpipe;

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  std::iostream::sync_with_stdio(false);

  try {
    std::cout << "LinPipe is up and running" << std::endl;
  } catch (LinpipeError& error) {
    std::cerr << "An unhandled exception has occurred, terminating: " << error.what() << std::endl;
  }

  return 0;
}
