
#include "common.h"

#include "dev/kbelik/specific_entity_info.h"

namespace linpipe::kbelik {

SpecificEntityInfo::SpecificEntityInfo() {}

SpecificEntityInfo::SpecificEntityInfo(Json& js) {
  if (js.contains("label"))
    label = js["label"];
  if (js.contains("description"))
    description = js["description"];
  //if (js.contains("wiki"))
  //  if(js["wiki"].contains("text"))
  //    text = js["wiki"]["text"];
  if (js.contains("aliases"))
    for (string alias : js["aliases"])
      aliases.insert(alias);
}

} // namespace linpipe::kbelik