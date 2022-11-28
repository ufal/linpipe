// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "core/corpus.h"
#include "core/pipeline_state.h"

namespace linpipe {

class Operation {
 public:
  virtual ~Operation() {}

  static unique_ptr<Operation> create(const string description);

  virtual void execute(Corpus& corpus, PipelineState& state) = 0;

  virtual void reserve_models(PipelineState& state);

 protected:
  Operation() {};

  vector<string> _model_names;
};

} // namespace linpipe
