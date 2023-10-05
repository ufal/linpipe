// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "core/pipeline.h"

namespace linpipe {

Pipeline::Pipeline(const string description) {
  if (description.empty()) {
    throw LinpipeError{"Pipeline::create: Invalid description '", description, "'"};
  }
  operation_ = Operation::create(description);
}

bool Pipeline::execute(Corpus& corpus) {
  // TODO: Initialize state

  operation_->reserve_models(state);
  operation_->execute(corpus, state);

  // TODO: Correctly return true/false
  return true;
}

} // namespace linpipe
