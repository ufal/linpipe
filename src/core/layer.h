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
#include "core/selector.h"

namespace linpipe {

class Layer;
class View;

struct ViewProvider {
  std::vector<Selector> selectors;
  std::unique_ptr<View> (*create)(const Layer& layer);
};

class Layer {
 public:
  static constexpr std::string_view TYPE = "layer";

  virtual ~Layer() = default;

  static std::unique_ptr<Layer> create(std::string_view type, std::string_view name = {});
  static std::unique_ptr<Layer> create_from_json(Json& json);

  virtual std::span<const ViewProvider> views() const = 0;
  std::unique_ptr<View> create_view(std::span<const Selector> selectors) const;

  virtual std::string_view type() const = 0;

  std::string name;
  std::string model;
  std::string license;
  std::string timestamp;  // ISO 8601 date-time in UTC, e.g., 2026-01-01T12:00:00Z

  bool matches(std::span<const Selector> selectors) const;

  virtual Json to_json() const;
  virtual std::string to_html() const = 0;

 protected:
  explicit Layer(std::string_view name) : name(name) {}

  virtual void from_json(Json& json);
  virtual bool matches_selector(const Selector& selector) const;
  static std::span<const ViewProvider> provided_views() { return {}; }
};

template<typename T, typename Base = Layer>
class TypedLayer : public Base {
 public:
  std::string_view type() const override {
    static_assert(T::TYPE != Base::TYPE, "Classes derived from TypedLayer must define their own TYPE");
    return T::TYPE;
  }

  std::span<const ViewProvider> views() const override { return T::provided_views(); }

 private:
  template<typename... Args>
  explicit TypedLayer(Args&&... args) : Base(std::forward<Args>(args)...) {}
  friend T;
};

} // namespace linpipe
