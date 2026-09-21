#!/usr/bin/env python3

# Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

"""generate_languages_cpp.py

Generates languages.cpp from two sources:

- The Wikipedia "List of ISO 639 language codes" table, for every language that
has a Set 1 (639-1) code.
- SIL's official ISO 639-3 code table
(https://iso639-3.sil.org/code_tables/download_tables), for languages that do
NOT have a Set 1 code.

For these SIL-only entries, iso639_1 is left as "" (empty) and
non_iso639_1_codes holds whichever of Id/Part2T/Part2B are non-empty.

TODO(Jana):
- include UD codes.

Usage:
    python3 generate_languages_cpp.py [-o languages.cpp]
    python3 generate_languages_cpp.py --input saved_page.html [-o languages.cpp]
    python3 generate_languages_cpp.py --sil-input iso-639-3.tab [-o languages.cpp]
    python3 generate_languages_cpp.py --skip-non-iso639-1 [-o languages.cpp]
"""

import argparse
import re
import sys
import urllib.request
from html.parser import HTMLParser

WIKI_URL = "https://en.wikipedia.org/wiki/List_of_ISO_639_language_codes"

# SIL is the ISO 639-3 registration authority; this is their canonical
# tab-separated code table, which also covers languages that have no Set
# 1 (639-1) code. Columns:
#   Id  Part2B  Part2T  Part1  Scope  Language_Type  Ref_Name  Comment
SIL_URL = "https://iso639-3.sil.org/sites/iso639-3/files/downloads/iso-639-3.tab"


class TableParser(HTMLParser):
    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.tables = []
        self._table = None
        self._row = None
        self._cell = None
        self._skip_depth = 0  # inside <sup>/<style>/<script>: ignore text

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if tag == "table":
            self._table = {"class": attrs.get("class", ""), "rows": []}
            self.tables.append(self._table)
        elif tag == "tr" and self._table is not None:
            self._row = []
            self._table["rows"].append(self._row)
        elif tag in ("td", "th") and self._row is not None:
            colspan = self._safe_int(attrs.get("colspan"), 1)
            rowspan = self._safe_int(attrs.get("rowspan"), 1)
            self._cell = {"tag": tag, "colspan": colspan, "rowspan": rowspan, "text": []}
        elif tag in ("sup", "style", "script") and self._cell is not None:
            self._skip_depth += 1

    def handle_startendtag(self, tag, attrs):
        # e.g. <br/> inside a cell -- treat as a space so text doesn't glue together
        if tag == "br" and self._cell is not None and self._skip_depth == 0:
            self._cell["text"].append(" ")

    def handle_endtag(self, tag):
        if tag == "table":
            self._table = None
        elif tag == "tr":
            self._row = None
        elif tag in ("td", "th") and self._cell is not None:
            text = "".join(self._cell["text"])
            text = re.sub(r"\s+", " ", text).strip()
            self._row.append((self._cell["tag"], self._cell["colspan"], self._cell["rowspan"], text))
            self._cell = None
        elif tag in ("sup", "style", "script") and self._skip_depth > 0:
            self._skip_depth -= 1

    def handle_data(self, data):
        if self._cell is not None and self._skip_depth == 0:
            self._cell["text"].append(data)

    @staticmethod
    def _safe_int(value, default):
        try:
            return int(value)
        except (TypeError, ValueError):
            return default


def build_grid(rows):
    grid = []
    pending = {}  # column index -> [remaining_rowspan, text]
    for row in rows:
        logical_row = []
        col = 0
        cells = list(row)
        ci = 0
        max_col_guess = max(
            [c for c in pending] + [0]
        ) + sum(cell[1] for cell in cells) + 1
        while col < max_col_guess:
            if col in pending:
                remaining, text = pending[col]
                logical_row.append(text)
                if remaining <= 1:
                    del pending[col]
                else:
                    pending[col][0] = remaining - 1
                col += 1
                continue
            if ci >= len(cells):
                break
            _tag, colspan, rowspan, text = cells[ci]
            ci += 1
            for i in range(colspan):
                logical_row.append(text)
                if rowspan > 1:
                    pending[col + i] = [rowspan - 1, text]
            col += colspan
        grid.append(logical_row)
    return grid


def normalize_label(s):
    return re.sub(r"\s+", " ", s).strip().lower()


