// Filename: collisionLevelState.h
// Created by:  drose (16Mar02)
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

#ifndef COLLISIONLEVELSTATE_H
#define COLLISIONLEVELSTATE_H

#include "pandabase.h"

#include "luse.h"
#include "pointerToArray.h"
#include "geometricBoundingVolume.h"
#include "nodePath.h"
#include "workingNodePath.h"
#include "pointerTo.h"
#include "plist.h"

class CollisionSolid;
class CollisionNode;

////////////////////////////////////////////////////////////////////
//       Class : CollisionLevelState
// Description : This is the state information the
//               CollisionTraverser retains for each level during
//               traversal.
////////////////////////////////////////////////////////////////////
class CollisionLevelState {
public:
  class ColliderDef {
  public:
    CollisionSolid *_collider;
    CollisionNode *_node;
    NodePath _node_path;
  };

  INLINE CollisionLevelState(const NodePath &node_path);
  INLINE CollisionLevelState(const CollisionLevelState &parent, 
                               PandaNode *child);

  void clear();
  void reserve(int max_colliders);
  void prepare_collider(const ColliderDef &def);

  bool any_in_bounds();
  void apply_transform();
  
  INLINE NodePath get_node_path() const;
  INLINE PandaNode *node() const;

  INLINE int get_num_colliders() const;
  INLINE bool has_collider(int n) const;
  INLINE bool has_any_collider() const;

  INLINE CollisionSolid *get_collider(int n) const;
  INLINE CollisionNode *get_collider_node(int n) const;
  INLINE NodePath get_collider_node_path(int n) const;
  INLINE const GeometricBoundingVolume *get_local_bound(int n) const;
  INLINE const GeometricBoundingVolume *get_parent_bound(int n) const;

  INLINE void omit_collider(int n);

private:
  // ColliderMask here is a locally-defined value that simply serves
  // to keep track of the colliders that are still interested in the
  // current node.  Don't confuse it with CollideMask, which is a set
  // of user-defined bits that specify which CollisionSolids may
  // possibly intersect with each other.
  typedef int ColliderMask;

  INLINE ColliderMask get_mask(int n) const;

  WorkingNodePath _node_path;

  typedef PTA(ColliderDef) Colliders;
  Colliders _colliders;
  ColliderMask _current;

  typedef PTA(CPT(GeometricBoundingVolume)) BoundingVolumes;
  BoundingVolumes _local_bounds;
  BoundingVolumes _parent_bounds;
};

#include "collisionLevelState.I"

#endif


