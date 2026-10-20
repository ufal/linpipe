# This file is part of LinPipe <http://github.com/ufal/linpipe/>.
#
# Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include ../linpipe/Makefile.common

ifndef PYTHON_INCLUDE
  PYTHON_INCLUDE := $(shell $(or $(PYTHON),python3) -c "import sysconfig; print(sysconfig.get_path('include'))")
endif

BINDING_MODULE=$(if $(filter macos-%,$(PLATFORM)),linpipe/_linpipe.so,$(call dynlib,linpipe/_linpipe))
BINDING_C_FLAGS+=$(if $(PYTHON_INCLUDE),,$(error PYTHON_INCLUDE must be set to the directory containing Python.h))
BINDING_C_FLAGS+=$(call include_dir,$(PYTHON_INCLUDE))
BINDING_C_FLAGS+=$(if $(findstring -gcc-,$(PLATFORM))$(findstring -clang-,$(PLATFORM)),-w)

.PHONY: all swig
all: $(BINDING_MODULE)
swig: linpipe_python.cpp linpipe/__init__.py linpipe/__init__.pyi

$(call dynobj,linpipe_python) $(BINDING_MODULE): C_FLAGS:=$(BINDING_C_FLAGS)
$(BINDING_MODULE): $(call dynobj,linpipe_python) $(BINDING_LINPIPE_OBJECTS)
	$(call link_dynlib,$@,$^)

linpipe_python.cpp linpipe/__init__.py linpipe/__init__.pyi &: linpipe_python.i ../linpipe/linpipe.i
	@$(call mkdir,linpipe)
	swig $(SWIG_FLAGS) -python -outdir linpipe -o linpipe_python.cpp -builtin -pyi -typehints -doxygen -features doxygen:notranslate $<
	@$(call mv,linpipe/linpipe.py,linpipe/__init__.py)
	@$(call mv,linpipe/linpipe.pyi,linpipe/__init__.pyi)

.PHONY: clean
clean:
	@$(call rm,.build linpipe_python.cpp linpipe)
