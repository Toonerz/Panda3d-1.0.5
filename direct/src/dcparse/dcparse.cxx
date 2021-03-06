// Filename: dcparse.cxx
// Created by:  drose (05Oct00)
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

#include "dcbase.h"
#include "dcFile.h"
#include "dcClass.h"
#include "dcTypedef.h"
#include "memoryUsage.h"

#ifndef HAVE_GETOPT
#include "gnu_getopt.h"
#else
#include <getopt.h>
#endif

void
usage() {
  cerr << 
    "\n"
    "Usage:\n\n"
    "dcparse [-v | -b]  [file1 file2 ...]\n"
    "dcparse -h\n\n";
}

void
help() {
  usage();
  cerr << 
    "This program reads one or more DC files, which are used to describe the\n"
    "communication channels in the distributed class system.  By default,\n"
    "the file(s) are read and concatenated, and a single hash code is printed\n"
    "corresponding to the file's contents.\n\n"

    "With -b, this writes a brief version of the file to standard output\n"
    "instead.  With -v, this writes a more verbose version.\n\n";
}

int
main(int argc, char *argv[]) {
  //  extern char *optarg;
  extern int optind;
  const char *optstr = "bvh";

  bool dump_verbose = false;
  bool dump_brief = false;

  int flag = getopt(argc, argv, optstr);

  while (flag != EOF) {
    switch (flag) {
    case 'b':
      dump_brief = true;
      break;

    case 'v':
      dump_verbose = true;
      break;

    case 'h':
      help();
      exit(1);

    default:
      exit(1);
    }
    flag = getopt(argc, argv, optstr);
  }

  argc -= (optind-1);
  argv += (optind-1);

  if (argc < 2) {
    usage();
    exit(1);
  }

  DCFile file;
  for (int i = 1; i < argc; i++) {
    if (!file.read(argv[i])) {
      return (1);
    }
  }

  if (!file.all_objects_valid() && !dump_brief) {
    cerr << "File is incomplete.  The following objects are undefined:\n";

    int num_typedefs = file.get_num_typedefs();
    int i;
    for (i = 0; i < num_typedefs; i++) {
      DCTypedef *dtypedef = file.get_typedef(i);
      if (dtypedef->is_bogus_typedef()) {
        cerr << "  " << dtypedef->get_name() << "\n";
      }
    }

    int num_classes = file.get_num_classes();
    for (i = 0; i < num_classes; i++) {
      DCClass *dclass = file.get_class(i);
      if (dclass->is_bogus_class()) {
        cerr << "  " << dclass->get_name() << "\n";
      }
    }

    return (1);
  }

  if (dump_verbose || dump_brief) {
    if (!file.write(cout, dump_brief)) {
      return (1);
    }

  } else {
    long hash = file.get_hash();
    cerr << "File hash is " << hash << "\n";
  }

#ifdef DO_MEMORY_USAGE
  if (MemoryUsage::is_tracking()) {
    file.clear();
    MemoryUsage::show_current_types();
    for (int i = 1; i < argc; i++) {
      file.read(argv[i]);
    }
    file.clear();
    MemoryUsage::show_current_types();
  }
#endif

  return (0);
}
