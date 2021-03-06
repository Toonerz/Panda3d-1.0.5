// Filename: fog.cxx
// Created by:  drose (14Mar02)
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

#include "pandabase.h"

#include "fog.h"

#include "mathNumbers.h"
#include "nodePath.h"
#include "transformState.h"
#include "bamReader.h"
#include "bamWriter.h"
#include "datagram.h"
#include "datagramIterator.h"

#include <stddef.h>

TypeHandle Fog::_type_handle;

ostream &
operator << (ostream &out, Fog::Mode mode) {
  switch (mode) {
  case Fog::M_linear:
    return out << "linear";

  case Fog::M_exponential:
    return out << "exponential";

  case Fog::M_exponential_squared:
    return out << "exponential-squared";
  }

  return out << "**invalid**(" << (int)mode << ")";
}

////////////////////////////////////////////////////////////////////
//     Function: Fog::Constructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
Fog::
Fog(const string &name) : 
  PandaNode(name) 
{
  _mode = M_linear;
  _color.set(1.0f, 1.0f, 1.0f, 1.0f);
  _linear_onset_point.set(0.0f, 0.0f, 0.0f);
  _linear_opaque_point.set(0.0f, 100.0f, 0.0f);
  _exp_density = 0.5f;
  _linear_fallback_cosa = -1.0f;
  _linear_fallback_onset = 0.0f;
  _linear_fallback_opaque = 0.0f;
  _transformed_onset = 0.0f;
  _transformed_opaque = 0.0f;
}

////////////////////////////////////////////////////////////////////
//     Function: Fog::Copy Constructor
//       Access: Protected
//  Description:
////////////////////////////////////////////////////////////////////
Fog::
Fog(const Fog &copy) :
  PandaNode(copy)
{
  _mode = copy._mode;
  _color = copy._color;
  _linear_onset_point = copy._linear_onset_point;
  _linear_opaque_point = copy._linear_opaque_point;
  _exp_density = copy._exp_density;
  _linear_fallback_cosa = copy._linear_fallback_cosa;
  _linear_fallback_onset = copy._linear_fallback_onset;
  _linear_fallback_opaque = copy._linear_fallback_opaque;
  _transformed_onset = copy._transformed_onset;
  _transformed_opaque = copy._transformed_opaque;
}

////////////////////////////////////////////////////////////////////
//     Function: Fog::Destructor
//       Access: Public, Virtual
//  Description:
////////////////////////////////////////////////////////////////////
Fog::
~Fog() {
}

////////////////////////////////////////////////////////////////////
//     Function: Fog::make_copy
//       Access: Public, Virtual
//  Description: Returns a newly-allocated Node that is a shallow copy
//               of this one.  It will be a different Node pointer,
//               but its internal data may or may not be shared with
//               that of the original Node.
////////////////////////////////////////////////////////////////////
PandaNode *Fog::
make_copy() const {
  return new Fog(*this);
}

////////////////////////////////////////////////////////////////////
//     Function: Fog::xform
//       Access: Public, Virtual
//  Description: Transforms the contents of this node by the indicated
//               matrix, if it means anything to do so.  For most
//               kinds of nodes, this does nothing.
////////////////////////////////////////////////////////////////////
void Fog::
xform(const LMatrix4f &mat) {
  _linear_onset_point = _linear_onset_point * mat;
  _linear_opaque_point = _linear_opaque_point * mat;
}

////////////////////////////////////////////////////////////////////
//     Function: Fog::output
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
void Fog::
output(ostream &out) const {
  out << "fog: " << _mode;
  switch (_mode) {
  case M_linear:
    out << "(" << _linear_onset_point << ") -> ("
        << _linear_opaque_point << ")";
    break;

  case M_exponential:
  case M_exponential_squared:
    out << _exp_density;
    break;
  };
}

