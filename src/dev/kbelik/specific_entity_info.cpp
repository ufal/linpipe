
#include "common.h"

#include "dev/kbelik/specific_entity_info.h"

namespace linpipe::kbelik {

SpecificEntityInfo::SpecificEntityInfo() {}

SpecificEntityInfo::SpecificEntityInfo(Json& js) {
  label = js["label"];
  description = js["description"];
  text = js["text"];

  for (auto alias : js["aliases"])
    aliases.insert(alias);
}

} // namespace linpipe::kbelik