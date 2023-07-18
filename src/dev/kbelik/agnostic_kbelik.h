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

#include "dev/kbelik/map_keys/qid8.h"
#include "dev/kbelik/map_values/agnostic_entity_info_huff.h"

#include "dev/kbelik/general_kbelik.h"

namespace linpipe::kbelik {

typedef GeneralKbelik<map_keys::QID8, map_values::AgnosticEntityInfoH> AgnosticKbelik;

} // namespace linpipe::kbelik
