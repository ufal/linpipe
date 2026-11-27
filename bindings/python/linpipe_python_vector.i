// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// The Python vectors additionally provide the sequence protocol: len, indexing
// with negative indices, item assignment and deletion, lazy iteration, and
// construction from an iterable. Out-of-range accesses raise an IndexError.
%include <std_common.i>
%include <std_except.i>

%{
#include "common.h"

// A C++ exception signalling that a Python exception has already been set.
struct LinPipePythonError {};

// The items of a Python iterable, as references released on destruction.
class LinPipePythonItems {
 public:
  explicit LinPipePythonItems(PyObject* values) {
    if (!values) return;
    PyObject* iterator = PyObject_GetIter(values);
    if (!iterator) throw LinPipePythonError();
    while (PyObject* item = PyIter_Next(iterator)) items_.push_back(item);
    Py_DECREF(iterator);
    if (PyErr_Occurred()) throw LinPipePythonError();
  }
  ~LinPipePythonItems() {
    for (PyObject* item : items_) Py_DECREF(item);
  }
  const std::vector<PyObject*>& items() const { return items_; }

 private:
  std::vector<PyObject*> items_;
};

// Return the pointer to the given element of the given type, or raise a Python exception.
static void* linpipe_vector_element(PyObject* element, swig_type_info* type, const char* vector_name, const char* element_name) {
  void* pointer = nullptr;
  if (!SWIG_IsOK(SWIG_ConvertPtr(element, &pointer, type, SWIG_POINTER_NO_NULL))) {
    PyErr_Format(PyExc_TypeError, "the elements of %s must be %s, not %s", vector_name, element_name, Py_TYPE(element)->tp_name);
    throw LinPipePythonError();
  }
  return pointer;
}
%}
%typemap(in, numinputs=0, noblock=1) PyObject** LINPIPE_SELF { $1 = &$self; }

// The part common to both the regular and the unique_ptr vectors.
%define %_linpipe_python_vector(Vector, Name, Type, PyType, Doc)
%ignore Vector::vector();
%feature("compactdefaultargs") Vector::vector;
%catches(std::out_of_range) Vector::__delitem__;
%catches(std::out_of_range) Vector::__getitem__;
%catches(std::out_of_range) Vector::__setitem__;
%catches(std::out_of_range) Vector::back;
%catches(std::out_of_range) Vector::erase;
%catches(std::out_of_range) Vector::front;
%catches(std::out_of_range) Vector::get;
%catches(std::out_of_range) Vector::insert;
%catches(std::out_of_range) Vector::pop_back;
%catches(std::out_of_range) Vector::set;
%feature("python:slot", "sq_length", functype="lenfunc") Vector::__len__;
%feature("python:slot", "sq_item", functype="ssizeargfunc") Vector::__getitem__;
%feature("python:slot", "mp_ass_subscript", functype="objobjargproc") Vector::__setitem__;
%feature("python:slot", "tp_iter", functype="getiterfunc") Vector::__iter__;
%typemap(pytyping) Vector, const Vector&, Vector&, Vector* #Name;
%feature("docstring") Vector Doc;
%feature("docstring") Vector::__delitem__ %{Remove the element at the given index.

Negative indices count from the end.

Args:
  index: the index of the element.

Raises:
  IndexError: if the index is out of range.%}
%feature("docstring") Vector::__getitem__ %{Return the element at the given index.

Negative indices count from the end.

Args:
  index: the index of the element.

Returns:
  the element at the given index; a reference for elements that are objects.

Raises:
  IndexError: if the index is out of range.%}
%feature("docstring") Vector::__iter__ %{Iterate over the elements.

Returns:
  an iterator over the elements.%}
%feature("docstring") Vector::__len__ %{Return the number of elements.

Returns:
  the number of elements.%}
%feature("docstring") Vector::back %{Return the last element.

Returns:
  the last element; a reference for elements that are objects.

Raises:
  IndexError: if there are no elements.%}
%feature("docstring") Vector::clear %{Remove all elements.%}
%feature("docstring") Vector::erase %{Remove the element at the given index.

Negative indices count from the end.

Args:
  index: the index of the element.

Raises:
  IndexError: if the index is out of range.%}
%feature("docstring") Vector::empty %{Return whether there are no elements.

Returns:
  whether the number of elements is zero.%}
%feature("docstring") Vector::front %{Return the first element.

Returns:
  the first element; a reference for elements that are objects.

Raises:
  IndexError: if there are no elements.%}
%feature("docstring") Vector::get %{Return the element at the given index.

Negative indices count from the end.

Args:
  index: the index of the element.

Returns:
  the element at the given index; a reference for elements that are objects.

Raises:
  IndexError: if the index is out of range.%}
%feature("docstring") Vector::pop_back %{Remove the last element.

Raises:
  IndexError: if there are no elements.%}
%feature("docstring") Vector::size %{Return the number of elements; the same as [len][].

Returns:
  the number of elements.%}
%insert("stub_pyi") %{
#@signature Name.__init__(values: "typing.Iterable[PyType]"=())
#@signature Name.__iter__() -> "typing.Iterator[PyType]"
#@signature Name.__setitem__(index: "int", value: "PyType") -> "None"
%}
%extend Vector {
  size_t __len__() const { return $self->size(); }
  PyObject* __iter__(PyObject** LINPIPE_SELF) { return PySeqIter_New(*LINPIPE_SELF); }
  void __delitem__(long long index) { $self->erase($self->begin() + linpipe_vector_index(index, $self->size())); }
  // Python deletes items using the mp_ass_subscript slot without a value.
  void __setitem__(long long index) { $self->erase($self->begin() + linpipe_vector_index(index, $self->size())); }
}
%enddef

