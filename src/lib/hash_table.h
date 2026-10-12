// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <functional>
#include <type_traits>

#include "common.h"

#include "lib/unordered_dense/unordered_dense.h"

namespace linpipe {

// A ankerl hash function for any string-like type convertible to std::string_view.
struct ankerl_string_view_hash {
  using is_transparent = void; // enable heterogeneous overloads
  using is_avalanching = void; // mark class as high quality avalanching hash

  [[nodiscard]] auto operator()(std::string_view str) const noexcept -> uint64_t {
    return ankerl::unordered_dense::hash<std::string_view>{}(str);
  }
};

// Helper templates for hash selection based on key type.
template<typename T>
constexpr bool ankerl_is_string_key_v =
    std::is_same_v<std::decay_t<T>, std::string> ||
    std::is_same_v<std::decay_t<T>, std::string_view>;

template<typename Key, typename DefaultHash>
using ankerl_select_hash_t = std::conditional_t<
    std::is_same_v<DefaultHash, void>,
    std::conditional_t<ankerl_is_string_key_v<Key>, ankerl_string_view_hash, ankerl::unordered_dense::hash<Key>>,
    DefaultHash>;

template<typename Key, typename DefaultEqual>
using ankerl_select_equal_t = std::conditional_t<
    std::is_same_v<DefaultEqual, void>,
    std::conditional_t<ankerl_is_string_key_v<Key>, std::equal_to<>, std::equal_to<Key>>,
    DefaultEqual>;

} // namespace linpipe
