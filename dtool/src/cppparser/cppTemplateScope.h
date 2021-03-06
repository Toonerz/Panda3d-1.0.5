// Filename: cppTemplateScope.h
// Created by:  drose (28Oct99)
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

#ifndef CPPTEMPLATESCOPE_H
#define CPPTEMPLATESCOPE_H

#include "dtoolbase.h"

#include "cppScope.h"
#include "cppTemplateParameterList.h"

///////////////////////////////////////////////////////////////////
//       Class : CPPTemplateScope
// Description : This is an implicit scope that is created following
//               the appearance of a "template<class x, class y>" or
//               some such line in a C++ file.  It simply defines the
//               template parameters.
////////////////////////////////////////////////////////////////////
class CPPTemplateScope : public CPPScope {
public:
  CPPTemplateScope(CPPScope *parent_scope);

  void add_template_parameter(CPPDeclaration *param);

  virtual void add_declaration(CPPDeclaration *decl, CPPScope *global_scope,
                               CPPPreprocessor *preprocessor,
                               const cppyyltype &pos);
  virtual void add_enum_value(CPPInstance *inst);
  virtual void define_extension_type(CPPExtensionType *type);
  virtual void define_namespace(CPPNamespace *scope);
  virtual void add_using(CPPUsing *using_decl, CPPScope *global_scope,
                         CPPPreprocessor *error_sink = NULL);

  virtual bool is_fully_specified() const;

  virtual string get_simple_name() const;
  virtual string get_local_name(CPPScope *scope = NULL) const;
  virtual string get_fully_scoped_name() const;

  virtual void output(ostream &out, CPPScope *scope) const;

  virtual CPPTemplateScope *as_template_scope();

  CPPTemplateParameterList _parameters;
};

#endif
