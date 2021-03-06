// Filename: mayaSavePview.cxx
// Created by:  drose (27Oct03)
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

#include "mayaSavePview.h"

#include <maya/MString.h>
#include <maya/MFnPlugin.h>
#include <maya/MFileIO.h>

#include <stdlib.h>

#ifdef WIN32_VC
#include <process.h>
#endif

////////////////////////////////////////////////////////////////////
//     Function: MayaSavePview::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
MayaSavePview::
MayaSavePview() {
}

////////////////////////////////////////////////////////////////////
//     Function: MayaSavePview::doIt
//       Access: Public, Virtual
//  Description: Called when the plugin command is invoked.
////////////////////////////////////////////////////////////////////
MStatus MayaSavePview::
doIt(const MArgList &) {
  MStatus result;
  
  // First, make sure the current buffer is saved.
  result = MFileIO::save(false);
  if (result != MS::kSuccess) {
    return result;
  }

  MString filename = MFileIO::currentFile();

#ifdef WIN32_VC
  // On Windows, we use the spawn function to run pview
  // asynchronously.
  MString quoted = MString("\"") + filename + MString("\"");
  int retval = _spawnlp(_P_DETACH, "pview", 
                        "pview", "-cl", quoted.asChar(), NULL);
  if (retval == -1) {
    return MS::kFailure;
  }

#else  // WIN32_VC
  // On non-Windows (e.g. Unix), we just use the system function,
  // which runs synchronously.  We could fork a process, but no one's
  // asked for this yet.
  MString command = MString("pview -c \"") + filename + MString("\"");

  int command_result = system(command.asChar());
  if (command_result != 0) {
    return MS::kFailure;
  }
#endif // WIN32_VC

  return MS::kSuccess;
}

////////////////////////////////////////////////////////////////////
//     Function: MayaSavePview::creator
//       Access: Public, Static
//  Description: This is used to create a new instance of the plugin.
////////////////////////////////////////////////////////////////////
void *MayaSavePview::
creator() {
  return new MayaSavePview;
}



////////////////////////////////////////////////////////////////////
//     Function: initializePlugin
//  Description: Called by Maya when the plugin is loaded.
////////////////////////////////////////////////////////////////////
EXPCL_MISC MStatus 
initializePlugin(MObject obj) {
  MFnPlugin plugin(obj, "VR Studio", "1.0");
  MStatus status;
  status = plugin.registerCommand("pview", MayaSavePview::creator);
  if (!status) {
    status.perror("registerCommand");
  }

  return status;
}

////////////////////////////////////////////////////////////////////
//     Function: uninitializePlugin
//  Description: Called by Maya when the plugin is unloaded.
////////////////////////////////////////////////////////////////////
EXPCL_MISC MStatus
uninitializePlugin(MObject obj) {
  MFnPlugin plugin(obj);
  MStatus status;
  status = plugin.deregisterCommand("pview");

  if (!status) {
    status.perror("deregisterCommand");
  }
  return status;
}
