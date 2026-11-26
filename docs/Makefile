.PHONY: all
all: site

.PHONY: api
API_STUB := pyi_api/linpipe/__init__.pyi
$(API_STUB): $(wildcard ../bindings/linpipe/*.i ../bindings/python/*.i)
	$(MAKE) -C ../bindings/python swig
	mkdir -p $(dir $@)
	cp ../bindings/python/linpipe/__init__.pyi $@
api: $(API_STUB)

.PHONY: site serve
PREREQUISITIES := Makefile zensical.toml $(wildcard src/* src/*/* src/*/*/*) $(API_STUB)
site: $(PREREQUISITIES)
	venv/bin/zensical build
	@rm -rf site/assets/images site/assets/stylesheets/classic site/partials
serve: $(PREREQUISITIES)
	venv/bin/zensical serve -f zensical.toml -a localhost:1234

venv:
	python3 -m venv venv
	venv/bin/pip install --no-cache-dir zensical markdown-exec mkdocstrings-python black

.PHONY: clean
clean:
	rm -rf site/ pyi_api/
