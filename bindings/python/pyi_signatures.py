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
the signatures as `#@signature Class.method(parameters)` lines, optionally
followed by ` -> return_type`, inserted to the stub using `%insert("stub_pyi")`;
this script substitutes them (keeping `self`, and the return annotation unless
given).

The marker lines are removed from the stub. The script fails, removing the stub
so that it is regenerated, if a marker is duplicated or unused or an *args remains.
"""

import os
import re
import sys

SIGNATURE = re.compile(r"^#@signature (?:(\w+)\.)?(\w+)\((.*)\)(?: -> (.+))?$")
CLASS = re.compile(r"^class (\w+)\b")
DEF = re.compile(r"^(\s*)def (\w+)\((self(?=[,)]))?(?:, )?(.*?)\)( -> [^:]+)?:(.*)$")


def fill_signatures(lines: list[str]) -> tuple[list[str], list[str]]:
    signatures, stub, output, errors = {}, [], [], []
    for line in lines:
        if marker := SIGNATURE.match(line):
            key = (marker[1], marker[2])
            if key in signatures:
                errors.append("duplicate #@signature for {}".format(".".join(filter(None, key))))
            signatures[key] = (marker[3], marker[4])
        else:
            stub.append(line)

    used, current_class = set(), None
    for line in stub:
        if line and not line[0].isspace():
            current_class = (match := CLASS.match(line)) and match[1]
        if function := DEF.match(line):
            indent, name, self, parameters, returns, body = function.groups()
            key = (current_class if indent else None, name)
            if key in signatures:
                parameters, signature_returns = signatures[key]
                parameters = ", ".join(filter(None, [self, parameters]))
                returns = f" -> {signature_returns}" if signature_returns else (returns or "")
                output.append(f"{indent}def {name}({parameters}){returns}:{body}")
                used.add(key)
                continue
            if "*args" in parameters:
                errors.append("missing #@signature for {}".format(".".join(filter(None, key))))
        output.append(line)
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
