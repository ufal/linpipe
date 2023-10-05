// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "operations/composite.h"
#include "operations/load.h"
#include "operations/operation.h"
#include "operations/save.h"
#include "operations/tokenize.h"
#include "utils/arguments.h"

namespace linpipe {

unique_ptr<Operation> Operation::create(const string description) {
  vector<string> descriptions;

  Arguments args;
  args.parse_operations(descriptions, description);

  // Not sure if this can actually happen, probably will be already detected
  // and thrown inside parse_operations.
  if (descriptions.size() == 0) {
    throw LinpipeError{"Operation::create: No operation specified in description '", description, "'"};
  }

  if (descriptions.size() > 1) {  // Composite
    return make_unique<operations::Composite>(description);
  }
  else {  // simple (leaf) operations
    if (description.find(" -load", 0) == 0) { // Load
      return make_unique<operations::Load>(description);
    }
    if (description.find(" -save", 0) == 0) { // Save
      return make_unique<operations::Save>(description);
    }
    if (description.find(" -tokenize", 0) == 0) { // Tokenize
      return make_unique<operations::Tokenize>(description);
    }
  }

  // Something went wrong, description was not parsed.
  throw LinpipeError{"Operation::create: Invalid description '", description, "'"};
}

void Operation::reserve_models(PipelineState& state) {
  for (const string& model_name : model_names_) {
    state.model_manager->reserve(model_name);
  }
}

} // namespace linpipe
