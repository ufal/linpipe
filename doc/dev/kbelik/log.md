# Log of KBELik Progress

### Mar 20, 2023
- Rename _index_ to _map_; map is composed of index and values.
  - [ ] Update assignment.md.

- Implement mmap for Windows using `CreateFile`, `CreateFileMapping`,
  and `MapViewOfFile`; closing is performed by `UnmapViewOfFile` and
  `CloseHandle`.

- Persistent map will be an interface parametrized by the key and
  value types; the first implementation is a sorted set of values with
  and index pointing to chunks of fixed number of values, using exponential
  widening search.

- [ ] Install Wine and VisualC++ compiler on AIC.

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
