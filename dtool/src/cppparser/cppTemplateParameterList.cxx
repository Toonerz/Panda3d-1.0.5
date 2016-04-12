// Filename: cppTemplateParameterList.cxx
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


#include "cppTemplateParameterList.h"
#include "cppClassTemplateParameter.h"
#include "cppInstance.h"
#include "cppExpression.h"

////////////////////////////////////////////////////////////////////
//     Function: CPPTemplateParameterList::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
CPPTemplateParameterList::
CPPTemplateParameterList() {
}

////////////////////////////////////////////////////////////////////
//     Function: CPPTemplateParameterList::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
string CPPTemplateParameterList::
get_string() const {
  ostringstream strname;
  strname << "< " << *this << " >";
  return strname.str();
}

////////////////////////////////////////////////////////////////////
//     Function: CPPTemplateParameterList::build_subst_decl
//       Access: Public
//  Description: Matches up the actual parameters one-to-one with the
//               formal parameters they are replacing, so the template
//               may be instantiated by swapping out each occurrence
//               of a template standin type with its appropriate
//               replacement.
////////////////////////////////////////////////////////////////////
void CPPTemplateParameterList::
build_subst_decl(const CPPTemplateParameterList &formal_params,
                 CPPDeclaration::SubstDecl &subst,
                 CPPScope *current_scope, CPPScope *global_scope) const {
  Parameters::const_iterator pfi, pai;
  for (pfi = formal_params._parameters.begin(), pai = _parameters.begin();
       pfi != formal_params._parameters.end() && pai != _parameters.end();
       ++pfi, ++pai) {
    CPPDeclaration *formal = *pfi;
    CPPDeclaration *actual = *pai;

    if (actual->as_type()) {
      actual = actual->as_type()->resolve_type(current_scope, global_scope);
    }

    if (!(formal == actual)) {
      subst.insert(CPPDeclaration::SubstDecl::value_type(formal, actual));
    }
  }

  // Fill in the default template parameters.
  while (pfi != formal_params._parameters.end()) {
    CPPDeclaration *decl = (*pfi);
    if (decl->as_instance()) {
      // A value template parameter.  Its default is an expression.
      CPPInstance *inst = decl->as_instance();
      if (inst->_initializer != NULL) {
        CPPDeclaration *decl =
          inst->_initializer->substitute_decl(subst, current_scope,
                                              global_scope);
        if (!(*decl == *inst)) {
          subst.insert(CPPDeclaration::SubstDecl::value_type
                       (inst, decl));
        }
      }
    } else if (decl->as_class_template_parameter()) {
      // A class template parameter.
      CPPClassTemplateParameter *cparam = decl->as_class_template_parameter();
      if (cparam->_default_type != NULL) {
        CPPDeclaration *decl =
          cparam->_default_type->substitute_decl(subst, current_scope,
                                                 global_scope);
        if (!(*cparam == *decl)) {
          subst.insert(CPPDeclaration::SubstDecl::value_type
                       (cparam, decl));
        }
      }
    }
    ++pfi;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: CPPTemplateParameterList::is_fully_specified
//       Access: Public
//  Description: This function returns true if all the parameters in
//               the list are real expressions or classes, and not
//               types yet to-be-determined or template parameter
//               types.  That is, this returns true for a normal
//               template instantiation, and false for a template
//               instantiation based on template parameters that have
//               not yet been specified.
////////////////////////////////////////////////////////////////////
bool CPPTemplateParameterList::
is_fully_specified() const {
  for (int i = 0; i < (int)_parameters.size(); i++) {
    if (!_parameters[i]->is_fully_specified()) {
      return false;
    }
  }
  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: CPPTemplateParameterList::is_tbd
//       Access: Public
//  Description: Returns true if any type within the parameter list is
//               a CPPTBDType and thus isn't fully determined right
//               now.
////////////////////////////////////////////////////////////////////
bool CPPTemplateParameterList::
is_tbd() const {
  for (int i = 0; i < (int)_parameters.size(); i++) {
    CPPType *type = _parameters[i]->as_type();
    if (type != (CPPType *)NULL &&
        (type->is_tbd() || type->as_class_template_parameter() != NULL)) {
      return true;
    }
    CPPExpression *expr = _parameters[i]->as_expression();
    if (expr != NULL && expr->is_tbd()) {
      return true;
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////
//     Function: CPPTemplateParameterList::Equivalence Operator
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
bool CPPTemplateParameterList::
operator == (const CPPTemplateParameterList &other) const {
  if (_parameters.size() != other._parameters.size()) {
    return false;
  }
  for (int i = 0; i < (int)_parameters.size(); i++) {
    if (*_parameters[i] != *other._parameters[i]) {
      return false;
    }
  }
  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: CPPTemplateParameterList::Nonequivalence Operator
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
bool CPPTemplateParameterList::
operator != (const CPPTemplateParameterList &other) const {
  return !(*this == other);
}

////////////////////////////////////////////////////////////////////
//     Function: CPPTemplateParameterList::Ordering Operator
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
bool CPPTemplateParameterList::
operator < (const CPPTemplateParameterList &other) const {
  if (_parameters.size() != other._parameters.size()) {
    return _parameters.size() < other._parameters.size();
  }
  for (int i = 0; i < (int)_parameters.size(); i++) {
    if (*_parameters[i] != *other._parameters[i]) {
      return *_parameters[i] < *other._parameters[i];
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////
//     Function: CPPTemplateParameterList::substitute_decl
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
CPPTemplateParameterList *CPPTemplateParameterList::
substitute_decl(CPPDeclaration::SubstDecl &subst,
                CPPScope *current_scope, CPPScope *global_scope) {
  CPPTemplateParameterList *rep = new CPPTemplateParameterList(*this);

  bool anything_changed = false;
  for (int i = 0; i < (int)rep->_parameters.size(); i++) {
    rep->_parameters[i] =
      _parameters[i]->substitute_decl(subst, current_scope, global_scope);
    if (rep->_parameters[i] != _parameters[i]) {
      anything_changed = true;
    }
  }

  if (!anything_changed) {
    delete rep;
    rep = this;
  }

  return rep;
}

////////////////////////////////////////////////////////////////////
//     Function: CPPTemplateParameterList::output
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
void CPPTemplateParameterList::
output(ostream &out, CPPScope *scope) const {
  if (!_parameters.empty()) {
    Parameters::const_iterator pi = _parameters.begin();
    (*pi)->output(out, 0, scope, false);

    ++pi;
    while (pi != _parameters.end()) {
      out << ", ";
      (*pi)->output(out, 0, scope, false);
      ++pi;
    }
  }
}

////////////////////////////////////////////////////////////////////
//     Function: CPPTemplateParameterList::write_formal
//       Access: Public
//  Description: Writes the list as a set of formal parameters for a
//               template scope.  Includes the keyword "template" and
//               the angle brackets, as well as the trailing newline.
////////////////////////////////////////////////////////////////////
void CPPTemplateParameterList::
write_formal(ostream &out, CPPScope *scope) const {
  out << "template<";
  if (!_parameters.empty()) {
    Parameters::const_iterator pi = _parameters.begin();
    (*pi)->output(out, 0, scope, true);

    ++pi;
    while (pi != _parameters.end()) {
      out << ", ";
      (*pi)->output(out, 0, scope, true);
      ++pi;
    }
  }
  out << ">\n";
}