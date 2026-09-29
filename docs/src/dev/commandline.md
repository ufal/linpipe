## Command-line parameters

LinPipe is run as a pipeline of operations. Each operation starts with `--`
followed by its name, and everything up to the next operation belongs to it.
Operations are executed in the order in which they appear on the command line.

```
./linpipe --operation [arguments...] --operation [arguments...] ...
```

For example:

```
./linpipe --load -format text test.in --save -format lif test.out
```

This runs two operations: `load` with the named argument `format=text` and the
positional argument `test.in`, followed by `save` with `format=lif` and
`test.out`.

### Operations

An operation name is written as `--name`, e.g. `--load` or `--tag`. The command
line must begin with an operation; anything before the first operation is an
error.

### Named arguments

Named arguments are written with a single hyphen, either as `-name value` or
as `-name=value`. For example, `-format text`, `-format=text` or
`-batch_size 32`.

In the `-name value` form the next token is always taken as the value, so
values may themselves start with a hyphen, e.g. `-threshold -1`. In the
`-name=value` form the token is split on the first `=` only, so the value may
contain further `=` characters (see format descriptions below), and it may be
empty (`-name=`).

If the same named argument is given twice for one operation, the later value
wins.

### Positional arguments

Any other token is a positional argument of the current operation, e.g. an
input or output file name. Positional and named arguments may be interleaved;
the order of positional arguments is preserved:

```
./linpipe --load first.in -format text second.in
```

### Values with spaces

On the command line, values containing spaces are written with the usual shell
quoting:

```
./linpipe --load "/tmp/my file.txt" --save -title="My corpus" out.txt
```

LinPipe can also be given the whole pipeline as a single description string,
e.g. from Python or through the web server. The description is split into
tokens on whitespace, with the following quoting rules:

- Double quotes group characters, including whitespace, into one token, e.g.
  `"/tmp/my file.txt"`. Quotes may appear anywhere in a token, so
  `-title="My corpus"` is the single token `-title=My corpus`.
- Inside or outside quotes, `\"` is a literal quote and `\\` is a literal
  backslash. Any other backslash is kept as is, so `C:\dir` needs no escaping.
- `""` is an empty token.
- A missing closing quote is an error.

The command-line example above corresponds to this description:

```
--load "/tmp/my file.txt" --save -title="My corpus" out.txt
```

### Format descriptions

The value of `-format` is either a predefined format name, such as
`conll`, or a format name followed by key-value settings in parentheses:

```
-format conll(1=name:type,2=:lemmas,2_default=_,3=:chunks,3_default=_,4=:named_entities,4_encoding=bio)
-format=conll(1=name:type,2=:lemmas,2_default=_,3=:chunks,3_default=_,4=:named_entities,4_encoding=bio)
```

Settings are separated by `,`, and each key is separated from its value by `=`.
If a description contains spaces, quote it in the shell.

### Current limitations

- Any token starting with `--` followed by a name starts a new operation, even
  where a value is expected, and quoting does not change that. Such a value
  must be written as `-name=--value`.
- A positional argument starting with a hyphen (e.g. `-1`) is read as the name
  of a named argument. This does not affect values of named arguments
  (`-threshold -1` works).
- A named argument at the end of an operation with no value after it is an
  error.
- A `--` without an operation name after it (e.g. `--load -- test.in`) is an
  error.
