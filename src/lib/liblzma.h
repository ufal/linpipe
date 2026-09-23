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

bool lzma_compress(const std::byte* data, size_t length, std::vector<std::byte>& output, uint32_t preset=6);
bool lzma_decompress_all(const std::byte* data, size_t length, std::vector<std::byte>& output);
size_t lzma_decompress_one(const std::byte* data, size_t length, std::vector<std::byte>& output);

} // namespace linpipe
