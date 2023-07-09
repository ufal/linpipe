#include "common.h"

#include "dev/kbelik/agnostic_kbelik.h"

#include "dev/kbelik/typed_value.h"
#include "dev/kbelik/utils.h"

#include "dev/kbelik/byte_serializer_deserializer.h"
#include "dev/kbelik/map_values/agnostic_entity_info_huff.h"
#include "dev/kbelik/map_values/huffman.h"
#include "dev/kbelik/map_values/id.h"

#include "dev/kbelik/dynamic_map.h"
#include "dev/kbelik/persistent_map.h"

namespace linpipe::kbelik {

// Agnostic Kbelik

AgnosticKbelik::AgnosticKbelik(filesystem::path kbelik_path, size_t offset, int64_t /*length*/) {
  this->kbelik_path = kbelik_path;
  size_t bsds_bytes = load_bsds(offset);
  load_map(offset + bsds_bytes, -1);
}

bool AgnosticKbelik::find(ID id, AgnosticEntityInfo& value) const {
  return map->find(id, value);
}

void AgnosticKbelik::close() {
  map->close();
}

bool AgnosticKbelik::opened() const {
  return map->opened();
}

size_t AgnosticKbelik::load_bsds(size_t offset) {
  size_t bsds_size = 0;

  std::ifstream ifs(kbelik_path, std::ios::binary | std::ios::in);
  byte b;

  auto huff = HuffmanTree();
  vector<byte> huff_bytes;


  if (ifs.is_open()) {
    ifs.seekg(offset);
    while (ifs.read((char*)&b, sizeof(b))) {
      huff_bytes.push_back(b);
      if(b == huff.end_serialize_symbol()) 
        break;
    }
    ifs.close();
  } 
  else {
    throw LinpipeError("Failed to open file for reading.\n");
  }
  huff.deserialize(huff_bytes);
  bsds.huffman = huff;

  bsds_size += huff_bytes.size();
  return bsds_size;
}

void AgnosticKbelik::load_map(size_t offset, int64_t length) {
  map = new PersistentMap<map_values::ID, map_values::AgnosticEntityInfoH>(kbelik_path, offset, length, &bsds);
}

AgnosticKbelik::~AgnosticKbelik() {
  close();
  delete map;
}

} // linpipe::kbelik