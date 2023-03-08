# LinPipe Codestyle

- We use full C++17.

- We indent by two spaces, and the sources should contain no tabs.

- Classes are named using CamelCase, functions and variables using snake_case,
  constants usually in ALL_UPPERCASE.

- Single-line statements usually do not use curly brackets, unless an `else`
  could be ambiguous, or the other block of `if`-`else` is multiline.

- We do not have a strict line-length limit, but the code should target 120
  characters per line.

## Technical Remarks

- Multiple header file inclusion is prevented using `#pragma once`.

- Every header file and every source without a corresponding header file
  should start by including `common.h`.

- The include paths can either be relative to the source file, or absolute
  from the project root; we always compile from the project root.

- All code must be in the `linpipe` namespace (note that `std` is opened in it).

- Right now we use a single exception type `LinpipeError` to throw LinPipe
  errors (of course, library functions throw `std` exceptions).
