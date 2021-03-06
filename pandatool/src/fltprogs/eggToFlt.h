// Filename: eggToFlt.h
// Created by:  drose (01Oct03)
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

#ifndef EGGTOFLT_H
#define EGGTOFLT_H

#include "pandatoolbase.h"

#include "eggToSomething.h"
#include "fltHeader.h"
#include "fltGeometry.h"
#include "pointerTo.h"
#include "pmap.h"
#include "vector_string.h"

class EggGroup;
class EggVertex;
class EggPrimitive;
class EggTexture;
class EggTransform3d;
class FltVertex;
class FltBead;
class FltTexture;

////////////////////////////////////////////////////////////////////
//       Class : EggToFlt
// Description : A program to read an egg file and write a flt file.
////////////////////////////////////////////////////////////////////
class EggToFlt : public EggToSomething {
public:
  EggToFlt();

  void run();

private:
  static bool dispatch_attr(const string &opt, const string &arg, void *var);

  void traverse(EggNode *egg_node, FltBead *flt_node,
                FltGeometry::BillboardType billboard);
  void convert_primitive(EggPrimitive *egg_primitive, FltBead *flt_node, 
                         FltGeometry::BillboardType billboard);
  void convert_group(EggGroup *egg_group, FltBead *flt_node,
                     FltGeometry::BillboardType billboard);
  void apply_transform(EggTransform3d *egg_transform, FltBead *flt_node);
  void apply_egg_syntax(const string &egg_syntax, FltRecord *flt_record);
  FltVertex *get_flt_vertex(EggVertex *egg_vertex, EggNode *context);
  FltTexture *get_flt_texture(EggTexture *egg_texture);

  FltHeader::AttrUpdate _auto_attr_update;

  PT(FltHeader) _flt_header;
  
  typedef pmap<EggVertex *, FltVertex *> VertexMap;
  typedef pmap<const LMatrix4d *, VertexMap> VertexMapPerFrame;
  VertexMapPerFrame _vertex_map_per_frame;
  
  typedef pmap<Filename, FltTexture *> TextureMap;
  TextureMap _texture_map;
};

#endif

