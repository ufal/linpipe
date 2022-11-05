// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "operations/model_based_operation.h"

namespace linpipe {

void ModelBasedOperation::reserve_models(PipelineState& state) {
  for (const string& model_name : _model_names) {
    state.model_manager->reserve(model_name);
  }
}

} // namespace linpipe
