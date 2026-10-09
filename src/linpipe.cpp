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
    LOG(INFO, "LinPipe is up and running");
  } catch (LinpipeError& error) {
    LOG(FATAL, "An unhandled exception has occurred, terminating: " << error.what());
    return 1;
  }

  return 0;
}
