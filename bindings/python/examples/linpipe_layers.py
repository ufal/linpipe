#!/usr/bin/env python3

# This file is part of LinPipe <http://github.com/ufal/linpipe/>.
#
# Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import sys

import linpipe

layer = linpipe.PlainTextLayer()
layer.text = " ".join(sys.argv[1:]) or "Hello, world!"
print("Layer as JSON:", layer.to_json(indent=2))

view = layer.create_view("plain_text")
print(f"Text from a view of type {view.type()}: {view.text()}")

copy = linpipe.Layer.create_from_json(layer.to_json())
print("Layer recreated from JSON has class", type(copy).__name__)

try:
    linpipe.Layer.create("unknown_type")
except linpipe.LinPipeError as error:
    print("Caught LinPipeError:", error)
