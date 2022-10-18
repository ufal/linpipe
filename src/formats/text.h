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
#include "formats/format.h"

namespace linpipe::formats {

class Text : public Format {
 public:
  virtual bool load(Document& document, istream& input, const string_view source_path) override;
  virtual void save(Document& document, ostream& output) override;
  Text() : Format("text") {};
};

} // namespace linpipe
