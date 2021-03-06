// Filename: eggExternalReference.cxx
// Created by:  drose (11Feb99)
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

#include "eggExternalReference.h"
#include "eggMiscFuncs.h"

#include "indent.h"
#include "string_utils.h"

TypeHandle EggExternalReference::_type_handle;


////////////////////////////////////////////////////////////////////
//     Function: EggExternalReference::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
EggExternalReference::
EggExternalReference(const string &node_name, const string &filename)
  : EggFilenameNode(node_name, filename) {
}

////////////////////////////////////////////////////////////////////
//     Function: EggExternalReference::Copy constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
EggExternalReference::
EggExternalReference(const EggExternalReference &copy)
  : EggFilenameNode(copy) {
}

////////////////////////////////////////////////////////////////////
//     Function: EggExternalReference::Copy assignment operator
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
EggExternalReference &EggExternalReference::
operator = (const EggExternalReference &copy) {
  EggFilenameNode::operator = (copy);
  return *this;
}

////////////////////////////////////////////////////////////////////
//     Function: EggExternalReference::write
//       Access: Public, Virtual
//  Description: Writes the reference to the indicated output
//               stream in Egg format.
////////////////////////////////////////////////////////////////////
void EggExternalReference::
write(ostream &out, int indent_level) const {
  write_header(out, indent_level, "<File>");
  enquote_string(out, get_filename(), indent_level + 2) << "\n";
  indent(out, indent_level) << "}\n";
}

////////////////////////////////////////////////////////////////////
//     Function: EggExternalReference::get_default_extension
//       Access: Public, Virtual
//  Description: Returns the default extension for this filename type.
////////////////////////////////////////////////////////////////////
string EggExternalReference::
get_default_extension() const {
  return string("egg");
}
