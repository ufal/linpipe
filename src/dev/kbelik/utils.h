#pragma once

#include "common.h"

namespace linpipe::kbelik {

enum class Ternary {
  True,
  False,
  Maybe
};

void split(const string& text, const string& delimiter, vector<string>& splitted); 

} // linpipe::kbelik
