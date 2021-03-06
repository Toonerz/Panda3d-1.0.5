//
// genPyCode.pp
//
// This file defines the script to auto-generate a sensible genPyCode
// for the user based on the Config.pp variables in effect at the time
// ppremake is run.  The generated script will know which directories
// to generate its output to, as well as which source files to read
// for the input.
//

#define install_dir $[$[upcase $[PACKAGE]]_INSTALL]
#define install_lib_dir $[or $[INSTALL_LIB_DIR],$[install_dir]/lib]
#define install_bin_dir $[or $[INSTALL_BIN_DIR],$[install_dir]/bin]
#define install_igatedb_dir $[or $[INSTALL_IGATEDB_DIR],$[install_dir]/etc]

#define python python
#if $[and $[WINDOWS_PLATFORM],$[< $[OPTIMIZE],3]]
  #define python $[python]_d
#endif

// If we're on Win32 without Cygwin, generate a genPyCode.bat file;
// for all other platforms, generate a genPyCode sh script.  Although
// it's true that on non-Win32 platforms we don't need the script
// (since the python file itself could be made directly executable),
// we generate the script anyway to be consistent with Win32, which
// does require it.

#if $[eq $[PLATFORM],Win32]

#output genPyCode.bat
@echo off
rem #### Generated automatically by $[PPREMAKE] $[PPREMAKE_VERSION] from $[notdir $[THISFILENAME]].
rem ################################# DO NOT EDIT ###########################

$[python] -u $[osfilename $[install_bin_dir]/genPyCode.py] %1 %2 %3 %4 %5 %6 %7 %8 %9
#end genPyCode.bat

#else  // Win32

#output genPyCode
#! /bin/sh
#### Generated automatically by $[PPREMAKE] $[PPREMAKE_VERSION] from $[notdir $[THISFILENAME]].
################################# DO NOT EDIT ###########################

$[python] -u '$[osfilename $[install_bin_dir]/genPyCode.py]' "$@"
#end genPyCode

#endif  // Win32

#output genPyCode.py
#! /usr/bin/env python
#### Generated automatically by $[PPREMAKE] $[PPREMAKE_VERSION] from $[notdir $[THISFILENAME]].
################################# DO NOT EDIT ###########################

import os
import sys
import glob

#if $[CTPROJS]
# This script was generated while the user was using the ctattach
# tools.  That had better still be the case.

def deCygwinify(path):
    if os.name in ['nt'] and path[0] == '/':
        # On Windows, we may need to convert from a Cygwin-style path
        # to a native Windows path.

        # Check for a case like /i/ or /p/: this converts
        # to i:/ or p:/.

        dirs = path.split('/')
        if len(dirs) > 2 and len(dirs[1]) == 1:
            path = '%s:\%s' % (dirs[1], '\\'.join(dirs[2:]))

        else:
            # Otherwise, prepend $PANDA_ROOT and flip the slashes.
            pandaRoot = os.getenv('PANDA_ROOT')
            if pandaRoot:
                path = os.path.normpath(pandaRoot + path)

    return path

ctprojs = os.getenv('CTPROJS')
if not ctprojs:
    print "You are no longer attached to any trees!"
    sys.exit(1)
    
directDir = os.getenv('DIRECT')
if not directDir:
    print "You are not attached to DIRECT!"
    sys.exit(1)

directDir = deCygwinify(directDir)

# Make sure that direct.showbase.FindCtaPaths gets imported.
parent, base = os.path.split(directDir)

if parent not in sys.path:
    sys.path.append(parent)

import direct.showbase.FindCtaPaths

#endif  // CTPROJS

from direct.ffi import DoGenPyCode
from direct.ffi import FFIConstants

# The following parameters were baked in to this script at the time
# ppremake was run in Direct.
DoGenPyCode.outputDir = r'$[osfilename $[install_lib_dir]/pandac]'
DoGenPyCode.extensionsDir = r'$[osfilename $[TOPDIR]/src/extensions]'
DoGenPyCode.interrogateLib = r'libdtoolconfig'
DoGenPyCode.codeLibs = r'$[GENPYCODE_LIBS]'.split()
DoGenPyCode.etcPath = [r'$[osfilename $[install_igatedb_dir]]']

#if $[>= $[OPTIMIZE], 4]
FFIConstants.wantComments = 0
FFIConstants.wantTypeChecking = 0
#endif

#if $[CTPROJS]

# Actually, the user is expected to be using ctattach, so never mind
# on the baked-in stuff--replace it with the dynamic settings from
# ctattach.
DoGenPyCode.outputDir = os.path.join(directDir, 'lib', 'pandac')
DoGenPyCode.extensionsDir = os.path.join(directDir, 'src', 'extensions')
DoGenPyCode.etcPath = []

# Look for additional packages (other than the basic three)
# that the user might be dynamically attached to.
packages = []
for proj in ctprojs.split():
    projName = proj.split(':')[0]
    packages.append(projName)
packages.reverse()

for package in packages:
    packageDir = os.getenv(package)
    if packageDir:
        packageDir = deCygwinify(packageDir)
        etcDir = os.path.join(packageDir, 'etc')
        try:
            inFiles = glob.glob(os.path.join(etcDir, '*.in'))
        except:
            inFiles = []
        if inFiles:
            DoGenPyCode.etcPath.append(etcDir)

        if package not in ['WINTOOLS', 'DTOOL', 'DIRECT', 'PANDA']:
            libDir = os.path.join(packageDir, 'lib')
            try:
                files = os.listdir(libDir)
            except:
                files = []
            for file in files:
                if os.path.isfile(os.path.join(libDir, file)):
                    basename, ext = os.path.splitext(file)

                    # Try to import the library.  If we can import it,
                    # instrument it.
                    try:
                        __import__(basename, globals(), locals())
                        isModule = 1
                    except:
                        isModule = 0

                    if isModule:
                        if basename not in DoGenPyCode.codeLibs:
                            DoGenPyCode.codeLibs.append(basename)
#endif  // CTPROJS

DoGenPyCode.run()

#end genPyCode.py
