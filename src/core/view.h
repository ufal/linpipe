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
#include "core/layer.h"

namespace linpipe {

class View {
 public:
  static constexpr std::string_view TYPE = "view";

  virtual ~View() = default;

  virtual std::string_view type() const = 0;

  std::string_view model() const { return layer_->model; }
  std::string_view license() const { return layer_->license; }
  std::string_view timestamp() const { return layer_->timestamp; }  // ISO 8601 date-time in UTC, e.g., 2026-01-01T12:00:00Z

 protected:
  explicit View(const Layer& layer) : layer_(&layer) {}

 private:
  const Layer* layer_;
};

template<typename T, typename Base = View>
class TypedView : public Base {
 public:
  std::string_view type() const override {
    static_assert(T::TYPE != Base::TYPE, "Classes derived from TypedView must define their own TYPE");
    return T::TYPE;
  }

 private:
  template<typename... Args>
  explicit TypedView(Args&&... args) : Base(std::forward<Args>(args)...) {}
  friend T;
};

} // namespace linpipe
