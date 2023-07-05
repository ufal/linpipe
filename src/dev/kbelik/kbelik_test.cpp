// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cmath>
#include <cstring>
#include <fstream>

#include "common.h"
#include "lib/doctest.h"
#include "lib/json.h"

#include "dev/kbelik/agnostic_kbelik.h"
#include "dev/kbelik/byte_serializer_deserializer.h"
#include "dev/kbelik/dynamic_map.h"
#include "dev/kbelik/huffman.h"
#include "dev/kbelik/named_entity.h"

//#include "dev/kbelik/map_values/agnostic_entity_info.h"
#include "dev/kbelik/map_values/agnostic_entity_info_huff.h"
#include "dev/kbelik/map_values/bits.h"
#include "dev/kbelik/map_values/bytes.h"
#include "dev/kbelik/map_values/bytes_vli.h"
//#include "dev/kbelik/map_values/chars.h"
#include "dev/kbelik/map_values/id.h"
#include "dev/kbelik/map_values/int4.h"
#include "dev/kbelik/map_values/int8.h"
#include "dev/kbelik/map_values/simple_json.h"
#include "dev/kbelik/map_values/typed_value.h"
#include "dev/kbelik/map_values/vli.h"

#include "dev/kbelik/persistent_map.h"
#include "dev/kbelik/id.h"
#include "dev/kbelik/typed_value.h"

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
*/

/*

TEST_CASE("Specific kbelik") {
}
*/

TEST_CASE("TypedValue") {
  SUBCASE("general functionality tests with subtype string") {
    TypedValue tv(TypedValueSubtype::string, "test");
    SUBCASE("Testing get_as_string") {
      pair<string, string> result = tv.get_as_string();
      CHECK(result.first == "string");
      CHECK(result.second == "test");
    }
    SUBCASE("Testing get_val") {
      string value = tv.get_val();
      CHECK(value == "test");
    }
    SUBCASE("Testing get_subtype") {
      TypedValueSubtype subtype = tv.get_subtype();
      CHECK(subtype == TypedValueSubtype::string);
    }
  }

  SUBCASE("Default constructor") {
    TypedValue tv;
  }

  SUBCASE("Non-default constructor") {
    TypedValue tv(TypedValueSubtype::qid, "Q123");
    CHECK(tv.get_subtype() == TypedValueSubtype::qid);
    CHECK(tv.get_val() == "Q123");
  }

  SUBCASE("get_as_string") {
    TypedValue tv(TypedValueSubtype::qid, "Q123");
    auto p = tv.get_as_string();
    CHECK(p.first == "qid");
    CHECK(p.second == "Q123");
  }

  SUBCASE("Equality operator") {
    TypedValue tv1(TypedValueSubtype::qid, "Q123");
    TypedValue tv2(TypedValueSubtype::qid, "Q123");
    CHECK(tv1 == tv2);
  }
  SUBCASE("Non-default constructor with specifiers") {
    TypedValue tv(TypedValueSubtype::monolingualtext, "Hello", "en");
    CHECK(tv.get_subtype() == TypedValueSubtype::monolingualtext);
    CHECK(tv.get_val() == "Hello");
  }
  
  SUBCASE("String constructor") {
    TypedValue tv("qid", "Q123");
    CHECK(tv.get_subtype() == TypedValueSubtype::qid);
    CHECK(tv.get_val() == "Q123");
   }

  SUBCASE("String constructor with specifiers") {
    TypedValue tv("monolingualtext:en", "Hello");
    CHECK(tv.get_subtype() == TypedValueSubtype::monolingualtext);
    CHECK(tv.get_val() == "Hello");
  }

  SUBCASE("Check get_as_string with quantity") {
    TypedValue tv(TypedValueSubtype::quantity, "42", "Q3232");
    auto p = tv.get_as_string();
    CHECK(p.first == "quantity:Q3232"); 
    CHECK(p.second == "42.000000");
  }

  SUBCASE("Check get_as_string with monolingualtext") {
    TypedValue tv(TypedValueSubtype::monolingualtext, "Hello", "en");
    auto p = tv.get_as_string();
    CHECK(p.first == "monolingualtext:en");
    CHECK(p.second == "Hello");
  }

  SUBCASE("Equality operator with different objects") {
    TypedValue tv1(TypedValueSubtype::qid, "Q123");
    TypedValue tv2(TypedValueSubtype::qid, "Q124");
    CHECK_FALSE(tv1 == tv2);
  }

  SUBCASE("string_to_subtype with unknown subtype") {
    CHECK_THROWS_AS(TypedValue::string_to_subtype("unknown"), LinpipeError);
  }

  SUBCASE("subtype_to_string with unknown subtype") {
    CHECK_THROWS_AS(TypedValue::subtype_to_string(static_cast<TypedValueSubtype>(9999)), LinpipeError);
  }

  SUBCASE("String constructor with unknown subtype") {
    CHECK_THROWS_AS(TypedValue("unknown", "value"), LinpipeError);
  }
  SUBCASE("Real usecase") {
    Json big = Json::parse(R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "pof": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"]}})del");
    auto clms = big["claims"];
    for(auto& [key, val] : clms.items()) {
      string sub_type = val.at(0).at(0);
      string type_value = val.at(0).at(1);
      auto tv = TypedValue(sub_type, type_value);
      CHECK(tv.get_as_string() == pair<string, string>(sub_type, type_value));
    }
  }

}

