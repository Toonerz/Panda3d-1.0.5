// Filename: boxEmitter.h
// Created by:  charles (22Jun00)
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

#ifndef BOXEMITTER_H
#define BOXEMITTER_H

#include "baseParticleEmitter.h"

////////////////////////////////////////////////////////////////////
//       Class : BoxEmitter
// Description : Describes a voluminous box region in which
//               particles are generated.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDAPHYSICS BoxEmitter : public BaseParticleEmitter {
PUBLISHED:
  BoxEmitter();
  BoxEmitter(const BoxEmitter &copy);
  virtual ~BoxEmitter();

  virtual BaseParticleEmitter *make_copy();

  INLINE void set_min_bound(const LPoint3f& vmin);
  INLINE void set_max_bound(const LPoint3f& vmax);

  INLINE LPoint3f get_min_bound() const;
  INLINE LPoint3f get_max_bound() const;

  virtual void output(ostream &out) const;
  virtual void write(ostream &out, int indent=0) const;

private:
  LPoint3f _vmin;
  LPoint3f _vmax;

  // CUSTOM EMISSION PARAMETERS
  // none

  virtual void assign_initial_position(LPoint3f& pos);
  virtual void assign_initial_velocity(LVector3f& vel);
};

#include "boxEmitter.I"

#endif // BOXEMITTER_H
