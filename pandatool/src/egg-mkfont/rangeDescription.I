// Filename: rangeDescription.I
// Created by:  drose (07Sep03)
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


////////////////////////////////////////////////////////////////////
//     Function: RangeDescription::add_singleton
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void RangeDescription::
add_singleton(int code) {
  _range_list.push_back(Range(code));
}

////////////////////////////////////////////////////////////////////
//     Function: RangeDescription::add_range
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void RangeDescription::
add_range(int from_code, int to_code) {
  _range_list.push_back(Range(from_code, to_code));
}

////////////////////////////////////////////////////////////////////
//     Function: RangeDescription::is_empty
//       Access: Public
//  Description: Returns true if there are no codes described in the
//               range.
////////////////////////////////////////////////////////////////////
INLINE bool RangeDescription::
is_empty() const {
  return _range_list.empty();
}

////////////////////////////////////////////////////////////////////
//     Function: RangeDescription::Range::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
INLINE RangeDescription::Range::
Range(int code) :
  _from_code(code),
  _to_code(code)
{
}

////////////////////////////////////////////////////////////////////
//     Function: RangeDescription::Range::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
INLINE RangeDescription::Range::
Range(int from_code, int to_code) :
  _from_code(from_code),
  _to_code(to_code)
{
}

INLINE ostream &operator << (ostream &out, const RangeDescription &range) {
  range.output(out);
  return out;
}
