#pragma once

#include <filesystem>

#include "lib/json.h"

#include "common.h"

#include "dev/kbelik/byte_serializer_deserializer.h"
#include "dev/kbelik/dynamic_map.h"
#include "dev/kbelik/id.h"
#include "dev/kbelik/named_entity.h"
#include "dev/kbelik/persistent_map.h"
#include "dev/kbelik/typed_value.h"
#include "dev/kbelik/utils.h"

#include "dev/kbelik/map_values/agnostic_entity_info_huff.h"
#include "dev/kbelik/map_values/id.h"

namespace linpipe::kbelik {

class AgnosticKbelik {
 public:
  // Rovnou map_path
  AgnosticKbelik(filesystem::path map_path, size_t offset=0, int64_t length=-1);
  ~AgnosticKbelik();

  bool find(ID id, AgnosticEntityInfo& value) const;
  void close(); // Should end mmap.

  bool opened() const;

  static inline void build(istream& jsons, ostream& result);
 private:
  // Chápe PM formát dat?
  filesystem::path kbelik_path;
  ByteSerializerDeserializers bsds;
  PersistentMap<map_values::ID, map_values::AgnosticEntityInfoH>* map = nullptr;

  size_t load_bsds (size_t offset);
  void load_map (size_t offset, int64_t length);

  static inline void build_bsds(istream& jsons, ByteSerializerDeserializers& bsds);
  static inline void build_map(istream& jsons, DynamicMap<map_values::ID, map_values::AgnosticEntityInfoH>& dm);
};

void AgnosticKbelik::build(istream& jsons, ostream& result) {
  ByteSerializerDeserializers bsds;
  AgnosticKbelik::build_bsds(jsons, bsds);
  
  auto dm = DynamicMap<map_values::ID, map_values::AgnosticEntityInfoH>(&bsds);
  AgnosticKbelik::build_map(jsons, dm);

  vector<byte> huff_serialized;
  bsds.huffman.serialize(huff_serialized);
  result.write(reinterpret_cast<const char*>(huff_serialized.data()), huff_serialized.size());

  dm.save_map(result, test);
}

void AgnosticKbelik::build_bsds(istream& jsons, ByteSerializerDeserializers& bsds) {
  auto huff = HuffmanTree();

  auto remember_pos = jsons.tellg();

  string line;
  while (getline(jsons, line)) {
    huff.add(line);
  }

  huff.build();

  bsds.huffman = huff;

  jsons.clear();
  jsons.seekg(remember_pos);
}

void AgnosticKbelik::build_map(istream& jsons, DynamicMap<map_values::ID, map_values::AgnosticEntityInfoH>& dm) {
  string line;
  while (getline(jsons, line)) {
    auto js = Json::parse(line);
    string q_str = js["qid"];
    auto id = ID(q_str);
    auto aei = AgnosticEntityInfo(js);
    dm.add(id, aei);
  }
}


} // namespace linpipe::kbelik
