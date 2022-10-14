// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "core/document.h"

namespace linpipe {

Layer* Document::get_layer(const string_view name) {
}

Layer* Document::add_layer(unique_ptr<Layer>&& layer, bool unique_name_if_duplicate) {
}

void Document::rename_layer(const string_view name, const string_view target) {
}

void Document::del_layer(const string_view name) {
}

} // namespace linpipe
