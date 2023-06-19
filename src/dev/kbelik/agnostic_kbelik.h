#pragma once

#include <filesystem>

#include "lib/json.h"

#include "common.h"
//#include "dev/kbelik/id.h"
#include "dev/kbelik/persistent_map.h"
#include "dev/kbelik/typed_value.h"

namespace linpipe::kbelik {

class AgnosticEntityInfo {
 public:
  map<string, TypedValue> claims;

  AgnosticEntityInfo() {
    claims = map<string, TypedValue>();
  }

  AgnosticEntityInfo(Json& clms) {
    from_wikidata_json(clms);
  }

  AgnosticEntityInfo(string representation) {
    from_string_representation(representation);
  }
  
  void from_wikidata_json(Json& clms) {
    for(auto& [key, val] : clms.items()) {
      string sub_type = val.at(0).at(0);
      string type_value = val.at(0).at(1);
      if (sub_type.find("quantity") != string::npos)
        continue;
      if (sub_type.find("monolingual") != string::npos)
        continue;
      claims[key] = TypedValue(sub_type, type_value);
    }
  }

  string delimiter() const { return "$||$";}

  void from_string_representation(string data) {
    vector<string> splitted;
    size_t start = 0;
    size_t end = 0;
    while((end = data.find(delimiter(), start)) != string::npos) {
      splitted.push_back(data.substr(start, end - start));
      start = end + delimiter().size();
    }
    splitted.push_back(data.substr(start));

    if (splitted.size()&1)
      throw LinpipeError("The deserializaton of the string failed. Different number of serialized fields is expected.");
    claims = map<string, TypedValue>();
    for (size_t i = 0; i < splitted.size(); i += 2) {
      claims[splitted[i]] = TypedValue(splitted[i + 1]);
    }
  }

  string to_string_representation() const {
    string res;
    for (auto &[k, tv] : claims) 
      res += k + delimiter() + tv.to_string_representation() + delimiter();
    res = res.substr(0, res.size() - delimiter().size());
    return res;
  }
};

/*
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
*/
} // namespace linpipe::kbelik
