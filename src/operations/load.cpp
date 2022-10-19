// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"
#include "operations/load.h"
#include "operations/utils/arguments.h"

namespace linpipe {

Load::Load(const string_view description) {
  unordered_map<string, string> args;

  Arguments arguments;
  arguments.parse(args, description);

  _format = Format::create(args["format"]);

  // TODO: process other arguments
}

void Load::execute(Corpus& /*corpus*/, PipelineState& /*state*/) {
  // TODO
}

} // namespace linpipe
