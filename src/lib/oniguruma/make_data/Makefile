# This file is part of LinPipe <http://github.com/ufal/linpipe/>.
#
# Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

UNICODE_VERSION = 18.0.0
UNICODE_DATA = GraphemeBreakProperty.txt # egcb script
UNICODE_DATA += CaseFolding.txt # fold script
UNICODE_DATA += UnicodeData.txt DerivedCoreProperties.txt Scripts.txt PropList.txt # property script
UNICODE_DATA += emoji-data.txt PropertyAliases.txt PropertyValueAliases.txt Blocks.txt # property script
UNICODE_DATA += WordBreakProperty.txt # wb script

.PHONY: all
all: unicode_data
	sh make_unicode_egcb.sh
	sh make_unicode_fold.sh
	sh make_unicode_property.sh
	sh make_unicode_wb.sh

.PHONY: unicode_data
unicode_data: $(UNICODE_DATA)

$(UNICODE_DATA):%:
	wget https://www.unicode.org/Public/$(UNICODE_VERSION)/ucd/$* || \
	wget https://www.unicode.org/Public/$(UNICODE_VERSION)/ucd/auxiliary/$* || \
	wget https://www.unicode.org/Public/$(UNICODE_VERSION)/ucd/emoji/$*

.PHONY: clean
clean:
	@rm -f $(UNICODE_DATA) UNICODE_PROPERTIES $(wildcard *.c)
