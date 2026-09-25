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

Named arguments are written with a single hyphen, `-name value`, and the value
is always the next token. For example, `-format text` or `-batch_size 32`.

Because the next token is always taken as the value, values may themselves
start with a hyphen, e.g. `-threshold -1`.

If the same named argument is given twice for one operation, the later value
wins.

### Positional arguments

Any other token is a positional argument of the current operation, e.g. an
input or output file name. Positional and named arguments may be interleaved;
the order of positional arguments is preserved:

```
./linpipe --load first.in -format text second.in
```

### Format descriptions

The value of `-format` is either a predefined format name, such as
`conll`, or a format name followed by key-value settings in parentheses:

```
-format conll(1=name:type,2=:lemmas,2_default=_,3=:chunks,3_default=_,4=:named_entities,4_encoding=bio)
```

Settings are separated by `,`, and each key is separated from its value by `=`.
The whole description must be a single token, so it must not contain spaces.

### Current limitations

- All tokens must be separated by spaces. The `-name=value` form is not
  supported yet, so write `-format text`, not `-format=text`.
- Quoting is not supported, so values cannot contain spaces.
- A positional argument starting with a hyphen (e.g. `-1`) is read as the name
  of a named argument. This does not affect values of named arguments
  (`-threshold -1` works).
- A named argument at the end of an operation with no value after it is
  silently ignored.
- A `--` without an operation name after it (e.g. `--load -- test.in`) is an
  error.
