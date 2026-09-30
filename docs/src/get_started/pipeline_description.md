## Overview

LinPipe runs a pipeline of operations. The pipeline is described with the same
syntax regardless of how LinPipe is used:

- on the command line, as the arguments of the `linpipe` executable;
- as a library, e.g. from Python, where the pipeline is given as a single
  description string;
- as a REST service, where the pipeline is given as a description string in
  the request.

The examples below mostly use the command line, but everything in this
document applies equally to description strings.

The command-line examples are run from the repository root, with LinPipe
built in `src/`, and use the example inputs `examples/text.txt` (plain text)
and `examples/ner.conll` (tokens with named entities in BIO encoding).

### Structure of a Pipeline

Each operation starts with `--` followed by its name, and everything up to the
next operation belongs to it. Operations are executed in the order in which
they appear.

On the command line:

```
src/linpipe --operation [arguments...] --operation [arguments...] ...
```

As a description string, the same without the program name:

```
--operation [arguments...] --operation [arguments...] ...
```

For example, the command line

```
src/linpipe --load -format text examples/text.txt --save -format lif text.lif
```

and the description string

```
--load -format text examples/text.txt --save -format lif text.lif
```

both run two operations: `load` with the named argument `format=text` and the
positional argument `examples/text.txt`, followed by `save` with
`format=lif` and `text.lif`.

### Operations

An operation name is written as `--name`, e.g. `--load` or `--tag`. The
pipeline must begin with an operation; anything before the first operation is
an error.

### Named Arguments

Named arguments are written with a single hyphen, either as `-name value` or
as `-name=value`. For example, `-format text`, `-format=text` or
`-batch_size 32`.

In the `-name value` form the next token is always taken as the value, so
values may themselves start with a hyphen, e.g. `-threshold -1`. In the
`-name=value` form the token is split on the first `=` only, so the value may
contain further `=` characters (see [Format descriptions](#format-descriptions)),
and it may be empty (`-name=`).

If the same named argument is given twice for one operation, the later value
wins.

### Positional Arguments

Any other token is a positional argument of the current operation, e.g. an
input or output file name. Positional and named arguments may be interleaved;
the order of positional arguments is preserved:

```
src/linpipe --load first.in -format text second.in
```

### Tokenization

All the rules above operate on tokens. How the tokens are obtained depends on
the entry point.

**Command line.** The shell splits the command line into tokens, so values
containing spaces are written with the usual shell quoting:

```
src/linpipe --load "/tmp/my file.txt" --save -title="My corpus" out.txt
```

**Description string** (library, REST service). LinPipe splits the string into
tokens on whitespace, with the following quoting rules:

- Double quotes group characters, including whitespace, into one token, e.g.
  `"/tmp/my file.txt"`. Quotes may appear anywhere in a token, so
  `-title="My corpus"` is the single token `-title=My corpus`.
- Inside or outside quotes, `\"` is a literal quote and `\\` is a literal
  backslash. Any other backslash is kept as is, so `C:\dir` needs no escaping.
- `""` is an empty token.
- A missing closing quote is an error.

A description string is not processed by a shell, so only these rules apply;
for example, single quotes have no special meaning. The command-line example
above corresponds to this description string:

```
--load "/tmp/my file.txt" --save -title="My corpus" out.txt
```

### Format Descriptions

The value of `-format` is either a predefined format name, such as
`conll`, or a format name followed by key-value settings in parentheses.
Settings are separated by `,`, and each key is separated from its value by `=`.

In a description string, a format description can be written directly:

```
-format conll(1=form:token_layer,2=ner:spans,2_encoding=BIO)
-format=conll(1=form:token_layer,2=ner:spans,2_encoding=BIO)
```

If it contains spaces, enclose it (or the part containing spaces) in double
quotes.

On the command line, parentheses are special characters in most shells, so
the whole format description should be quoted:

```
src/linpipe --load -format "conll(1=form:token_layer,2=ner:spans,2_encoding=BIO)" examples/ner.conll
```

### Current Limitations

These limitations apply to all entry points.

- Any token starting with `--` followed by a name starts a new operation, even
  where a value is expected. Quoting does not change that, neither shell
  quoting nor double quotes in a description string. Such a value must be
  written as `-name=--value`.
- A positional argument starting with a hyphen (e.g. `-1`) is read as the name
  of a named argument. This does not affect values of named arguments
  (`-threshold -1` works).
- A named argument at the end of an operation with no value after it is an
  error.
- A `--` without an operation name after it (e.g.
  `--load -- examples/text.txt`) is an error.
