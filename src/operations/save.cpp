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
  // Create arguments map and fill default values
  unordered_map<string, string> args;
  args["format"] = "lif";
  args["target_path"] = "";

  // Parse arguments
  Arguments arguments;
  arguments.parse_arguments(args, description);

  // Process parsed arguments
  _format = Format::create(args["format"]);
  _target_path = args["target_path"];
}

void Save::execute(Corpus& /*corpus*/, PipelineState& /*state*/) {
  // TODO
}

} // namespace linpipe::operations
