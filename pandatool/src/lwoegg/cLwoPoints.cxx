// Filename: cLwoPoints.cxx
// Created by:  drose (25Apr01)
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

#include "cLwoPoints.h"
#include "lwoToEggConverter.h"
#include "cLwoLayer.h"

#include "lwoVertexMap.h"
#include "string_utils.h"

////////////////////////////////////////////////////////////////////
//     Function: CLwoPoints::add_vmap
//       Access: Public
//  Description: Associates the indicated VertexMap with the points
//               set.  This may define such niceties as UV coordinates
//               or per-vertex color.
////////////////////////////////////////////////////////////////////
void CLwoPoints::
add_vmap(const LwoVertexMap *lwo_vmap) {
  IffId map_type = lwo_vmap->_map_type;
  const string &name = lwo_vmap->_name;

  bool inserted;
  if (map_type == IffId("TXUV")) {
    inserted =
      _txuv.insert(VMap::value_type(name, lwo_vmap)).second;

  } else if (map_type == IffId("PICK")) {
    inserted =
      _pick.insert(VMap::value_type(name, lwo_vmap)).second;

  } else {
    return;
  }

  if (!inserted) {
    nout << "Multiple vertex maps on the same points of type "
         << map_type << " named " << name << "\n";
  }
}

////////////////////////////////////////////////////////////////////
//     Function: CLwoPoints::get_uv
//       Access: Public
//  Description: Returns true if there is a UV of the indicated name
//               associated with the given vertex, false otherwise.
//               If true, fills in uv with the value.
////////////////////////////////////////////////////////////////////
bool CLwoPoints::
get_uv(const string &uv_name, int n, LPoint2f &uv) const {
  VMap::const_iterator ni = _txuv.find(uv_name);
  if (ni == _txuv.end()) {
    return false;
  }

  const LwoVertexMap *vmap = (*ni).second;
  if (vmap->_dimension != 2) {
    nout << "Unexpected dimension of " << vmap->_dimension
         << " for UV map " << uv_name << "\n";
    return false;
  }

  if (!vmap->has_value(n)) {
    return false;
  }

  PTA_float value = vmap->get_value(n);

  uv.set(value[0], value[1]);
  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: CLwoPoints::make_egg
//       Access: Public
//  Description: Creates the egg structures associated with this
//               Lightwave object.
////////////////////////////////////////////////////////////////////
void CLwoPoints::
make_egg() {
  // Generate a vpool name based on the layer index, for lack of
  // anything better.
  string vpool_name = "layer" + format_string(_layer->get_number());
  _egg_vpool = new EggVertexPool(vpool_name);
}

////////////////////////////////////////////////////////////////////
//     Function: CLwoPoints::connect_egg
//       Access: Public
//  Description: Connects all the egg structures together.
////////////////////////////////////////////////////////////////////
void CLwoPoints::
connect_egg() {
  if (!_egg_vpool->empty()) {
    _layer->_egg_group->add_child(_egg_vpool.p());
  }
}

