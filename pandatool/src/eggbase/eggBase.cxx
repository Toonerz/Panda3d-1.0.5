// Filename: eggBase.cxx
// Created by:  drose (14Feb00)
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

#include "eggBase.h"

#include "eggGroupNode.h"
#include "eggTexture.h"
#include "eggFilenameNode.h"
#include "eggComment.h"
#include "dcast.h"
#include "string_utils.h"

////////////////////////////////////////////////////////////////////
//     Function: EggBase::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
EggBase::
EggBase() {
  add_option
    ("cs", "coordinate-system", 80,
     "Specify the coordinate system to operate in.  This may be one of "
     "'y-up', 'z-up', 'y-up-left', or 'z-up-left'.",
     &EggBase::dispatch_coordinate_system,
     &_got_coordinate_system, &_coordinate_system);

  _normals_mode = NM_preserve;
  _normals_threshold = 0.0;

  _got_transform = false;
  _transform = LMatrix4d::ident_mat();

  _got_coordinate_system = false;
  _coordinate_system = CS_yup_right;

  _noabs = false;
}

////////////////////////////////////////////////////////////////////
//     Function: EggBase::add_normals_options
//       Access: Public
//  Description: Adds -no, -np, etc. as valid options for this
//               program.  If the user specifies one of the options on
//               the command line, the normals will be adjusted when
//               the egg file is written out.
////////////////////////////////////////////////////////////////////
void EggBase::
add_normals_options() {
  static NormalsMode strip = NM_strip;
  static NormalsMode polygon = NM_polygon;
  static NormalsMode vertex = NM_vertex;
  static NormalsMode preserve = NM_preserve;

  add_option
    ("no", "", 48,
     "Strip all normals.",
     &EggBase::dispatch_normals, NULL, &strip);

  add_option
    ("np", "", 48,
     "Strip existing normals and redefine polygon normals.",
     &EggBase::dispatch_normals, NULL, &polygon);

  add_option
    ("nv", "threshold", 48,
     "Strip existing normals and redefine vertex normals.  Consider an edge "
     "between adjacent polygons to be smooth if the angle between them "
     "is less than threshold degrees.",
     &EggBase::dispatch_normals, NULL, &vertex);

  add_option
    ("nn", "", 48,
     "Preserve normals exactly as they are.  This is the default.",
     &EggBase::dispatch_normals, NULL, &preserve);
}

////////////////////////////////////////////////////////////////////
//     Function: EggBase::add_transform_options
//       Access: Public
//  Description: Adds -TS, -TT, etc. as valid options for this
//               program.  If the user specifies one of the options on
//               the command line, the data will be transformed when
//               the egg file is written out.
////////////////////////////////////////////////////////////////////
void EggBase::
add_transform_options() {
  add_option
    ("TS", "sx[,sy,sz]", 49,
     "Scale the model uniformly by the given factor (if only one number "
     "is given) or in each axis by sx, sy, sz (if three numbers are given).",
     &EggBase::dispatch_scale, &_got_transform, &_transform);

  add_option
    ("TR", "x,y,z", 49,
     "Rotate the model x degrees about the x axis, then y degrees about the "
     "y axis, and then z degrees about the z axis.",
     &EggBase::dispatch_rotate_xyz, &_got_transform, &_transform);

  add_option
    ("TA", "angle,x,y,z", 49,
     "Rotate the model angle degrees counterclockwise about the given "
     "axis.",
     &EggBase::dispatch_rotate_axis, &_got_transform, &_transform);

  add_option
    ("TT", "x,y,z", 49,
     "Translate the model by the indicated amount.\n\n"
     "All transformation options (-TS, -TR, -TA, -TT) are cumulative and are "
     "applied in the order they are encountered on the command line.",
     &EggBase::dispatch_translate, &_got_transform, &_transform);
}

