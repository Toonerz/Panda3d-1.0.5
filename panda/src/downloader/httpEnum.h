// Filename: httpEnum.h
// Created by:  drose (25Oct02)
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

#ifndef HTTPENUM_H
#define HTTPENUM_H

#include "pandabase.h"

// This module requires OpenSSL to compile, even if you do not intend
// to use this to establish https connections; this is because it uses
// the OpenSSL library to portably handle all of the socket
// communications.

#ifdef HAVE_SSL

////////////////////////////////////////////////////////////////////
//       Class : HTTPEnum
// Description : This class is just used as a namespace wrapper for
//               some of the enumerated types used by various classes
//               within the HTTPClient family.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDAEXPRESS HTTPEnum {
PUBLISHED:
  enum HTTPVersion {
    HV_09,  // HTTP 0.9 or older
    HV_10,  // HTTP 1.0
    HV_11,  // HTTP 1.1
    HV_other,
  };

  enum Method {
    M_options,
    M_get,
    M_head,
    M_post,
    M_put,
    M_delete,
    M_trace,
    M_connect,
  };
};

ostream &operator << (ostream &out, HTTPEnum::Method method);

#endif // HAVE_SSL

#endif

