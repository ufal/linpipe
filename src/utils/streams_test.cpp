// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>
#include <sstream>

#include "lib/doctest/doctest.h"
#include "lib/xz.h"
#include "utils/streams.h"
#include "utils/test_utils.h"

namespace linpipe {

namespace {

constexpr int DATA_SIZE = 1 << 14;
constexpr int CHUNK_SIZE = 1 << 10;

std::string read_all(std::istream& is) {
  std::string result;
  for (std::array<char, CHUNK_SIZE> buffer; is.read(buffer.data(), buffer.size()) || is.gcount();)
    result.append(buffer.data(), is.gcount());
  return result;
}

std::string read_file(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::binary);
  REQUIRE(file);
  return read_all(file);
}

} // namespace

TEST_CASE("OutputStream without compression") {
  std::ostringstream target;
  OutputStream os(target);
  os << "Hello" << ' ' << 42;
  os.close();
  CHECK(target.view() == "Hello 42");
}

TEST_CASE("OutputStream compress") {
  std::string data(DATA_SIZE, 'a'), decompressed;
  std::ostringstream compressed;
  OutputStream os(compressed, Compression::XZ);

  os << data;
  os.close();
  CHECK(xz_decompress(compressed.str(), decompressed) == compressed.str().size());
  CHECK(decompressed == data);

  CHECK_THROWS_AS(os << data << std::flush, LinpipeError);
}

TEST_CASE("OutputStream decompress") {
  std::string data(DATA_SIZE, 'a'), compressed;
  REQUIRE(xz_compress(data, compressed));
  std::ostringstream decompressed;
  OutputStream os(decompressed, Compression::XZ, CompressionMode::DECOMPRESS);

  SUBCASE("single stream") {
    os << compressed;
    os.close();
    CHECK(decompressed.str() == data);
  }
  SUBCASE("concatenated streams") {
    os << compressed << compressed;
    os.close();
    CHECK(decompressed.str() == data + data);
  }
  SUBCASE("truncated stream") {
    os << compressed.substr(0, compressed.size() / 2);
    CHECK_THROWS_AS(os.close(), LinpipeError);
  }
}

TEST_CASE("InputStream without compression") {
  std::istringstream source("Hello 42");
  InputStream is(source);
  std::string word;
  int number = 0;
  is >> word >> number;
  CHECK(word == "Hello");
  CHECK(number == 42);
  CHECK(!(is >> word));
}

TEST_CASE("InputStream decompress") {
  std::string data(DATA_SIZE, 'a'), compressed;
  REQUIRE(xz_compress(data, compressed));

  SUBCASE("single stream") {
    std::istringstream source(compressed);
    InputStream is(source, Compression::XZ);
    CHECK(read_all(is) == data);
  }
  SUBCASE("concatenated streams") {
    std::istringstream source(compressed + compressed);
    InputStream is(source, Compression::XZ);
    CHECK(read_all(is) == data + data);
  }
  SUBCASE("truncated stream") {
    std::istringstream source(compressed.substr(0, compressed.size() / 2));
    InputStream is(source, Compression::XZ);
    CHECK_THROWS_AS(read_all(is), LinpipeError);
  }
}

TEST_CASE("Streams with files") {
  TempDir dir;
  std::string data(DATA_SIZE, 'a');

  SUBCASE("automatic compression") {
    for (const auto* name : {"data.txt", "data.txt.xz"}) {
      auto path = dir.path() / name;
      OutputStream os(path);
      os << data;
      os.close();

      std::string decompressed;
      if (compression_from_path(path) == Compression::XZ)
        CHECK(xz_decompress(read_file(path), decompressed) > 0);
      else
        decompressed = read_file(path);
      CHECK(decompressed == data);

      InputStream is(path);
      CHECK(read_all(is) == data);
      is.close();
    }
  }
  SUBCASE("explicit compression") {
    auto path = dir.path() / "data.bin";
    OutputStream(path, std::ios::binary, Compression::XZ) << data;
    std::string decompressed;
    CHECK(xz_decompress(read_file(path), decompressed) > 0);
    CHECK(decompressed == data);

    InputStream is(path, std::ios::binary, Compression::XZ);
    CHECK(read_all(is) == data);
  }
}

} // namespace linpipe