// The sq_item slot of __getitem__ receives negative indices already increased
// by the vector size, so a negative index is out of range there.
%define %_linpipe_python_vector_index(index)
  (index < 0 ? throw std::out_of_range("index out of range") : linpipe_vector_index(index, $self->size()))
%enddef

%define LINPIPE_VECTOR_CUSTOMIZE(Name, Type, PyType, Doc)
%traits_swigtype(Type);
%fragment(SWIG_Traits_frag(Type));
%ignore std::vector<Type>::vector(const std::vector<Type>&);
%_linpipe_python_vector(std::vector<Type>, Name, Type, PyType, Doc)
%feature("docstring") std::vector<Type>::vector %{Create a new Name, optionally containing copies of the given values.

Args:
  values: the values to copy.%}
%feature("docstring") std::vector<Type>::__setitem__ %{Replace the element at the given index.

Negative indices count from the end.

Args:
  index: the index of the element.
  value: the new value of the element.

Raises:
  IndexError: if the index is out of range.%}
%feature("docstring") std::vector<Type>::insert %{Insert a value before the element at the given index.

Negative indices count from the end; the index may also be equal to the number
of elements, which appends the value.

Args:
  index: the index of the new element.
  value: the value to insert.

Raises:
  IndexError: if the index is out of range.%}
%feature("docstring") std::vector<Type>::push_back %{Append a value to the end.

Args:
  value: the value to append.%}
%feature("docstring") std::vector<Type>::set %{Replace the element at the given index.

Negative indices count from the end.

Args:
  index: the index of the element.
  value: the new value of the element.

Raises:
  IndexError: if the index is out of range.%}
%extend std::vector<Type> {
  vector(PyObject* values = nullptr) {
    LinPipePythonItems items(values);
    auto vector = std::make_unique<std::vector<Type>>();
    for (PyObject* item : items.items()) {
      if (!swig::check<Type>(item)) {
        PyErr_Format(PyExc_TypeError, "the elements of %s must be %s, not %s", #Name, #PyType, Py_TYPE(item)->tp_name);
        throw LinPipePythonError();
      }
      vector->push_back(swig::as<Type>(item));
    }
    return vector.release();
  }
  const Type& __getitem__(long long index) const { return (*$self)[%_linpipe_python_vector_index(index)]; }
  void __setitem__(long long index, const Type& value) { (*$self)[linpipe_vector_index(index, $self->size())] = value; }
}
%enddef

%define LINPIPE_UNIQUE_PTR_VECTOR_CUSTOMIZE(Name, Type, Doc)
%traits_swigtype(Type);
%fragment(SWIG_Traits_frag(Type));
%_linpipe_python_vector(std::vector<std::unique_ptr<Type>>, Name, Type, Type, Doc)
%feature("docstring") std::vector<std::unique_ptr<Type>>::vector %{Create a new Name, optionally moving the given values to its elements.

The given values are moved to newly allocated elements, and are reset to the
state of newly constructed objects.

Args:
  values: the values to move.%}
%feature("docstring") std::vector<std::unique_ptr<Type>>::__setitem__ %{Replace the element at the given index by moving the given value to it.

Negative indices count from the end. The given value is moved to a newly
allocated element, and is reset to the state of a newly constructed object.

Args:
  index: the index of the element.
  value: the value to move.

Raises:
  IndexError: if the index is out of range.%}
%feature("docstring") std::vector<std::unique_ptr<Type>>::insert %{Insert a new element before the element at the given index by moving the given value to it.

Negative indices count from the end; the index may also be equal to the number
of elements, which appends the element. The given value is moved to a newly
allocated element, and is reset to the state of a newly constructed object.

Args:
  index: the index of the new element.
  value: the value to move.

Raises:
  IndexError: if the index is out of range.%}
%feature("docstring") std::vector<std::unique_ptr<Type>>::push_back %{Append a new element by moving the given value to it.

The given value is moved to a newly allocated element, and is reset to the state
of a newly constructed object.

Args:
  value: the value to move.%}
%feature("docstring") std::vector<std::unique_ptr<Type>>::set %{Replace the element at the given index by moving the given value to it.

Negative indices count from the end. The given value is moved to a newly
allocated element, and is reset to the state of a newly constructed object.

Args:
  index: the index of the element.
  value: the value to move.

Raises:
  IndexError: if the index is out of range.%}
%extend std::vector<std::unique_ptr<Type>> {
  vector(PyObject* values = nullptr) {
    LinPipePythonItems items(values);
    std::vector<Type*> elements;
    for (PyObject* item : items.items())
      elements.push_back(static_cast<Type*>(linpipe_vector_element(item, swig::type_info<Type>(), #Name, #Type)));
    auto vector = std::make_unique<std::vector<std::unique_ptr<Type>>>();
    for (Type* element : elements) vector->push_back(linpipe_vector_move_to_unique(*element));
    return vector.release();
  }
  Type& __getitem__(long long index) const { return *(*$self)[%_linpipe_python_vector_index(index)]; }
  void __setitem__(long long index, Type& value) {
    size_t i = linpipe_vector_index(index, $self->size());
    (*$self)[i] = linpipe_vector_move_to_unique(value);
  }
}
%enddef
