#!/usr/bin/env python3

# Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
import os

def escape_c_string(text: str | bytes) -> str:
    """Octal-escape a string for use in C source code."""
    result = []
    for c in (text if isinstance(text, bytes) else text.encode("utf-8")).decode("latin-1"):
        if c in ['"', "\\", "?"]:
            result.append(f"\\{c}")
        elif ord(c) < 32 or ord(c) >= 127:
            result.append("\\{:03o}".format(ord(c)))
        else:
            result.append(c)
    return "".join(result)

def maybe_empty(text: str) -> str:
    return f'"{text}"' if text else "nullptr"

# Load the language codes.
languages = []
with open("iso-639-3.tab", "r", encoding="utf-8") as iso_file:
    for line in iso_file:
        iso_3, iso_2b, iso_2t, iso_1, _scope, _language_type, name, *rest = line.rstrip("\n").split("\t")
        languages.append([iso_3, iso_2b, iso_1, name])
languages.sort(key=lambda x: x[0])  # Sort by ISO 639-3 code.

code_to_language = {}
for i, language in enumerate(languages):
    iso_3, iso_2b, iso_1, name = language
    for iso in {iso_3, iso_2b, iso_1}:
        if iso:
            assert iso not in code_to_language, f"Duplicate ISO code: {iso}"
            code_to_language[iso] = i

for language in languages:
    iso_3 = language[0]
    flag_data = None
    if os.path.exists(f"flags/{iso_3}.png"):
        with open(f"flags/{iso_3}.png", "rb") as flag_file:
            flag_data = escape_c_string(flag_file.read())
    language.append(maybe_empty(flag_data))

# Load languages.cpp for future generation.
lines_before, lines_after = [], []
with open("languages.cpp", "r", encoding="utf-8") as languages_file:
    for line in languages_file:
        if "Languages::languages = {" in line:
            break
        lines_before.append(line.rstrip("\n"))

    for line in languages_file:
        if line.rstrip("\n") == "}":
            break

    for line in languages_file:
        lines_after.append(line.rstrip("\n"))

# Generate the languages.cpp file.
with open("languages.cpp", "w", encoding="utf-8") as languages_file:
    print(*lines_before, file=languages_file, sep="\n")

    print(f"const std::array<Language, {len(languages)}> Languages::languages = {{{{", file=languages_file)
    for i, language in enumerate(sorted(languages, key=lambda x: x[0])):
        iso_3, iso_2b, iso_1, name, flag = language
        print(f'  {{"{escape_c_string(name)}", {maybe_empty(iso_1)}, {maybe_empty(iso_2b)}, "{iso_3}", {flag}}},', file=languages_file)
    print("}};\n", file=languages_file)

    print("namespace {\n", file=languages_file)
    print(f"const std::array<std::pair<const char*, uint16_t>, {len(code_to_language)}> code_to_language = {{{{", file=languages_file)
    for code, index in sorted(code_to_language.items()):
        print(f'  {{"{code}", {index}}},', file=languages_file)
    print("}};\n\n}", file=languages_file)

    print(*lines_after, file=languages_file, sep="\n")
