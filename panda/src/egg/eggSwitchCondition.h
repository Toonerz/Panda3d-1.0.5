// Filename: eggSwitchCondition.h
// Created by:  drose (08Feb99)
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

#ifndef EGGSWITCHCONDITION
#define EGGSWITCHCONDITION

#include "pandabase.h"

#include "eggObject.h"
#include "luse.h"

////////////////////////////////////////////////////////////////////
//       Class : EggSwitchCondition
// Description : This corresponds to a <SwitchCondition> entry within
//               a group.  It indicates the condition at which a
//               level-of-detail is switched in or out.  This is
//               actually an abstract base class for potentially any
//               number of specific different kinds of switching
//               conditions; presently, only a <Distance> type is
//               actually supported.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDAEGG EggSwitchCondition : public EggObject {
PUBLISHED:
  virtual EggSwitchCondition *make_copy() const=0;
  virtual void write(ostream &out, int indent_level) const=0;

  virtual void transform(const LMatrix4d &mat)=0;


public:

  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    EggObject::init_type();
    register_type(_type_handle, "EggSwitchCondition",
                  EggObject::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;
};


////////////////////////////////////////////////////////////////////
//       Class : EggSwitchConditionDistance
// Description : A SwitchCondition that switches the levels-of-detail
//               based on distance from the camera's eyepoint.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDAEGG EggSwitchConditionDistance : public EggSwitchCondition {
public:
  EggSwitchConditionDistance(double switch_in, double switch_out,
                             const LPoint3d &center, double fade = 0.0);

  virtual EggSwitchCondition *make_copy() const;
  virtual void write(ostream &out, int indent_level) const;

  virtual void transform(const LMatrix4d &mat);

  double _switch_in, _switch_out, _fade;
  LPoint3d _center;

public:

  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    EggSwitchCondition::init_type();
    register_type(_type_handle, "EggSwitchConditionDistance",
                  EggSwitchCondition::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }

private:
  static TypeHandle _type_handle;
};


#endif

