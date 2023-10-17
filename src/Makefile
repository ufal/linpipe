# This file is part of LinPipe <http://github.com/ufal/linpipe/>.
#
# Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

CPP_STANDARD = c++17
include Makefile.builtem

# Targets
.PHONY: default lib test_compile test all
default: $(call exe,linpipe)
lib: $(call lib,liblinpipe)
test_compile: $(call exe,linpipe_test)
test: test_compile
	$(call platform_name,./$(call exe,linpipe_test))
all: default lib test_compile

# Compilation flags
C_FLAGS += $(call include_dir,.)
C_FLAGS += $(treat_warnings_as_errors)
C_FLAGS += $(if $(filter release,$(MODE)),$(disable_assert))
LD_FLAGS += $(use_threads)

# Collect all Makefile.include files
LINPIPE_OBJECTS =
LINPIPE_TESTS =
include $(wildcard Makefile.include */Makefile.include */*/Makefile.include)

# Main targets
$(call exe,linpipe): $(call obj,linpipe $(LINPIPE_OBJECTS) utils/win_wmain_utf8)
	$(call link_exe,$@,$^,$(call win_subsystem,console,wmain))
$(call exe,linpipe_test): $(call obj,linpipe_test $(LINPIPE_OBJECTS) $(LINPIPE_TESTS) utils/win_wmain_utf8)
	$(call link_exe,$@,$^,$(call win_subsystem,console,wmain))
$(call lib,liblinpipe): $(call obj,$(LINPIPE_OBJECTS))
	$(call link_lib,$@,$^)

# Add rudimental support for compiling C sources
$(call obj,%): $$(call builtem_demangle,$$*).c $$(COMPILE_DEPS) | .build
	$(filter-out -std=% -stdlib=% /std:%,$(call compile,$@,$(if $(filter win-vs-%,$(PLATFORM)),,-x c )$<))
ifneq ($(call dynobj,%),$(call obj,%))
$(call dynobj,%): $$(call builtem_demangle,$$*).cpp $$(COMPILE_DEPS) | .build
	$(filter-out -std=% -stdlib=% /std:%,$(call compile_dyn,$@,$(if $(filter win-vs-%,$(PLATFORM)),,-x c )$<))
endif

# Cleaning
.PHONY: clean
clean:
	@$(call rm,.build $(call all_exe,linpipe linpipe_test $(EXECUTABLES)) $(call all_lib,liblinpipe))
