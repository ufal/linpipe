# This file is part of LinPipe <http://github.com/ufal/linpipe/>.
#
# Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

.PHONY: all
all:

.PHONY: sdist wheel
sdist:
	python3 -m build --sdist
wheel:
	python3 -m build --wheel

.PHONY: venv einstall
venv:
	[ -d venv/ ] || python3 -m venv venv
	venv/bin/pip install setuptools build
einstall:
	venv/bin/pip install --no-build-isolation -ve .

.PHONY: clean
clean:
	rm -rf build/ dist/ *.egg-info/

.PHONY: veryclean
veryclean: clean
	rm -rf venv/
