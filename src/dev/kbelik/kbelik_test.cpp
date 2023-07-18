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
#include "dev/kbelik/agnostic_entity_info.h"
#include "dev/kbelik/byte_serializer_deserializer.h"
#include "dev/kbelik/dynamic_map.h"
#include "dev/kbelik/general_kbelik.h"
#include "dev/kbelik/huffman.h"
#include "dev/kbelik/named_entity.h"
#include "dev/kbelik/specific_entity_info.h"

//#include "dev/kbelik/map_values/agnostic_entity_info.h"
#include "dev/kbelik/map_values/agnostic_entity_info_huff.h"
#include "dev/kbelik/map_values/bits.h"
#include "dev/kbelik/map_values/bytes.h"
#include "dev/kbelik/map_values/bytes_vli.h"
//#include "dev/kbelik/map_values/chars.h"
#include "dev/kbelik/map_values/fli.h"
#include "dev/kbelik/map_values/id.h"
#include "dev/kbelik/map_values/int4.h"
#include "dev/kbelik/map_values/int8.h"
#include "dev/kbelik/map_values/simple_json.h"
#include "dev/kbelik/map_values/specific_entity_info_huff.h"
#include "dev/kbelik/map_values/typed_value.h"
#include "dev/kbelik/map_values/vli.h"

#include "dev/kbelik/persistent_map.h"
#include "dev/kbelik/id.h"
#include "dev/kbelik/typed_value.h"

