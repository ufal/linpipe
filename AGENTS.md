# LinPipe

LinPipe is an NLP tool which loads an input, applies a configurable sequence of
NLP operations on it, and writes the resulting output.

It consists of several components.

## LinPipe C++

The main component is a C++ inference engine, binary, and a REST server. It is
in the `src` subdirectory.

Temporarily, the `src_old` directory contains old sources to be gradually moved
to `src`. Use it only as a reference; do not modify or migrate it unless told to.

New sources and tests must be added to `LINPIPE_OBJECTS` and `LINPIPE_TESTS` in
the `Makefile.include` file in the corresponding directory; otherwise, they are
not compiled.

Commands in the `src` directory:
- `make -j4 linpipe test` compiles the sources (with warnings as errors) and runs
  tests; make sure it passes after every change to LinPipe C++ sources.
- `make -j4 check` runs clang-format and clang-tidy on the changed or added
  headers and sources (clang-tidy on the full repo is quite slow); again, all
  source changes should pass this.
- `make reformat` reformats all changed or added headers and sources.
- `make reformat/PATH` and `make relint/PATH` reformat and try to fix linting
  errors of a given single file.

Codestyle:
- Use C++20 with a limited set of features -- avoid modules, `std::format`, and
  chrono additions; it is OK to use full C++17, concepts, ranges, `std::span`,
  and others, plus we vendor `fmt::format`.
- Indent by two spaces; the sources should contain no tabs.
- Classes are named using `CamelCase`, functions and variables using `snake_case`,
  constants usually in `ALL_UPPERCASE`.
- Single-line statements usually do not use curly brackets, unless an `else`
  could be ambiguous, or the other block of `if`-`else` is multiline.
- We do not have a strict line-length limit, but the code should target 140
  characters per line.
- Private class members have underscore at the end.
- All sources must be ASCII.
- No excessive trivial comments; the code should document itself.
- There is a `src/.clang-format` and `src/.clang-tidy` with all the details.

Vendored libraries:
- LinPipe contains quite a few vendored libraries.
- The vendored libraries are present in `src/lib/*/`, which are exempt from
  style checks, linting checks, warnings-as-errors, and modifications.
- However, sources directly in `src/lib` are various adapters and APIs powered
  by the vendored libraries and are part of LinPipe.

Tests:
- Doctest is used for testing, with tests in `*_test.cpp` and a few utilities
  in `src/utils/test_utils.h`. The tests should be simple and understandable,
  not excessive.

While local testing runs on Linux, the code should be multiplatform and GitHub
Actions compile it under Linux gcc/clang, Windows VS, macOS clang, with both
amd64 and aarch64 on all platforms.

## LinPipe Bindings

Apart from the LinPipe binary and a REST service, swig-based bindings are
provided, with the Python one being the main entry point for the users.
The main swig interface file is `bindings/linpipe/linpipe.i` and the Python
bindings can be compiled by running `make -j4` in `bindings/python`.
While there are other language bindings, don't build them automatically.

The main user-facing API is generated from the `bindings/linpipe/linpipe.i`,
where comments with doxygen-like markup (`///` and `/**`) are exported verbatim
as Google-style Python docstrings by swig and then processed by zensical.

A PyPI package `linpipe` is also provided, with build scripts in `bindings/pypi`.
Currently there are no tests, but they are planned to be added later.

## LinPipe Training

Python-only `linpipe.training` package providing scripts for training and
evaluation of LinPipe models is present in `training` subdirectory.

## Zensical Docs

The `docs` directory contains LinPipe documentation that is built by zensical.
Do not interfere with it unless told to.

# Rules
- Do not commit to the git repo unless told to.
- When drafting commits, use short imperative commit messages with first line
  shorter than 70 characters and ending with a period. More details might follow
  when necessary in a body after a blank line.
- Never push to the GitHub repo under any circumstances.
- Run tests and formatting and linting checks when modifying LinPipe C++.
- Make Python bindings when modifying the swig interface files or LinPipe
  public API interfaces.
