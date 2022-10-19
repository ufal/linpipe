// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "core/pipeline.h"

namespace linpipe {

Pipeline::Pipeline(const string_view description) {
  // TODO: Construct the pipeline
  throw LinpipeError{"Pipeline::create: Invalid description '", description, "'"};
}

bool Pipeline::execute(Corpus& corpus) {
  // TODO: Initialize state

  _operation->reserve_models(state);
  _operation->execute(corpus, state);

  // TODO: Correctly return true/false
  return true;
}

} // namespace linpipe
