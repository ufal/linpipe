// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <array>

#include "lib/doctest/doctest.h"
#include "lib/xz.h"

namespace linpipe {

TEST_CASE_TEMPLATE("xz_compress roundtrip", Container, std::string, std::vector<std::byte>) {
  using T = Container::value_type;
  auto data = "Testing LZMA compression in LinPipe"sv | std::views::transform([](char c) { return T(c); });
  Container input(data.begin(), data.end());

  Container compressed, decompressed;
  CHECK(xz_compress(input, compressed));
  CHECK(xz_decompress(compressed, decompressed) == compressed.size());
  CHECK(decompressed == input);

  auto concatenated = std::array{compressed, compressed} | std::views::join;
  Container compressed_twice(concatenated.begin(), concatenated.end());
  CHECK(xz_decompress(compressed_twice, decompressed) == compressed_twice.size());
  CHECK(std::ranges::equal(decompressed, std::array{input, input} | std::views::join));

  CHECK(xz_decompress(compressed_twice, decompressed, true) == compressed.size());
  CHECK(decompressed == input);
}

} // namespace linpipe
