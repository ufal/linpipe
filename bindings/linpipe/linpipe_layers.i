// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%{
#include "core/layer.h"
#include "core/selector.h"
#include "layers/plain_text_layer.h"
#include "lib/json/json.h"
%}

LINPIPE_FACTORY(Layer, PlainTextLayer);
%typemap(pytyping) Layer*, Layer& "Layer";
/**
 A base class of all LinPipe layers.
 */
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
    static Layer* create(LINPIPE_STRING_VIEW type, LINPIPE_STRING_VIEW name = "") { return Layer::create(type, name).release(); }
  }

  %newobject create_from_json;
  %extend {
    /**
     Create a new layer from its JSON representation.

     Args:
       json: the JSON representation of the layer, as produced by [linpipe.Layer.to_json][].

     Returns:
       a new layer described by the given JSON.*/
    static Layer* create_from_json(LINPIPE_STRING_VIEW json) {
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
       the unique view of the layer matching all the selectors.

     Raises:
       LinPipeError: if no view or multiple views match the selectors, or if the selectors are invalid.*/
    View* create_view(LINPIPE_STRING_VIEW selectors) const { return $self->create_view(parse_selectors(selectors)).release(); }
  }

  /**
   Return the type of the layer.

   Returns:
     the layer type, e.g., `plain_text`.*/
  virtual LINPIPE_STRING_VIEW type() const = 0;

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
    bool matches(LINPIPE_STRING_VIEW selectors) const { return $self->matches(parse_selectors(selectors)); }
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
  explicit PlainTextLayer(LINPIPE_STRING_VIEW name = "");

  LINPIPE_STRING_VIEW type() const;
  std::string to_html() const;

  /// The text of the layer.
  std::string text;
};

LINPIPE_UNIQUE_PTR_VECTOR(Layers, Layer, "A vector of layers.");
