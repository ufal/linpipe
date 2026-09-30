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

namespace linpipe::layers {

// A half-open span [begin, end) of indices, e.g., offsets into the source text.
class IndexSpan {
  public:
    IndexSpan() = default;
    IndexSpan(int begin, int end) : begin(begin), end(end) {}

    int size() const { return end - begin; }
    bool empty() const { return begin >= end; }

    bool operator==(const IndexSpan& other) const { return begin == other.begin && end == other.end; }
    bool operator!=(const IndexSpan& other) const { return !(*this == other); }

    int begin = 0;
    int end = 0;
};

} // namespace linpipe::layers
