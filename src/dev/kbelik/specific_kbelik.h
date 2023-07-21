#pragma once

#include "common.h"

#include "dev/kbelik/general_kbelik.h"

#include "dev/kbelik/map_values/specific_entity_info_huff.h"
#include "dev/kbelik/map_keys/qid4.h"

namespace linpipe::kbelik {

typedef GeneralKbelik<map_keys::QID8, map_values::SpecificEntityInfoH> SpecificKbelik;
 
} // linpipe::kbelik