TEST_CASE("AgnosticEntityInfo") {
  SUBCASE("Basic functionality") {
    Json big = Json::parse(R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "pof": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"]}})del");
    auto aei = AgnosticEntityInfo(big);
    CHECK(aei.claims["Commons category"].tv.get_as_string() == pair<string, string>("string", "Theodor-Lessing-Haus (Hannover)"));
    CHECK(aei.claims["Commons category"].optionals.empty());
    CHECK(!aei.claims["located on street"].optionals.empty());
  }
  SUBCASE("Empty contructor") {
    auto aei = AgnosticEntityInfo();
    CHECK(aei.claims.empty());
  }
  SUBCASE("Equality") {
    Json big = Json::parse(R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "pof": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"]}})del");
    auto aei = AgnosticEntityInfo(big);
    auto aei2 = AgnosticEntityInfo(big);
    auto aei3 = AgnosticEntityInfo();
    SUBCASE("=") {
      CHECK(aei == aei);
      CHECK(aei == aei2);
      CHECK(aei3 == aei3);
    }
    SUBCASE("!=") {
      CHECK(aei != aei3);
    }
  }
  SUBCASE("fictional") {
    SUBCASE("Ternary::True") {
      Json big = Json::parse(R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "pof": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"], "fictional": true}})del");
      auto aei = AgnosticEntityInfo(big);
      CHECK(aei.fictional == Ternary::True);
    }
    SUBCASE("Ternary::False") {
      Json big = Json::parse(R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "pof": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"], "fictional": false}})del");
      auto aei = AgnosticEntityInfo(big);
      CHECK(aei.fictional == Ternary::False);
    }
    SUBCASE("Ternary::Maybe") {
      Json big = Json::parse(R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "pof": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"] }})del");
      auto aei = AgnosticEntityInfo(big);
      CHECK(aei.fictional == Ternary::Maybe);
    }
  }
}

TEST_CASE("Named entity converter") {
  SUBCASE("named entity to bool") {
    vector<NamedEntity> ne = {static_cast<NamedEntity>(5)};
    vector<bool> expected = {false, false, false, false, false, true};
    CHECK(expected == NamedEntityConverter::ne_to_bools(ne));
  }
  SUBCASE("Bool to named entity") {
    vector<bool> vb = {true, false, false, true};
    vector<NamedEntity> expected = {
      static_cast<NamedEntity>(0),
      static_cast<NamedEntity>(3)
      };
    CHECK(expected == NamedEntityConverter::bools_to_ne(vb));
  }
}

