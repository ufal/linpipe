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
#include <sstream>

#include "lib/doctest/doctest.h"
#include "lib/xz.h"
#include "lib/xz_stream.h"

namespace linpipe {

TEST_CASE_TEMPLATE("xz_compress roundtrip", Container, std::string, std::vector<std::byte>) {
  using T = typename Container::value_type;
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

static constexpr int DATA_SIZE = 1 << 14;
static constexpr int CHUNK_SIZE = 1 << 10;

std::string read_all(std::istream& is) {
  std::string result;
  for (std::array<char, CHUNK_SIZE> buffer; is.read(buffer.data(), buffer.size()) || is.gcount();)
    result.append(buffer.data(), is.gcount());
  return result;
}

TEST_CASE("XZOStream compress") {
  std::string data(DATA_SIZE, 'a'), decompressed;
  std::ostringstream compressed;
  XZOStream os(compressed, XZMode::COMPRESS);

  os << data;
  os.close();
  CHECK(os.good());
  CHECK(xz_decompress(compressed.str(), decompressed) == compressed.str().size());
  CHECK(decompressed == data);
}

TEST_CASE("XZOStream decompress") {
  std::string data(DATA_SIZE, 'a'), compressed;
  REQUIRE(xz_compress(data, compressed));
  std::ostringstream decompressed;
  XZOStream os(decompressed, XZMode::DECOMPRESS);

  SUBCASE("single stream") {
    os << compressed;
    os.close();
    CHECK(os.good());
    CHECK(decompressed.str() == data);
  }
  SUBCASE("concatenated streams") {
    os << compressed << compressed;
    os.close();
    CHECK(os.good());
    CHECK(decompressed.str() == data + data);
  }
  SUBCASE("truncated stream") {
    os << compressed.substr(0, compressed.size() / 2);
    os.close();
    CHECK(os.bad());
  }
}

TEST_CASE("XZIStream") {
  std::string data(DATA_SIZE, 'a'), compressed;
  REQUIRE(xz_compress(data, compressed));

  SUBCASE("single stream") {
    std::istringstream source(compressed);
    XZIStream is(source);
    CHECK(read_all(is) == data);
    CHECK(!is.bad());
  }
  SUBCASE("concatenated streams") {
    std::istringstream source(compressed + compressed);
    XZIStream is(source);
    CHECK(read_all(is) == data + data);
    CHECK(!is.bad());
  }
  SUBCASE("truncated stream") {
    std::istringstream source(compressed.substr(0, compressed.size() / 2));
    XZIStream is(source);
    read_all(is);
    CHECK(is.bad());
  }
}

} // namespace linpipe
