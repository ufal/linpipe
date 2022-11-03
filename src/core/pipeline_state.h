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
#include "models/model_manager.h"

namespace linpipe {

enum class Server { local, web };

class PipelineState {
 public:
  PipelineState(Server a_server = Server::local);

  ModelManager* model_manager;

  Server server;

  istream* default_input;
  ostream* default_output;
};

} // namespace linpipe
