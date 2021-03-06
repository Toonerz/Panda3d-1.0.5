// Filename: somethingToEgg.h
// Created by:  drose (15Feb00)
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

#ifndef SOMETHINGTOEGG_H
#define SOMETHINGTOEGG_H

#include "pandatoolbase.h"

#include "eggConverter.h"
#include "distanceUnit.h"
#include "animationConvert.h"

class SomethingToEggConverter;

////////////////////////////////////////////////////////////////////
//       Class : SomethingToEgg
// Description : This is the general base class for a file-converter
//               program that reads some model file format and
//               generates an egg file.
////////////////////////////////////////////////////////////////////
class SomethingToEgg : public EggConverter {
public:
  SomethingToEgg(const string &format_name,
                 const string &preferred_extension = string(),
                 bool allow_last_param = true,
                 bool allow_stdout = true);

  void add_units_options();
  void add_animation_options();
  void add_merge_externals_options();

protected:
  void apply_units_scale(EggData &data);
  void apply_parameters(SomethingToEggConverter &converter);

  virtual bool handle_args(Args &args);
  virtual bool post_command_line();
  virtual void post_process_egg_file();

  static bool dispatch_animation_convert(const string &opt, const string &arg, void *var);


  Filename _input_filename;

  DistanceUnit _input_units;
  DistanceUnit _output_units;

  AnimationConvert _animation_convert;
  string _character_name;
  double _start_frame;
  double _end_frame;
  double _frame_inc;
  double _neutral_frame;
  double _input_frame_rate;
  double _output_frame_rate;
  bool _got_start_frame;
  bool _got_end_frame;
  bool _got_frame_inc;
  bool _got_neutral_frame;
  bool _got_input_frame_rate;
  bool _got_output_frame_rate;

  bool _merge_externals;
  bool _allow_errors;
};

#endif


