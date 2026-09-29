# Examples

Small example inputs for trying out LinPipe. The commands below assume
LinPipe has been built in `src/` and are run from the repository root.

## Files

- `text.txt`: a short plain-text document.
- `ner.conll`: the same text tokenized and annotated with named entities in
  the CoNLL-2003 style. It has one token per line, with the token and its
  entity tag in BIO encoding (`B-` begins an entity, `I-` continues it, `O`
  is outside any entity) separated by a tab, and an empty line after every
  sentence. The entity types are `PER`, `ORG`, `LOC` and `MISC`.

Unlike the original CoNLL-2003 data, `ner.conll` separates columns by tabs,
contains only the token and entity columns, and has no `-DOCSTART-` lines.

## Commands

When reading from standard input, LinPipe writes the result to standard
output. When given an input file, `--save` writes to `<input>.out` unless an
output path is given.

Load plain text and save it as LIF:

```sh
src/linpipe --load -format text --save -format lif < examples/text.txt
```

Load the CoNLL file and save it as LIF. The format description names the
columns: column 1 is a token layer called `form`, column 2 is a spans layer
called `ner` in BIO encoding:

```sh
src/linpipe --load -format "conll(1=form:token_layer,2=ner:spans,2_encoding=BIO)" --save -format lif < examples/ner.conll
```

Load the CoNLL file and save it as CoNLL again; the output is identical to
the input:

```sh
src/linpipe --load -format "conll(1=form:token_layer,2=ner:spans,2_encoding=BIO)" \
            --save -format "conll(1=form:token_layer,2=ner:spans,2_encoding=BIO)" < examples/ner.conll
```
