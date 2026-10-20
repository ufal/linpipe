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
%include "std_vector.i"

%{
#include "common.h"
using namespace linpipe;
%}

%{
#include "version/version.h"
%}
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
