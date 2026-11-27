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

  // Move this layer to a newly allocated layer of the same type, resetting
  // this layer to the state of a newly constructed layer of its type.
  virtual std::unique_ptr<Layer> move_to_unique() = 0;

  virtual std::span<const ViewProvider> views() const = 0;
  std::unique_ptr<View> create_view(std::span<const Selector> selectors, bool throw_on_error = true) const;
  template<std::derived_from<View> T>
  std::unique_ptr<T> create_view(std::span<const Selector> selectors, bool throw_on_error = true) const;

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
  Layer(Layer&&) = default;
  Layer& operator=(Layer&&) = default;

  virtual void from_json(Json& json);
  virtual bool matches_selector(const Selector& selector) const;
  static std::span<const ViewProvider> provided_views() { return {}; }
};

template<std::derived_from<View> T>
std::unique_ptr<T> Layer::create_view(std::span<const Selector> selectors, bool throw_on_error) const {
  auto view = create_view(selectors, throw_on_error);
  if (!view) return nullptr;
  if (!dynamic_cast<T*>(view.get())) {
    if (!throw_on_error) return nullptr;
    throw LinPipeError("The view of layer '{}:name={}' matching the selectors '{}' is not a '{}' view", type(), name, selectors, T::TYPE);
  }
  return std::unique_ptr<T>(static_cast<T*>(view.release()));
}

template<typename T, typename Base = Layer>
class TypedLayer : public Base {
 public:
  std::string_view type() const override {
    static_assert(T::TYPE != Base::TYPE, "Classes derived from TypedLayer must define their own TYPE");
    return T::TYPE;
  }

  std::span<const ViewProvider> views() const override { return T::provided_views(); }

  std::unique_ptr<Layer> move_to_unique() override {
    T& self = static_cast<T&>(*this);
    auto moved = std::make_unique<T>(std::move(self));
    self = T();
    return moved;
  }

 private:
  template<typename... Args>
  explicit TypedLayer(Args&&... args) : Base(std::forward<Args>(args)...) {}
  friend T;
};

} // namespace linpipe
