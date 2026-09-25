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
#include "view.h"

namespace linpipe {

class TokenView : public View {
  public:
    struct const_iterator {
      const TokenView* view;
      size_t i;

      const std::string& operator*() const { return view->text(i); }
      const_iterator& operator++() { ++i; return *this; }
      bool operator!=(const const_iterator& other) const { return i != other.i; }
    };

    virtual ~TokenView() = default;

    virtual size_t size() const = 0;
    virtual const std::string& text(size_t i) const = 0;

    const_iterator begin() const { return {this, 0}; }
    const_iterator end() const { return {this, size()}; }

};


} // namespace linpipe
