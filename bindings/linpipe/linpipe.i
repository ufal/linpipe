// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%begin %{
#define SWIG_TYPE_TABLE linpipe
%}

%include "std_string.i"
%include <typemaps/swigmacros.swg>

// String parameters and return values corresponding to std::string_view in
// C++; a language can define it as std::string_view to avoid copies, while the
// default std::string works in all languages.
#ifndef LINPIPE_STRING_VIEW
#define LINPIPE_STRING_VIEW std::string
#endif

// Allow keyword arguments.
%feature("kwargs");

%{
#include "common.h"
using namespace linpipe;
%}

// All C++ exceptions are converted to LinPipeError in the target language.
%{
static std::string linpipe_error_message() {
  try {
    throw;
  } catch (const LinPipeError& error) {
    return fmt::format("{} at {}:{} in {}", error.what(), error.location().file_name(), error.location().line(),
                       error.location().function_name());
  } catch (const std::exception& error) {
    return error.what();
  } catch (...) {
    return "unknown exception";
  }
}
%}

// A language-specific interface file may define LINPIPE_RAISE_ERROR(message)
// raising its own LinPipeError before including this file; otherwise
// a generic runtime error prefixed with "LinPipeError: " is raised.
#ifndef LINPIPE_RAISE_ERROR
%include "exception.i"
#define LINPIPE_RAISE_ERROR(message) SWIG_exception(SWIG_RuntimeError, ("LinPipeError: " + message).c_str())
#endif
%exception {
  try {
    $action
  } catch (...) {
    LINPIPE_RAISE_ERROR(linpipe_error_message());
  }
}

// LINPIPE_FACTORY(Base, Types...) makes every function returning Base* or Base& return
// the listed class whose TYPE equals the object's type(), or Base otherwise.
// A language-specific interface file may define its own LINPIPE_FACTORY
// before including this file; otherwise the generic version below is used,
// which selects the wrapper type in C++ (Python, Perl, Ruby, PHP, Lua, ...).
#ifndef LINPIPE_FACTORY
%define %_linpipe_factory_descriptor(Type)
    if ($1->type() == Type::TYPE) descriptor = $descriptor(Type*);
%enddef
%define LINPIPE_FACTORY(Base, Types...)
%typemap(out) Base*, Base& {
  swig_type_info* descriptor = $descriptor(Base*);
  if ($1) {
    %formacro(%_linpipe_factory_descriptor, Types)
  }
  %set_output(SWIG_NewPointerObj(%as_voidptr($1), descriptor, $owner | %newpointer_flags));
}
%enddef
#endif

// Helper for language-specific factories: the name of the listed class
// whose TYPE equals the object's type(), or Base otherwise.
%define %_linpipe_factory_name(Type)
    if ($self->type() == Type::TYPE) return #Type;
%enddef
%define LINPIPE_FACTORY_CLASS_NAME(Base, Types...)
%extend Base {
  const char* swig_factory_class_name() const {
    %formacro(%_linpipe_factory_name, Types)
    return #Base;
  }
}
%enddef

// SWIG cannot write string default values to a .pyi stub, and generates *args
// instead; methods with them must therefore state their Python signature using
// LINPIPE_PYI_SIGNATURE, which is filled in by bindings/python/pyi_signatures.py.
#ifdef SWIGPYTHON
%define LINPIPE_PYI_SIGNATURE(Signature)
%insert("stub_pyi") %{
#@signature Signature
%}
%enddef
#else
#define LINPIPE_PYI_SIGNATURE(Signature)
#endif

%include "linpipe_vector.i"

LINPIPE_VECTOR(Strings, std::string, str, "A vector of strings.");

// The views must precede the layers, whose methods return views.
%include "linpipe_views.i"
%include "linpipe_layers.i"
%include "linpipe_version.i"
