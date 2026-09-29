// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
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

std::unique_ptr<Operation> Operation::create(const std::string description) {
  std::vector<std::string> descriptions;

  Arguments args;
  args.parse_operations(descriptions, description);

  if (descriptions.size() == 0) {
    throw LinpipeError{"Operation::create: No operation specified in description '", description, "'"};
  }

  if (descriptions.size() > 1) {  // Composite
    return std::make_unique<operations::Composite>(description);
  }
  else {  // simple (leaf) operations
    // The first token is "--name" (checked by parse_operations).
    std::vector<std::string> tokens;
    Arguments::tokenize(tokens, description);
    const std::string name = tokens[0].substr(2);

    if (name == "load") {
      return std::make_unique<operations::Load>(description);
    }
    if (name == "save") {
      return std::make_unique<operations::Save>(description);
    }
    if (name == "tokenize") {
      return std::make_unique<operations::Tokenize>(description);
    }
  }

  // Something went wrong, description was not parsed.
  throw LinpipeError{"Operation::create: Invalid description '", description, "'"};
}

void Operation::reserve_models(PipelineState& state) {
  for (const std::string& model_name : model_names_) {
    state.model_manager->reserve(model_name);
  }
}

} // namespace linpipe
