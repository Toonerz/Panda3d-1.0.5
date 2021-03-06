// Filename: pre_maya_include.h
// Created by:  drose (11Apr02)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001 - 2004, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://etc.cmu.edu/panda3d/docs/license/ .
//
// To contact the maintainers of this program write to
// panda3d-general@lists.sourceforge.net .
//
////////////////////////////////////////////////////////////////////

// This header file defines a few things that are necessary to define
// before including any Maya headers, just to work around some of
// Maya's assumptions about the compiler.  It must not try to protect
// itself from multiple inclusion with #ifdef .. #endif, since it must
// be used each time it is included.

// Maya will try to typedef bool unless this symbol is defined.
#ifndef _BOOL
#define _BOOL 1
#endif

#ifdef MAYA_PRE_5_0
// Old versions of Maya, before version 5.0, used <iosteam.h>
// etc. instead of the new <iostream> headers.  This requires some
// workarounds to make this work compatibly with Panda, which uses the
// new headers.

// In windows, the antiquated headers define completely unrelated (and
// incompatible) classes from those declared in the new headers.  On
// the other hand, in gcc the antiquated headers seem to be references
// to the new template classes, so under gcc we also have to declare
// typedefs to make this work.
#ifdef __GNUC__
#ifndef PRE_MAYA_INCLUDE_H
#define PRE_MAYA_INCLUDE_H
#include <iostream.h>
typedef ostream maya_ostream;
typedef istream maya_istream;
#endif
#endif  // __GNUC__

#define ostream maya_ostream
#define istream maya_istream

#else  // MAYA_PRE_5_0

// In Maya 5.0, the headers seem to provide the manifest
// REQUIRE_IOSTREAM, which forces it to use the new <iostream> headers
// instead of the old <iostream.h> headers.  It also says this is for
// Linux only, but it seems to work just fine on Windows, obviating
// the need for sneaky #defines in this and in post_maya_include.h.
#ifdef HAVE_IOSTREAM
#define REQUIRE_IOSTREAM
#endif  // HAVE_IOSTREAM

#endif  // MAYA_PRE_5_0

