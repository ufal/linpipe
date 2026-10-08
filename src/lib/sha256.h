// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <span>

#include "common.h"

namespace linpipe {

class SHA256 {
 public:
  static constexpr size_t digest_size = 32;

  SHA256();
  ~SHA256();

  SHA256(SHA256&&) noexcept;
  SHA256& operator=(SHA256&&) noexcept;
  SHA256(const SHA256&) = delete;
  SHA256& operator=(const SHA256&) = delete;

  SHA256& update(std::string_view data);
  SHA256& update(std::span<const std::byte> data);

  std::array<std::byte, digest_size> digest() const;
  std::string hexdigest() const;

 private:
  struct Impl;
  std::unique_ptr<Impl> impl;
};

} // namespace linpipe
