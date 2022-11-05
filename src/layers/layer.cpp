// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/layer.h"
#include "lib/json.h"

namespace linpipe {

const string& Layer::name() {
  return _name;
}

void Layer::set_name(const string_view name) {
  _name = name;
}

} // namespace linpipe
