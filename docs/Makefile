.PHONY: all
all: site

PREREQUISITIES := Makefile zensical.toml $(wildcard src/* src/*/* src/*/*/*)
site: get_assets $(PREREQUISITIES)
	venv/bin/zensical build
	@rm -rf site/assets/images site/assets/stylesheets/classic site/partials

.PHONY: serve
serve: $(PREREQUISITIES)
	venv/bin/zensical serve -f zensical.toml -a localhost:1234

venv:
	python3 -m venv venv
	venv/bin/pip install --no-cache-dir zensical markdown-exec

.PHONY: clean
clean:
	rm -rf site/

.PHONY: clean_assets
clean_assets:
	rm -rf $(ASSETS)
