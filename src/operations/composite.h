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
#include "operations/operation.h"

namespace linpipe {

class Composite : public Operation {
 public:
  Composite(const string_view description);
  virtual void execute(Corpus& corpus, PipelineState& state) override;
};

} // namespace linpipe
