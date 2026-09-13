# LinPipe Codestyle

- Use full C++17.

- Indent by two spaces; the sources should contain no tabs.

- Classes are named using `CamelCase`, functions and variables using `snake_case`,
  constants usually in `ALL_UPPERCASE`.

- Single-line statements usually do not use curly brackets, unless an `else`
  could be ambiguous, or the other block of `if`-`else` is multiline.

- We do not have a strict line-length limit, but the code should target 140
  characters per line.

- Private class members have underscore at the end.

## Technical Remarks

- Multiple header file inclusion is prevented using `#pragma once`.

- Every header file should include `common.h`.

- The source files should include `common.h` only when not included
  transitively from some header file.

- The include paths should be preferably absolute from the project root.

- All code must be in the `linpipe` namespace (note that `std` is opened in it).

- Right now we use a single exception type `LinpipeError` to throw LinPipe
  errors (of course, library functions throw `std` exceptions).
