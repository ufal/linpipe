// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest/doctest.h"
#include "lib/sha256.h"

namespace linpipe {

namespace {

constexpr std::string_view empty_sha256 = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
constexpr std::string_view abc_sha256 = "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
constexpr std::string_view two_blocks = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
constexpr std::string_view two_blocks_sha256 = "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1";
constexpr std::string_view million_a_sha256 = "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0";
constexpr std::string_view all_bytes_sha256 = "40aff2e9d2d8922e47afd4648e6967497158785fbd1da870e7110266bf944880";

std::array<std::byte, 256> all_bytes() {
  std::array<std::byte, 256> bytes;
  for (size_t i = 0; i < bytes.size(); i++) bytes[i] = static_cast<std::byte>(i);
  return bytes;
}

} // namespace

TEST_CASE("SHA256 hashes known values") {
  CHECK(SHA256().hexdigest() == empty_sha256);
  CHECK(SHA256().update("abc").hexdigest() == abc_sha256);
  CHECK(SHA256().update(two_blocks).hexdigest() == two_blocks_sha256);
  CHECK(SHA256().update(std::string(1'000'000, 'a')).hexdigest() == million_a_sha256);
}

TEST_CASE("SHA256 hashes all byte values given as a byte span") {
  CHECK(SHA256().update(all_bytes()).hexdigest() == all_bytes_sha256);
}

TEST_CASE("SHA256 incremental updates at any split point give the same result") {
  for (size_t split = 0; split <= two_blocks.size(); split++) {
    CAPTURE(split);
    SHA256 sha256;
    sha256.update(two_blocks.substr(0, split)).update(two_blocks.substr(split));
    CHECK(sha256.hexdigest() == two_blocks_sha256);
  }
}

TEST_CASE("SHA256 digests do not finalize the computation") {
  SHA256 sha256;
  sha256.update("ab");
  CHECK(sha256.hexdigest() == SHA256().update("ab").hexdigest());
  CHECK(sha256.hexdigest() == SHA256().update("ab").hexdigest());
  sha256.update("c");
  CHECK(sha256.hexdigest() == abc_sha256);
}

TEST_CASE("SHA256 digest has digest_size bytes matching the hexdigest") {
  auto digest = SHA256().update("abc").digest();
  REQUIRE(digest.size() == SHA256::digest_size);

  std::string hex = fmt::format("{:02x}", fmt::join(digest, ""));
  CHECK(hex == abc_sha256);
}

} // namespace linpipe
