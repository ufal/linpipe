// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "formats/format.h"
#include "operations/operation.h"

namespace linpipe::operations {

class Load : public Operation {
 public:
  Load(const string description);
  virtual void execute(Corpus& corpus, PipelineState& state) override;

 private:
  void read_from_handle_(Corpus& corpus, istream& input_file, const string source_path);

  unique_ptr<Format> format_;
  vector<string> source_paths_;
};

} // namespace linpipe::operations
