// Filename: linearVectorForce.cxx
// Created by:  charles (14Jun00)
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

#include "datagram.h"
#include "datagramIterator.h"
#include "bamReader.h"
#include "bamWriter.h"

#include "linearVectorForce.h"

TypeHandle LinearVectorForce::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function : LinearVectorForce
//       Access : Public
//  Description : Vector Constructor
////////////////////////////////////////////////////////////////////
LinearVectorForce::
LinearVectorForce(const LVector3f& vec, float a, bool mass) :
  LinearForce(a, mass),
  _fvec(vec) {
}

////////////////////////////////////////////////////////////////////
//     Function : LinearVectorForce
//       Access : Public
//  Description : Default/Piecewise constructor
////////////////////////////////////////////////////////////////////
LinearVectorForce::
LinearVectorForce(float x, float y, float z, float a, bool mass) :
  LinearForce(a, mass) {
  _fvec.set(x, y, z);
}

////////////////////////////////////////////////////////////////////
//     Function : LinearVectorForce
//       Access : Public
//  Description : Copy Constructor
////////////////////////////////////////////////////////////////////
LinearVectorForce::
LinearVectorForce(const LinearVectorForce &copy) :
  LinearForce(copy) {
  _fvec = copy._fvec;
}

////////////////////////////////////////////////////////////////////
//     Function : LinearVectorForce
//       Access : Public
//  Description : Destructor
////////////////////////////////////////////////////////////////////
LinearVectorForce::
~LinearVectorForce() {
}

////////////////////////////////////////////////////////////////////
//    Function : make_copy
//      Access : Public, virtual
// Description : copier
////////////////////////////////////////////////////////////////////
LinearForce *LinearVectorForce::
make_copy() {
  return new LinearVectorForce(*this);
}

////////////////////////////////////////////////////////////////////
//    Function : get_child_vector
//      Access : Public
// Description : vector access
////////////////////////////////////////////////////////////////////
LVector3f LinearVectorForce::
get_child_vector(const PhysicsObject *) {
  return _fvec;
}

////////////////////////////////////////////////////////////////////
//     Function : output
//       Access : Public
//  Description : Write a string representation of this instance to
//                <out>.
////////////////////////////////////////////////////////////////////
void LinearVectorForce::
output(ostream &out) const {
  #ifndef NDEBUG //[
  out<<"LinearVectorForce";
  #endif //] NDEBUG
}

////////////////////////////////////////////////////////////////////
//     Function : write
//       Access : Public
//  Description : Write a string representation of this instance to
//                <out>.
////////////////////////////////////////////////////////////////////
void LinearVectorForce::
write(ostream &out, unsigned int indent) const {
  #ifndef NDEBUG //[
  out.width(indent); out<<""; out<<"LinearVectorForce:\n";
  out.width(indent+2); out<<""; out<<"_fvec "<<_fvec<<"\n";
  LinearForce::write(out, indent+2);
  #endif //] NDEBUG
}