def find_target_table(tables):
    """Pick the wikitable that has a header row containing Set 1/2/3 columns."""
    best, best_score = None, -1
    for table in tables:
        if "wikitable" not in table["class"]:
            continue
        rows = table["rows"]
        header_rows = [r for r in rows if r and all(c[0] == "th" for c in r)]
        if not header_rows:
            continue
        grid = build_grid(header_rows)
        labels = [normalize_label(x) for x in grid[-1]] if grid else []
        score = sum(
            1
            for key in ("language name", "set 1", "set 3", "t", "b")
            if key in labels or any(key == lbl for lbl in labels)
        )
        data_rows = [r for r in rows if any(c[0] == "td" for c in r)]
        if score > best_score and len(data_rows) > 20:
            best_score = score
            best = table
    if best is None:
        raise RuntimeError(
            "Could not find the ISO 639 codes table. The page structure may "
            "have changed -- inspect TableParser().tables and adjust "
            "find_target_table()/column matching below."
        )
    return best


def clean_text(text):
    text = re.sub(r"\[.*?\]", "", text)  # stray footnote brackets, if any slipped through
    text = text.strip()
    if text in ("—", "-", "–", ""):
        return ""
    return text


def split_codes(cell_text):
    """A cell can hold multiple codes, e.g. 'foo, bar' or 'foo (older)'."""
    if not cell_text:
        return []
    parts = re.split(r"[,/\n]| or ", cell_text)
    codes = []
    for p in parts:
        p = re.sub(r"\(.*?\)", "", p).strip()
        if re.fullmatch(r"[a-z]{2,3}", p):
            codes.append(p)
    return codes


def escape_cpp_string(s):
    return s.replace("\\", "\\\\").replace('"', '\\"')


def col_index(labels, *must_equal_any, contains=None):
    for i, lbl in enumerate(labels):
        if must_equal_any and lbl in must_equal_any:
            return i
        if contains and contains in lbl:
            return i
    return None


def extract_entries(table):
    rows = table["rows"]
    header_rows = [r for r in rows if r and all(c[0] == "th" for c in r)]
    data_rows = [r for r in rows if any(c[0] == "td" for c in r)]

    header_grid = build_grid(header_rows)
    labels = [normalize_label(x) for x in header_grid[-1]] if header_grid else []

    name_idx = col_index(labels, contains="language name") or 0
    set1_idx = col_index(labels, "set 1") if col_index(labels, "set 1") is not None else col_index(labels, contains="set 1")
    t_idx = col_index(labels, "t")
    b_idx = col_index(labels, "b")
    set3_idx = col_index(labels, contains="set 3")

    missing = [
        n
        for n, v in [("name", name_idx), ("Set 1", set1_idx), ("T", t_idx), ("B", b_idx), ("Set 3", set3_idx)]
        if v is None
    ]
    if missing:
        print(f"Warning: could not find columns for: {missing}. Header labels: {labels}", file=sys.stderr)

    data_grid = build_grid(data_rows)

    entries = []
    seen_names = set()
    for logical_row in data_grid:
        def cell(idx):
            if idx is None or idx >= len(logical_row):
                return ""
            return clean_text(logical_row[idx])

        name = cell(name_idx)
        if not name or name in seen_names:
            continue
        seen_names.add(name)

        iso1_codes = split_codes(cell(set1_idx))
        iso1 = iso1_codes[0] if iso1_codes else ""

        other_codes = []
        for idx in (t_idx, b_idx, set3_idx):
            for code in split_codes(cell(idx)):
                if code != iso1 and code not in other_codes:
                    other_codes.append(code)

        entries.append((name, iso1, other_codes))
    return entries


