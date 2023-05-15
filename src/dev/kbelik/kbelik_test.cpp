// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstring>
#include <fstream>

#include "common.h"
#include "lib/doctest.h"

#include "dev/kbelik/dynamic_map.h"
#include "dev/kbelik/map_values/bytes.h"
//#include "dev/kbelik/map_values/chars.h"
#include "dev/kbelik/map_values/int4.h"
#include "dev/kbelik/persistent_map.h"

namespace linpipe {
namespace kbelik {

TEST_CASE("Dynamic map") {
  SUBCASE("Int4") {
    DynamicMap<int, map_values::Int4> dm = DynamicMap<int, map_values::Int4>();
    SUBCASE("Add, erase -- big") {
      REQUIRE(dm.length() == 0);
      int to_add = 200;
      for (int i = 0; i < to_add; ++i) {
        dm.add(i, 10 + i); 
      }
      CHECK(dm.length() == to_add);
      for (int i = 0; i < to_add; ++i) {
        dm.erase(i);
      }
      CHECK(dm.length() == 0);
    }
    SUBCASE("Add, erase -- small") {
      REQUIRE(dm.length() == 0);
      dm.add(10, 0);
      dm.add(10, 0);
      CHECK(dm.length() == 1);
      dm.erase(10);
      CHECK(dm.length() == 0);
    }
    SUBCASE("find present") {
      REQUIRE(dm.length() == 0);
      int expected = 0;
      dm.add(10, expected);
      int res;
      bool flag = dm.find(10, res);
      CHECK(res == expected);
      CHECK(flag);
      dm.erase(10);
    }
    SUBCASE("find not present") {
      REQUIRE(dm.length() == 0);
      int expected = 0;
      dm.add(10, expected);
      int res;
      bool flag = dm.find(11, res);
      CHECK(!flag);
      dm.erase(10);
      flag = dm.find(10, res);
      CHECK(!flag);
    }
    SUBCASE("basic save map") {
      REQUIRE(dm.length() == 0);
      dm.add(10, 1);
      dm.add(20, 2);
      dm.add(30, 3);
      stringstream ss;
      REQUIRE(ss.str() == "");
      dm.save_map(ss, test);
      REQUIRE(ss.str().size() > 3 * map_values::Int4::length(0) + sizeof(int) * 3);
    }
  }
  SUBCASE("Bytes") {
    auto dm = DynamicMap<int, map_values::Bytes<int8_t>>();
    SUBCASE("Add, erase -- small") {
      REQUIRE(dm.length() == 0);
      dm.add(10, {(byte)1, (byte)2});
      dm.add(10, {(byte)1, (byte)2, (byte)3});
      CHECK(dm.length() == 1);
      dm.erase(10);
      CHECK(dm.length() == 0);
    }
    SUBCASE("find") {
      REQUIRE(dm.length() == 0);
      vector<byte>expected = {(byte) 0, (byte) 200};
      dm.add(1, expected);
      vector<byte> res;
      bool flag = dm.find(1, res);
      for (size_t i = 0; i < res.size(); ++i)
        CHECK(res[i] == expected[i]);
      CHECK(flag);
      flag = dm.find(11, res);
      CHECK(!flag);
      dm.erase(10);
      flag = dm.find(10, res);
      CHECK(!flag);
    }
    SUBCASE("Add, erase -- big") {
      REQUIRE(dm.length() == 0);
      int to_add = 200;
      for (int i = 0; i < to_add; ++i) {
        if (i&1)
          dm.add(i, {(byte) (i + 15)}); 
        else
          dm.add(i, {(byte) i, (byte) (i +1)}); 
      }
      CHECK(dm.length() == to_add);
      for (int i = 0; i < to_add; ++i) {
        dm.erase(i);
      }
      CHECK(dm.length() == 0);
    }
  }
}

TEST_CASE("Persistent map") {
  DynamicMap<int, map_values::Int4> dm = DynamicMap<int, map_values::Int4>();
  int vals_cnt = 10;
  for (int i = 0; i < vals_cnt; ++i) 
    dm.add(i, i);
  filesystem::create_directories("./temp");
  ofstream ofs("temp/test_map.bin", ofstream::out | ofstream::binary);
  dm.save_map(ofs, test);
  ofs.close();
  filesystem::path fp("temp/test_map.bin");
  auto pm = PersistentMap<int, map_values::Int4>(fp);
  SUBCASE("close") {
    int res;
    pm.close();
    CHECK_THROWS_AS(pm.find(1, res), const LinpipeError);
  }
  SUBCASE("find success") {
    map_values::Int4::Type res;
    bool flag;
    for (int i = 0; i < vals_cnt; ++i) { 
      flag = pm.find(i, res);
      CHECK(flag);
      CHECK(res == i);
    }
  }
  SUBCASE("find fail") {
    int res;
    bool flag = pm.find(-1, res);
    CHECK(!flag);
  }
  SUBCASE("map type loaded") {
    MapType t = pm.get_map_type();
    CHECK(t == test);
  }
  /*
  SUBCASE("Persistent map with offset") {
    ofs.open("temp/test_map_offset.bin", ofstream::out | ofstream::binary);
    ofs.write({(byte)1, (byte)2}, 2);
    dm.save_map(ofs);
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
    dm.save_map(ofs);
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
  */
}

/*

   TEST_CASE("Agnostic kbelik") {
   }

   TEST_CASE("Specific kbelik") {
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

  SUBCASE("Correct length, serialized") {
    CHECK(4 == Int4::length(data));
  }
  SUBCASE("Correct length, deserialized") {
    CHECK(4 == Int4::length(42));
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

template<typename SizeType>
void test_bytes(const char* name) {
  SUBCASE(name) {
    SizeType sz = 3 + sizeof(SizeType);
    byte* data = new byte[sz];
    memcpy(data, &sz, sizeof(SizeType));
    data[sizeof(SizeType)] = (byte)2;
    data[sizeof(SizeType) + 1] = (byte)2;
    data[sizeof(SizeType) + 2] = (byte)1;

    vector<byte> expected(3);
    for (size_t i = 0; i < 3; ++i)
      expected[i] = data[i + sizeof(SizeType)];

    SUBCASE("Correct length") {
      SUBCASE("serialized") {
        CHECK(sz == Bytes<SizeType>::length(data));
      }
      SUBCASE("deserialized") {
        vector<byte> data2(12345, (byte)0);
        CHECK(data2.size() + sizeof(SizeType) == Bytes<SizeType>::length(data2));
      }
    }
    SUBCASE("Correct deserialization") {
      vector<byte> res;
      Bytes<SizeType>::deserialize(data, res);
      for (int i = 0; i < 3; ++i)
        CHECK(expected[i] == res[i]);
    }
    SUBCASE("Correct serialization") {
      vector<byte> res;
      Bytes<SizeType>::serialize(expected, res);
      for (size_t i = 0; i < res.size(); ++i) 
        CHECK(res[i] == data[i]);

    }
    delete[] data;
  }
}

TEST_CASE("Bytes") {
  test_bytes<uint8_t>("1 byte");
  test_bytes<uint16_t>("2 byte");
  test_bytes<uint32_t>("4 byte");
}

/*
TEST_CASE("Chars") {
  SUBCASE("Long") {
  }
  SUBCASE("Short") {
    string textstr = "I'm not crazy, my mother had me tested.";
    vector<char> text(textstr.begin(), textstr.end());
    size_t sz = 300;
    vector<byte> data(sz);
    SUBCASE("Serialization and deserialization are invertible") {
      string result;
      Chars<uint8_t>::serialize(text, data.data());
      Chars<uint8_t>::deserialize(data.data(), result);
      cout << string(text.begin(), text.end());
      CHECK(string(text.begin(), text.end()) == result);
    }
  }
}
*/

} // namespace map_values

} // namespace linpipe
} // namespace kbelik
