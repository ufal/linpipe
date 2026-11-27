// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%{
#include "core/view.h"
#include "views/plain_text_view.h"
%}

LINPIPE_FACTORY(View, PlainTextView);
%typemap(pytyping) View*, View& "View";
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
  virtual LINPIPE_STRING_VIEW type() const = 0;

  /**
   Return the model which created the underlying layer.

   Returns:
     the model of the layer, or an empty string.*/
  LINPIPE_STRING_VIEW model() const;

  /**
   Return the license of the underlying layer data.

   Returns:
     the license of the layer, or an empty string.*/
  LINPIPE_STRING_VIEW license() const;

  /**
   Return the creation time of the underlying layer.

   Returns:
     the timestamp of the layer as an ISO 8601 date-time in UTC, or an empty string.*/
  LINPIPE_STRING_VIEW timestamp() const;
};

/**
 A view providing a plain text.*/
class PlainTextView : public View {
 public:
  /**
   Return the plain text.

   Returns:
     the text of the underlying layer.*/
  virtual LINPIPE_STRING_VIEW text() const = 0;
};
