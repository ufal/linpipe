#pragma once

#include "common.h"

#include "dev/kbelik/general_kbelik.h"

#include "dev/kbelik/map_keys/qid8.h"
#include "dev/kbelik/map_values/agnostic_entity_info_huff.h"

namespace linpipe::kbelik {

typedef GeneralKbelik<map_keys::QID8, map_values::AgnosticEntityInfoH> AgnosticKbelik;

} // namespace linpipe::kbelik
