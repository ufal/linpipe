// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_map>

#include "common.h"
#include "formats/format.h"

namespace linpipe::formats {

class Conll : public Format {
 public:
  Conll(std::string_view description);

  std::unique_ptr<Document> load(std::istream& input, std::string_view source_path) override;
  void save(Document& document, std::ostream& output) override;

 private:
  std::unordered_map<std::string, std::string> args_;
  std::vector<std::string> types_;  // layer types corresponding to columns
  std::vector<std::string> names_;  // layer names corresponding to columns
  std::vector<std::string> encodings_;  // span encodings
};

} // namespace linpipe::formats
