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

namespace linpipe {

class Document;

class Layer {
 public:
  virtual ~Layer() = default;

  static std::unique_ptr<Layer> create(std::string_view type, std::string_view name = {});

  virtual void from_json(const Json& json) = 0;
  virtual Json to_json() = 0;
  virtual std::string to_html() = 0;

  const std::string& name() { return name_; }
  const std::string& type() { return type_; }

  // TODO: add more structured metadata info

 protected:
  Layer(std::string_view type, std::string_view name) : type_(type), name_(name) {}

  std::string type_;
  std::string name_;
  friend class Document;
};

} // namespace linpipe