TEST_CASE("ID") {
  ID nq = ID("STR123");
  ID q = ID("Q1234");
  SUBCASE("knows its type") {
    CHECK(q.is_qid());
    CHECK(!nq.is_qid());
  }
  SUBCASE("'qid' field") {
    CHECK(1234 == q.qid());
    CHECK(-1 == nq.qid());
  }
  SUBCASE("string representation") {
    CHECK("STR123" == nq.str());
    CHECK("Q1234" == q.str());
  }
  SUBCASE("equality") {
    ID q2 = ID("Q1234");
    CHECK(q2 == q);
    CHECK(q2 != nq);
  }
}

namespace map_values {

TEST_CASE("Bits") {
  SUBCASE("short") {
    vector<bool> v = {true, true, false, false, true, false};
    vector<byte> data;
    Bits::serialize(v, data);
    SUBCASE("length") {
      CHECK(Bits::length(v) == Bits::length(data.data()));
    }
    SUBCASE("serialization and deserialization") {
      vector<bool> v2;
      Bits::deserialize(data.data(), v2);
      CHECK(v2 == v);
    }
  }
  SUBCASE("long multiple of 8") {
    vector<bool> v(3000);
    for (int i = 0; i < 1000; ++i)
      v[i] = true;
    vector<byte> data;
    Bits::serialize(v, data);
    SUBCASE("length") {
      CHECK(Bits::length(v) == Bits::length(data.data()));
    }
    SUBCASE("serialization and deserialization") {
      vector<bool> v2;
      Bits::deserialize(data.data(), v2);
      CHECK(v2 == v);
    }
  }
  SUBCASE("long not multiple of 8") {
    vector<bool> v(3003);
    for (int i = 0; i < 1000; ++i)
      v[i] = true;
    vector<byte> data;
    Bits::serialize(v, data);
    SUBCASE("length") {
      CHECK(Bits::length(v) == Bits::length(data.data()));
    }
    SUBCASE("serialization and deserialization") {
      vector<bool> v2;
      Bits::deserialize(data.data(), v2);
      CHECK(v2 == v);
    }
  }
}

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
TEST_CASE("Int8") {
  byte* data = new byte[4];
  data[0] = (byte)2;
  data[1] = (byte)1;
  data[2] = (byte)2;
  data[3] = (byte)2;
  data[0] = (byte)2;
  data[1] = (byte)1;
  data[2] = (byte)2;
  data[3] = (byte)2;

  int64_t expected = (int64_t)data[0];
  for (int i = 1; i <= 7; ++i)
    expected |= (int64_t)data[i] << (8 * i);

  SUBCASE("Correct length, serialized") {
    CHECK(8 == Int8::length(data));
  }
  SUBCASE("Correct length, deserialized") {
    CHECK(8 == Int8::length(42));
  }
  SUBCASE("Correct deserialization") {
    int64_t res;
    Int8::deserialize(data, res);
    CHECK(expected == res);
  }
  SUBCASE("Correct serialization") {
    vector<byte> res;
    Int8::serialize(expected, res);
    for (int i = 0; i < 4; ++i) 
      CHECK(res[i] == data[i]);
  }
  delete[] data;
}

TEST_CASE("ID -- map value") {
  vector<byte> data_q, data_n;
  auto qid = linpipe::kbelik::ID("Q12222234");
  auto no_qid = linpipe::kbelik::ID("PER");
  ID::serialize(qid, data_q);
  ID::serialize(no_qid, data_n);
  SUBCASE("Lengths") {
    SUBCASE("QID")
      CHECK(ID::length(qid) == ID::length(data_q.data()));
    SUBCASE("NO-QID")
      CHECK(ID::length(no_qid) == ID::length(data_n.data()));
  }
  SUBCASE("Serialization and deserialization") {
    linpipe::kbelik::ID id;
    SUBCASE("QID") {
      ID::deserialize(data_q.data(), id);
      CHECK(id == qid);
    }
    SUBCASE("NO-QID") {
      ID::deserialize(data_n.data(), id);
      CHECK(id == no_qid);
    }
  }
}

TEST_CASE("TypedValue -- map value") {
  vector<byte> data;
  ByteSerializerDeserializers bsds;
  string alpha = "abcdefghijklmnopqrstuvwxyz";
  bsds.huffman.add(alpha);
  transform(alpha.begin(), alpha.end(), alpha.begin(), ::toupper);
  bsds.huffman.add(alpha);
  bsds.huffman.add("1234567890 :,.;*+-/");
  SUBCASE("qid") {
    string qid = "Q1234567890987654";
    bsds.huffman.build();
    auto tv = linpipe::kbelik::TypedValue("qid", qid);
    TypedValue::serialize(tv, bsds, data);
    linpipe::kbelik::TypedValue tv2;
    TypedValue::deserialize(data.data(), bsds, tv2);
    CHECK(qid == tv2.get_val());
  }
  SUBCASE("non qid") {
    string str = "This is some peculiar string";
    bsds.huffman.build();
    auto tv = linpipe::kbelik::TypedValue("string", str);
    TypedValue::serialize(tv, bsds, data);
    linpipe::kbelik::TypedValue tv2;
    TypedValue::deserialize(data.data(), bsds, tv2);
    CHECK(str == tv2.get_val());
  }
  SUBCASE("monolingualtext") {
    string str = "Řeřicha";
    bsds.huffman.add("Řeřicha");
    bsds.huffman.build();
    auto tv = linpipe::kbelik::TypedValue("monolingualtext:cs", str);
    TypedValue::serialize(tv, bsds, data);
    linpipe::kbelik::TypedValue tv2;
    TypedValue::deserialize(data.data(), bsds, tv2);
    CHECK(pair<string, string>("monolingualtext:cs", str) == tv2.get_as_string());
  }
  SUBCASE("time julian") {
    string str = "+1970+08-22";
    bsds.huffman.build();
    auto tv = linpipe::kbelik::TypedValue("time:julian", str);
    TypedValue::serialize(tv, bsds, data);
    linpipe::kbelik::TypedValue tv2;
    TypedValue::deserialize(data.data(), bsds, tv2);
    CHECK(pair<string, string>("time:julian", str) == tv2.get_as_string());
  }
  SUBCASE("time gregorian") {
    string str = "+1970+08-22";
    bsds.huffman.build();
    auto tv = linpipe::kbelik::TypedValue("time:gregorian", str);
    TypedValue::serialize(tv, bsds, data);
    linpipe::kbelik::TypedValue tv2;
    TypedValue::deserialize(data.data(), bsds, tv2);
    CHECK(pair<string, string>("time:gregorian", str) == tv2.get_as_string());
  }
  SUBCASE("quantity") {
    SUBCASE("with description string") {
      string str = "111129292929";
      bsds.huffman.build();
      auto tv = linpipe::kbelik::TypedValue("quantity:Q1:desc string", str);
      TypedValue::serialize(tv, bsds, data);
      linpipe::kbelik::TypedValue tv2;
      TypedValue::deserialize(data.data(), bsds, tv2);
      CHECK(tv == tv2);
    }
    SUBCASE("without description string") {
      string str = "111129292929";
      bsds.huffman.build();
      auto tv = linpipe::kbelik::TypedValue("quantity:Q1", str);
      TypedValue::serialize(tv, bsds, data);
      linpipe::kbelik::TypedValue tv2;
      TypedValue::deserialize(data.data(), bsds, tv2);
      CHECK(tv == tv2);
    }
  }
  SUBCASE("Real usecase") {
    string raw = R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "pof": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"]}})del";
    Json big = Json::parse(raw);
    auto huff = &bsds.huffman;
    huff->add(raw);
    huff->build();
    auto clms = big["claims"];
    for(auto& [key, val] : clms.items()) {
      string sub_type = val.at(0).at(0);
      string type_value = val.at(0).at(1);
      auto tv = linpipe::kbelik::TypedValue(sub_type, type_value);
      TypedValue::serialize(tv, bsds, data);
      linpipe::kbelik:: TypedValue tv2;
      TypedValue::deserialize(data.data(), bsds, tv2);
      SUBCASE("equality") {
        CHECK(tv == tv2);
      }
      SUBCASE("length") {
        CHECK(TypedValue::length(data.data()) == TypedValue::length(tv, bsds));
      }
    }
  }
}

TEST_CASE("AgnosticEntityInfoHuffman -- map value") { 
  ByteSerializerDeserializers bsds;
  string raw = R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "pof": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"]}})del";
  Json big = Json::parse(raw);
  auto huff = &bsds.huffman;
  huff->add(raw);
  huff->add("|$");
  huff->build();
  vector<byte> data;
  auto ori = linpipe::kbelik::AgnosticEntityInfo(big);
  AgnosticEntityInfoH::serialize(ori, bsds, data);
  
  SUBCASE("Lengths") {
    size_t l1, l2;
    l1 = AgnosticEntityInfoH::length(ori, bsds);
    l2 = AgnosticEntityInfoH::length(data.data());
    CHECK(l1 == l2);
  }
  SUBCASE("Serialization/Desirizalization") {
    linpipe::kbelik::AgnosticEntityInfo aei;
    AgnosticEntityInfoH::deserialize(data.data(), bsds, aei);
    CHECK(aei.claims == ori.claims);
  }
}
/*

TEST_CASE("AgnosticEntityInfo -- map value") { 

  Json big = Json::parse(R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "pof": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"]}})del");
  vector<byte> data;
  auto clms = big["claims"];
  auto ori = linpipe::kbelik::AgnosticEntityInfo(clms);
  AgnosticEntityInfo::serialize(ori, data);

  SUBCASE("Lengths") {
    size_t l1, l2;
    l1 = AgnosticEntityInfo::length(clms);
    l2 = AgnosticEntityInfo::length(data.data());
    CHECK(l1 == l2);
  }
  SUBCASE("Serialization/Desirizalization") {
    linpipe::kbelik::AgnosticEntityInfo aei;
    AgnosticEntityInfo::deserialize(data.data(), aei);
    CHECK(aei.claims == ori.claims);
  }
}
*/

template<typename SizeType>
void test_bytes(const char* name) {
  SUBCASE(name) {
    SizeType sz = 3 + sizeof(SizeType);
    vector<byte> d = vector<byte>(sz);
    byte* data = d.data();
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
  }
}

TEST_CASE("Bytes") {
  test_bytes<uint8_t>("1 byte");
  test_bytes<uint16_t>("2 byte");
  test_bytes<uint32_t>("4 byte");
}

TEST_CASE("BytesVLI") {
  auto test = [](vector<byte> data) {
    vector<byte> data2;
    BytesVLI::serialize(data, data2);
    SUBCASE("Correct length") {
      CHECK(BytesVLI::length(data2.data()) == BytesVLI::length(data));
    }
    SUBCASE("Correct de/serialization") {
      vector<byte> data3;
      BytesVLI::deserialize(data2.data(), data3);
      CHECK(data3 == data);
    }
  };
  vector<byte> d;
  int x = 1;
  for (int i = 0; i < 10000000; ++i) {
    d.push_back((byte)(i % 256));
    if (i == x) {
      test(d);
      x *= 100;
    }
  }
}

int msb(uint64_t x) {
  int res = 0;
  while (x) {
    res++;
    x >>= 1;
  }
  return res;
}

TEST_CASE("VLI") {
  for (uint64_t i = 0; i < 288230376151711744; i = 3 * i + 100) {
    vector<byte> data;
    VLI::serialize(i, data);
    SUBCASE("Correct length from serialized") {
      size_t expected = max((size_t)1, (size_t)ceil(msb(i) / 7.0));
      //cout << i << ' ' << expected << '\n'; 
      CHECK(expected == VLI::length(data.data()));
    }
    SUBCASE("Correct length from deserialized") {
      size_t expected = max((size_t)1, (size_t)ceil(msb(i) / 7.0));
      CHECK(expected == VLI::length(i));
    }
    SUBCASE("Serialization/deserialization works.") {
      uint64_t des;
      VLI::deserialize(data.data(), des);
      CHECK(des == i);
    }
  }
}
TEST_CASE("SimpleJson") {
  Json small = Json::parse(R"(
    {
        "pi": 3.141,
        "happy": true
    }
  )");

  Json big = Json::parse(R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "part of": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"]}})del");

  auto length = [](Json& j) {
    vector<byte> s;
    SimpleJson::serialize(j, s);

    size_t se, de;
    de = SimpleJson::length(j);
    se = SimpleJson::length(s.data());
    CHECK(de == se);
  };

  auto serialization_deserialization = [](Json& j) {
    vector<byte> s;
    SimpleJson::serialize(j, s);

    Json des;
    SimpleJson::deserialize(s.data(), des);
    CHECK(des.dump() == j.dump());
  };



  SUBCASE("Equal lengths small") {
    length(small);
  }
  SUBCASE("Serialization/deserialization works small.") {
    serialization_deserialization(small);
  }
  SUBCASE("Equal lengths big") {
    length(big);
  }
  SUBCASE("Serialization/deserialization works big.") {
    serialization_deserialization(big);
  }
  SUBCASE("Map serialization") {
    auto m = map<string, string>();
    m["a"] = "hi";
    m["xyz"] = "abc";
    vector<byte> s;

    Json out;

    SimpleJson::serialize(m, s);
    SimpleJson::deserialize(s.data(), out);
    CHECK(m["a"] == out["a"]);
    CHECK(m["xyz"] == out["xyz"]);
  }
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
  
TEST_CASE("HuffmanTree") {
  SUBCASE("Encode and decode short") {
    vector<byte> data;
    string t1, t2;
    auto huff = HuffmanTree();
    huff.add("abcdefgh");
    huff.build();
    t1 = "abcdefgh";
    huff.encode(t1, data);
    huff.decode(data.data(),t2);
    CHECK(t1 == t2);
    t1 = "";
    huff.encode(t1, data);
    huff.decode(data.data(),t2);
    CHECK(t1 == t2);
    t1 = "a";
    huff.encode(t1, data);
    huff.decode(data.data(),t2);
    CHECK(t1 == t2);
    t1 = "aaaaaaaaaaaaaababababab";
    huff.encode(t1, data);
    huff.decode(data.data(),t2);
    CHECK(t1 == t2);
  }
  SUBCASE("Non-english chars") {
    vector<byte> data;
    string t1, t2;
    auto huff = HuffmanTree();
    huff.add("abcdefgh");
    huff.add("čšďëéŕúôçóůéöľáĺč");
    huff.build();
    t1 = "abcdefgh";
    huff.encode(t1, data);
    huff.decode(data.data(),t2);
    CHECK(t1 == t2);
    t1 = "čšďëéŕúáĺč";
    huff.encode(t1, data);
    huff.decode(data.data(),t2);
    CHECK(t1 == t2);
  }
  SUBCASE("Encode and decode long") {
    vector<byte> data;
    string t1, t2;
    auto huff = HuffmanTree();
    string alpha = "abcdefghijklmnopqrstuvwxyz";
    huff.add(alpha);
    transform(alpha.begin(), alpha.end(), alpha.begin(), ::toupper);
    huff.add(alpha);
    huff.add("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
    huff.add("?!,");
    huff.build();
    t1 = "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo. Nemo enim ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit, sed quia consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt. Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur? Quis autem vel eum iure reprehenderit qui in ea voluptate velit esse quam nihil molestiae consequatur, vel illum qui dolorem eum fugiat quo voluptas nulla pariatur?";
    huff.encode(t1, data);
    huff.decode(data.data(),t2);
    CHECK(t1 == t2);
  }
  SUBCASE("Dump") {
    vector<byte> data;
    string t1, t2;
    auto huff = HuffmanTree();
    string alpha = "abcdefghijklmnopqrstuvwxyz";
    huff.add(alpha);
    huff.add("aaaaaaabbc");
    huff.build();
    huff.serialize(data);
    auto huff2 = HuffmanTree();
    huff2.deserialize(data.data());
    huff2.encode(alpha, data);
    huff2.decode(data.data(),t2);
    CHECK(alpha == t2);
  }
  SUBCASE("Empty build error") {
    auto huff = HuffmanTree();
    CHECK_THROWS_AS(huff.build(), const LinpipeError);
  }
  SUBCASE("Repeated build error") {
    auto huff = HuffmanTree();
    huff.add("a");
    huff.build();
    CHECK_THROWS_AS(huff.build(), const LinpipeError);
  }
}

} // namespace kbelik
} // namespace linpipe