////////////////////////////////////////////////////////////////////
//     Function: EggBase::convert_paths
//       Access: Public, Static
//  Description: Recursively walks the egg hierarchy.  Any filenames
//               encountered are replaced according to the indicated
//               PathReplace.
////////////////////////////////////////////////////////////////////
void EggBase::
convert_paths(EggNode *node, PathReplace *path_replace,
              const DSearchPath &additional_path) {
  if (node->is_of_type(EggTexture::get_class_type())) {
    EggTexture *egg_tex = DCAST(EggTexture, node);
    Filename fullpath = 
      path_replace->match_path(egg_tex->get_filename(), additional_path);
    egg_tex->set_filename(path_replace->store_path(fullpath));
    egg_tex->set_fullpath(fullpath);

    if (egg_tex->has_alpha_filename()) {
      Filename alpha_fullpath = 
        path_replace->match_path(egg_tex->get_alpha_filename(), additional_path);
      egg_tex->set_alpha_filename(path_replace->store_path(alpha_fullpath));
      egg_tex->set_alpha_fullpath(alpha_fullpath);
    }

  } else if (node->is_of_type(EggFilenameNode::get_class_type())) {
    EggFilenameNode *egg_fnode = DCAST(EggFilenameNode, node);

    Filename fullpath = 
      path_replace->match_path(egg_fnode->get_filename(), additional_path);
    egg_fnode->set_filename(path_replace->store_path(fullpath));
    egg_fnode->set_fullpath(fullpath);

  } else if (node->is_of_type(EggGroupNode::get_class_type())) {
    EggGroupNode *egg_group = DCAST(EggGroupNode, node);
    EggGroupNode::const_iterator ci;
    for (ci = egg_group->begin(); ci != egg_group->end(); ++ci) {
      convert_paths(*ci, path_replace, additional_path);
    }
  }
}

////////////////////////////////////////////////////////////////////
//     Function: EggBase::append_command_comment
//       Access: Protected
//  Description: Inserts a comment into the beginning of the indicated
//               egg file corresponding to the command line that
//               invoked this program.
//
//               Normally this function is called automatically when
//               appropriate by EggWriter, and it's not necessary to
//               call it explicitly.
////////////////////////////////////////////////////////////////////
void EggBase::
append_command_comment(EggData &data) {
  append_command_comment(data, get_exec_command());
}

////////////////////////////////////////////////////////////////////
//     Function: EggBase::append_command_comment
//       Access: Protected, Static
//  Description: Inserts a comment into the beginning of the indicated
//               egg file corresponding to the command line that
//               invoked this program.
//
//               Normally this function is called automatically when
//               appropriate by EggWriter, and it's not necessary to
//               call it explicitly.
////////////////////////////////////////////////////////////////////
void EggBase::
append_command_comment(EggData &data, const string &comment) {
  data.insert(data.begin(), new EggComment("", comment));
}

////////////////////////////////////////////////////////////////////
//     Function: EggBase::dispatch_normals
//       Access: Protected, Static
//  Description: Accepts one of -no, -np, etc. and sets _normals_mode
//               as indicated.  The void * argument is a pointer to a
//               NormalsMode variable that indicates which switch was
//               passed.
////////////////////////////////////////////////////////////////////
bool EggBase::
dispatch_normals(ProgramBase *self, const string &opt, const string &arg, void *mode) {
  EggBase *base = (EggBase *)self;
  return base->ns_dispatch_normals(opt, arg, mode);
}

