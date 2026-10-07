// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "lib/doctest/doctest.h"
#include "lib/sha256.h"

namespace linpipe {

namespace {

// Expected SHA-256 values.
constexpr std::string_view empty_sha256 = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
constexpr std::string_view abc_sha256 = "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
constexpr std::string_view two_blocks = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
constexpr std::string_view two_blocks_sha256 = "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1";
constexpr std::string_view million_a_sha256 = "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0";
constexpr std::string_view all_bytes_sha256 = "40aff2e9d2d8922e47afd4648e6967497158785fbd1da870e7110266bf944880";

// Returns all 256 byte values in increasing order.
std::vector<std::byte> all_bytes() {
  std::vector<std::byte> bytes(256);
  for (size_t i = 0; i < bytes.size(); i++) bytes[i] = static_cast<std::byte>(i);
  return bytes;
}

} // namespace

TEST_CASE("SHA256 known values") {
  SUBCASE("hashes empty input") {
    CHECK(SHA256().hexdigest() == empty_sha256);
    CHECK(SHA256().update("").hexdigest() == empty_sha256);
    CHECK(SHA256().update(std::span<const std::byte>()).hexdigest() == empty_sha256);
  }

  SUBCASE("hashes short and multi-block input") {
    CHECK(SHA256().update("abc").hexdigest() == abc_sha256);
    CHECK(SHA256().update(two_blocks).hexdigest() == two_blocks_sha256);
    CHECK(SHA256().update(std::string(1'000'000, 'a')).hexdigest() == million_a_sha256);
  }

  SUBCASE("hashes all byte values, including zero bytes") {
    CHECK(SHA256().update(all_bytes()).hexdigest() == all_bytes_sha256);
  }
}

TEST_CASE("SHA256 incremental computation") {
  SUBCASE("chained updates equal a single update") {
    CHECK(SHA256().update("a").update("").update("bc").hexdigest() == abc_sha256);
  }

  SUBCASE("any split point gives the same result") {
    // The input spans two 64-byte blocks, so splits hit both sides of the boundary.
    for (size_t split = 0; split <= two_blocks.size(); split++) {
      CAPTURE(split);
      SHA256 sha256;
      sha256.update(two_blocks.substr(0, split)).update(two_blocks.substr(split));
      CHECK(sha256.hexdigest() == two_blocks_sha256);
    }
  }

  SUBCASE("byte-by-byte updates give the same result") {
    SHA256 sha256;
    for (char c : two_blocks) sha256.update(std::string_view(&c, 1));
    CHECK(sha256.hexdigest() == two_blocks_sha256);
  }

  SUBCASE("digests do not finalize the computation") {
    SHA256 sha256;
    sha256.update("ab");
    CHECK(sha256.hexdigest() == SHA256().update("ab").hexdigest());
    CHECK(sha256.hexdigest() == SHA256().update("ab").hexdigest());
    sha256.update("c");
    CHECK(sha256.hexdigest() == abc_sha256);
    CHECK(sha256.digest() == SHA256().update("abc").digest());
  }

  SUBCASE("independent instances do not interfere") {
    SHA256 first, second;
    first.update("ab");
    second.update("x");
    first.update("c");
    CHECK(first.hexdigest() == abc_sha256);
    CHECK(second.hexdigest() == SHA256().update("x").hexdigest());
  }
}

TEST_CASE("SHA256 input and output types") {
  SUBCASE("string_view and byte span inputs agree") {
    std::string text = "abc";
    std::vector<std::byte> bytes = {std::byte{'a'}, std::byte{'b'}, std::byte{'c'}};
    CHECK(SHA256().update(text).hexdigest() == abc_sha256);
    CHECK(SHA256().update(bytes).hexdigest() == abc_sha256);
    CHECK(SHA256().update(std::as_bytes(std::span(text))).hexdigest() == abc_sha256);
    CHECK(SHA256().update("a").update(std::span(bytes).subspan(1)).hexdigest() == abc_sha256);
  }

  SUBCASE("digest has digest_size bytes matching the hexdigest") {
    auto digest = SHA256().update("abc").digest();
    REQUIRE(digest.size() == SHA256::digest_size);

    std::string hex;
    for (auto byte : digest) hex += fmt::format("{:02x}", std::to_integer<unsigned>(byte));
    CHECK(hex == abc_sha256);
  }

  SUBCASE("hexdigest is lowercase with 2 * digest_size characters") {
    auto hex = SHA256().update(all_bytes()).hexdigest();
    CHECK(hex.size() == 2 * SHA256::digest_size);
    CHECK(hex.find_first_not_of("0123456789abcdef") == std::string::npos);
  }
}

TEST_CASE("SHA256 move semantics") {
  SUBCASE("move construction keeps the state") {
    SHA256 source;
    source.update("ab");
    SHA256 target(std::move(source));
    target.update("c");
    CHECK(target.hexdigest() == abc_sha256);
  }

  SUBCASE("move assignment keeps the state") {
    SHA256 source, target;
    source.update("ab");
    target.update("unrelated");
    target = std::move(source);
    target.update("c");
    CHECK(target.hexdigest() == abc_sha256);
  }

  SUBCASE("a moved-from object can be assigned to") {
    SHA256 source;
    SHA256 target(std::move(source));
    source = SHA256();
    CHECK(source.update("abc").hexdigest() == abc_sha256);
  }
}

} // namespace linpipe
