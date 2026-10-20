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

.PHONY: prepare swig sources
prepare: swig sources
swig:
	make -C ../python swig
	mkdir -p linpipe src
	cp ../python/linpipe_python.cpp src/
	cp ../python/linpipe/__init__.py* linpipe/
	sed '$$ a\\n__version__ = "'"$$(sed -n 's/^version = "\(.*\)"$$/\1/p' pyproject.toml)"'"' -i linpipe/__init__.py
	sed -e '$$ a\\n__version__ : str\n"""The version of the linpipe Python package."""' -i linpipe/__init__.pyi
	>linpipe/py.typed
sources:
	rsync -a -m --exclude=/.build/ --include='*/' --include='*.h' --include='/*/**.c' --include='/*/**.cpp' --exclude='*' ../../src/ src/
	make --no-print-directory -sC ../../src sources | sed 's@^@src/@' >CMakeLists.sources

.PHONY: sdist wheel
sdist: prepare
	python3 -m build --sdist
wheel: prepare
	python3 -m build --wheel

.PHONY: venv ebuild
venv:
	[ -d venv/ ] || python3 -m venv venv
	venv/bin/pip install scikit-build-core ninja build
ebuild: prepare
	venv/bin/pip install --no-build-isolation -Ceditable.rebuild=true -ve .

.PHONY: clean
clean:
	rm -rf build/ dist/ linpipe/ src/ CMakeLists.sources

.PHONY: veryclean
veryclean: clean
	rm -rf venv/
