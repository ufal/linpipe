// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"
#include "core/corpus.h"
#include "core/pipeline.h"

using namespace linpipe;

int main(int argc, char* argv[]) {
  iostream::sync_with_stdio(false);

  // Concatenate commandline arguments into a string description
  string description = "";
  for (int i = 1; i < argc; i++) {
    description.append(" ").append(string(argv[i]));
  }

  // Create and execute pipeline
  try {
    Pipeline pipeline = Pipeline(description);

    Corpus corpus;
    pipeline.execute(corpus);
  }
  catch (LinpipeError& error) {
    LOG(FATAL, "An unhandled exception has occurred, terminating: " << error.what());
  }

  return 0;
}
