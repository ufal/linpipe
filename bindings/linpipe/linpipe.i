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
%include "std_string_view.i"
%include "std_vector.i"
%include <typemaps/swigmacros.swg>
%typemap(pytyping) std::string_view "str";

%{
#include "core/layer.h"
#include "core/selector.h"
#include "core/view.h"
#include "layers/plain_text_layer.h"
#include "lib/json/json.h"
#include "version/version.h"
#include "views/plain_text_view.h"
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

// LINPIPE_FACTORY(Base, Types...) makes every function returning Base* return
// the listed class whose TYPE equals the object's type(), or Base otherwise.
// A language-specific interface file may define its own LINPIPE_FACTORY
// before including this file; otherwise the generic version below is used,
// which selects the wrapper type in C++ (Python, Perl, Ruby, PHP, Lua, ...).
#ifndef LINPIPE_FACTORY
%define %_linpipe_factory_descriptor(Type)
    if ($1->type() == Type::TYPE) descriptor = $descriptor(Type*);
%enddef
%define LINPIPE_FACTORY(Base, Types...)
%typemap(out) Base* {
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

LINPIPE_FACTORY(View, PlainTextView);
%typemap(pytyping) View* "View";
/**
 A base class of all LinPipe views.

 A view provides access to the data of a layer; it refers to the layer,
 so the layer must outlive it.*/
class View {
 public:
  /**
   Return the type of the view.

   Returns:
     the view type, e.g., `plain_text`.*/
  virtual std::string_view type() const = 0;

  /**
   Return the model which created the underlying layer.

   Returns:
     the model of the layer, or an empty string.*/
  std::string_view model() const;

  /**
   Return the license of the underlying layer data.

   Returns:
     the license of the layer, or an empty string.*/
  std::string_view license() const;

  /**
   Return the creation time of the underlying layer.

   Returns:
     the timestamp of the layer as an ISO 8601 date-time in UTC, or an empty string.*/
  std::string_view timestamp() const;
};

/**
 A view providing a plain text.*/
class PlainTextView : public View {
 public:
  /**
   Return the plain text.

   Returns:
     the text of the underlying layer.*/
  virtual std::string_view text() const = 0;
};

LINPIPE_FACTORY(Layer, PlainTextLayer);
%typemap(pytyping) Layer* "Layer";
/**
 A base class of all LinPipe layers.

 A layer holds one kind of annotation of a document, for example its plain text.
 Its data can be accessed through views, created by Layer.create_view.*/
class Layer {
 public:
  LINPIPE_PYI_SIGNATURE(Layer.create(type: "str", name: "str"=""))
  %newobject create;
  %extend {
    /**
     Create a new layer of the given type.

     Args:
       type: the type of the layer to create, e.g., `plain_text`.
       name: the name of the layer; the layer type is used when empty.

     Returns:
       a new layer of the given type.*/
    static Layer* create(std::string_view type, std::string_view name = "") { return Layer::create(type, name).release(); }
  }

  %newobject create_from_json;
  %extend {
    /**
     Create a new layer from its JSON representation.

     Args:
       json: the JSON representation of the layer, as produced by Layer.to_json.

     Returns:
       a new layer described by the given JSON.*/
    static Layer* create_from_json(std::string_view json) {
      Json parsed = Json::parse(json);
      return Layer::create_from_json(parsed).release();
    }
  }

  %newobject create_view;
  %extend {
    /**
     Create a view of the layer matching the given selectors.

     The view refers to the layer, so the layer must outlive it.

     Args:
       selectors: colon-separated `key=value` or `value` selectors, where a missing key means `type`;
         a literal colon is written as `::`.

     Returns:
       the unique view of the layer matching all the selectors.*/
    View* create_view(std::string_view selectors) const { return $self->create_view(parse_selectors(selectors)).release(); }
  }

  /**
   Return the type of the layer.

   Returns:
     the layer type, e.g., `plain_text`.*/
  virtual std::string_view type() const = 0;

  /// The name of the layer.
  std::string name;
  /// The model which created the layer, if any.
  std::string model;
  /// The license of the layer data, if any.
  std::string license;
  /// The creation time of the layer as an ISO 8601 date-time in UTC, e.g., 2026-01-01T12:00:00Z.
  std::string timestamp;

  %extend {
    /**
     Check whether the layer matches the given selectors.

     Args:
       selectors: colon-separated `key=value` or `value` selectors, where a missing key means `type`;
         a literal colon is written as `::`.

     Returns:
       whether the layer matches all the selectors.*/
    bool matches(std::string_view selectors) const { return $self->matches(parse_selectors(selectors)); }
  }

  %extend {
    /**
     Format the layer as JSON.

     Args:
       indent: the number of spaces to indent the JSON with; a compact single-line JSON is produced when negative.

     Returns:
       the JSON representation of the layer.*/
    std::string to_json(int indent = -1) const { return $self->to_json().dump(indent); }
  }

  /**
   Format the layer as HTML.

   Returns:
     an HTML representation of the layer.*/
  virtual std::string to_html() const = 0;
};

/**
 A layer containing a plain text.*/
class PlainTextLayer : public Layer {
 public:
  LINPIPE_PYI_SIGNATURE(PlainTextLayer.__init__(name: "str"=""))
  /**
   Create an empty plain text layer.

   Args:
     name: the name of the layer; the layer type is used when empty.*/
  explicit PlainTextLayer(std::string_view name = "");

  std::string_view type() const;
  std::string to_html() const;

  /// The text of the layer.
  std::string text;
};

%typemap(pytyping) Version "Version";
/**
 A class representing a LinPipe Version.*/
class Version {
 public:
  /// Major version number.
  unsigned major;
  /// Minor version number.
  unsigned minor;
  /// Patch version number.
  unsigned patch;
  /// Prerelease label; release version if empty, development version otherwise.
  std::string prerelease;

  /**
   Format a Version as a string.

   Returns:
     the current version formatted as string.*/
  std::string as_string() const;

  /**
   Returns the current version.

   Returns:
     a Version object representing the current version.*/
  static Version current();
};
