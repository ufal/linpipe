// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "models/model.h"

namespace linpipe {

class ModelHub {
  /* Provides LinPipe models by name, downloading them from a remote repository
     and keeping them in a local cache directory.
  */

  public:
    static constexpr std::string_view default_repo_url = "https://ufal.mff.cuni.cz/~strakova/linpipe_repo/models.json";
    static constexpr std::string_view repo_json_name = "models.json";

    explicit ModelHub(const std::string& dir = {}, const std::string& repo_url = {});
    ~ModelHub();

    Model* get_model(const std::string& name);

  private:
    static std::string default_dir();
    void ensure_local_repo();

    const std::string dir;       // Local cache directory (UTF-8).
    const std::string repo_url;  // URL of the repository overview JSON.
    std::unique_ptr<Json> repo;  // Loaded overview, nullptr until ensure_local_repo() succeeds.
};

} // namespace linpipe
