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

namespace linpipe::lzma {

template<class T>
concept ByteLike = std::same_as<T, std::byte> || std::same_as<T, char> || std::same_as<T, signed char> || std::same_as<T, unsigned char>;

template<class C>
concept ByteContainer =
    ByteLike<typename C::value_type>
    && (std::same_as<C, std::vector<typename C::value_type>> || std::same_as<C, std::basic_string<typename C::value_type>>);

template<ByteContainer C>
bool compress(std::span<const typename C::value_type> data, C& output, uint32_t preset = 6);

template<ByteContainer C>
size_t decompress(std::span<const typename C::value_type> data, C& output, bool only_first_block = false);

} // namespace linpipe::lzma
