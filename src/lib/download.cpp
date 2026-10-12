// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <charconv>

#include "lib/download.h"
#include "lib/httplib/httplib.h"
#include "lib/re.h"

namespace linpipe {

void download(std::string_view url, std::ostream& o, bool progress) {
  static RE re(R"(([a-zA-Z][a-zA-Z0-9+.\-]*)://(?:[^@/?#]*@)?([^/?#]*)([^#]*))");

  RE::Match m;
  if (!re.match(url, &m))
    throw LinpipeError{"download: Cannot parse the given URL: ", url};

  std::string_view protocol = m.groups[0], authority = m.groups[1], path = m.groups[2];
  if (protocol != "http" && protocol != "https")
    throw LinpipeError{"download: Unsupported network protocol '", protocol, "' in URL '", url, "'"};
  if (authority.empty())
    throw LinpipeError{"download: Missing host in URL: ", url};

  httplib::Client client(std::string{protocol} + "://" + std::string{authority});
  if (!client.is_valid())
    throw LinpipeError{"download: Cannot download URL: ", url};
  client.set_follow_location(true);

  constexpr size_t chunk_size = 65536;
  std::string buffer;
  buffer.reserve(chunk_size);
  bool write_failed = false;
  auto flush = [&] {
    o.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    buffer.clear();
    write_failed = !o;
    return !write_failed;
  };

  int status = 0;
  uint64_t total = 0, received = 0, last_logged = 0;
  int last_percent = -1;

  auto result = client.Get(
      path.empty() ? std::string{"/"} : std::string{path},  // path
      [&](const httplib::Response& response) {              // response handler
        status = response.status;
        if (status < 200 || status >= 300) return false;

        std::string length = response.get_header_value("Content-Length");
        std::from_chars(length.data(), length.data() + length.size(), total);
        return true;
      },
      [&](const char* data, size_t length) {                // content receiver
        received += length;
        buffer.append(data, length);
        if (buffer.size() >= chunk_size && !flush()) return false;

        if (progress) {
          if (total) {
            int percent = static_cast<int>(std::min<uint64_t>(received * 100 / total, 100));
            if (percent != last_percent) {
              last_percent = percent;
              LOG(PROGRESS, "Downloading " << url << ": " << percent << "%");
            }
          } else if (received - last_logged >= (1 << 20)) {
            last_logged = received;
            LOG(PROGRESS, "Downloading " << url << ": " << (received >> 20) << " MiB");
          }
        }
        return true;
      });

  if (status && (status < 200 || status >= 300))
    throw LinpipeError{"download: Cannot download URL ", url, ": HTTP status ", std::to_string(status)};
  if (write_failed)
    throw LinpipeError{"download: Cannot write the downloaded data of URL: ", url};
  if (!result)
    throw LinpipeError{"download: Cannot download URL '", url, "': ", httplib::to_string(result.error())};
  if (!flush())
    throw LinpipeError{"download: Cannot write the downloaded data of URL: ", url};
}

} // namespace linpipe
