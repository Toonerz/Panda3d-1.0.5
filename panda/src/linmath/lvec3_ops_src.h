// Filename: lvec3_ops_src.h
// Created by:  drose (08Mar00)
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

// When possible, operators have been defined within the classes.
// This file defines operator functions outside of classes where
// necessary.  It also defines some convenient out-of-class wrappers
// around in-class functions (like dot, length, normalize).


// scalar * vec (vec * scalar is defined in class)
INLINE_LINMATH FLOATNAME(LVecBase3)
operator * (FLOATTYPE scalar, const FLOATNAME(LVecBase3) &a);

INLINE_LINMATH FLOATNAME(LPoint3)
operator * (FLOATTYPE scalar, const FLOATNAME(LPoint3) &a);

INLINE_LINMATH FLOATNAME(LVector3)
operator * (FLOATTYPE scalar, const FLOATNAME(LVector3) &a);


// dot product
INLINE_LINMATH FLOATTYPE
dot(const FLOATNAME(LVecBase3) &a, const FLOATNAME(LVecBase3) &b);


// cross product
INLINE_LINMATH FLOATNAME(LVecBase3)
cross(const FLOATNAME(LVecBase3) &a, const FLOATNAME(LVecBase3) &b);

INLINE_LINMATH FLOATNAME(LVector3)
cross(const FLOATNAME(LVector3) &a, const FLOATNAME(LVector3) &b);


// Length of a vector.
INLINE_LINMATH FLOATTYPE
length(const FLOATNAME(LVector3) &a);

// A normalized vector.
INLINE_LINMATH FLOATNAME(LVector3)
normalize(const FLOATNAME(LVector3) &v);

INLINE_LINMATH void
generic_write_datagram(Datagram &dest, const FLOATNAME(LVecBase3) &value);
INLINE_LINMATH void
generic_read_datagram(FLOATNAME(LVecBase3) &result, DatagramIterator &source);

#include "lvec3_ops_src.I"