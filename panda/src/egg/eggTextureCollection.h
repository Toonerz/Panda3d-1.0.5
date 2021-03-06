// Filename: eggTextureCollection.h
// Created by:  drose (15Feb00)
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

#ifndef EGGTEXTURECOLLECTION_H
#define EGGTEXTURECOLLECTION_H

#include "pandabase.h"

#include "eggTexture.h"
#include "eggGroupNode.h"
#include "vector_PT_EggTexture.h"

#include "pmap.h"

////////////////////////////////////////////////////////////////////
//       Class : EggTextureCollection
// Description : This is a collection of textures by TRef name.  It
//               can extract the textures from an egg file and sort
//               them all together; it can also manage the creation of
//               unique textures and the assignment of unique TRef
//               names.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDAEGG EggTextureCollection {

  // This is a bit of private interface stuff that must be here as a
  // forward reference.  This allows us to define the
  // EggTextureCollection as an STL container.

private:
  typedef pmap<PT_EggTexture, int> Textures;
  typedef vector_PT_EggTexture OrderedTextures;

public:
  typedef OrderedTextures::const_iterator iterator;
  typedef iterator const_iterator;
  typedef OrderedTextures::size_type size_type;

  typedef pmap<PT_EggTexture,  PT_EggTexture > TextureReplacement;

  // Here begins the actual public interface to EggTextureCollection.

PUBLISHED:
  EggTextureCollection();
  EggTextureCollection(const EggTextureCollection &copy);
  EggTextureCollection &operator = (const EggTextureCollection &copy);
  ~EggTextureCollection();

  void clear();

  int extract_textures(EggGroupNode *node);

  bool is_empty() const;
  int get_num_textures() const;
  EggTexture *get_texture(int index) const;

public:
  EggGroupNode::iterator insert_textures(EggGroupNode *node);
  EggGroupNode::iterator insert_textures(EggGroupNode *node, EggGroupNode::iterator position);

PUBLISHED:
  int find_used_textures(EggNode *node);
  void remove_unused_textures(EggNode *node);

  int collapse_equivalent_textures(int eq, EggGroupNode *node);
  int collapse_equivalent_textures(int eq, TextureReplacement &removed);
  static void replace_textures(EggGroupNode *node,
                               const TextureReplacement &replace);

  void uniquify_trefs();
  void sort_by_tref();

public:
  // Can be used to traverse all the textures in the collection, in
  // order as last sorted.
  INLINE iterator begin() const;
  INLINE iterator end() const;
  INLINE bool empty() const;
  INLINE size_type size() const;

PUBLISHED:
  bool add_texture(EggTexture *texture);
  bool remove_texture(EggTexture *texture);

  // create_unique_texture() creates a new texture if there is not
  // already one equivalent (according to eq, see
  // EggTexture::is_equivalent_to()) to the indicated texture, or
  // returns the existing one if there is.
  EggTexture *create_unique_texture(const EggTexture &copy, int eq);

  // Find a texture with a particular TRef name.
  EggTexture *find_tref(const string &tref_name) const;

  // Find a texture with a particular filename.
  EggTexture *find_filename(const Filename &filename) const;

private:
  Textures _textures;
  OrderedTextures _ordered_textures;
};

#include "eggTextureCollection.I"

#endif
