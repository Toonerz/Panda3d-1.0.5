// Filename: datagram_ui.cxx
// Created by:  drose (09Feb00)
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

#include "datagram_ui.h"
#include "datagramIterator.h"

#include <string>
#include <stdlib.h>
#include <ctype.h>

enum DatagramElement {
  DE_int32,
  DE_float64,
  DE_string,
  DE_end
};

istream &
operator >> (istream &in, NetDatagram &datagram) {
  datagram.clear();

  // First, read a line of text.
  string line;
  getline(in, line);

  // Now parse the text.
  size_t p = 0;
  while (p < line.length()) {
    // Skip whitespace
    while (p < line.length() && isspace(line[p])) {
      p++;
    }

    // What have we got?
    if (p < line.length()) {
      if (isdigit(line[p]) || line[p] == '-') {
        // A number.
        size_t start = p;
        p++;
        while (p < line.length() && isdigit(line[p])) {
          p++;
        }
        if (p < line.length() && line[p] == '.') {
          // A floating-point number.
          p++;
          while (p < line.length() && isdigit(line[p])) {
            p++;
          }
          double num = atof(line.substr(start, p - start).c_str());
          datagram.add_int8(DE_float64);
          datagram.add_float64(num);
        } else {
          // An integer.
          int num = atoi(line.substr(start, p - start).c_str());
          datagram.add_int8(DE_int32);
          datagram.add_int32(num);
        }

      } else if (line[p] == '"') {
        // A quoted string.
        p++;
        size_t start = p;
        while (p < line.length() && line[p] != '"') {
          p++;
        }
        string str = line.substr(start, p - start);
        datagram.add_int8(DE_string);
        datagram.add_string(str);
        p++;

      } else {
        // An unquoted string.
        size_t start = p;
        while (p < line.length() && !isspace(line[p])) {
          p++;
        }
        string str = line.substr(start, p - start);
        datagram.add_int8(DE_string);
        datagram.add_string(str);
      }
    }
  }
  datagram.add_int8(DE_end);
  return in;
}

ostream &
operator << (ostream &out, const NetDatagram &datagram) {
  DatagramIterator di(datagram);

  DatagramElement de = (DatagramElement)di.get_int8();
  while (de != DE_end) {
    switch (de) {
    case DE_int32:
      out << di.get_int32() << " ";
      break;

    case DE_float64:
      out << di.get_float64() << " ";
      break;

    case DE_string:
      out << "\"" << di.get_string() << "\" ";
      break;

    default:
      out << "(invalid datagram)";
      return out;
    }
    de = (DatagramElement)di.get_int8();
  }
  return out;
}
