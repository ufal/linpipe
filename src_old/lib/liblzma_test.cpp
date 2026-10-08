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
#include "lib/liblzma.h"
#include "lib/liblzma_stream.h"

namespace linpipe {

TEST_CASE_TEMPLATE("lzma::compress roundtrip", T, char, signed char, unsigned char, std::byte) {
  auto data = "Testing LZMA compression in LinPipe"sv | std::views::transform([](char c) { return T(c); });
  std::vector<T> input(data.begin(), data.end());

  std::vector<T> compressed, decompressed;
  CHECK(lzma::compress(input, compressed));
  CHECK(lzma::decompress(compressed, decompressed) == compressed.size());
  CHECK(decompressed == input);

  auto concatenated = std::array{compressed, compressed} | std::views::join;
  std::vector<T> compressed_twice(concatenated.begin(), concatenated.end());
  CHECK(lzma::decompress(compressed_twice, decompressed) == compressed_twice.size());
  CHECK(std::ranges::equal(decompressed, std::array{input, input} | std::views::join));

  CHECK(lzma::decompress(compressed_twice, decompressed, true) == compressed.size());
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

TEST_CASE("lzma::OStream compress") {
  std::string data(DATA_SIZE, 'a'), decompressed;
  std::ostringstream compressed;
  lzma::OStream os(compressed, lzma::Mode::COMPRESS);

  os << data;
  os.close();
  CHECK(os.good());
  CHECK(lzma::decompress(compressed.str(), decompressed) == compressed.str().size());
  CHECK(decompressed == data);
}

TEST_CASE("lzma::OStream decompress") {
  std::string data(DATA_SIZE, 'a'), compressed;
  REQUIRE(lzma::compress(data, compressed));
  std::ostringstream decompressed;
  lzma::OStream os(decompressed, lzma::Mode::DECOMPRESS);

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

TEST_CASE("lzma::IStream") {
  std::string data(DATA_SIZE, 'a'), compressed;
  REQUIRE(lzma::compress(data, compressed));

  SUBCASE("single stream") {
    std::istringstream source(compressed);
    lzma::IStream is(source);
    CHECK(read_all(is) == data);
    CHECK(!is.bad());
  }
  SUBCASE("concatenated streams") {
    std::istringstream source(compressed + compressed);
    lzma::IStream is(source);
    CHECK(read_all(is) == data + data);
    CHECK(!is.bad());
  }
  SUBCASE("truncated stream") {
    std::istringstream source(compressed.substr(0, compressed.size() / 2));
    lzma::IStream is(source);
    read_all(is);
    CHECK(is.bad());
  }
}

} // namespace linpipe
