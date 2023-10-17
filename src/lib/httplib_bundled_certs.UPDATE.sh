#!/bin/sh

# This file is part of C++-Builtem <http://github.com/ufal/cpp_builtem/>.
#
# Copyright 2014-2023 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

(
sed -n '1,/certificates\[] = {$/p' httplib_bundled_certs.cpp
cat /etc/ssl/certs/ca-certificates.crt | python3 -c '
import base64
import fileinput
certs = []

for line in fileinput.input():
  line = line.rstrip("\n")
  if line.startswith("-----BEGIN CERTIFICATE"):
    cert = ""
  elif line.startswith("-----END CERTIFICATE"):
    certs.append(base64.b64decode(cert, validate=True))
  else:
    cert += line

for cert in sorted(certs):
  output = [f"  {{{len(cert)}, \""]
  for byte in cert:
    if len(output[-1]) >= 110:
      output[-1] += "\""
      output.append("    \"")
    if byte >= 32 and byte <= 126 and chr(byte) not in "\"\\?:%":
      output[-1] += chr(byte)
    elif byte == ord("\r"):
      output[-1] += "\\r"
    elif byte == ord("\n"):
      output[-1] += "\\n"
    elif byte == ord("\t"):
      output[-1] += "\\t"
    elif byte == ord("\""):
      output[-1] += "\\\""
    elif byte == ord("\\"):
      output[-1] += "\\\\"
    else:
      output[-1] += "\\{:03o}".format(byte)
  output[-1] += "\"},"
  print(*output, sep="\n")
'
sed -n '/^};$/,$p' httplib_bundled_certs.cpp
) >httplib_bundled_certs.cpp.new
mv httplib_bundled_certs.cpp.new httplib_bundled_certs.cpp
