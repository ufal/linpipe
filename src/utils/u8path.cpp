// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "utils/u8path.h"

namespace linpipe {

std::filesystem::path path_from_u8(std::string_view str) {
#if defined(__cpp_lib_char8_t)
  // Convert string_view to std::u8string_view to avoid deprecated u8path constructor.
  return std::filesystem::path(u8string_view(reinterpret_cast<const char8_t*>(str.data()), str.size()));
#else
  // Use u8path method.
  return std::filesystem::u8path(str);
#endif
}

std::string path_to_u8(const std::filesystem::path& path) {
#if defined(__cpp_lib_char8_t)
  // Create a copy of the u8string as std::string.
  auto u8_path = path.u8string();
  return std::string(reinterpret_cast<const char*>(u8_path.data()), u8_path.size());
#else
  // Return path::u8string directly.
  return path.u8string();
#endif
}

} // namespace linpipe