////////////////////////////////////////////////////////////////////
//     Function: Fog::adjust_to_camera
//       Access: Public
//  Description: This function is intended to be called by the cull
//               traverser to compute the appropriate camera-relative
//               onset and opaque distances, based on the fog node's
//               position within the scene graph (if linear fog is in
//               effect).
////////////////////////////////////////////////////////////////////
void Fog::
adjust_to_camera(const TransformState *camera_transform) {
  LVector3f forward = LVector3f::forward();

  LPoint3f onset_point, opaque_point;
  if (get_num_parents() != 0) {
    // Linear fog is relative to the fog's net transform in the scene
    // graph.
    NodePath this_np(this);

    CPT(TransformState) rel_transform = 
      camera_transform->invert_compose(this_np.get_net_transform());
    
    const LMatrix4f &mat = rel_transform->get_mat();

    // How far out of whack are we?
    LVector3f fog_vector = (_linear_opaque_point - _linear_onset_point) * mat;
    fog_vector.normalize();
    float cosa = fog_vector.dot(forward);
    if (cabs(cosa) < _linear_fallback_cosa) {
      // The fog vector is too far from the eye vector; use the
      // fallback mode.
      _transformed_onset = _linear_fallback_onset;
      _transformed_opaque = _linear_fallback_opaque;

    } else {
      _transformed_onset = forward.dot(_linear_onset_point * mat);
      _transformed_opaque = forward.dot(_linear_opaque_point * mat);
    }

  } else {
    // Not a camera-relative fog.
    _transformed_onset = forward.dot(_linear_onset_point);
    _transformed_opaque = forward.dot(_linear_opaque_point);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: Fog::get_linear_range
//       Access: Public
//  Description: Retrieves the current onset and offset ranges.
////////////////////////////////////////////////////////////////////
void Fog::
get_linear_range(float &onset, float &opaque) {
  onset = _transformed_onset;
  opaque = _transformed_opaque;
}

////////////////////////////////////////////////////////////////////
//     Function: Fog::register_with_read_factory
//       Access: Public, Static
//  Description: Tells the BamReader how to create objects of type
//               Fog.
////////////////////////////////////////////////////////////////////
void Fog::
register_with_read_factory() {
  BamReader::get_factory()->register_factory(get_class_type(), make_from_bam);
}

////////////////////////////////////////////////////////////////////
//     Function: Fog::write_datagram
//       Access: Public, Virtual
//  Description: Writes the contents of this object to the datagram
//               for shipping out to a Bam file.
////////////////////////////////////////////////////////////////////
void Fog::
write_datagram(BamWriter *manager, Datagram &dg) {
  PandaNode::write_datagram(manager, dg);

  dg.add_int8(_mode);
  _color.write_datagram(dg);
  _linear_onset_point.write_datagram(dg);
  _linear_opaque_point.write_datagram(dg);
  dg.add_float32(_exp_density);
  dg.add_float32(_linear_fallback_cosa);
  dg.add_float32(_linear_fallback_onset);
  dg.add_float32(_linear_fallback_opaque);
}

////////////////////////////////////////////////////////////////////
//     Function: Fog::make_from_bam
//       Access: Protected, Static
//  Description: This function is called by the BamReader's factory
//               when a new object of type Fog is encountered
//               in the Bam file.  It should create the Fog
//               and extract its information from the file.
////////////////////////////////////////////////////////////////////
TypedWritable *Fog::
make_from_bam(const FactoryParams &params) {
  Fog *node = new Fog("");
  DatagramIterator scan;
  BamReader *manager;

  parse_params(params, scan, manager);
  node->fillin(scan, manager);

  return node;
}

////////////////////////////////////////////////////////////////////
//     Function: Fog::fillin
//       Access: Protected
//  Description: This internal function is called by make_from_bam to
//               read in all of the relevant data from the BamFile for
//               the new Fog.
////////////////////////////////////////////////////////////////////
void Fog::
fillin(DatagramIterator &scan, BamReader *manager) {
  PandaNode::fillin(scan, manager);

  _mode = (Mode)scan.get_int8();
  _color.read_datagram(scan);
  _linear_onset_point.read_datagram(scan);
  _linear_opaque_point.read_datagram(scan);
  _exp_density = scan.get_float32();
  _linear_fallback_cosa = scan.get_float32();
  _linear_fallback_onset = scan.get_float32();
  _linear_fallback_opaque = scan.get_float32();
}
