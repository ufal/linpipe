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
#include "layers/index_span.h"
#include "view.h"

namespace linpipe {

// Sentences as half-open spans of token indices.
//
// The spans are normalized: they are non-empty, ordered, and contiguous,
// covering every token exactly once. Iterating all sentences therefore
// visits all tokens; a document without tokens has no sentences.
//
// Combine with TokenViewSlice to iterate the tokens of each sentence.
class SentenceView : public View {
  public:
    virtual ~SentenceView() = default;

    virtual size_t size() const = 0;
    virtual layers::IndexSpan span(size_t i) const = 0;
};

} // namespace linpipe