namespace linpipe {
namespace kbelik {

TEST_CASE("Dynamic map") {
  SUBCASE("Int4") {
    auto mv = map_values::Int4();
    auto mk = map_values::Int4();
    auto dm = DynamicMap<map_values::Int4, map_values::Int4>(mk, mv);
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
      REQUIRE(ss.str().size() > 3 * map_values::Int4().length(0) + sizeof(int) * 3);
    }
  }
  SUBCASE("Bytes") {
    auto mv = map_values::Bytes<int8_t>();
    auto mk = map_values::Int4();
    auto dm = DynamicMap<map_values::Int4, map_values::Bytes<int8_t>>(mk, mv);
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
  }SUBCASE("ID") {
    auto mv = map_values::Int4();
    auto mk = map_values::ID();
    auto dm = DynamicMap<map_values::ID, map_values::Int4>(mk, mv);
    SUBCASE("Add, erase -- big") {
      REQUIRE(dm.length() == 0);
      int to_add = 200;
      for (int i = 0; i < to_add; ++i) {
        dm.add(ID("Q" + to_string(i)), 10 + i); 
      }
      CHECK(dm.length() == to_add);
      for (int i = 0; i < to_add; ++i) {
        dm.erase(ID("Q" + to_string(i)));
      }
      CHECK(dm.length() == 0);
    }
    SUBCASE("Add, erase -- small") {
      REQUIRE(dm.length() == 0);
      dm.add(ID("Q10"), 0);
      dm.add(ID("Q10"), 0);
      CHECK(dm.length() == 1);
      dm.erase(ID("Q10"));
      CHECK(dm.length() == 0);
    }
    SUBCASE("find present") {
      REQUIRE(dm.length() == 0);
      int expected = 0;
      dm.add(ID("Q10"), expected);
      int res;
      bool flag = dm.find(ID("Q10"), res);
      CHECK(res == expected);
      CHECK(flag);
      dm.erase(ID("Q10"));
    }
    SUBCASE("find not present") {
      REQUIRE(dm.length() == 0);
      int expected = 0;
      dm.add(ID("Q10"), expected);
      int res;
      bool flag = dm.find(ID("Q11"), res);
      CHECK(!flag);
      dm.erase(ID("Q10"));
      flag = dm.find(ID("Q10"), res);
      CHECK(!flag);
    }
    SUBCASE("basic save map") {
      REQUIRE(dm.length() == 0);
      dm.add(ID("Q10"), 1);
      dm.add(ID("Q20"), 2);
      dm.add(ID("Q30"), 3);
      stringstream ss;
      REQUIRE(ss.str() == "");
      dm.save_map(ss, test);
    }
  }
}

TEST_CASE("Persistent map") {
  SUBCASE("mock") {
    auto mv = map_values::Int4();
    auto mk = map_values::ID();
    auto dm = DynamicMap<map_values::ID, map_values::Int4>(mk, mv);
    int vals_cnt = 10;
    for (int i = 0; i < vals_cnt; ++i) 
      dm.add(ID("Q" + to_string(i)), i);
    filesystem::create_directories("./temp");
    ofstream ofs("temp/test_map.bin", ofstream::out | ofstream::binary);
    dm.save_map(ofs, test);
    ofs.close();
    filesystem::path fp("temp/test_map.bin");
    auto mv2 = map_values::Int4();
    auto pm = PersistentMap<map_values::ID, map_values::Int4>(fp, mk, mv2);
    SUBCASE("close") {
      int res;
      pm.close();
      CHECK_THROWS_AS(pm.find(ID("Q1"), res), const LinpipeError);
    }
    SUBCASE("find success") {
      map_values::Int4::Type res;
      bool flag;
      for (int i = 0; i < vals_cnt; ++i) { 
        flag = pm.find("Q" + to_string(i), res);
        CHECK(flag);
        CHECK(res == i);
      }
    }
    SUBCASE("find fail") {
      int res;
      bool flag = pm.find(ID("Q" + to_string(vals_cnt + 10)), res);
      CHECK(!flag);
    }
    SUBCASE("map type loaded") {
      MapType t = pm.get_map_type();
      CHECK(t == test);
    }
    SUBCASE("Persistent map with offset") {
      ofs.open("temp/test_map_offset.bin", ofstream::out | ofstream::binary);
      char data[2] = {(char)1, (char)2};
      ofs.write(data, 2);
      dm.save_map(ofs, test);
      ofs.close();
      filesystem::path fp_offset("temp/test_map_offset.bin");
      auto pm_offset = PersistentMap<map_values::ID, map_values::Int4>(fp_offset, mk, mv, 2);
      int res;
      bool flag = pm_offset.find(ID("Q1"), res);
      CHECK(flag);
      CHECK(res == 1);
      flag = pm_offset.find(ID("Q" + to_string(vals_cnt + 10)), res);
      CHECK(!flag);
    }
    SUBCASE("Persistent map with length") {
      ofs.open("temp/test_map_length.bin", ofstream::out | ofstream::binary);
      dm.save_map(ofs, test);
      size_t len = ofs.tellp();
      char data[2] = {(char)1, (char)2};
      ofs.write(data, 2);
      ofs.close();
      filesystem::path fp_length("temp/test_map_length.bin");
      auto pm_length = PersistentMap<map_values::ID, map_values::Int4>(fp_length, mk, mv, 0, len);
      int res;
      bool flag = pm_length.find(ID("Q1"), res);
      CHECK(flag);
      CHECK(res == 1);
      flag = pm_length.find(ID("Q" + to_string(vals_cnt + 10)), res);
      CHECK(!flag);
    }
  }
  SUBCASE("Real usecase") {
    string raw1 = R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "part of": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"]}})del";
    string raw2 = R"del({"qid": "Q96890921", "claims": {"instance of": [["qid", "Q5:human", {}]], "sex or gender": [["qid", "Q6581097:male", {}]], "country of citizenship": [["qid", "Q142:France", {}]], "occupation": [["qid", "Q42973:architect", {}]], "date of birth": [["time:gregorian", "+1814-05-21T00:00:00Z", {}]], "place of birth": [["qid", "Q971866:Croutelle", {}]], "date of death": [["time:gregorian", "+1889-01-23T00:00:00Z", {}]], "place of death": [["qid", "Q949093:Migné-Auxances", {}]], "given name": [["qid", "Q1499767:Jean-Baptiste", {}]], "languages spoken, written or signed": [["qid", "Q150:French", {}]]}, "named_entities": {"type": ["PER"]}})del";
    string raw3 = R"del({"qid": "Q49496872", "claims": {"LfDS object ID": [["external-id", "09241968", {}]], "instance of": [["qid", "Q3947:house", {}]], "coordinate location": [["globe-coordinate", "50.844588063567 12.455779739649", {}]], "inception": [["time:gregorian", "+1920-01-17T00:00:00Z", {}]], "heritage designation": [["qid", "Q11691318:cultural heritage monument in Germany", {}]], "country": [["qid", "Q183:Germany", {}]], "located in the administrative territorial entity": [["qid", "Q20083:Meerane", {}]], "street address": [["monolingualtext:de", "Philippstraße 68; 70; 72; 74", {}]]}, "named_entities": {"type": ["LOC"]}})del";
    string raw4 = R"del({"qid": "Q66638937", "claims": {"instance of": [["qid", "Q66619497:HDFC Bank branch", {}]], "country": [["qid", "Q668:India", {}]], "operator": [["qid", "Q631047:HDFC Bank call now‪ 09382691063‬= ‪09382691063‬", {}]], "located in the administrative territorial entity": [["qid", "Q620297:Umaria district", {}]], "Indian Financial System Code": [["external-id", "HDFC0001778", {}]]}, "named_entities": {"type": ["ORG", "LOC"]}})del";
    string raw5 = R"del({"qid": "Q112471937", "claims": {"occupation": [["qid", "Q36180:writer", {}], ["qid", "Q201788:historian", {}]], "date of death": [["time:gregorian", "+2012-00-00T00:00:00Z", {}]], "NKCR AUT ID": [["external-id", "jo2013794598", {"subject named as": [["string", "Cvetkov, Sergej Vasil'jevič"]]}]], "given name": [["qid", "Q18946707:Sergej", {"series ordinal": [["string", "1"]]}]], "date of birth": [["time:gregorian", "+1952-00-00T00:00:00Z", {}]], "instance of": [["qid", "Q5:human", {}]], "VIAF ID": [["external-id", "120746555", {}]], "ISNI": [["external-id", "0000 0004 4928 8498", {}]], "Library of Congress authority ID": [["external-id", "n00092256", {}]], "Bibliothèque nationale de France ID": [["external-id", "150142744", {}]], "IdRef ID": [["external-id", "137102690", {}]], "PLWABN ID": [["external-id", "9810632583205606", {}]], "National Library of Israel J9U ID": [["external-id", "987007439703105171", {}]]}, "named_entities": {"type": ["PER"]}})del";

    auto huff = HuffmanTree();
    huff.add(raw1);
    huff.add(raw2);
    huff.add(raw3);
    huff.add(raw4);
    huff.add(raw5);
    huff.build();

    ByteSerializerDeserializers bsds;
    bsds.huffman = huff;

    auto mv = map_values::AgnosticEntityInfoH(bsds.huffman);
    auto mk = map_values::ID();

    auto dm = DynamicMap<map_values::ID, map_values::AgnosticEntityInfoH>(mk, mv);
    for (auto line : {raw1, raw2, raw3, raw4, raw5}) {
      auto js = Json::parse(line);
      string q_str = js["qid"];
      auto id = ID(q_str);
      auto aei = AgnosticEntityInfo(js);
      dm.add(id, aei);
    }

    ofstream ofs("temp/test_map.bin", ofstream::out | ofstream::binary);
    dm.save_map(ofs, test);
    ofs.close();

    filesystem::path fp("temp/test_map.bin");
    auto mv2 = map_values::AgnosticEntityInfoH(bsds.huffman);
    auto pm = PersistentMap<map_values::ID, map_values::AgnosticEntityInfoH>(fp, mk, mv2, 0, -1);
    SUBCASE("find") {
      auto aei = AgnosticEntityInfo();
      pm.find(ID("Q66638937"), aei);
      CHECK(aei.claims.size() == 5);
      CHECK(find(aei.named_entities.begin(), aei.named_entities.end(), NamedEntity::ORG) != aei.named_entities.end());
      CHECK(find(aei.named_entities.begin(), aei.named_entities.end(), NamedEntity::LOC) != aei.named_entities.end());
    }
  }
}

TEST_CASE("Agnostic kbelik") {
  string raw1 = R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "part of": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"]}})del";
  string raw2 = R"del({"qid": "Q96890921", "claims": {"instance of": [["qid", "Q5:human", {}]], "sex or gender": [["qid", "Q6581097:male", {}]], "country of citizenship": [["qid", "Q142:France", {}]], "occupation": [["qid", "Q42973:architect", {}]], "date of birth": [["time:gregorian", "+1814-05-21T00:00:00Z", {}]], "place of birth": [["qid", "Q971866:Croutelle", {}]], "date of death": [["time:gregorian", "+1889-01-23T00:00:00Z", {}]], "place of death": [["qid", "Q949093:Migné-Auxances", {}]], "given name": [["qid", "Q1499767:Jean-Baptiste", {}]], "languages spoken, written or signed": [["qid", "Q150:French", {}]]}, "named_entities": {"type": ["PER"]}})del";
  string raw3 = R"del({"qid": "Q49496872", "claims": {"LfDS object ID": [["external-id", "09241968", {}]], "instance of": [["qid", "Q3947:house", {}]], "coordinate location": [["globe-coordinate", "50.844588063567 12.455779739649", {}]], "inception": [["time:gregorian", "+1920-01-17T00:00:00Z", {}]], "heritage designation": [["qid", "Q11691318:cultural heritage monument in Germany", {}]], "country": [["qid", "Q183:Germany", {}]], "located in the administrative territorial entity": [["qid", "Q20083:Meerane", {}]], "street address": [["monolingualtext:de", "Philippstraße 68; 70; 72; 74", {}]]}, "named_entities": {"type": ["LOC"]}})del";
  string raw4 = R"del({"qid": "Q66638937", "claims": {"instance of": [["qid", "Q66619497:HDFC Bank branch", {}]], "country": [["qid", "Q668:India", {}]], "operator": [["qid", "Q631047:HDFC Bank call now‪ 09382691063‬= ‪09382691063‬", {}]], "located in the administrative territorial entity": [["qid", "Q620297:Umaria district", {}]], "Indian Financial System Code": [["external-id", "HDFC0001778", {}]]}, "named_entities": {"type": ["ORG", "LOC"]}})del";
  string raw5 = R"del({"qid": "Q112471937", "claims": {"occupation": [["qid", "Q36180:writer", {}], ["qid", "Q201788:historian", {}]], "date of death": [["time:gregorian", "+2012-00-00T00:00:00Z", {}]], "NKCR AUT ID": [["external-id", "jo2013794598", {"subject named as": [["string", "Cvetkov, Sergej Vasil'jevič"]]}]], "given name": [["qid", "Q18946707:Sergej", {"series ordinal": [["string", "1"]]}]], "date of birth": [["time:gregorian", "+1952-00-00T00:00:00Z", {}]], "instance of": [["qid", "Q5:human", {}]], "VIAF ID": [["external-id", "120746555", {}]], "ISNI": [["external-id", "0000 0004 4928 8498", {}]], "Library of Congress authority ID": [["external-id", "n00092256", {}]], "Bibliothèque nationale de France ID": [["external-id", "150142744", {}]], "IdRef ID": [["external-id", "137102690", {}]], "PLWABN ID": [["external-id", "9810632583205606", {}]], "National Library of Israel J9U ID": [["external-id", "987007439703105171", {}]]}, "named_entities": {"type": ["PER"]}})del";

  string raw = raw1 + "\n" + raw2 + "\n" + raw3 + "\n" + raw4 + "\n" + raw5;

  istringstream jsons = istringstream(raw);

  ofstream ofs("temp/test_ak.bin", ofstream::out | ofstream::binary);
  GeneralKbelik<map_values::AgnosticEntityInfoH>::build(jsons, ofs);
  ofs.close();
  filesystem::path fp("temp/test_ak.bin");

  SUBCASE("Build") {
    ifstream file(fp, ifstream::ate | ifstream::binary); // Open file and position read pointer at the end
    streampos size = file.tellg(); // Get current position which corresponds to file size
    file.close();
    CHECK(size > 0);
  }
  SUBCASE("Instance methods") {
    SUBCASE("Constructor") {
      SUBCASE("Basic") {
        auto ak = GeneralKbelik<map_values::AgnosticEntityInfoH>(fp);
        auto aei = AgnosticEntityInfo();
        ak.find(ID("Q2417271"), aei);
        CHECK(aei.claims.size() > 0);
      }
      SUBCASE("offset") {
        // TODO
      }
      SUBCASE("length") {
        // TODO
      }
    }
    SUBCASE("Find") {
      auto ak = GeneralKbelik<map_values::AgnosticEntityInfoH>(fp);
      auto aei = AgnosticEntityInfo();
      ak.find(ID("Q66638937"), aei);
      CHECK(aei.claims.size() == 5);
      CHECK(find(aei.named_entities.begin(), aei.named_entities.end(), NamedEntity::ORG) != aei.named_entities.end());
      CHECK(find(aei.named_entities.begin(), aei.named_entities.end(), NamedEntity::LOC) != aei.named_entities.end());
    }
    SUBCASE("Close") {
      auto ak = GeneralKbelik<map_values::AgnosticEntityInfoH>(fp);
      auto aei = AgnosticEntityInfo();
      ak.close();
      CHECK_THROWS_AS(ak.find(ID("Q66638937"), aei), LinpipeError);
    }
  }
}

TEST_CASE("Specific kbelik") {
  string raw1 = R"del({"qid": "Q7365597", "lang": "cs", "label": "Ronen", "aliases": ["רונן"], "description": "příjmení (רונן)"})del";
  string raw2 = R"del({"qid": "Q6723460", "lang": "cs", "description": "asteroid"})del";
  string raw3 = R"del({"qid": "Q6723460", "lang": "cs", "description": "asteroid", "wiki": {"title": "Slunce", "text": "Slunce je jak znamo asteroid"}})del";
  
  string raw = raw1 + "\n" + raw2 + "\n" + raw3;

  istringstream jsons = istringstream(raw);

  ofstream ofs("temp/test_sk.bin", ofstream::out | ofstream::binary);
  GeneralKbelik<map_values::SpecificEntityInfoH>::build(jsons, ofs);
  ofs.close();
  filesystem::path fp("temp/test_sk.bin");

  SUBCASE("Build") {
    ifstream file(fp, ifstream::ate | ifstream::binary); // Open file and position read pointer at the end
    streampos size = file.tellg(); // Get current position which corresponds to file size
    file.close();
    CHECK(size > 0);
  }
  SUBCASE("Instance methods") {
    SUBCASE("Constructor") {
      SUBCASE("Basic") {
        auto sk = GeneralKbelik<map_values::SpecificEntityInfoH>(fp);
        auto sei = SpecificEntityInfo();
        bool flag = sk.find(ID("Q2417271"), sei);
        CHECK(!flag);
      }
      SUBCASE("offset") {
        // TODO
      }
      SUBCASE("length") {
        // TODO
      }
    }
    SUBCASE("Find") {
      auto sk = GeneralKbelik<map_values::SpecificEntityInfoH>(fp);
      auto sei = SpecificEntityInfo();
      sk.find(ID("Q7365597"), sei);
      CHECK(sei.label == "Ronen");
    }
    SUBCASE("Close") {
      auto sk = GeneralKbelik<map_values::SpecificEntityInfoH>(fp);
      auto sei = SpecificEntityInfo();
      sk.close();
      CHECK_THROWS_AS(sk.find(ID("Q66638937"), sei), LinpipeError);
    }
  }
}

TEST_CASE("Specific entity info") {
  string raw1 = R"del({"qid": "Q7365597", "lang": "cs", "label": "Ronen", "aliases": ["רונן"], "description": "příjmení (רונן)"})del";
  string raw2 = R"del({"qid": "Q6723460", "lang": "cs", "description": "asteroid"})del";
  string raw3 = R"del({"qid": "Q6723460", "lang": "cs", "description": "asteroid", "wiki": {"title": "Slunce", "text": "Slunce je jak znamo asteroid"}})del";

  auto js1 = Json::parse(raw1);
  auto js2 = Json::parse(raw2);
  auto js3 = Json::parse(raw3);

  auto sp1 = SpecificEntityInfo(js1);
  auto sp2 = SpecificEntityInfo(js2);
  auto sp3 = SpecificEntityInfo(js3);

  SUBCASE("From json") {
    SUBCASE("==") {
      CHECK(sp1 == sp1);
      CHECK(sp2 == sp2);
      CHECK(sp3 == sp3);
    }
    SUBCASE("!=") {
      CHECK(sp1 != sp2);
      CHECK(sp1 != sp3);
      //CHECK(sp2 != sp3);
    }

    SUBCASE("fields") {
      CHECK(sp1.label == "Ronen");
      CHECK(sp3.description == "asteroid");
      //CHECK(sp3.text == "Slunce je jak znamo asteroid");
      //CHECK(sp1.text == "");
      CHECK(sp1.aliases.find("רונן") != sp1.aliases.end());
    }
  }
}

TEST_CASE("TypedValue") {
  SUBCASE("general functionality tests with subtype string") {
    TypedValue tv("string", "test");
    SUBCASE("Testing get as string") {
      CHECK(tv.get_type_string() == "string");
      CHECK(tv.get_string() == "test");
    }
    SUBCASE("Testing get_subtype") {
      TypedValueSubtype subtype = tv.get_type();
      CHECK(subtype == TypedValueSubtype::string);
    }
  }

  SUBCASE("Default constructor") {
    TypedValue tv;
  }

  SUBCASE("Non-default constructor") {
    TypedValue tv("id", "Q123");
    CHECK(tv.get_type() == TypedValueSubtype::id);
    CHECK(tv.get_string() == ID("Q123"));
  }

  SUBCASE("Equality operator") {
    TypedValue tv1("id", "Q123");
    TypedValue tv2("id", "Q123");
    CHECK(tv1 == tv2);
  }
  SUBCASE("Language") {
    TypedValue tv("monolingualtext:en", "Hello");
    CHECK(tv.get_type() == TypedValueSubtype::monolingualtext);
    CHECK(tv.get_string() == "Hello");
    CHECK(tv.get_language() == "en");
  }

  SUBCASE("Quantity") {
    TypedValue tv("quantity:Q1:km", "123");
    CHECK(tv.get_type() == TypedValueSubtype::quantity);
    TypedValue::Quantity q {ID("Q1"), "km", 123.0};
    CHECK(tv.get_quantity() == q);
  }

  SUBCASE("Time") {
    TypedValue tv("time:gregorian", "2019-01-01T00:00:00Z");
    CHECK(tv.get_type() == TypedValueSubtype::time_gregorian);
    TypedValue::Time tm {2019, 1, 1, "gregorian"};
    CHECK(tv.get_time().year == tm.year);
    CHECK(tv.get_time().month == tm.month);
    CHECK(tv.get_time().day == tm.day);
    CHECK(tv.get_time().calendar == tm.calendar);

  }

  SUBCASE("Not- midnight time") {
    TypedValue tv("time:gregorian", "2019-01-01T10:00:00Z");
    CHECK_THROWS_AS(tv.get_time(), LinpipeError);
  }
  
  SUBCASE("Equality operator with different objects") {
    TypedValue tv1("id", "Q123");
    TypedValue tv2("id", "Q124");
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
    Json big = Json::parse(R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["id", "Q183:Germany", {}]], "instance of": [["id", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["id", "Q1997469:Nord", {}]], "heritage designation": [["id", "Q811165:architectural heritage monument", {}]], "pof": [["id", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["id", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["id", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["id", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"]}})del");
    auto clms = big["claims"];
    for(auto& [key, val] : clms.items()) {
      string sub_type = val.at(0).at(0);
      string type_value = val.at(0).at(1);
      auto tv = TypedValue(sub_type, type_value);
      CHECK(tv.get_type_string() == sub_type);
      CHECK(tv.get_string() == type_value);
    }
  }

}

TEST_CASE("AgnosticEntityInfo") {
  SUBCASE("Basic functionality") {
    Json big = Json::parse(R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}], ["string", "Wau", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "pof": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"]}})del");
    auto aei = AgnosticEntityInfo(big);
    CHECK(aei.claims["Commons category"][0].tv.get_string() == "Theodor-Lessing-Haus (Hannover)");
    CHECK(aei.claims["Commons category"][1].tv.get_string() == "Wau");
    CHECK(aei.claims["Commons category"][0].optionals.empty());
    CHECK(!aei.claims["located on street"][0].optionals.empty());
  }
  SUBCASE("Optionals present") {
    Json big = Json::parse(R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {"k": [["string", "x"], ["string", "y"]]}], ["string", "Wau", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "pof": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"]}})del");
    auto aei = AgnosticEntityInfo(big);
    CHECK(aei.claims["Commons category"][0].optionals["k"].size() == 2);
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
  SUBCASE("<>") {
    ID q3 = ID("Q123");
    CHECK(q3 < q);
    CHECK(q > q3);
  }
}

namespace map_values {

TEST_CASE("Bits") {
  auto mv = Bits();
  SUBCASE("short") {
    vector<bool> v = {true, true, false, false, true, false};
    vector<byte> data;
    mv.serialize(v, data);
    SUBCASE("length") {
      CHECK(mv.length(v) == mv.length(data.data()));
    }
    SUBCASE("serialization and deserialization") {
      vector<bool> v2;
      mv.deserialize(data.data(), v2);
      CHECK(v2 == v);
    }
  }
  SUBCASE("long multiple of 8") {
    vector<bool> v(3000);
    for (int i = 0; i < 1000; ++i)
      v[i] = true;
    vector<byte> data;
    mv.serialize(v, data);
    SUBCASE("length") {
      CHECK(mv.length(v) == mv.length(data.data()));
    }
    SUBCASE("serialization and deserialization") {
      vector<bool> v2;
      mv.deserialize(data.data(), v2);
      CHECK(v2 == v);
    }
  }
  SUBCASE("long not multiple of 8") {
    vector<bool> v(3003);
    for (int i = 0; i < 1000; ++i)
      v[i] = true;
    vector<byte> data;
    mv.serialize(v, data);
    SUBCASE("length") {
      CHECK(mv.length(v) == mv.length(data.data()));
    }
    SUBCASE("serialization and deserialization") {
      vector<bool> v2;
      mv.deserialize(data.data(), v2);
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

  auto mv = Int4();

  int expected = (int)data[0];
  for (int i = 1; i <= 3; ++i)
    expected |= (int)data[i] << (8 * i);

  SUBCASE("Correct length, serialized") {
    CHECK(4 == mv.length(data));
  }
  SUBCASE("Correct length, deserialized") {
    CHECK(4 == mv.length(42));
  }
  SUBCASE("Correct deserialization") {
    int res;
    mv.deserialize(data, res);
    CHECK(expected == res);
  }
  SUBCASE("Correct serialization") {
    vector<byte> res;
    mv.serialize(expected, res);
    for (int i = 0; i < 4; ++i) 
      CHECK(res[i] == data[i]);
  }
  delete[] data;
}
TEST_CASE("Int8") {
  byte* data = new byte[8];
  data[0] = (byte)2;
  data[1] = (byte)1;
  data[2] = (byte)2;
  data[3] = (byte)2;
  data[4] = (byte)2;
  data[5] = (byte)1;
  data[6] = (byte)2;
  data[7] = (byte)2;

  auto mv = Int8();

  int64_t expected = (int64_t)data[0];
  for (int i = 1; i <= 7; ++i)
    expected |= (int64_t)data[i] << (8 * i);

  SUBCASE("Correct length, serialized") {
    CHECK(8 == mv.length(data));
  }
  SUBCASE("Correct length, deserialized") {
    CHECK(8 == mv.length(42));
  }
  SUBCASE("Correct deserialization") {
    int64_t res;
    mv.deserialize(data, res);
    CHECK(expected == res);
  }
  SUBCASE("Correct serialization") {
    vector<byte> res;
    mv.serialize(expected, res);
    for (int i = 0; i < 4; ++i) 
      CHECK(res[i] == data[i]);
  }
  delete[] data;
}

TEST_CASE("ID -- map value") {
  vector<byte> data_q, data_n;
  auto qid = linpipe::kbelik::ID("Q12222234");
  auto no_qid = linpipe::kbelik::ID("PER");
  auto mv = ID();
  mv.serialize(qid, data_q);
  mv.serialize(no_qid, data_n);
  SUBCASE("Lengths") {
    SUBCASE("QID")
      CHECK(mv.length(qid) == mv.length(data_q.data()));
    SUBCASE("NO-QID")
      CHECK(mv.length(no_qid) == mv.length(data_n.data()));
  }
  SUBCASE("Serialization and deserialization") {
    linpipe::kbelik::ID id;
    SUBCASE("QID") {
      mv.deserialize(data_q.data(), id);
      CHECK(id == qid);
    }
    SUBCASE("NO-QID") {
      mv.deserialize(data_n.data(), id);
      CHECK(id == no_qid);
    }
  }
}

TEST_CASE("TypedValue -- map value") {
  vector<byte> data;
  string alpha = "abcdefghijklmnopqrstuvwxyz";
  HuffmanTree huffman;
  huffman.add(alpha);
  transform(alpha.begin(), alpha.end(), alpha.begin(), ::toupper);
  huffman.add(alpha);
  huffman.add("1234567890 :,.;*+-/");

  auto mv = TypedValue(huffman);

  SUBCASE("qid") {
    string qid = "Q1234567890987654";
    huffman.build();
    auto tv = linpipe::kbelik::TypedValue("id", qid);
    mv.serialize(tv, data);
    linpipe::kbelik::TypedValue tv2;
    mv.deserialize(data.data(), tv2);
    CHECK(qid == tv2.get_string());
  }
  SUBCASE("non qid") {
    string str = "This is some peculiar string";
    huffman.build();
    auto tv = linpipe::kbelik::TypedValue("string", str);
    mv.serialize(tv, data);
    linpipe::kbelik::TypedValue tv2;
    mv.deserialize(data.data(), tv2);
    CHECK(str == tv2.get_string());
  }
  SUBCASE("monolingualtext") {
    string str = "Řeřicha";
    huffman.add("Řeřicha");
    huffman.build();
    auto tv = linpipe::kbelik::TypedValue("monolingualtext:cs", str);
    mv.serialize(tv, data);
    linpipe::kbelik::TypedValue tv2;
    mv.deserialize(data.data(), tv2);
    CHECK(tv == tv2);
  }
  SUBCASE("time julian") {
    string str = "+1970+08-22";
    huffman.build();
    auto tv = linpipe::kbelik::TypedValue("time:julian", str);
    mv.serialize(tv, data);
    linpipe::kbelik::TypedValue tv2;
    mv.deserialize(data.data(), tv2);
    CHECK(tv == tv2);
  }
  SUBCASE("time gregorian") {
    string str = "+1970+08-22";
    huffman.build();
    auto tv = linpipe::kbelik::TypedValue("time:gregorian", str);
    mv.serialize(tv, data);
    linpipe::kbelik::TypedValue tv2;
    mv.deserialize(data.data(), tv2);
    CHECK(tv == tv2);
  }
  SUBCASE("quantity") {
    SUBCASE("with description string") {
      string str = "111129292929";
      huffman.build();
      auto tv = linpipe::kbelik::TypedValue("quantity:Q1:desc string", str);
      mv.serialize(tv, data);
      linpipe::kbelik::TypedValue tv2;
      mv.deserialize(data.data(), tv2);
      CHECK(tv == tv2);
    }
    SUBCASE("without description string") {
      string str = "111129292929";
      huffman.build();
      auto tv = linpipe::kbelik::TypedValue("quantity:Q1", str);
      mv.serialize(tv, data);
      linpipe::kbelik::TypedValue tv2;
      mv.deserialize(data.data(), tv2);
      CHECK(tv == tv2);
    }
  }
  SUBCASE("Real usecase") {
    string raw = R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["id", "Q183:Germany", {}]], "instance of": [["id", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["id", "Q1997469:Nord", {}]], "heritage designation": [["id", "Q811165:architectural heritage monument", {}]], "pof": [["id", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["id", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["id", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["id", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"]}})del";
    Json big = Json::parse(raw);
    huffman.add(raw);
    huffman.build();
    auto clms = big["claims"];
    for(auto& [key, val] : clms.items()) {
      string sub_type = val.at(0).at(0);
      string type_value = val.at(0).at(1);
      auto tv = linpipe::kbelik::TypedValue(sub_type, type_value);
      mv.serialize(tv, data);
      linpipe::kbelik:: TypedValue tv2;
      mv.deserialize(data.data(), tv2);
      SUBCASE("equality") {
        CHECK(tv == tv2);
      }
      SUBCASE("length") {
        CHECK(mv.length(data.data()) == mv.length(tv));
      }
    }
  }
}

TEST_CASE("AgnosticEntityInfoHuffman -- map value") { 
  //string raw = R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "pof": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"]}})del";
  string raw = R"del({"qid": "Q2417271", "claims": {"Commons category": [["string", "Theodor-Lessing-Haus (Hannover)", {"k": [["string", "x"], ["string", "y"]]}], ["string", "Wau", {}]], "coordinate location": [["globe-coordinate", "52.3834 9.71923", {}]], "country": [["qid", "Q183:Germany", {}]], "instance of": [["qid", "Q811979:architectural structure", {}]], "image": [["commonsMedia", "Theodor-Lessing-Haus Hannover Schriftzug über dem Haupteingang I.jpg", {}]], "located in the administrative territorial entity": [["qid", "Q1997469:Nord", {}]], "heritage designation": [["qid", "Q811165:architectural heritage monument", {}]], "pof": [["qid", "Q678982:Leibniz University Hannover", {}]], "Google Knowledge Graph ID": [["external-id", "/g/1hb_dzzdq", {}]], "located on street": [["qid", "Q105835889:Welfengarten", {"house number": [["string", "2c"]]}]], "named after": [["qid", "Q61446:Theodor Lessing", {}]], "image of interior": [["commonsMedia", "Theodor-Lessing-Haus Hannover Blick von der umlaufenden Empore zur Auskunft Information.jpg", {}]], "located in the statistical territorial entity": [["qid", "Q97762617:Nordstadt", {}]]}, "named_entities": {"type": ["LOC"]}})del";
  Json big = Json::parse(raw);
  auto huff = HuffmanTree();
  huff.add(raw);
  huff.add("|$");
  huff.build();
  vector<byte> data;
  auto ori = linpipe::kbelik::AgnosticEntityInfo(big);
  
  auto mv = AgnosticEntityInfoH(huff);

  mv.serialize(ori, data);
  
  SUBCASE("Lengths") {
    size_t l1, l2;
    l1 = mv.length(ori);
    l2 = mv.length(data.data());
    CHECK(l1 == l2);
  }
  SUBCASE("Serialization/Desirizalization") {
    linpipe::kbelik::AgnosticEntityInfo aei;
    mv.deserialize(data.data(), aei);
    CHECK(aei.claims == ori.claims);
  }
}

TEST_CASE("SpecificEntityInfoH -- map value") {
  string raw = R"del({"qid": "Q6723460", "label": "blabla", "lang": "cs", "description": "asteroid", "wiki": {"title": "Slunce", "text": "Slunce je jak znamo asteroid"}})del";
  Json big = Json::parse(raw);
  auto huff = HuffmanTree();
  huff.add(raw);
  huff.build();
  vector<byte> data;
  auto mv = SpecificEntityInfoH(huff);
  auto ori = linpipe::kbelik::SpecificEntityInfo(big);
  mv.serialize(ori, data);

  SUBCASE("Lengths") {
    size_t l1, l2;
    l1 = mv.length(ori);
    l2 = mv.length(data.data());
    CHECK(l1 == l2);
  }
  SUBCASE("Serialization/Desirizalization") {
    linpipe::kbelik::SpecificEntityInfo sp;
    mv.deserialize(data.data(), sp);
  
    CHECK(sp == ori);
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

    auto mv = Bytes<SizeType>();

    vector<byte> expected(3);
    for (size_t i = 0; i < 3; ++i)
      expected[i] = data[i + sizeof(SizeType)];

    SUBCASE("Correct length") {
      SUBCASE("serialized") {
        CHECK(sz == mv.length(data));
      }
      SUBCASE("deserialized") {
        vector<byte> data2(12345, (byte)0);
        CHECK(data2.size() + sizeof(SizeType) == mv.length(data2));
      }
    }
    SUBCASE("Correct deserialization") {
      vector<byte> res;
      mv.deserialize(data, res);
      for (int i = 0; i < 3; ++i)
        CHECK(expected[i] == res[i]);
    }
    SUBCASE("Correct serialization") {
      vector<byte> res;
      mv.serialize(expected, res);
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
    auto mv = BytesVLI();
    mv.serialize(data, data2);
    SUBCASE("Correct length") {
      CHECK(mv.length(data2.data()) == mv.length(data));
    }
    SUBCASE("Correct de/serialization") {
      vector<byte> data3;
      mv.deserialize(data2.data(), data3);
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
    auto mv = VLI();
    mv.serialize(i, data);
    SUBCASE("Correct length from serialized") {
      size_t expected = max((size_t)1, (size_t)ceil(msb(i) / 7.0));
      CHECK(expected == mv.length(data.data()));
    }
    SUBCASE("Correct length from deserialized") {
      size_t expected = max((size_t)1, (size_t)ceil(msb(i) / 7.0));
      CHECK(expected == mv.length(i));
    }
    SUBCASE("Serialization/deserialization works.") {
      uint64_t des;
      mv.deserialize(data.data(), des);
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

  auto mv = SimpleJson();

  auto length = [&](Json& j) {
    vector<byte> s;
    mv.serialize(j, s);

    size_t se, de;
    de = mv.length(j);
    se = mv.length(s.data());
    CHECK(de == se);
  };

  auto serialization_deserialization = [&](Json& j) {
    vector<byte> s;
    mv.serialize(j, s);

    Json des;
    mv.deserialize(s.data(), des);
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

    mv.serialize(m, s);
    mv.deserialize(s.data(), out);
    CHECK(m["a"] == out["a"]);
    CHECK(m["xyz"] == out["xyz"]);
  }
}

TEST_CASE("FLI") {
  SUBCASE("length") {
    for (int i = 1; i <= 8; ++i) {
      auto mv = FLI(i);
      vector<byte> data;
      mv.serialize(1, data);
      CHECK(mv.length(data.data()) == i);
      CHECK(mv.length(2) == i);
    }
  }
  SUBCASE("serialize/deserialize") {
    SUBCASE("3 byte") {
      auto mv = FLI(3);
      for (uint64_t i = 0; i < 1000; i += 50) {
        vector<byte> data;
        mv.serialize(i, data);
        uint64_t des;
        mv.deserialize(data.data(), des);
        CHECK(des == i);
      }
    }
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
