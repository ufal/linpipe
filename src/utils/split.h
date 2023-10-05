// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"

namespace linpipe {

size_t split(string_view str, char delimiter, vector<string_view>& parts, size_t max_splits=0);
size_t split(string_view str, string_view delimiter, vector<string_view>& parts, size_t max_splits=0);

} // namespace linpipe
