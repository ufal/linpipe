# This file is part of LinPipe <http://github.com/ufal/linpipe/>.
#
# Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

CPP_STANDARD = c++17
include Makefile.builtem
include Makefile.include

.PHONY: default lib test_compile test all
default: $(call exe,linpipe)
lib: $(call lib,liblinpipe)
test_compile: $(call exe,linpipe_test)
test: test_compile
	$(call platform_name,./$(call exe,linpipe_test))
all: default test_compile # lib will be added once we have at least one source

# Flags
C_FLAGS += $(call include_dir,.)
C_FLAGS += $(treat_warnings_as_errors)

# Targets
$(call exe,linpipe): $(call obj,$(LINPIPE_OBJECTS) linpipe utils/win_wmain_utf8)
	$(call link_exe,$@,$^,$(call win_subsystem,console,wmain))
$(call exe,linpipe_test): $(call obj,$(LINPIPE_OBJECTS) $(LINPIPE_TESTS) linpipe_test utils/win_wmain_utf8)
	$(call link_exe,$@,$^,$(call win_subsystem,console,wmain))
$(call lib,liblinpipe): $(call obj,$(LINPIPE_OBJECTS))
	$(call link_lib,$@,$^)

# cleaning
.PHONY: clean
clean:
	@$(call rm,.build $(call all_exe,linpipe linpipe_test) $(call all_lib,liblinpipe))