////////////////////////////////////////////////////////////////////
//     Function: EggBase::ns_dispatch_normals
//       Access: Protected
//  Description: Accepts one of -no, -np, etc. and sets _normals_mode
//               as indicated.  The void * argument is a pointer to a
//               NormalsMode variable that indicates which switch was
//               passed.
////////////////////////////////////////////////////////////////////
bool EggBase::
ns_dispatch_normals(const string &opt, const string &arg, void *mode) {
  _normals_mode = *(NormalsMode *)mode;

  if (_normals_mode == NM_vertex) {
    if (!string_to_double(arg, _normals_threshold)) {
      nout << "Invalid numeric parameter for -" << opt << ": "
           << arg << "\n";
      return false;
    }
  }

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: EggBase::dispatch_scale
//       Access: Protected, Static
//  Description: Handles -TS, which specifies a scale transform.  Var
//               is an LMatrix4d.
////////////////////////////////////////////////////////////////////
bool EggBase::
dispatch_scale(const string &opt, const string &arg, void *var) {
  LMatrix4d *transform = (LMatrix4d *)var;

  vector_string words;
  tokenize(arg, words, ",");

  double sx, sy, sz;

  bool okflag = false;
  if (words.size() == 3) {
    okflag =
      string_to_double(words[0], sx) &&
      string_to_double(words[1], sy) &&
      string_to_double(words[2], sz);

  } else if (words.size() == 1) {
    okflag =
      string_to_double(words[0], sx);
    sy = sz = sx;
  }

  if (!okflag) {
    nout << "-" << opt
         << " requires one or three numbers separated by commas.\n";
    return false;
  }

  *transform = (*transform) * LMatrix4d::scale_mat(sx, sy, sz);

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: EggBase::dispatch_rotate_xyz
//       Access: Protected, Static
//  Description: Handles -TR, which specifies a rotate transform about
//               the three cardinal axes.  Var is an LMatrix4d.
////////////////////////////////////////////////////////////////////
bool EggBase::
dispatch_rotate_xyz(ProgramBase *self, const string &opt, const string &arg, void *var) {
  EggBase *base = (EggBase *)self;
  return base->ns_dispatch_rotate_xyz(opt, arg, var);
}

////////////////////////////////////////////////////////////////////
//     Function: EggBase::ns_dispatch_rotate_xyz
//       Access: Protected
//  Description: Handles -TR, which specifies a rotate transform about
//               the three cardinal axes.  Var is an LMatrix4d.
////////////////////////////////////////////////////////////////////
bool EggBase::
ns_dispatch_rotate_xyz(const string &opt, const string &arg, void *var) {
  LMatrix4d *transform = (LMatrix4d *)var;

  vector_string words;
  tokenize(arg, words, ",");

  LVecBase3d xyz;

  bool okflag = false;
  if (words.size() == 3) {
    okflag =
      string_to_double(words[0], xyz[0]) &&
      string_to_double(words[1], xyz[1]) &&
      string_to_double(words[2], xyz[2]);
  }

  if (!okflag) {
    nout << "-" << opt
         << " requires three numbers separated by commas.\n";
    return false;
  }

  LMatrix4d mat =
    LMatrix4d::rotate_mat(xyz[0], LVector3d(1.0, 0.0, 0.0), _coordinate_system) *
    LMatrix4d::rotate_mat(xyz[1], LVector3d(0.0, 1.0, 0.0), _coordinate_system) *
    LMatrix4d::rotate_mat(xyz[2], LVector3d(0.0, 0.0, 1.0), _coordinate_system);

  *transform = (*transform) * mat;

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: EggBase::dispatch_rotate_axis
//       Access: Protected, Static
//  Description: Handles -TA, which specifies a rotate transform about
//               an arbitrary axis.  Var is an LMatrix4d.
////////////////////////////////////////////////////////////////////
bool EggBase::
dispatch_rotate_axis(ProgramBase *self, const string &opt, const string &arg, void *var) {
  EggBase *base = (EggBase *)self;
  return base->ns_dispatch_rotate_axis(opt, arg, var);
}

////////////////////////////////////////////////////////////////////
//     Function: EggBase::ns_dispatch_rotate_axis
//       Access: Protected
//  Description: Handles -TA, which specifies a rotate transform about
//               an arbitrary axis.  Var is an LMatrix4d.
////////////////////////////////////////////////////////////////////
bool EggBase::
ns_dispatch_rotate_axis(const string &opt, const string &arg, void *var) {
  LMatrix4d *transform = (LMatrix4d *)var;

  vector_string words;
  tokenize(arg, words, ",");

  double angle;
  LVecBase3d axis;

  bool okflag = false;
  if (words.size() == 4) {
    okflag =
      string_to_double(words[0], angle) &&
      string_to_double(words[1], axis[0]) &&
      string_to_double(words[2], axis[1]) &&
      string_to_double(words[3], axis[2]);
  }

  if (!okflag) {
    nout << "-" << opt
         << " requires four numbers separated by commas.\n";
    return false;
  }

  *transform = (*transform) * LMatrix4d::rotate_mat(angle, axis, _coordinate_system);

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: EggBase::dispatch_translate
//       Access: Protected, Static
//  Description: Handles -TT, which specifies a translate transform.
//               Var is an LMatrix4d.
////////////////////////////////////////////////////////////////////
bool EggBase::
dispatch_translate(const string &opt, const string &arg, void *var) {
  LMatrix4d *transform = (LMatrix4d *)var;

  vector_string words;
  tokenize(arg, words, ",");

  LVector3d trans;

  bool okflag = false;
  if (words.size() == 3) {
    okflag =
      string_to_double(words[0], trans[0]) &&
      string_to_double(words[1], trans[1]) &&
      string_to_double(words[2], trans[2]);
  }

  if (!okflag) {
    nout << "-" << opt
         << " requires three numbers separated by commas.\n";
    return false;
  }

  *transform = (*transform) * LMatrix4d::translate_mat(trans);

  return true;
}
