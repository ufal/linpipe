#pragma once

#include <filesystem>

#include "common.h"
#include "dev/kbelik/id.h"
#include "dev/kbelik/persistent_map.h"

namespace linpipe::kbelik {

class AgnosticEntityInfo {
 public:
  map<string, string> claims;
};

class map_values::AgnosticEntityInfo {
 public:
  using Type = kbelik::AgnosticEntityInfo;

  static length();
};

class AgnosticKbelik {
 public:
  // Rovnou map_path
  AgnosticKbelik(filesystem::path map_path, size_t offset=0, size_t length=-1);
  ~AgnosticKbelik();

  bool find(ID id, AgnosticEntityInfo& value) const;
  void close(); // Should end mmap.

  static void build(istream& json);
 private:
  // Chápe PM formát dat?
  PersistentMap<ID, map_values::AgnosticEntityInfo> map;
  void load (filesystem::path map_path);
};

} // namespace linpipe::kbelik
