// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "format.h"
#include "text.h"

namespace linpipe {

unique_ptr<Format> Format::create(const string_view description) {
  if (description == "text") return make_unique<formats::Text>();

  return nullptr;
}

} // namespace linpipe
