// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// The vectors provide the same core API in all target languages; the methods
// returning elements return references for elements that are objects. SWIG's
// own std::vector is not used, because it provides a different API in every
// language; instead, the following minimal declaration is extended by the core
// API and by language-specific additions (e.g., the Python sequence protocol).

%feature("kwargs", "0");
namespace std {
template <class T> class vector {
 public:
  vector();
  vector(const vector& other);

  size_t size() const;
  bool empty() const;
  void clear();
  void push_back(const T& value);
};
}
%feature("kwargs");

%{
static size_t linpipe_vector_index(long long index, size_t size) {
  long long signed_size = size;
  if (index < -signed_size || index >= signed_size) throw std::out_of_range("index out of range");
  return index < 0 ? index + signed_size : index;
}

// An index for inserting, which can also be equal to the size.
static size_t linpipe_vector_insert_index(long long index, size_t size) {
  long long signed_size = size;
  if (index < -signed_size || index > signed_size) throw std::out_of_range("index out of range");
  return index < 0 ? index + signed_size : index;
}

static void linpipe_vector_nonempty(size_t size) {
  if (!size) throw std::out_of_range("vector is empty");
}

// Move the given value to a newly allocated object of its dynamic type, which
// resets the value to the state of a newly constructed object. Types which may
// have descendants must provide move_to_unique doing that without slicing.
template <class T>
static std::unique_ptr<T> linpipe_vector_move_to_unique(T& value) {
  if constexpr (requires { value.move_to_unique(); }) {
    return value.move_to_unique();
  } else {
    static_assert(!std::is_polymorphic_v<T> || std::is_final_v<T>,
                  "a type which may have descendants must provide move_to_unique to be moved without slicing");
    auto moved = std::make_unique<T>(std::move(value));
    value = T();
    return moved;
  }
}
%}

// A language-specific interface file may define LINPIPE_VECTOR_CUSTOMIZE and
// LINPIPE_UNIQUE_PTR_VECTOR_CUSTOMIZE with the same arguments as the macros
// below, which are then used to customize every vector before it is defined.
#ifndef LINPIPE_VECTOR_CUSTOMIZE
#define LINPIPE_VECTOR_CUSTOMIZE(Name, Type, PyType, Doc)
#endif
#ifndef LINPIPE_UNIQUE_PTR_VECTOR_CUSTOMIZE
#define LINPIPE_UNIQUE_PTR_VECTOR_CUSTOMIZE(Name, Type, Doc)
#endif

// LINPIPE_VECTOR(Name, Type, PyType, Doc) exports std::vector<Type> as a class
// Name documented by Doc, whose elements have the Python type PyType. The core
// API methods accessing elements are bounds-checked, and erase, get, insert, and
// set also accept negative indices counting from the end.
%define LINPIPE_VECTOR(Name, Type, PyType, Doc)
%feature("kwargs", "0");
LINPIPE_VECTOR_CUSTOMIZE(Name, Type, PyType, Doc)
%extend std::vector<Type> {
  const Type& back() const {
    linpipe_vector_nonempty($self->size());
    return $self->back();
  }
  void erase(long long index) { $self->erase($self->begin() + linpipe_vector_index(index, $self->size())); }
  const Type& front() const {
    linpipe_vector_nonempty($self->size());
    return $self->front();
  }
  const Type& get(long long index) const { return (*$self)[linpipe_vector_index(index, $self->size())]; }
  void insert(long long index, const Type& value) { $self->insert($self->begin() + linpipe_vector_insert_index(index, $self->size()), value); }
  void pop_back() {
    linpipe_vector_nonempty($self->size());
    $self->pop_back();
  }
  void set(long long index, const Type& value) { (*$self)[linpipe_vector_index(index, $self->size())] = value; }
}
%template(Name) std::vector<Type>;
%feature("kwargs");
%enddef

// LINPIPE_UNIQUE_PTR_VECTOR(Name, Type, Doc) exports std::vector<std::unique_ptr<Type>>
// as a class Name documented by Doc, with the same core API as LINPIPE_VECTOR,
// except for the copy constructor. The methods inserting elements move the given
// values to newly allocated elements, resetting the values to the state of newly
// constructed objects; the methods returning elements return references.
%define LINPIPE_UNIQUE_PTR_VECTOR(Name, Type, Doc)
%feature("kwargs", "0");
namespace std {
template <> class vector<std::unique_ptr<Type>> {
 public:
  vector();

  size_t size() const;
  bool empty() const;
  void clear();
};
}
LINPIPE_UNIQUE_PTR_VECTOR_CUSTOMIZE(Name, Type, Doc)
%extend std::vector<std::unique_ptr<Type>> {
  Type& back() const {
    linpipe_vector_nonempty($self->size());
    return *$self->back();
  }
  void erase(long long index) { $self->erase($self->begin() + linpipe_vector_index(index, $self->size())); }
  Type& front() const {
    linpipe_vector_nonempty($self->size());
    return *$self->front();
  }
  Type& get(long long index) const { return *(*$self)[linpipe_vector_index(index, $self->size())]; }
  void insert(long long index, Type& value) {
    size_t i = linpipe_vector_insert_index(index, $self->size());
    $self->insert($self->begin() + i, linpipe_vector_move_to_unique(value));
  }
  void pop_back() {
    linpipe_vector_nonempty($self->size());
    $self->pop_back();
  }
  void push_back(Type& value) { $self->push_back(linpipe_vector_move_to_unique(value)); }
  void set(long long index, Type& value) {
    size_t i = linpipe_vector_index(index, $self->size());
    (*$self)[i] = linpipe_vector_move_to_unique(value);
  }
}
%template(Name) std::vector<std::unique_ptr<Type>>;
%feature("kwargs");
%enddef
