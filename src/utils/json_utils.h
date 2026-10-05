// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"

namespace linpipe {

Json json_parse(std::string_view caller, std::string_view input);

void json_assert_object(std::string_view caller, const Json& json);
Json json_get_key(std::string_view caller, const Json& json, std::string_view key);
Json json_get_array(std::string_view caller, const Json& json, std::string_view key);
Json json_get_object(std::string_view caller, const Json& json, std::string_view key);

std::string json_get_string(std::string_view caller, const Json& json, std::string_view key);
void json_get_string(std::string_view caller, const Json& json, std::string_view key, std::string& output);

void json_get_unsigned_vector(std::string_view caller, const Json& json, std::string_view key, std::vector<unsigned>& output);
void json_get_unsigned_pair_vector(std::string_view caller, const Json& json, std::string_view key, std::vector<std::pair<unsigned, unsigned>>& output);
void json_get_string_vector(std::string_view caller, const Json& json, std::string_view key, std::vector<std::string>& output);

} // namespace linpipe
