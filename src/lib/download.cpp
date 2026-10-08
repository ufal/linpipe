// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/download.h"
#include "lib/httplib/httplib.h"

namespace linpipe {

void download(std::string_view /*url*/, std::ostream& /*o*/, bool /*progress = false*/) {
  // Succeeds and returns nothing, or throws LinpipeError.

  // 1. Parse the url into protokol and body, accepts only "http" and "https".

  // 2. Download file in pieces (65k) into ostream.

  // 3. Prints progress using LOG(PROGRESS) from common.h, but not after every
  // 65k, only after like 1% change.

  // Other: Don't override the timeout in httplib.
}

} // namespace linpipe
