// Filename: frameRateMeter.h
// Created by:  drose (23Dec03)
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

#ifndef FRAMERATEMETER_H
#define FRAMERATEMETER_H

#include "pandabase.h"
#include "textNode.h"
#include "nodePath.h"
#include "graphicsOutput.h"
#include "displayRegion.h"
#include "pointerTo.h"
#include "pStatCollector.h"

class GraphicsChannel;
class ClockObject;

////////////////////////////////////////////////////////////////////
//       Class : FrameRateMeter
// Description : This is a special TextNode that automatically updates
//               itself with the current frame rate.  It can be placed
//               anywhere in the world where you'd like to see the
//               frame rate.
//
//               It also has a special mode in which it may be
//               attached directly to a channel or window.  If this is
//               done, it creates a DisplayRegion for itself and renders
//               itself in the upper-right-hand corner.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDA FrameRateMeter : public TextNode {
PUBLISHED:
  FrameRateMeter(const string &name);
  virtual ~FrameRateMeter();

  void setup_window(GraphicsOutput *window);
  void clear_window();

  INLINE GraphicsOutput *get_window() const;
  INLINE DisplayRegion *get_display_region() const;

  INLINE void set_update_interval(double update_interval);
  INLINE double get_update_interval() const;

  INLINE void set_text_pattern(const string &text_pattern);
  INLINE const string &get_text_pattern() const;

  INLINE void set_clock_object(ClockObject *clock_object);
  INLINE ClockObject *get_clock_object() const;

protected:
  virtual bool cull_callback(CullTraverser *trav, CullTraverserData &data);

private:
  void do_update();

private:
  PT(GraphicsOutput) _window;
  PT(DisplayRegion) _display_region;
  NodePath _root;

  double _update_interval;
  double _last_update;
  string _text_pattern;
  ClockObject *_clock_object;

  static PStatCollector _show_fps_pcollector;

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    TextNode::init_type();
    register_type(_type_handle, "FrameRateMeter",
                  TextNode::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;
};

#include "frameRateMeter.I"

#endif
