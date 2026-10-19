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

Each operation starts with `-` followed by its name, and everything up to the
next operation belongs to it. Operations are executed in the order in which
they appear.

On the command line:

```
src/linpipe -operation [arguments...] -operation [arguments...] ...
```

As a description string, the same without the program name:

```
-operation [arguments...] -operation [arguments...] ...
```

For example, the command line

```
src/linpipe -load --format text examples/text.txt -save --format lif text.lif
```

and the description string

```
-load --format text examples/text.txt -save --format lif text.lif
```

both run two operations: `load` with the named argument `format=text` and the
positional argument `examples/text.txt`, followed by `save` with
`format=lif` and `text.lif`.

### Operations

An operation name is written with a single hyphen as `-name`, e.g. `-load` or
`-tag`. The pipeline must begin with an operation; anything before the first
operation is an error.

An operation name cannot start with a digit or a `.`, so that negative numbers
such as `-1` or `-0.5` can be used as arguments (see
[Positional Arguments](#positional-arguments)). An operation name also cannot
contain `=`; a token such as `-format=text` is an error, because it is most
likely a named argument written with a single hyphen instead of two.

### Named Arguments

Named arguments are written with two hyphens, either as `--name value` or as
`--name=value`. For example, `--format text`, `--format=text` or
`--batch_size 32`.

In the `--name value` form the next token is taken as the value, so values may
themselves start with a hyphen, e.g. `--threshold -1`, `--input -` or
`--separator --`. The only exception is a token that starts an operation:
`--name -value` is an error, because `-value` is read as a new operation (see
[Current Limitations](#current-limitations)).

In the `--name=value` form the token is split on the first `=` only, so the
value may contain further `=` characters (see
[Format Descriptions](#format-descriptions)), it may start with a hyphen
(`--name=-value`), and it may be empty (`--name=`).

If the same named argument is given twice for one operation, the later value
wins.

### Positional Arguments

Any other token is a positional argument of the current operation, e.g. an
input or output file name. Positional and named arguments may be interleaved;
the order of positional arguments is preserved:

```
src/linpipe -load first.in --format text second.in
```

Some tokens starting with a hyphen are positional arguments as well:

- A lone `-`, which by convention stands for the standard input or output,
  e.g. `-load --format text -`. LinPipe passes it to the operation unchanged;
  whether it is supported depends on the operation.
- Negative numbers, i.e. a `-` followed by a digit or a `.`, such as `-1`,
  `-0.5` or `-.5`.
- Tokens starting with `---`.

### End of Options

A lone `--` ends option parsing, as usual for command-line programs. The `--`
itself is dropped, and every token after it is a positional argument of the
current operation, even if it looks like an operation, a named argument, or
another `--`. This allows passing positional arguments that start with a
hyphen, e.g. a file named `-data.txt`:

```
src/linpipe -load --format text -- -data.txt
```

Because everything after `--` belongs to the current operation, no further
operation can follow it, so `--` can only be used in the last operation of the
pipeline. A `--` at the very end of the pipeline is allowed and has no effect.

A `--` before the first operation is an error, like any other token there. A
`--` that is the value of a named argument in the `--name value` form (e.g.
`--separator --`) is just a value and does not end option parsing.

### Tokenization

All the rules above operate on tokens. How the tokens are obtained depends on
the entry point.

**Command line.** The shell splits the command line into tokens, so values
containing spaces are written with the usual shell quoting:

```
src/linpipe -load "/tmp/my file.txt" -save --title="My corpus" out.txt
```

**Description string** (library, REST service). LinPipe splits the string into
tokens on whitespace (space, tab, newline and carriage return), with the
following quoting rules:

- Double quotes group characters, including whitespace, into one token, e.g.
  `"/tmp/my file.txt"`. Quotes may appear anywhere in a token, so
  `--title="My corpus"` is the single token `--title=My corpus`, and
  `a"b c"d` is the single token `ab cd`.
- A backslash is an escape character only when it is followed by `"` or `\`:
  `\"` is a literal quote and `\\` is a literal backslash. This holds both
  inside and outside double quotes, and an escaped quote never opens or closes
  a quoted part, e.g. `--title="say \"hi\""` is the token `--title=say "hi"`.
- Any other backslash is kept as is, so `C:\dir` needs no escaping.
- Because `\\` and `\"` are escapes, a literal backslash must be doubled when it is followed by another backslash or by a double quote:
    - Consecutive backslashes are interpreted in pairs. For example, `\\server\share` is read as `\server\share`; write `\\\\server\share` to obtain two consecutive literal backslashes.
    - A backslash immediately before a quote must be escaped if the backslash is intended to be literal. For example, to represent a literal `\"`, write `\\\"`.
    - A backslash at the end of a quoted part escapes the closing quote, so `"C:\my dir\"` is an error; write `"C:\my dir\\"` instead. Outside quotes, a trailing backslash needs no escaping (`C:\dir\` is fine).
- `""` is an empty token.
- A missing closing quote is an error.

A description string is not processed by a shell, so only these rules apply;
for example, single quotes have no special meaning. The command-line example
above corresponds to this description string:

```
-load "/tmp/my file.txt" -save --title="My corpus" out.txt
```

### Format Descriptions

The value of `--format` is either a predefined format name, such as
`conll`, or a format name followed by key-value settings in parentheses.
Settings are separated by `,`, and each key is separated from its value by `=`.

In a description string, a format description can be written directly:

```
--format conll(1=form:token_layer,2=ner:spans,2_encoding=BIO)
--format=conll(1=form:token_layer,2=ner:spans,2_encoding=BIO)
```

If it contains spaces, enclose it (or the part containing spaces) in double
quotes.

The settings are parsed as follows:

- Everything before the first `(` is the format name.
- The settings are split on every `,`, and each setting is split on its first
  `=` only, so values may contain `=`. There is no escaping, so neither keys
  nor values can contain `,`.
- Every setting must contain `=`, otherwise it is an error. Keys and values
  may be empty (e.g. `2_default=`).
- If a key is given more than once, the first value wins (unlike named
  arguments, where the later value wins).

On the command line, parentheses are special characters in most shells, so
the whole format description should be quoted:

```
src/linpipe -load --format "conll(1=form:token_layer,2=ner:spans,2_encoding=BIO)" examples/ner.conll
```

### Current Limitations

These limitations apply to all entry points.

- Any token starting with `-` followed by a name starts a new operation, even
  where a value is expected. Quoting does not change that, neither shell
  quoting nor double quotes in a description string. Such a value must be
  written as `--name=-value`.
- For the same reason, a positional argument starting with a single hyphen
  followed by a name (e.g. `-data.txt`) is read as an operation. Such an
  argument must be written after `--` (see [End of Options](#end-of-options)),
  which is only possible in the last operation of the pipeline.
- A named argument at the end of the pipeline, or followed directly by an
  operation, is an error, because it has no value.
- A lone `-` or `--` cannot be used as an operation or as a named argument; a
  lone `-` is a positional argument and a lone `--` ends option parsing.
- In a format description, the closing `)` is not checked: the last character
  after the `(` is always removed. A missing `)` is therefore not reported, and
  the last character of the final value is silently lost, e.g.
  `conll(1=form` gives the setting `1=for`.
