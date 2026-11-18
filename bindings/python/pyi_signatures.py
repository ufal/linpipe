#!/usr/bin/env python3

# This file is part of LinPipe <http://github.com/ufal/linpipe/>.
#
# Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

"""Fill in the signatures that SWIG writes as *args to a .pyi stub.

SWIG cannot represent string default values in a .pyi stub, so it generates
`def method(*args, **kwargs)` instead. The interface file therefore provides
the signatures as `#@signature Class.method(parameters)` lines inserted to the
stub using `%insert("stub_pyi")`; this script substitutes them (keeping `self`
and the return annotation) and removes the marker lines. It fails, removing the
stub so that it is regenerated, if a signature is unused or an *args remains.
"""

import os
import re
import sys

MARKER = re.compile(r"^#@signature (?:(\w+)\.)?(\w+)\((.*)\)$")
CLASS = re.compile(r"^class (\w+)\b")
DEF = re.compile(r"^(\s*def (\w+)\()(self, )?\*args(?:, \*\*kwargs)?(\).*)$")
SWIG_CLASSES = {"SwigPyIterator"}


def fill_signatures(lines: list[str]) -> tuple[list[str], list[str]]:
    signatures, output, errors = {}, [], []
    for line in lines:
        if marker := MARKER.match(line):
            signatures[(marker[1], marker[2])] = marker[3]
        else:
            output.append(line)

    used, current_class = set(), None
    for i, line in enumerate(output):
        if line and not line[0].isspace():
            current_class = (match := CLASS.match(line)) and match[1]
        if function := DEF.match(line):
            key = (current_class if line[0].isspace() else None, function[2])
            if key in signatures:
                output[i] = function[1] + (function[3] or "") + signatures[key] + function[4]
                used.add(key)
            elif key[0] not in SWIG_CLASSES:
                errors.append("missing #@signature for {}".format(".".join(filter(None, key))))
    for key in signatures.keys() - used:
        errors.append("unused #@signature for {}".format(".".join(filter(None, key))))
    return output, errors


def main(path: str) -> None:
    with open(path, "r", encoding="utf-8") as stub:
        lines = stub.read().split("\n")

    lines, errors = fill_signatures(lines)
    if errors:
        os.remove(path)
        sys.exit("".join(f"{path}: {error}\n" for error in errors).rstrip("\n"))

    with open(path, "w", encoding="utf-8", newline="\n") as stub:
        stub.write("\n".join(lines))


if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit(f"Usage: {sys.argv[0]} stub.pyi")
    main(sys.argv[1])
