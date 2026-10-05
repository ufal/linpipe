// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "common.h"
#include "models/model.h"

namespace linpipe {

class ModelHub {
  public:
    static constexpr std::string_view default_repo_url = "https://ufal.mff.cuni.cz/~strakova/linpipe_repo/models.json";

    explicit ModelHub(const std::string& dir = {}, const std::string& repo_url = {});
    Model* get_model(const std::string& name);
  private:
    static std::string default_dir();

    const std::string dir;
    const std::string repo_url;
};

} // namespace linpipe
