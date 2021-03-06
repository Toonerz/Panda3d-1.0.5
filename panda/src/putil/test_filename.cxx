// Filename: test_filename.cxx
// Created by:  drose (18Jan99)
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

#include "filename.h"
#include "config_util.h"

#include "dSearchPath.h"

#include <stdlib.h>

void
 show_filename(const Filename &f) {
  nout
    << "get_fullpath = " << f.get_fullpath() << "\n"
    /*
    << "  _dirname_end = " << f._dirname_end
    << " _basename_start = " << f._basename_start << "\n"
    << "  _basename_end = " << f._basename_end
    << " _extension_start = " << f._extension_start << "\n"
    */

    << "get_dirname = " << f.get_dirname() << "\n"
    << "get_basename = " << f.get_basename() << "\n"
    << "get_fullpath_wo_extension = " << f.get_fullpath_wo_extension() << "\n"
    << "get_basename_wo_extension = " << f.get_basename_wo_extension() << "\n"
    << "get_extension = " << f.get_extension() << "\n"
    << "\n";
}


int
main(int argc, char *argv[]) {
  if (argc < 2) {
    nout << "Specify filename on command line.\n";
    exit(1);
  }

  Filename f(argv[1]);

  show_filename(f);

  if (argc >= 3) {
    Filename t(argv[2]);
    nout << "f is " << f << " exists = " << f.exists() << "\n"
         << "t is " << t << " exists = " << t.exists() << "\n";

    nout << "f.compare_timestamps(t, true, true) = "
         << f.compare_timestamps(t, true, true) << "\n"
         << "f.compare_timestamps(t, true, false) = "
         << f.compare_timestamps(t, true, false) << "\n"
         << "f.compare_timestamps(t, false, true) = "
         << f.compare_timestamps(t, false, true) << "\n"
         << "f.compare_timestamps(t, false, false) = "
         << f.compare_timestamps(t, false, false) << "\n";
  }
  /*
  if (argc >= 3) {
    nout << "\nRelative to: " << argv[2] << "\n";
    f.make_relative_to(argv[2]);
    show_filename(f);

  } else {
    nout << "Searching on model path: " << get_model_path() << "\n";
    if (f.resolve_filename(get_model_path())) {
      nout << "Found: " << f << "\n";
    } else {
      nout << "not found.\n";
    }

    int index = f.find_on_searchpath(get_model_path());
    nout << "Found at position " << index << ": " << f << "\n";
  }
  */

  /*
  f.set_extension("new_extension");
  show_filename(f);

  f.set_extension("");
  show_filename(f);

  f.set_extension("ext");
  show_filename(f);

  f.set_dirname("/new/dirname");
  show_filename(f);

  f.set_dirname("/");
  show_filename(f);

  f.set_dirname("");
  show_filename(f);

  f.set_dirname("/dir/name/");
  show_filename(f);

  f.set_basename_wo_extension("base_name");
  show_filename(f);
  */

  return(0);
}


