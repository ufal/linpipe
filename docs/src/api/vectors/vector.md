# Vector

A vector is a typed sequence of elements (a SWIG-wrapped `std::vector`); every
vector type is a separate class, e.g., [Strings][linpipe.Strings] or
[Layers][linpipe.Layers].

## Common API

All vectors provide the same core API in all languages: `get`, `set`, `size`,
`empty`, `clear`, `front`, `back`, `push_back`, `pop_back`, `insert`, `erase`,
and the constructor; `get`, `set`, `insert`, and `erase` accept negative indices
counting from the end.

### Python Additions

In Python, the vectors also support [len][], lazy iteration, indexing using the
`[]` operator (but slices are not supported as indices), item assignment and
deletion, and construction from any iterable. Accessing an element out of range
raises an [IndexError][] in Python (instead of a [LinPipeError][linpipe.LinPipeError]
raised in other languages).

## Kinds of Vectors

### Vectors of Copies

A vector of copies, like [Strings][linpipe.Strings], stores copies of the
inserted values, and the methods returning elements return their copies;
therefore, modifying a returned value does not modify the vector, and the
inserted values stay independent of the vector.

### Vectors of Uniquely-Owned Objects

A vector of uniquely-owned objects, like [Layers][linpipe.Layers], stores every
element separately, and the methods returning elements return references, so
modifying the returned object modifies the vector element. Such references are
valid until the element is replaced or removed, and the vector must be kept
alive as long as they are used.

Inserting an object into such a vector moves its content to a newly allocated
element, and the inserted object is reset to the state of a newly constructed
object (similarly to `std::move` in C++); the inserted object therefore stays
independent of the vector.
