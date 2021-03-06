// Filename: pointerTo.I
// Created by:  drose (10Feb99)
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
//     Function: PointerTo::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
template<class T>
INLINE PointerTo<T>::
PointerTo(To *ptr) : PointerToBase<T>(ptr) {
}

////////////////////////////////////////////////////////////////////
//     Function: PointerTo::Copy Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
template<class T>
INLINE PointerTo<T>::
PointerTo(const PointerTo<T> &copy) :
  PointerToBase<T>((const PointerToBase<T> &)copy)
{
}

////////////////////////////////////////////////////////////////////
//     Function: PointerTo::Dereference operator
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
template<class T>
INLINE TYPENAME PointerTo<T>::To &PointerTo<T>::
operator *() const {
  return *((To *)(this->_void_ptr));
}

////////////////////////////////////////////////////////////////////
//     Function: PointerTo::Member access operator
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
template<class T>
INLINE TYPENAME PointerTo<T>::To *PointerTo<T>::
operator -> () const {
  return (To *)(this->_void_ptr);
}

////////////////////////////////////////////////////////////////////
//     Function: PointerTo::Typecast operator
//       Access: Public
//  Description: We also have the typecast operator to automatically
//               convert PointerTo's to the required kind of actual
//               pointer.  This introduces ambiguities which the
//               compiler will resolve one way or the other, but we
//               don't care which way it goes because either will be
//               correct.
////////////////////////////////////////////////////////////////////
template<class T>
INLINE PointerTo<T>::
operator TYPENAME PointerToBase<T>::To *() const {
  return (To *)(this->_void_ptr);
}

////////////////////////////////////////////////////////////////////
//     Function: PointerTo::p
//       Access: Public
//  Description: Returns an ordinary pointer instead of a PointerTo.
//               Useful to work around compiler problems, particularly
//               for implicit upcasts.
////////////////////////////////////////////////////////////////////
template<class T>
INLINE TYPENAME PointerTo<T>::To *PointerTo<T>::
p() const {
  return (To *)(this->_void_ptr);
}

////////////////////////////////////////////////////////////////////
//     Function: PointerTo::Assignment operator
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
template<class T>
INLINE PointerTo<T> &PointerTo<T>::
operator = (To *ptr) {
  reassign(ptr);
  return *this;
}

////////////////////////////////////////////////////////////////////
//     Function: PointerTo::Assignment operator
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
template<class T>
INLINE PointerTo<T> &PointerTo<T>::
operator = (const PointerTo<T> &copy) {
  reassign((const PointerToBase<T> &)copy);
  return *this;
}

////////////////////////////////////////////////////////////////////
//     Function: ConstPointerTo::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
template<class T>
INLINE ConstPointerTo<T>::
ConstPointerTo(const TYPENAME ConstPointerTo<T>::To *ptr) :
  PointerToBase<T>((TYPENAME ConstPointerTo<T>::To *)ptr)
{
}

////////////////////////////////////////////////////////////////////
//     Function: ConstPointerTo::Copy Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
template<class T>
INLINE ConstPointerTo<T>::
ConstPointerTo(const PointerTo<T> &copy) :
  PointerToBase<T>((const PointerToBase<T> &)copy)
{
}

////////////////////////////////////////////////////////////////////
//     Function: ConstPointerTo::Copy Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
template<class T>
INLINE ConstPointerTo<T>::
ConstPointerTo(const ConstPointerTo<T> &copy) :
  PointerToBase<T>((const PointerToBase<T> &)copy)
{
}

////////////////////////////////////////////////////////////////////
//     Function: ConstPointerTo::Dereference operator
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
template<class T>
INLINE const TYPENAME ConstPointerTo<T>::To &ConstPointerTo<T>::
operator *() const {
  return *((To *)(this->_void_ptr));
}

////////////////////////////////////////////////////////////////////
//     Function: ConstPointerTo::Member access operator
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
template<class T>
INLINE const TYPENAME ConstPointerTo<T>::To *ConstPointerTo<T>::
operator -> () const {
  return (To *)(this->_void_ptr);
}

////////////////////////////////////////////////////////////////////
//     Function: ConstPointerTo::Typecast operator
//       Access: Public
//  Description: We also have the typecast operator to automatically
//               convert ConstPointerTo's to the required kind of actual
//               pointer.  This introduces ambiguities which the
//               compiler will resolve one way or the other, but we
//               don't care which way it goes because either will be
//               correct.
////////////////////////////////////////////////////////////////////

template<class T>
INLINE ConstPointerTo<T>::
operator const TYPENAME PointerToBase<T>::To *() const {
  return (To *)(this->_void_ptr);
}

////////////////////////////////////////////////////////////////////
//     Function: ConstPointerTo::p
//       Access: Public
//  Description: Returns an ordinary pointer instead of a ConstPointerTo.
//               Useful to work around compiler problems, particularly
//               for implicit upcasts.
////////////////////////////////////////////////////////////////////
template<class T>
INLINE const TYPENAME ConstPointerTo<T>::To *ConstPointerTo<T>::
p() const {
  return (To *)(this->_void_ptr);
}

////////////////////////////////////////////////////////////////////
//     Function: ConstPointerTo::Assignment operator
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
template<class T>
INLINE ConstPointerTo<T> &ConstPointerTo<T>::
operator = (const To *ptr) {
  reassign((To *)ptr);
  return *this;
}

////////////////////////////////////////////////////////////////////
//     Function: ConstPointerTo::Assignment operator
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
template<class T>
INLINE ConstPointerTo<T> &ConstPointerTo<T>::
operator = (const PointerTo<T> &copy) {
  reassign((const PointerToBase<T> &)copy);
  return *this;
}

////////////////////////////////////////////////////////////////////
//     Function: ConstPointerTo::Assignment operator
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
template<class T>
INLINE ConstPointerTo<T> &ConstPointerTo<T>::
operator = (const ConstPointerTo<T> &copy) {
  reassign((const PointerToBase<T> &)copy);
  return *this;
}
