# Log of KBELik Progress

### May 4, 2023
- Reviewing of source files of both maps and all `map_values` and discussion of their further
  improvements.

- Fixed warnings.

- Updated makefiles and moved code with templates to headers.

- Discussed possible ways to compress jsons from wikidata.
    - use external compressing algorithm, given enough context it could be
      reasonably efficient
    - use some external json compression algorithm, without any context it is
      unlikely to be good.
    - Devise custom compression for wikidata. It looks time consuming but could
      be the most efficient. Some ideas:
        - Create map mapping strings (keys of jsons) to numbers, there are a lot
	  of repeating keys
	- Use the fact that key gives us information about the structure of the
	  underlying value

- [ ] Implement header for variable length integers (only unsigned)

- [ ] serialization type (id) doesn't need to be given to a constructor of `dynamic_map`. It is only
  important for the saving of `persistent_map`. Also don't use integer to define
  serialization type but prefer enum.

- [ ] Write better algorithm for the index searching and try to mock the real
  use case.

- [ ] Calculate number of keys and their types in wikidata.

- [ ] `persistant_map` doesn't need indicator to know if the map is already
  mapped. This functionality can be provided by the file descriptor.


### Apr 17, 2023
- (Hopefully) mostly definitive version of public interfaces of `agnostic_kbelik,
  persistent_map, dynamic_map, map_values::Int4`

- Discussion of `map_values`

- Discussion of code-style.

### Apr 12, 2023
- Discussion of blueprints for `map_values` (reworked week later).

- No need to use virtual methods in MapValue because templated don't require it.

- Discussion of usage of unique_ptrs.

- factory methods vs. constructors

- Static class TypeValue (de)serializing Type. Discussion of how to implement
  it.

- persistent_map might need separate serializer

### Apr 3, 2023
- Updating LinPipe build infrastructure to support tests in `dev/kbelik`.

- Fixed the bug in serialization that caused `ofstream` construction to crash.

- Setting environment on AIC to be able to run Windows binaries.
### Mar 27, 2023
- Start compiling mmap on Windows.

- AgnosticKbelik header file discussion.

- [x] Upload final DaMuEl to AIC.

- [x] Use templates in persistent_map

- [x] Finish headers of both kbeliks

- [x] Implement an MVP of persistent_map that implements its own build and
  answers queries.

### Mar 20, 2023
- Rename _index_ to _map_; map is composed of index and values.
  - [x] Update assignment.md.

- Implement mmap for Windows using `CreateFile`, `CreateFileMapping`,
  and `MapViewOfFile`; closing is performed by `UnmapViewOfFile` and
  `CloseHandle`.

- Persistent map will be an interface parametrized by the key and
  value types; the first implementation is a sorted set of values with
  and index pointing to chunks of fixed number of values, using exponential
  widening search.

- [x] Install Wine and VisualC++ compiler on AIC.

### Mar 13, 2023
- Discussing the results of binary/exponential/interpolated search in the index.

- Discussing the histogram of Wikidata QIDs.

- [ ] Learn how to use `mmap` on major platforms (Linux, Windows, macOS).

- [ ] Start writing interfaces (header files) of the persistent index,
  the language agnostic KBELik, the language specific KBELik, and their
  encoders (serializers). These interfaces can be part of the detailed
  specification.

- [ ] Stretch goal: start writing the persistent index search if time allows.

### Mar 6, 2023
- Discussing posible implementations of the QID and string persistent indices,
  and the distribution of QIDs.

- [x] Think about a dazzling project name.
  - KBELik accepted as the project name.

- [ ] Decide a suitable workflow in the repository.
  - MS: I think the development should happen in a separate branch. However,
    it will be regularly merged with the `master` branch (maybe on the meetings?
    regularly through pull requests?). The commits to this log can go directly
    to the `master` branch.

### Feb 27, 2023
- Discussing possible architectures of the efficient persistent mmapable indices.

### Feb 20, 2023
- KBELik selected as the ISP topic.

- AIC cluster account created.

- [x] Brainstorming about how to implement efficient persistent mmapable indices.

### Feb 13, 2023
- Discussion of possible topics for the ISP.