def parse_sil_table(text):
    """Parse SIL's tab-separated iso-639-3.tab into (name, "", codes) entries,
    one per language that has NO Set 1 (639-1) code -- languages that do have
    one are already covered by the Wikipedia table and are skipped here."""
    lines = [line for line in text.splitlines() if line.strip()]
    if not lines:
        raise RuntimeError("SIL ISO 639-3 table is empty.")

    header = lines[0].split("\t")
    col_idx = {name.strip().lower(): i for i, name in enumerate(header)}
    required = ("id", "part2b", "part2t", "part1", "scope", "ref_name")
    missing = [c for c in required if c not in col_idx]
    if missing:
        raise RuntimeError(
            f"Unexpected SIL ISO 639-3 table format, missing columns: {missing}. "
            f"Header found: {header}. The table format may have changed -- "
            "adjust parse_sil_table() below."
        )

    def col(cols, name):
        i = col_idx[name.lower()]
        return cols[i].strip() if i < len(cols) else ""

    entries = []
    for line in lines[1:]:
        cols = line.split("\t")

        if col(cols, "Part1"):
            continue  # has a Set 1 code -> already covered via the Wikipedia table

        if col(cols, "Scope") == "S":
            continue  # special code (mul, und, mis, zxx, ...), not a real language

        name = col(cols, "Ref_Name")
        if not name:
            continue

        codes = []
        for code in (col(cols, "Id"), col(cols, "Part2T"), col(cols, "Part2B")):
            if code and code not in codes:
                codes.append(code)
        if not codes:
            continue

        entries.append((name, "", codes))
    return entries


def fetch_text(url):
    req = urllib.request.Request(url, headers={"User-Agent": "Mozilla/5.0 (compatible; LinPipeLangGen/1.0)"})
    with urllib.request.urlopen(req, timeout=30) as resp:
        charset = resp.headers.get_content_charset() or "utf-8"
        return resp.read().decode(charset, errors="replace")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-o", "--output", default="languages.cpp")
    parser.add_argument("--url", default=WIKI_URL)
    parser.add_argument("--input", help="Read Wikipedia HTML from this local file instead of fetching --url")
    parser.add_argument("--sil-url", default=SIL_URL)
    parser.add_argument("--sil-input", help="Read the SIL ISO 639-3 table from this local .tab file instead of fetching --sil-url")
    parser.add_argument("--skip-non-iso639-1", action="store_true", help="Don't merge in SIL's non-Set-1 languages; emit only the Wikipedia (Set-1) languages")
    args = parser.parse_args()

    if args.input:
        print(f"Reading {args.input} ...", file=sys.stderr)
        with open(args.input, encoding="utf-8", errors="replace") as f:
            html = f.read()
    else:
        print(f"Fetching {args.url} ...", file=sys.stderr)
        html = fetch_text(args.url)

    tp = TableParser()
    tp.feed(html)

    table = find_target_table(tp.tables)
    entries = extract_entries(table)
    print(f"Parsed {len(entries)} Set-1 language entries from Wikipedia.", file=sys.stderr)

    if not args.skip_non_iso639_1:
        if args.sil_input:
            print(f"Reading {args.sil_input} ...", file=sys.stderr)
            with open(args.sil_input, encoding="utf-8", errors="replace") as f:
                sil_text = f.read()
        else:
            print(f"Fetching {args.sil_url} ...", file=sys.stderr)
            sil_text = fetch_text(args.sil_url)

        sil_entries = parse_sil_table(sil_text)
        print(f"Parsed {len(sil_entries)} non-Set-1 language entries from SIL.", file=sys.stderr)

        seen_names = {name for name, _, _ in entries}
        for entry in sil_entries:
            if entry[0] not in seen_names:  # guard against an unexpected name collision
                seen_names.add(entry[0])
                entries.append(entry)

    lines = [
        '#include "languages.h"',
        "",
        "// This file is part of LinPipe <http://github.com/ufal/linpipe/>.",
        "//",
        "// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty",
        "// of Mathematics and Physics, Charles University in Prague, Czech Republic.",
        "//",
        "// This Source Code Form is subject to the terms of the Mozilla Public",
        "// License, v. 2.0. If a copy of the MPL was not distributed with this",
        "// file, You can obtain one at http://mozilla.org/MPL/2.0/.",
        "",
        "namespace linpipe {",
        "",
        "const vector<Language> allLanguages = {",
    ]
    for name, iso1, codes in entries:
        name_cpp = escape_cpp_string(name)
        iso1_cpp = escape_cpp_string(iso1)
        codes_cpp = ", ".join(f'"{escape_cpp_string(c)}"' for c in codes)
        lines.append(f'    {{"{name_cpp}", "{iso1_cpp}", {{{codes_cpp}}}}},')
    lines += ["};", "", "} // namespace linpipe"]

    with open(args.output, "w", encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")

    print(f"Wrote {len(entries)} entries to {args.output}", file=sys.stderr)


if __name__ == "__main__":
    main()
