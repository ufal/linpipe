# This file is part of LinPipe <http://github.com/ufal/linpipe/>.
#
# Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

CPP_STANDARD = c++20
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
C_FLAGS := $(patsubst -O3,-O2,$(C_FLAGS))
C_FLAGS += $(call include_dir,.)
C_FLAGS += $(treat_warnings_as_errors)
C_FLAGS += $(if $(filter release,$(MODE)),$(disable_assert))
LD_FLAGS += $(use_threads)
LD_FLAGS += $(if $(filter win-vs-%,$(PLATFORM)),$(foreach lib,advapi32 bcrypt user32 ws2_32,$(call use_library,$(lib))))
LD_FLAGS += $(if $(filter macos-%,$(PLATFORM)),-framework CoreFoundation -framework Security)

# Collect all Makefile.include files
LINPIPE_OBJECTS =
LINPIPE_TESTS =
include $(wildcard Makefile.include */Makefile.include */*/Makefile.include)

# Main targets
$(call exe,linpipe): $(call obj,linpipe $(LINPIPE_OBJECTS) utils/win_wmain_utf8)
	$(call link_exe,$@,$^,$(call win_subsystem,console,wmain) $(treat_warnings_as_no_errors))
$(call exe,linpipe_test): $(call obj,linpipe_test $(LINPIPE_OBJECTS) $(LINPIPE_TESTS) utils/win_wmain_utf8)
	$(call link_exe,$@,$^,$(call win_subsystem,console,wmain) $(treat_warnings_as_no_errors))
$(call lib,liblinpipe): $(call obj,$(LINPIPE_OBJECTS))
	$(call link_lib,$@,$^)

# Add rudimental support for compiling C sources
$(call obj,%): $$(call builtem_demangle,$$*).c $$(COMPILE_DEPS) | .build
	$(filter-out -std=% -stdlib=% /std:%,$(call compile,$@,$(if $(filter win-vs-%,$(PLATFORM)),,-x c )$<))
ifneq ($(call dynobj,%),$(call obj,%))
$(call dynobj,%): $$(call builtem_demangle,$$*).cpp $$(COMPILE_DEPS) | .build
	$(filter-out -std=% -stdlib=% /std:%,$(call compile_dyn,$@,$(if $(filter win-vs-%,$(PLATFORM)),,-x c )$<))
endif

# Sources for linging and style checks
our-sources-filter = $(sort $(foreach f,$1,$(if $(filter lib/%/,$(dir $f)),,$f)))
CHANGED_SOURCES = $(call our-sources-filter,$(filter %.cpp %.h,$(shell git diff --name-only --relative --diff-filter=d HEAD)))
ALL_SOURCES = $(call our-sources-filter,$(wildcard *.cpp *.h */*.cpp */*.h */*/*.cpp */*/*.h))

# Linting
define lint_files
  $(call echo,>.build/lint_$2.cpp,// NOLINTBEGIN(bugprone-suspicious-include))
  $(foreach f,$1,$(call echo,>>.build/lint_$2.cpp,#include "$f"))
  $(call echo,>>.build/lint_$2.cpp,// NOLINTEND(bugprone-suspicious-include))
  $(info Linting $1)
  @clang-tidy .build/lint_$2.cpp -- $(C_FLAGS)
endef
.PHONY: FORCE lint  # lint/all lint/path
lint: .build
	$(if $(CHANGED_SOURCES),$(call lint_files,$(CHANGED_SOURCES),current),@echo "No changed files to lint.")
lint/%: .build FORCE
	$(if $(filter all,$*),$(call lint_files,$(ALL_SOURCES),all),clang-tidy $* -- $(if $(filter %.h,$*),-x c++-header )$(C_FLAGS))

# Style checking and formating (both show diff and perform in-place)
.PHONY: style reformat  # style/all style/path format/path reformat/all reformat/path
style:
	$(if $(CHANGED_SOURCES),clang-format --dry-run --Werror $(CHANGED_SOURCES),@echo "No changed files to stylecheck.")
style/%: FORCE
	clang-format --dry-run --Werror $(if $(filter all,$*),$(ALL_SOURCES),$*)
format/%: FORCE
	clang-format $* | git diff --no-index $* -
reformat:
	$(if $(CHANGED_SOURCES),clang-format -i $(CHANGED_SOURCES),@echo "No changed files to format.")
reformat/%: FORCE
	clang-format -i $(if $(filter all,$*),$(ALL_SOURCES),$*)

# Combined style checking and linting
.PHONY: check  # check/all check/path
check: style lint
check/%: style/% lint/% ;

# Cleaning
.PHONY: clean
clean:
	@$(call rm,.build $(call all_exe,linpipe linpipe_test) $(call all_lib,liblinpipe))
