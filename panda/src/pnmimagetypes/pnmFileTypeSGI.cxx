// Filename: pnmFileTypeSGI.cxx
// Created by:  drose (17Jun00)
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

#include "pnmFileTypeSGI.h"
#include "config_pnmimagetypes.h"
#include "sgi.h"

#include "pnmFileTypeRegistry.h"
#include "bamReader.h"

static const char * const extensions_sgi[] = {
  "rgb", "rgba", "sgi"
};
static const int num_extensions_sgi = sizeof(extensions_sgi) / sizeof(const char *);

TypeHandle PNMFileTypeSGI::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: PNMFileTypeSGI::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
PNMFileTypeSGI::
PNMFileTypeSGI() {
}

////////////////////////////////////////////////////////////////////
//     Function: PNMFileTypeSGI::get_name
//       Access: Public, Virtual
//  Description: Returns a few words describing the file type.
////////////////////////////////////////////////////////////////////
string PNMFileTypeSGI::
get_name() const {
  return "SGI RGB";
}

////////////////////////////////////////////////////////////////////
//     Function: PNMFileTypeSGI::get_num_extensions
//       Access: Public, Virtual
//  Description: Returns the number of different possible filename
//               extensions associated with this particular file type.
////////////////////////////////////////////////////////////////////
int PNMFileTypeSGI::
get_num_extensions() const {
  return num_extensions_sgi;
}

////////////////////////////////////////////////////////////////////
//     Function: PNMFileTypeSGI::get_extension
//       Access: Public, Virtual
//  Description: Returns the nth possible filename extension
//               associated with this particular file type, without a
//               leading dot.
////////////////////////////////////////////////////////////////////
string PNMFileTypeSGI::
get_extension(int n) const {
  nassertr(n >= 0 && n < num_extensions_sgi, string());
  return extensions_sgi[n];
}

////////////////////////////////////////////////////////////////////
//     Function: PNMFileTypeSGI::get_suggested_extension
//       Access: Public, Virtual
//  Description: Returns a suitable filename extension (without a
//               leading dot) to suggest for files of this type, or
//               empty string if no suggestions are available.
////////////////////////////////////////////////////////////////////
string PNMFileTypeSGI::
get_suggested_extension() const {
  return "rgb";
}

////////////////////////////////////////////////////////////////////
//     Function: PNMFileTypeSGI::has_magic_number
//       Access: Public, Virtual
//  Description: Returns true if this particular file type uses a
//               magic number to identify it, false otherwise.
////////////////////////////////////////////////////////////////////
bool PNMFileTypeSGI::
has_magic_number() const {
  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: PNMFileTypeSGI::matches_magic_number
//       Access: Public, Virtual
//  Description: Returns true if the indicated "magic number" byte
//               stream (the initial few bytes read from the file)
//               matches this particular file type, false otherwise.
////////////////////////////////////////////////////////////////////
bool PNMFileTypeSGI::
matches_magic_number(const string &magic_number) const {
  nassertr(magic_number.size() >= 2, false);
  int mn =
    ((unsigned char)magic_number[0] << 8) |
    ((unsigned char)magic_number[1]);
  return (mn == SGI_MAGIC);
}

////////////////////////////////////////////////////////////////////
//     Function: PNMFileTypeSGI::make_reader
//       Access: Public, Virtual
//  Description: Allocates and returns a new PNMReader suitable for
//               reading from this file type, if possible.  If reading
//               from this file type is not supported, returns NULL.
////////////////////////////////////////////////////////////////////
PNMReader *PNMFileTypeSGI::
make_reader(istream *file, bool owns_file, const string &magic_number) {
  init_pnm();
  return new Reader(this, file, owns_file, magic_number);
}

////////////////////////////////////////////////////////////////////
//     Function: PNMFileTypeSGI::make_writer
//       Access: Public, Virtual
//  Description: Allocates and returns a new PNMWriter suitable for
//               reading from this file type, if possible.  If writing
//               files of this type is not supported, returns NULL.
////////////////////////////////////////////////////////////////////
PNMWriter *PNMFileTypeSGI::
make_writer(ostream *file, bool owns_file) {
  init_pnm();
  return new Writer(this, file, owns_file);
}


////////////////////////////////////////////////////////////////////
//     Function: PNMFileTypeSGI::register_with_read_factory
//       Access: Public, Static
//  Description: Registers the current object as something that can be
//               read from a Bam file.
////////////////////////////////////////////////////////////////////
void PNMFileTypeSGI::
register_with_read_factory() {
  BamReader::get_factory()->
    register_factory(get_class_type(), make_PNMFileTypeSGI);
}

////////////////////////////////////////////////////////////////////
//     Function: PNMFileTypeSGI::make_PNMFileTypeSGI
//       Access: Protected, Static
//  Description: This method is called by the BamReader when an object
//               of this type is encountered in a Bam file; it should
//               allocate and return a new object with all the data
//               read.
//
//               In the case of the PNMFileType objects, since these
//               objects are all shared, we just pull the object from
//               the registry.
////////////////////////////////////////////////////////////////////
TypedWritable *PNMFileTypeSGI::
make_PNMFileTypeSGI(const FactoryParams &params) {
  return PNMFileTypeRegistry::get_global_ptr()->get_type_by_handle(get_class_type());
}
