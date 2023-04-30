// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"
#include "lib/doctest.h"

#include "dev/kbelik/map_values/int4.cpp"
//#include "dev/kbelik/persistent_map.cpp"
#include "dev/kbelik/dynamic_map.cpp"

namespace linpipe {
namespace kbelik {

TEST_CASE("Dynamic map") {
  DynamicMap<int, map_values::Int4> dp = DynamicMap<int, map_values::Int4>();
  SUBCASE("Add, erase -- single") {
    REQUIRE(dp.length() == 0);
    int to_add = 20;
    for (int i = 0; i < to_add; ++i) {
      dp.add(i, 10 + i); 
    }
    CHECK(dp.length() == to_add);
    for (int i = 0; i < to_add; ++i) {
      dp.erase(i);
    }
    CHECK(dp.length() == 0);
  }
  SUBCASE("Add, erase -- multiple") {
    REQUIRE(dp.length() == 0);
    dp.add(10, 0);
    dp.add(10, 0);
    CHECK(dp.length() == 1);
    dp.erase(10);
    CHECK(dp.length() == 0);
  }
  SUBCASE("find present") {
    int expected = 0;
    dp.add(10, expected);
    int res;
    bool flag = dp.find(10, res);
    CHECK(res == expected);
    CHECK(flag);
    dp.eras(10);
  }
  SUBCASE("find present") {
    int expected = 0;
    dp.add(10, expected);
    int res;
    bool flag = dp.find(11, res);
    CHECK(!flag);
    dp.eras(10);
    flag = dp.find(10, res);
    CHECK(!flag);
  }
}

/*
TEST_CASE("Persistent map") {
  DynamicMap<int, map_values::Int4> dp;
  int vals_cnt = 10;
  for (int i = 0; i < vals_cnt; ++i) 
    dp.add(i, i);
  filesystem::create_directories("./temp");
  ofstream ofs;
  ofs.open("temp/test_map.bin", ofstream::out | ofstream::binary);
  dp.save_map(ofs);
  ofs.close();
  filesystem::path fp("temp/test_map.bin");
  auto pm = PersistentMap<int, map_values::Int4>(path);
  SUBCASE("close") {
    int res;
    pm.close();
    CHECK_THROWS_AS(pm.find(1, res), const LinpipeError);
  }
  SUBCASE("find success") {
    int res;
    bool flag;
    for (int i = 0; i < vals_cnt; ++i) { 
      flag = pm.find(i, res);
      CHECK(flag);
      CHECK(res == i);
    }
  }
  SUBCASE("find fail") {
    int res;
    bool flag = find(-1, res);
    CHECK(!flag);
  }
  SUBCASE("Persistent map with offset") {
    ofs.open("temp/test_map_offset.bin", ofstream::out | ofstream::binary);
    ofs.write({(byte)1, (byte)2}, 2);
    dp.save_map(ofs);
    ofs.close();
    filesystem::path fp_offset("temp/test_map.bin");
    auto pm_offset = PersistentMap<int, Int4>(fp_offset, 2);
    int res;
    bool flag = find(1, res);
    CHECK(flag);
    CHECK(ress == 1);
    int res;
    bool flag = find(-1, res);
    CHECK(!flag);
  }
  SUBCASE("Persistent map with length") {
    ofs.open("temp/test_map_length.bin", ofstream::out | ofstream::binary);
    dp.save_map(ofs);
    size_t length = ofs.tellp();
    ofs.write({(byte)1, (byte)2}, 2);
    ofs.close();
    auto pm_offset = PersistentMap<int, Int4>(fp_offset, length=length);
    int res;
    bool flag = find(1, res);
    CHECK(flag);
    CHECK(ress == 1);
    int res;
    bool flag = find(-1, res);
    CHECK(!flag);
  }
}

TEST_CASE("Agnostic kbelik") {
}

TEST_CASE("Specific kbelik") {
}
*/
/*
TEST_CASE("Byte value") {
  SUBCASE("Serialization and deserialization works") {
    auto d = make_unique<byte[]>(3);
    d[0] = (byte)2;
    d[1] = (byte)1;
    d[2] = (byte)2;
    auto b = ByteValue(move(d));
    auto ve = b.serialize();
    CHECK(3 == ve.size());
    CHECK((byte)2 == ve[0]);
    CHECK((byte)1 == ve[1]);
    CHECK((byte)2 == ve[2]);
  }
}
*/

namespace map_values {

TEST_CASE("Int4") {
    byte* data = new byte[4];
    data[0] = (byte)2;
    data[1] = (byte)1;
    data[2] = (byte)2;
    data[3] = (byte)2;

    int expected = (int)data[0];
    for (int i = 1; i <= 3; ++i)
      expected |= (int)data[i] << (8 * i);

  SUBCASE("Correct length") {
    CHECK(4 == Int4::length(data));
  }
  SUBCASE("Correct deserialization") {
    int res;
    Int4::deserialize(data, res);
    CHECK(expected == res);
  }
  SUBCASE("Correct serialization") {
    vector<byte> res;
    Int4::serialize(expected, res);
    for (int i = 0; i < 4; ++i) 
      CHECK(res[i] == data[i]);
  }
  delete[] data;
}

} // namespace map_values

} // namespace linpipe
} // namespace kbelik
