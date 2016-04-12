// Filename: animChannel.h
// Created by:  drose (22Feb99)
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

#ifndef ANIMCHANNEL_H
#define ANIMCHANNEL_H

#include "pandabase.h"

#include "animChannelBase.h"

#include "lmatrix.h"

////////////////////////////////////////////////////////////////////
//       Class : AnimChannel
// Description : This template class is the parent class for all kinds
//               of AnimChannels that return different values.
////////////////////////////////////////////////////////////////////
template<class SwitchType>
class AnimChannel : public AnimChannelBase {
protected:
  // The default constructor is protected: don't try to create an
  // AnimChannel without a parent.  To create an AnimChannel hierarchy,
  // you must first create an AnimBundle, and use that to create any
  // subsequent children.
  INLINE AnimChannel(const string &name = "");

public:
  typedef TYPENAME SwitchType::ValueType ValueType;

  INLINE AnimChannel(AnimGroup *parent, const string &name);

PUBLISHED:
  virtual void get_value(int frame, ValueType &value)=0;

  // These two functions only have meaning for matrix types.
  virtual void get_value_no_scale(int frame, ValueType &value);
  virtual void get_scale(int frame, float scale[3]);
  // The second parameter above should really by LVector3f instead of
  // float[3], but there seems to be a compiler bug in EGCS that
  // doesn't like that.  So we have this kludge for now.

  virtual TypeHandle get_value_type() const;

  //This class has no Read/Write functions as it is abstract
  //and defines no new data

public:
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    AnimChannelBase::init_type();
    register_type(_type_handle, SwitchType::get_channel_type_name(),
                  AnimChannelBase::get_class_type());
  }

private:
  static TypeHandle _type_handle;
};


// And now we instantiate a few useful types.

class EXPCL_PANDA ACMatrixSwitchType {
public:
  typedef LMatrix4f ValueType;
  static const char *get_channel_type_name() { return "AnimChannelMatrix"; }
  static const char *get_fixed_channel_type_name() { return "AnimChannelMatrixFixed"; }
  static const char *get_part_type_name() { return "MovingPart<LMatrix4f>"; }
  static void output_value(ostream &out, const ValueType &value);

  static void write_datagram(Datagram &dest, ValueType& me)
  {
    me.write_datagram(dest);
  }
  static void read_datagram(DatagramIterator &source, ValueType& me)
  {
    me.read_datagram(source);
  }
};

EXPORT_TEMPLATE_CLASS(EXPCL_PANDA, EXPTP_PANDA, AnimChannel<ACMatrixSwitchType>);
typedef AnimChannel<ACMatrixSwitchType> AnimChannelMatrix;


class EXPCL_PANDA ACScalarSwitchType {
public:
  typedef float ValueType;
  static const char *get_channel_type_name() { return "AnimChannelScalar"; }
  static const char *get_fixed_channel_type_name() { return "AnimChannelScalarFixed"; }
  static const char *get_part_type_name() { return "MovingPart<float>"; }
  static void output_value(ostream &out, ValueType value) {
    out << value;
  }
  static void write_datagram(Datagram &dest, ValueType& me)
  {
    dest.add_float32(me);
  }
  static void read_datagram(DatagramIterator &source, ValueType& me)
  {
    me = source.get_float32();
  }
};

EXPORT_TEMPLATE_CLASS(EXPCL_PANDA, EXPTP_PANDA, AnimChannel<ACScalarSwitchType>);
typedef AnimChannel<ACScalarSwitchType> AnimChannelScalar;


#include "animChannel.I"


// Tell GCC that we'll take care of the instantiation explicitly here.
#ifdef __GNUC__
#pragma interface
#endif

#endif
