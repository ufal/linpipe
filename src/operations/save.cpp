// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "operations/save.h"
#include "utils/arguments.h"

namespace linpipe::operations {

Save::Save(const string_view description) {
  unordered_map<string, string> args;
  // TODO fill arguments with default values

  Arguments arguments;
  arguments.parse_arguments(args, description);

  _format = Format::create(args["format"]);

  // TODO: process other arguments
}

void Save::execute(Corpus& /*corpus*/, PipelineState& /*state*/) {
  // TODO
}

} // namespace linpipe::operations
