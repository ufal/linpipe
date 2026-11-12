// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>
#include <iterator>

#include "models/test_model.h"

namespace linpipe {

TestModel::TestModel(const std::filesystem::path& dir, Json& /*config*/) : AssetModel(dir) {
  auto path = this->dir() / "test_model.txt";
  std::ifstream file(path, std::ios::binary);
  if (!file)
    throw LinpipeError("Cannot open file '{}'", path);
  content_.assign(std::istreambuf_iterator<char>(file), {});

  LOG(INFO, "TestModel loaded from {}: {}", this->dir(), content_);
}

} // namespace linpipe
