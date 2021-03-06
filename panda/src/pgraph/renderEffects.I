// Filename: renderEffects.I
// Created by:  drose (14Mar02)
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
//     Function: RenderEffects::Effect::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
INLINE RenderEffects::Effect::
Effect(const RenderEffect *effect) :
  _type(effect->get_type()),
  _effect(effect)
{
}

////////////////////////////////////////////////////////////////////
//     Function: RenderEffects::Effect::Constructor
//       Access: Public
//  Description: This constructor is only used when reading the
//               RenderEffects from a bam file.  At this point, the
//               effect pointer is unknown.
////////////////////////////////////////////////////////////////////
INLINE RenderEffects::Effect::
Effect() {
}

////////////////////////////////////////////////////////////////////
//     Function: RenderEffects::Effect::Constructor
//       Access: Public
//  Description: This constructor makes an invalid Effect with no
//               RenderEffect pointer; its purpose is just to make an
//               object we can use to look up a particular type in the
//               Effect set.
////////////////////////////////////////////////////////////////////
INLINE RenderEffects::Effect::
Effect(TypeHandle type) :
  _type(type),
  _effect(NULL)
{
}

////////////////////////////////////////////////////////////////////
//     Function: RenderEffects::Effect::Copy Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
INLINE RenderEffects::Effect::
Effect(const Effect &copy) :
  _type(copy._type),
  _effect(copy._effect)
{
}

////////////////////////////////////////////////////////////////////
//     Function: RenderEffects::Effect::Copy Assignment Operator
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void RenderEffects::Effect::
operator = (const Effect &copy) {
  _type = copy._type;
  _effect = copy._effect;
}

////////////////////////////////////////////////////////////////////
//     Function: RenderEffects::Effect::operator <
//       Access: Public
//  Description: This is used by the Effects set to uniquify
//               RenderEffects by type.  Only one RenderEffect of a
//               given type is allowed in the set.  This ordering must
//               also match the ordering reported by compare_to().
////////////////////////////////////////////////////////////////////
INLINE bool RenderEffects::Effect::
operator < (const Effect &other) const {
  return _type < other._type;
}

////////////////////////////////////////////////////////////////////
//     Function: RenderEffects::Effect::compare_to
//       Access: Public
//  Description: Provides an indication of whether a particular
//               effect is equivalent to another one, for purposes
//               of generating unique RenderEffectss.  This should
//               compare all properties of the Effect, but it is
//               important that the type is compared first, to be
//               consistent with the ordering defined by operator <.
////////////////////////////////////////////////////////////////////
INLINE int RenderEffects::Effect::
compare_to(const Effect &other) const {
  if (_type != other._type) {
    return _type.get_index() - other._type.get_index();
  }
  return _effect - other._effect;
}

////////////////////////////////////////////////////////////////////
//     Function: RenderEffects::is_empty
//       Access: Published
//  Description: Returns true if the state is empty, false otherwise.
////////////////////////////////////////////////////////////////////
INLINE bool RenderEffects::
is_empty() const {
  return _effects.empty();
}

////////////////////////////////////////////////////////////////////
//     Function: RenderEffects::get_num_effects
//       Access: Published
//  Description: Returns the number of separate effects indicated
//               in the state.
////////////////////////////////////////////////////////////////////
INLINE int RenderEffects::
get_num_effects() const {
  return _effects.size();
}

////////////////////////////////////////////////////////////////////
//     Function: RenderEffects::get_effect
//       Access: Published
//  Description: Returns the nth effect in the state.
////////////////////////////////////////////////////////////////////
INLINE const RenderEffect *RenderEffects::
get_effect(int n) const {
  nassertr(n >= 0 && n < (int)_effects.size(), NULL);
  return _effects[n]._effect;
}

////////////////////////////////////////////////////////////////////
//     Function: RenderEffects::has_decal
//       Access: Public
//  Description: This function is provided as an optimization, to
//               speed up the render-time checking for the existance
//               of a DecalEffect on this state.  It returns true if a
//               DecalEffect exists, false otherwise.  Note that since
//               there is no additional information stored on the
//               DecalEffect, there's no point in returning it if it
//               exists.
////////////////////////////////////////////////////////////////////
INLINE bool RenderEffects::
has_decal() const {
  if ((_flags & F_checked_decal) == 0) {
    // We pretend this function is const, even though it transparently
    // modifies the internal decal cache.
    ((RenderEffects *)this)->determine_decal();
  }
  return ((_flags & F_has_decal) != 0);
}

////////////////////////////////////////////////////////////////////
//     Function: RenderEffects::has_show_bounds
//       Access: Public
//  Description: This function is provided as an optimization, to
//               speed up the render-time checking for the existance
//               of a ShowBoundsEffect on this state.  It returns true
//               if a ShowBoundsEffect exists, false otherwise.  Note
//               that since there is no additional information stored
//               on the ShowBoundsEffect, there's no point in
//               returning it if it exists.
////////////////////////////////////////////////////////////////////
INLINE bool RenderEffects::
has_show_bounds() const {
  if ((_flags & F_checked_show_bounds) == 0) {
    // We pretend this function is const, even though it transparently
    // modifies the internal show_bounds cache.
    ((RenderEffects *)this)->determine_show_bounds();
  }
  return ((_flags & F_has_show_bounds) != 0);
}

////////////////////////////////////////////////////////////////////
//     Function: RenderEffects::has_cull_callback
//       Access: Public
//  Description: This function is provided as an optimization, to
//               speed up the render-time checking for the existance
//               of an effect with a cull_callback on this state.
////////////////////////////////////////////////////////////////////
INLINE bool RenderEffects::
has_cull_callback() const {
  if ((_flags & F_checked_cull_callback) == 0) {
    // We pretend this function is const, even though it transparently
    // modifies the internal cull_callback cache.
    ((RenderEffects *)this)->determine_cull_callback();
  }
  return ((_flags & F_has_cull_callback) != 0);
}
