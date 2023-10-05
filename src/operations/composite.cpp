// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "operations/composite.h"
#include "utils/arguments.h"

namespace linpipe::operations {

Composite::Composite(const string description) {
  vector<string> descriptions;

  Arguments args;
  args.parse_operations(descriptions, description);

  for (string d : descriptions) {
    operations_.push_back(Operation::create(d));
  }
}

void Composite::execute(Corpus& corpus, PipelineState& state) {
  for (auto& operation : operations_)
    operation->execute(corpus, state);
};

} // namespace linpipe::operations
