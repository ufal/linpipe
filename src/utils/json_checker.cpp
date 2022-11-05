// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/json.h"
#include "utils/json_checker.h"

namespace linpipe {

void JsonChecker::json_has_string(const string_view called_from, const Json& json, const string_view key) {
  if (!json.is_object())
    throw LinpipeError{called_from, ": The input json is not an object, but a ", json.type_name()};
  if (!json.contains(key))
    throw LinpipeError{called_from, ": The input json does not contain key '", key, "'"};
  if (!json.at(key).is_string())
    throw LinpipeError{called_from, ": The key '", key, "' does not contain a string, but a ", json.at(key).type_name()};
}

void JsonChecker::json_has_array(const string_view called_from, const Json& json, const string_view key) {
  if (!json.is_object())
    throw LinpipeError{called_from, ": The input json is not an object, but a ", json.type_name()};
  if (!json.contains(key))
    throw LinpipeError{called_from, ": The input json does not contain key '", key, "'"};
  if (!json.at(key).is_array())
    throw LinpipeError{called_from, ": The key '", key, "' does not contain an array, but a ", json.at(key).type_name()};
}

} // namespace linpipe
