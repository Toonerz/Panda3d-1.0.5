// Filename: eggMultiBase.cxx
// Created by:  drose (02Nov00)
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

#include "eggMultiBase.h"
#include "eggBase.h"
#include "eggData.h"
#include "eggComment.h"
#include "filename.h"
#include "dSearchPath.h"

////////////////////////////////////////////////////////////////////
//     Function: EggMultiBase::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
EggMultiBase::
EggMultiBase() {
  add_option
    ("f", "", 80,
     "Force complete loading: load up the egg file along with all of its "
     "external references.",
     &EggMultiBase::dispatch_none, &_force_complete);

  add_option
    ("noabs", "", 0,
     "Don't allow any of the named egg files to have absolute pathnames.  "
     "If any do, abort with an error.  This option is designed to help "
     "detect errors when populating or building a standalone model tree, "
     "which should be self-contained and include only relative pathnames.",
     &EggMultiBase::dispatch_none, &_noabs);
}

////////////////////////////////////////////////////////////////////
//     Function: EggMultiBase::post_process_egg_files
//       Access: Public
//  Description: Performs any processing of the egg file(s) that is
//               appropriate before writing them out.  This includes any
//               normal adjustments the user requested via -np, etc.
//
//               Normally, you should not need to call this function
//               directly; write_egg_files() calls it for you.  You
//               should call this only if you do not use
//               write_egg_files() to write out the resulting egg
//               files.
////////////////////////////////////////////////////////////////////
void EggMultiBase::
post_process_egg_files() {
  if (_eggs.empty()) {
    return;
  }

  Eggs::iterator ei;
  if (_got_transform) {
    nout << "Applying transform matrix:\n";
    _transform.write(nout, 2);
    LVecBase3d scale, hpr, translate;
    if (decompose_matrix(_transform, scale, hpr, translate,
                         _eggs[0]->get_coordinate_system())) {
      nout << "(scale " << scale << ", hpr " << hpr << ", translate "
           << translate << ")\n";
    }
    for (ei = _eggs.begin(); ei != _eggs.end(); ++ei) {
      (*ei)->transform(_transform);
    }
  }

  switch (_normals_mode) {
  case NM_strip:
    nout << "Stripping normals.\n";
    for (ei = _eggs.begin(); ei != _eggs.end(); ++ei) {
      (*ei)->strip_normals();
      (*ei)->remove_unused_vertices();
    }
    break;

  case NM_polygon:
    nout << "Recomputing polygon normals.\n";
    for (ei = _eggs.begin(); ei != _eggs.end(); ++ei) {
      (*ei)->recompute_polygon_normals();
      (*ei)->remove_unused_vertices();
    }
    break;

  case NM_vertex:
    nout << "Recomputing vertex normals.\n";
    for (ei = _eggs.begin(); ei != _eggs.end(); ++ei) {
      (*ei)->recompute_vertex_normals(_normals_threshold);
      (*ei)->remove_unused_vertices();
    }
    break;

  case NM_preserve:
    // Do nothing.
    break;
  }
}


////////////////////////////////////////////////////////////////////
//     Function: EggMultiBase::read_egg
//       Access: Protected, Virtual
//  Description: Allocates and returns a new EggData structure that
//               represents the indicated egg file.  If the egg file
//               cannot be read for some reason, returns NULL.
//
//               This can be overridden by derived classes to control
//               how the egg files are read, or to extend the
//               information stored with each egg structure, by
//               deriving from EggData.
////////////////////////////////////////////////////////////////////
PT(EggData) EggMultiBase::
read_egg(const Filename &filename) {
  PT(EggData) data = new EggData;

  if (!data->read(filename)) {
    // Failure reading.
    return (EggData *)NULL;
  }

  if (_noabs && data->original_had_absolute_pathnames()) {
    nout << filename.get_basename()
         << " includes absolute pathnames!\n";
    return (EggData *)NULL;
  }

  DSearchPath file_path;
  file_path.append_directory(filename.get_dirname());

  // We always resolve filenames first based on the source egg
  // filename, since egg files almost always store relative paths.
  // This is a temporary kludge around integrating the path_replace
  // system with the EggData better.
  data->resolve_filenames(file_path);

  if (_force_complete) {
    if (!data->load_externals()) {
      return (EggData *)NULL;
    }
  }

  // Now resolve the filenames again according to the user's
  // specified _path_replace.
  EggBase::convert_paths(data, _path_replace, file_path);

  if (_got_coordinate_system) {
    data->set_coordinate_system(_coordinate_system);
  } else {
    _coordinate_system = data->get_coordinate_system();
    _got_coordinate_system = true;
  }

  return data;
}
