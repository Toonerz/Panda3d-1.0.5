// Filename: parserDefs.h
// Created by:  drose (17Jan99)
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

#ifndef PARSER_H
#define PARSER_H

#include "pandabase.h"

#include "eggObject.h"

#include "pointerTo.h"
#include "pointerToArray.h"
#include "pta_double.h"

#include <string>

class EggGroupNode;

void egg_init_parser(istream &in, const string &filename,
                     EggObject *tos, EggGroupNode *egg_top_node);

void egg_cleanup_parser();

// This structure holds the return value for each token.
// Traditionally, this is a union, and is declared with the %union
// declaration in the parser.y file, but unions are pretty worthless
// in C++ (you can't include an object that has member functions in a
// union), so we'll use a class instead.  That means we need to
// declare it externally, here.

class EXPCL_PANDAEGG EggTokenType {
public:
  double _number;
  unsigned long _ulong;
  string _string;
  PT(EggObject) _egg;
  PTA_double _number_list;
};

// The yacc-generated code expects to use the symbol 'YYSTYPE' to
// refer to the above class.
#define YYSTYPE EggTokenType

#endif
