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
#include "core/document.h"
#include "layers/layer.h"

namespace linpipe {

class Format {
 public:
  virtual ~Format() {}

  static unique_ptr<Format> create(const string description);

  virtual unique_ptr<Document> load(istream& input, const string source_path) = 0;
  virtual void save(Document& document, ostream& output) = 0;
  virtual void save_corpus_start(ostream& output);
  virtual void save_corpus_end(ostream& output);
};

} // namespace linpipe
