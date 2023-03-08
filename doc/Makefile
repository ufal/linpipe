# This file is part of LinPipe <http://github.com/ufal/linpipe/>.
#
# Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

HTMLS = linpipe.html

.PHONY: all
all: $(HTMLS)

DOC_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

%.md: %.md.sh
	sh $< >$@

%.fragment.html: %.md
	$(DOC_DIR)scripts/preprocess <$< | $(DOC_DIR)scripts/md >$@

%.html: %.fragment.html
	$(DOC_DIR)scripts/html <$< >$@

.PHONY: clean
clean:
	rm -rf $(wildcard $(DOC_DIR)*.html) $(patsubst %.md.sh,%.md,$(wildcard $(DOC_DIR)*.md.sh))
