// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <filesystem>

#include "common.h"
#include "lib/json/json.h"
#include "models/model.h"

namespace linpipe {

std::unique_ptr<Model> create_model(std::string_view name, const std::filesystem::path& dir, Json& config);

} // namespace linpipe
