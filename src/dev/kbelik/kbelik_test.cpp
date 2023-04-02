// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.h"
#include "lib/doctest.h"
#include "dev/kbelik/persistent_map.cpp"

namespace linpipe {
namespace kbelik {

TEST_CASE("Persistent map") {
  filesystem::create_directories("./temp");
  map<int, vector<byte>> m;
  vector<byte> b1 = {byte{1},byte{2}};
  vector<byte> b2 = {byte{1},byte{2}};
  vector<byte> b3 = {byte{1},byte{2}};
  m[0] = b1;
  m[10] = b2;
  m[20] = b3;
  SUBCASE("get_val_space calculates correctly") {
    auto pm = PersistentMap<int, vector<byte>>();
    CHECK(pm.get_val_space(&m) == 6);
  }
  SUBCASE("get_val_space calculates correctly") {
    /*
    auto p = filesystem::path("temp/map.bin");
    PersistentMap<int, vector<byte>>::build(&m, p);
    byte* for_search;
    ifstream f;
    f.open(p, ios::in | ios::binary);
    if (f.is_open()) {
      f.seekg(0, std::ios::end);
      size_t size = f.tellg();
      f.seekg(0, std::ios::beg);
      for_search = new byte[size];
      f.read((char*) &for_search[0], size);

      CHECK(for_search[0] == byte{0});
      CHECK(for_search[8] == byte{10});
      CHECK(for_search[24] == byte{1});
      CHECK(for_search[25] == byte{2});
    }
    f.close();
    */
  }
}

} // namespace linpipe
} // namespace kbelik
