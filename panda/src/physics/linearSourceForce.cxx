// Filename: linearSourceForce.cxx
// Created by:  charles (21Jun00)
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

#include "linearSourceForce.h"

TypeHandle LinearSourceForce::_type_handle;

////////////////////////////////////////////////////////////////////
//    Function : LinearSourceForce
//      Access : Public
// Description : Simple constructor
////////////////////////////////////////////////////////////////////
LinearSourceForce::
LinearSourceForce(const LPoint3f& p, FalloffType f, float r, float a,
          bool mass) :
  LinearDistanceForce(p, f, r, a, mass) {
}

////////////////////////////////////////////////////////////////////
//    Function : LinearSourceForce
//      Access : Public
// Description : Simple constructor
////////////////////////////////////////////////////////////////////
LinearSourceForce::
LinearSourceForce() :
  LinearDistanceForce(LPoint3f(0.0f, 0.0f, 0.0f), FT_ONE_OVER_R_SQUARED,
                      1.0f, 1.0f, true) {
}

////////////////////////////////////////////////////////////////////
//    Function : LinearSourceForce
//      Access : Public
// Description : copy constructor
////////////////////////////////////////////////////////////////////
LinearSourceForce::
LinearSourceForce(const LinearSourceForce &copy) :
  LinearDistanceForce(copy) {
}

////////////////////////////////////////////////////////////////////
//    Function : ~LinearSourceForce
//      Access : Public
// Description : Simple destructor
////////////////////////////////////////////////////////////////////
LinearSourceForce::
~LinearSourceForce() {
}

////////////////////////////////////////////////////////////////////
//    Function : make_copy
//      Access : Public
// Description : copier
////////////////////////////////////////////////////////////////////
LinearForce *LinearSourceForce::
make_copy(void) {
  return new LinearSourceForce(*this);
}

////////////////////////////////////////////////////////////////////
//    Function : get_child_vector
//      Access : Public
// Description : virtual force query
////////////////////////////////////////////////////////////////////
LVector3f LinearSourceForce::
get_child_vector(const PhysicsObject *po) {
  return (po->get_position() - get_force_center()) * get_scalar_term();
}

////////////////////////////////////////////////////////////////////
//     Function : output
//       Access : Public
//  Description : Write a string representation of this instance to
//                <out>.
////////////////////////////////////////////////////////////////////
void LinearSourceForce::
output(ostream &out) const {
  #ifndef NDEBUG //[
  out<<"LinearSourceForce";
  #endif //] NDEBUG
}

////////////////////////////////////////////////////////////////////
//     Function : write
//       Access : Public
//  Description : Write a string representation of this instance to
//                <out>.
////////////////////////////////////////////////////////////////////
void LinearSourceForce::
write(ostream &out, unsigned int indent) const {
  #ifndef NDEBUG //[
  out.width(indent); out<<""; out<<"LinearSourceForce:\n";
  LinearDistanceForce::write(out, indent+2);
  #endif //] NDEBUG
}
