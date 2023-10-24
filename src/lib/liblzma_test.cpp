// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest.h"
#include "lib/liblzma.h"

namespace linpipe {

TEST_CASE("lzma_roundtrip") {
  auto input = "Testing LZMA compression in LinPipe"s;
  vector<byte> compressed;
  CHECK(lzma_compress((const byte*) input.data(), input.size(), compressed));

  vector<byte> decompressed;
  CHECK(lzma_decompress_all(compressed.data(), compressed.size(), decompressed));
  CHECK(input == string_view((const char*)decompressed.data(), decompressed.size()));

  for (size_t i = 0, limit = compressed.size(); i < limit; i++)
    compressed.push_back(compressed[i]);
  CHECK(lzma_decompress_all(compressed.data(), compressed.size(), decompressed));
  CHECK(string(input) + string(input) == string_view((const char*)decompressed.data(), decompressed.size()));

  CHECK(lzma_decompress_one(compressed.data(), compressed.size(), decompressed) == compressed.size() / 2);
  CHECK(input == string_view((const char*)decompressed.data(), decompressed.size()));
}

} // namespace linpipe
