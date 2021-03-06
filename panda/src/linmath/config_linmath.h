// Filename: config_linmath.h
// Created by:  drose (23Feb00)
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

#ifndef CONFIG_LINMATH_H
#define CONFIG_LINMATH_H

#include "pandabase.h"
#include "notifyCategoryProxy.h"
#include "configVariableBool.h"

NotifyCategoryDecl(linmath, EXPCL_PANDA, EXPTP_PANDA);

extern EXPCL_PANDA ConfigVariableBool paranoid_hpr_quat;
extern EXPCL_PANDA ConfigVariableBool temp_hpr_fix;

extern EXPCL_PANDA void init_liblinmath();

#endif
