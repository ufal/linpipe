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

void write_file(const std::filesystem::path& path, std::string_view data) {
  std::ofstream file(path, std::ios::binary);
  REQUIRE(file.write(data.data(), static_cast<std::streamsize>(data.size())));
}

using Batches = std::vector<std::string>;

Batches read_batches(MultiInputStream& is) {
  Batches batches;
  do batches.push_back(read_all(is));
  while (is.next_batch());
  return batches;
}

Batches read_batches(std::string_view data, StreamLimit limit) {
  std::istringstream source{std::string{data}};
  MultiInputStream is(source, Compression::NONE, limit);
  return read_batches(is);
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

TEST_CASE("MultiInputStream units") {
  SUBCASE("no limit") {
    std::istringstream source("a\n\nb\n");
    MultiInputStream is(source);
    CHECK(read_all(is) == "a\n\nb\n");
    CHECK(!is.next_batch());
  }
  SUBCASE("empty input") {
    CHECK(read_batches("", StreamLimit::units(StreamLimit::Unit::LINE, 1)) == Batches{""});
  }
  SUBCASE("lines") {
    std::string_view data = "\n\na\nb\n\n\nc\r\n\r\nd\re";
    CHECK(read_batches(data, StreamLimit::units(StreamLimit::Unit::LINE, 1)) == Batches{"\n\na\n", "b\n\n\n", "c\r\n\r\n", "d\r", "e"});
    CHECK(read_batches(data, StreamLimit::units(StreamLimit::Unit::LINE, 2)) == Batches{"\n\na\nb\n\n\n", "c\r\n\r\nd\r", "e"});
  }
  SUBCASE("paragraphs") {
    std::string_view data = "\na\nb\n\nc\r\nd\r\n\r\ne\r\rf\n";
    CHECK(read_batches(data, StreamLimit::units(StreamLimit::Unit::PARA, 1)) == Batches{"\na\nb\n\n", "c\r\nd\r\n\r\n", "e\r\r", "f\n"});
    CHECK(read_batches(data, StreamLimit::units(StreamLimit::Unit::PARA, 3)) == Batches{"\na\nb\n\nc\r\nd\r\n\r\ne\r\r", "f\n"});
  }
  SUBCASE("bytes") {
    std::string_view data = "ab\ncd\nefgh\nij\n";
    CHECK(read_batches(data, StreamLimit::bytes(StreamLimit::Unit::LINE, 4)) == Batches{"ab\ncd\n", "efgh\n", "ij\n"});
    CHECK(read_batches(data, StreamLimit::bytes(StreamLimit::Unit::LINE, 6)) == Batches{"ab\ncd\n", "efgh\nij\n"});
  }
  SUBCASE("files") {
    CHECK(read_batches("a\n\nb\n", StreamLimit::units(StreamLimit::Unit::FILE, 1)) == Batches{"a\n\nb\n"});
  }
}

TEST_CASE("MultiInputStream with files") {
  TempDir dir;
  auto a = dir.path() / "a.txt", b = dir.path() / "b.txt.xz", c = dir.path() / "c.txt", empty = dir.path() / "empty.txt";
  std::string b_compressed;
  REQUIRE(xz_compress("c\n\nd", b_compressed));
  write_file(a, "a\nbb\n");
  write_file(b, b_compressed);
  write_file(c, "e\r\n");
  write_file(empty, "");

  SUBCASE("no limit") {
    MultiInputStream is({a, empty, b, c});
    CHECK(read_all(is) == "a\nbb\nc\n\nde\r\n");
    CHECK(!is.next_batch());
  }
  SUBCASE("path and tellg") {
    MultiInputStream is({empty, a, empty, b, c, empty}, std::ios::binary, Compression::AUTO, StreamLimit::units(StreamLimit::Unit::PARA, 1));
    std::vector<std::tuple<std::optional<std::filesystem::path>, std::streamoff, std::string>> lines;
    do {
      while (true) {
        auto path = is.path();
        auto offset = is.tellg();
        std::string line;
        if (!std::getline(is, line))
          break;
        lines.emplace_back(path, offset, line);
      }
    } while (is.next_batch());
    CHECK(lines.size() == 6);
    CHECK(lines[0] == std::tuple{a, 0, "a"});
    CHECK(lines[1] == std::tuple{a, 2, "bb"});
    CHECK(lines[2] == std::tuple{b, 0, "c"});
    CHECK(lines[3] == std::tuple{b, 2, ""});
    CHECK(lines[4] == std::tuple{b, 3, "d"});
    CHECK(lines[5] == std::tuple{c, 0, "e\r"});
  }
  SUBCASE("units end at file ends") {
    MultiInputStream is({a, empty, b, c}, std::ios::binary, Compression::AUTO, StreamLimit::units(StreamLimit::Unit::LINE, 1));
    CHECK(read_batches(is) == Batches{"a\n", "bb\n", "c\n\n", "d", "e\r\n"});
  }
  SUBCASE("batches span files") {
    MultiInputStream is({a, empty, b, c}, std::ios::binary, Compression::AUTO, StreamLimit::units(StreamLimit::Unit::LINE, 3));
    CHECK(read_batches(is) == Batches{"a\nbb\nc\n\n", "de\r\n"});
  }
  SUBCASE("file units") {
    MultiInputStream is({empty, a, empty, b, c, empty}, std::ios::binary, Compression::AUTO, StreamLimit::units(StreamLimit::Unit::FILE, 1));
    CHECK(is.path() == a);
    CHECK(is.tellg() == 0);
    CHECK(read_all(is) == "a\nbb\n");
    CHECK(is.next_batch());
    CHECK(is.path() == b);
    CHECK(is.tellg() == 0);
    CHECK(read_all(is) == "c\n\nd");
    CHECK(is.next_batch());
    CHECK(is.path() == c);
    CHECK(read_all(is) == "e\r\n");
    CHECK(!is.next_batch());
  }
  SUBCASE("missing file") {
    MultiInputStream is({a, dir.path() / "missing.txt"});
    CHECK_THROWS_AS(read_all(is), LinpipeError);
  }
}

} // namespace linpipe
