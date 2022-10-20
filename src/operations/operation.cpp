// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "operations/composite.h"
#include "operations/load.h"
#include "operations/operation.h"

namespace linpipe {

unique_ptr<Operation> Operation::create(const string_view description) {
  // TODO:
  // We want something along these lines, but with a third-party regex lib.

  //regex rx("-[a-z]+");

  //auto words_begin = sregex_iterator(description.begin(), description.end(), rx);
  //auto words_end = sregex_iterator();

  //int n = distance(words_begin, words_end);

  //if (n > 1) { // Composite
  //  return make_unique<Composite>(description);
  //}
  //else { // Simple operations
  //  if (description.rfind("-load", 0) == 0) { // Load
  //    return make_unique<Load>(description);
  //  }
  //}

  if (description.rfind(" -load", 0) == 0) { // Load
    return make_unique<operations::Load>(description);
  }

  // Something went wrong, description was not parsed.
  throw LinpipeError{"Operation::create: Invalid description '", description, "'"};
}

void Operation::reserve_models(PipelineState& /*state*/) {
}

} // namespace linpipe
