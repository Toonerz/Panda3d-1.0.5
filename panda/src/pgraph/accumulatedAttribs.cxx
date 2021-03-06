// Filename: accumulatedAttribs.cxx
// Created by:  drose (30Jan03)
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

#include "accumulatedAttribs.h"
#include "sceneGraphReducer.h"
#include "geomTransformer.h"
#include "pandaNode.h"
#include "colorAttrib.h"
#include "colorScaleAttrib.h"
#include "texMatrixAttrib.h"
#include "textureAttrib.h"
#include "config_pgraph.h"


////////////////////////////////////////////////////////////////////
//     Function: AccumulatedAttribs::write
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
void AccumulatedAttribs::
write(ostream &out, int attrib_types, int indent_level) const {
  if ((attrib_types & SceneGraphReducer::TT_transform) != 0) {
    _transform->write(out, indent_level);
  }
  if ((attrib_types & SceneGraphReducer::TT_color) != 0) {
    if (_color == (const RenderAttrib *)NULL) {
      indent(out, indent_level) << "no color\n";
    } else {
      _color->write(out, indent_level);
    }
  }
  if ((attrib_types & SceneGraphReducer::TT_color_scale) != 0) {
    if (_color_scale == (const RenderAttrib *)NULL) {
      indent(out, indent_level) << "no color scale\n";
    } else {
      _color_scale->write(out, indent_level);
    }
  }
  if ((attrib_types & SceneGraphReducer::TT_tex_matrix) != 0) {
    if (_tex_matrix == (const RenderAttrib *)NULL) {
      indent(out, indent_level) << "no tex matrix\n";
    } else {
      _tex_matrix->write(out, indent_level);
    }
  }
  if ((attrib_types & SceneGraphReducer::TT_other) != 0) {
    _other->write(out, indent_level);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: AccumulatedAttribs::collect
//       Access: Public
//  Description: Collects the state and transform from the indicated
//               node and adds it to the accumulator, removing it from
//               the node.
////////////////////////////////////////////////////////////////////
void AccumulatedAttribs::
collect(PandaNode *node, int attrib_types) {
  if ((attrib_types & SceneGraphReducer::TT_transform) != 0) {
    // Collect the node's transform.
    nassertv(_transform != (TransformState *)NULL);
    _transform = _transform->compose(node->get_transform());
    node->set_transform(TransformState::make_identity());
    node->reset_prev_transform();
  }

  if ((attrib_types & SceneGraphReducer::TT_color) != 0) {
    const RenderAttrib *node_attrib = 
      node->get_attrib(ColorAttrib::get_class_type());
    if (node_attrib != (const RenderAttrib *)NULL) {
      // The node has a color attribute; apply it.
      if (_color == (const RenderAttrib *)NULL) {
        _color = node_attrib;
      } else {
        _color = _color->compose(node_attrib);
      }
      node->clear_attrib(ColorAttrib::get_class_type());
    }
  }

  if ((attrib_types & SceneGraphReducer::TT_color_scale) != 0) {
    const RenderAttrib *node_attrib = 
      node->get_attrib(ColorScaleAttrib::get_class_type());
    if (node_attrib != (const RenderAttrib *)NULL) {
      // The node has a color scale attribute; apply it.
      if (_color_scale == (const RenderAttrib *)NULL) {
        _color_scale = node_attrib;
      } else {
        _color_scale = _color_scale->compose(node_attrib);
      }
      node->clear_attrib(ColorScaleAttrib::get_class_type());
    }
  }

  if ((attrib_types & SceneGraphReducer::TT_tex_matrix) != 0) {
    const RenderAttrib *node_attrib = 
      node->get_attrib(TexMatrixAttrib::get_class_type());
    if (node_attrib != (const RenderAttrib *)NULL) {
      // The node has a tex matrix attribute; apply it.
      if (_tex_matrix == (const RenderAttrib *)NULL) {
        _tex_matrix = node_attrib;
      } else {
        _tex_matrix = _tex_matrix->compose(node_attrib);
      }
      node->clear_attrib(TexMatrixAttrib::get_class_type());
    }

    // We also need to accumulate the texture state if we are
    // accumulating texture matrix.
    const RenderAttrib *tex_attrib = 
      node->get_attrib(TextureAttrib::get_class_type());
    if (tex_attrib != (const RenderAttrib *)NULL) {
      if (_texture == (const RenderAttrib *)NULL) {
        _texture = tex_attrib;
      } else {
        _texture = _texture->compose(tex_attrib);
      }

      // However, we don't remove the texture state from the node.
      // We're just accumulating it so we can tell which texture
      // coordinates are safe to flatten.
    }
  }

  if ((attrib_types & SceneGraphReducer::TT_other) != 0) {
    // Collect everything else.
    nassertv(_other != (RenderState *)NULL);
    _other = _other->compose(node->get_state());
    node->set_state(RenderState::make_empty());
  }
}

////////////////////////////////////////////////////////////////////
//     Function: AccumulatedAttribs::collect
//       Access: Public
//  Description: Collects the state and transform from the indicated
//               node and adds it to the accumulator, removing it from
//               the state (and returning a new state).
////////////////////////////////////////////////////////////////////
CPT(RenderState) AccumulatedAttribs::
collect(const RenderState *state, int attrib_types) {
  CPT(RenderState) new_state = state;

  if ((attrib_types & SceneGraphReducer::TT_color) != 0) {
    const RenderAttrib *node_attrib = 
      new_state->get_attrib(ColorAttrib::get_class_type());
    if (node_attrib != (const RenderAttrib *)NULL) {
      // The node has a color attribute; apply it.
      if (_color == (const RenderAttrib *)NULL) {
        _color = node_attrib;
      } else {
        _color = _color->compose(node_attrib);
      }
      new_state = new_state->remove_attrib(ColorAttrib::get_class_type());
    }
  }

  if ((attrib_types & SceneGraphReducer::TT_color_scale) != 0) {
    const RenderAttrib *node_attrib = 
      new_state->get_attrib(ColorScaleAttrib::get_class_type());
    if (node_attrib != (const RenderAttrib *)NULL) {
      if (_color_scale == (const RenderAttrib *)NULL) {
        _color_scale = node_attrib;
      } else {
        _color_scale = _color_scale->compose(node_attrib);
      }
      new_state = new_state->remove_attrib(ColorScaleAttrib::get_class_type());
    }
  }

  if ((attrib_types & SceneGraphReducer::TT_tex_matrix) != 0) {
    const RenderAttrib *node_attrib = 
      new_state->get_attrib(TexMatrixAttrib::get_class_type());
    if (node_attrib != (const RenderAttrib *)NULL) {
      if (_tex_matrix == (const RenderAttrib *)NULL) {
        _tex_matrix = node_attrib;
      } else {
        _tex_matrix = _tex_matrix->compose(node_attrib);
      }
      new_state = new_state->remove_attrib(TexMatrixAttrib::get_class_type());
    }

    // We also need to accumulate the texture state if we are
    // accumulating texture matrix.
    const RenderAttrib *tex_attrib = 
      new_state->get_attrib(TextureAttrib::get_class_type());
    if (tex_attrib != (const RenderAttrib *)NULL) {
      if (_texture == (const RenderAttrib *)NULL) {
        _texture = tex_attrib;
      } else {
        _texture = _texture->compose(tex_attrib);
      }

      // However, we don't remove the texture state from the node.
      // We're just accumulating it so we can tell which texture
      // coordinates are safe to flatten.
    }
  }

  if ((attrib_types & SceneGraphReducer::TT_other) != 0) {
    // Collect everything else.
    nassertr(_other != (RenderState *)NULL, new_state);
    _other = _other->compose(new_state);
    new_state = RenderState::make_empty();
  }

  return new_state;
}

////////////////////////////////////////////////////////////////////
//     Function: AccumulatedAttribs::apply_to_node
//       Access: Public
//  Description: Stores the indicated attributes in the node's
//               transform and state information; does not attempt to
//               apply the properties to the vertices.  Clears the
//               attributes from the accumulator for future
//               traversals.
////////////////////////////////////////////////////////////////////
void AccumulatedAttribs::
apply_to_node(PandaNode *node, int attrib_types) {
  if ((attrib_types & SceneGraphReducer::TT_transform) != 0) {
    node->set_transform(_transform->compose(node->get_transform()));
    node->reset_prev_transform();
    _transform = TransformState::make_identity();
  }

  if ((attrib_types & SceneGraphReducer::TT_color) != 0) {
    if (_color != (RenderAttrib *)NULL) {
      const RenderAttrib *node_attrib =
        node->get_attrib(ColorAttrib::get_class_type());
      if (node_attrib != (RenderAttrib *)NULL) {
        node->set_attrib(_color->compose(node_attrib));
      } else {
        node->set_attrib(_color);
      }
      _color = (RenderAttrib *)NULL;
    }
  }

  if ((attrib_types & SceneGraphReducer::TT_color_scale) != 0) {
    if (_color_scale != (RenderAttrib *)NULL) {
      const RenderAttrib *node_attrib =
        node->get_attrib(ColorScaleAttrib::get_class_type());
      if (node_attrib != (RenderAttrib *)NULL) {
        node->set_attrib(_color_scale->compose(node_attrib));
      } else {
        node->set_attrib(_color_scale);
      }
      _color_scale = (RenderAttrib *)NULL;
    }
  }

  if ((attrib_types & SceneGraphReducer::TT_tex_matrix) != 0) {
    if (_tex_matrix != (RenderAttrib *)NULL) {
      const RenderAttrib *node_attrib =
        node->get_attrib(TexMatrixAttrib::get_class_type());
      if (node_attrib != (RenderAttrib *)NULL) {
        node->set_attrib(_tex_matrix->compose(node_attrib));
      } else {
        node->set_attrib(_tex_matrix);
      }
      _tex_matrix = (RenderAttrib *)NULL;
    }
  }

  if ((attrib_types & SceneGraphReducer::TT_other) != 0) {
    node->set_state(_other->compose(node->get_state()));
    _other = RenderState::make_empty();
  }
}
