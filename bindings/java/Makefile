# This file is part of LinPipe <http://github.com/ufal/linpipe/>.
#
# Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include ../linpipe/Makefile.common

BINDING_MODULE=$(call dynlib,$(if $(filter win-%,$(PLATFORM)),linpipe_java,liblinpipe_java))
BINDING_C_FLAGS+=$(if $(findstring -gcc-,$(PLATFORM))$(findstring -clang-,$(PLATFORM)),-fno-strict-aliasing)
ifneq ($(filter linux-%,$(PLATFORM)),)
  BINDING_C_FLAGS+=$(call include_dir,jni/linux/)
else ifneq ($(filter win-%,$(PLATFORM)),)
  BINDING_C_FLAGS+=$(call include_dir,jni/win32)
else ifneq ($(filter macos-%,$(PLATFORM)),)
  BINDING_C_FLAGS+=$(call include_dir,jni/darwin)
endif

.PHONY: all swig
all: $(BINDING_MODULE) linpipe.jar
swig: linpipe_java.cpp

$(call dynobj,linpipe_java) $(BINDING_MODULE): C_FLAGS:=$(BINDING_C_FLAGS)
$(BINDING_MODULE): $(call dynobj,linpipe_java) $(BINDING_LINPIPE_OBJECTS)
	$(call link_dynlib,$@,$^)

linpipe_java.cpp: linpipe_java.i ../linpipe/linpipe.i
	@$(call mkdir,cz/cuni/mff/ufal/linpipe)
	swig $(SWIG_FLAGS) -java -package cz.cuni.mff.ufal.linpipe -outdir $(call platform_name,cz/cuni/mff/ufal/linpipe) -o $@ $<

linpipe.jar: linpipe_java.cpp
	javac -source 8 -Xlint:-options $(wildcard cz/cuni/mff/ufal/linpipe/*.java)
	jar cf $@ $(patsubst %.java,%.class,$(wildcard cz/cuni/mff/ufal/linpipe/*.java))

.PHONY: clean
clean:
	@$(call rm,.build cz $(call all_dynlib,linpipe_java liblinpipe_java) linpipe.jar linpipe_java.cpp $(wildcard examples/*.class))
