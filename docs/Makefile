.PHONY: all
all: site

PREREQUISITIES := Makefile zensical.toml $(wildcard src/* src/*/* src/*/*/*)
site: get_assets $(PREREQUISITIES)
	venv/bin/zensical build
	@rm -rf site/assets/images site/assets/stylesheets/classic site/partials

.PHONY: serve
serve: get_assets $(PREREQUISITIES)
	venv/bin/zensical serve -f zensical.toml -a localhost:1234

.PHONY: get_assets
ASSETS := src/assets/katex src/assets/fonts
get_assets: $(ASSETS)
src/assets/katex:
	@mkdir -p $@/fonts
	@curl https://cdn.jsdelivr.net/npm/katex/dist/katex.min.js >$@/katex.min.js
	@curl https://cdn.jsdelivr.net/npm/katex/dist/katex.min.css >$@/katex.min.css
	@grep -o 'fonts/[^ /)]*.woff2' $@/katex.min.css | while read path; do curl https://cdn.jsdelivr.net/npm/katex/dist/$$path >$@/$$path; done

src/assets/fonts:
	@mkdir -p $@
	@curl 'https://fonts.googleapis.com/css?family=Inter:300,300i,400,400i,500,500i,700,700i|JetBrains+Mono:400,400i,700,700i&display=fallback' >$@/fonts.css
	@grep -o 'https://[^ )]*.ttf' $@/fonts.css | while read path; do curl $$path >$@/$$(basename $$path); done
	@sed 's#url([^)]*/\([^/)]*.ttf)\)#url(\1#g' $@/fonts.css -i

venv:
	python3 -m venv venv
	venv/bin/pip install --no-cache-dir zensical markdown-exec

.PHONY: clean
clean:
	rm -rf site/

.PHONY: clean_assets
clean_assets:
	rm -rf $(ASSETS)
