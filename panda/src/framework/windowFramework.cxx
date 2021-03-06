// Filename: windowFramework.cxx
// Created by:  drose (02Apr02)
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

#include "windowFramework.h"
#include "pandaFramework.h"
#include "displayRegion.h"
#include "buttonThrower.h"
#include "transform2sg.h"
#include "dSearchPath.h"
#include "filename.h"
#include "loader.h"
#include "keyboardButton.h"
#include "geomTri.h"
#include "texturePool.h"
#include "textureAttrib.h"
#include "perspectiveLens.h"
#include "orthographicLens.h"
#include "auto_bind.h"
#include "ambientLight.h"
#include "directionalLight.h"
#include "lightAttrib.h"
#include "boundingSphere.h"
#include "deg_2_rad.h"
#include "config_framework.h"
#include "depthTestAttrib.h"
#include "depthWriteAttrib.h"
#include "cullFaceAttrib.h"
#include "rescaleNormalAttrib.h"
#include "pgTop.h"
#include "geomNode.h"
#include "geomTristrip.h"
#include "texture.h"
#include "pnmImage.h"
#include "loaderFileTypeRegistry.h"
#include "pnmFileTypeRegistry.h"
#include "pnmImage.h"

// This is generated data for the standard texture we apply to the
// blue triangle.
#include "rock_floor_src.cxx"

// This number is chosen arbitrarily to override any settings in model
// files.
static const int override_priority = 100;

TypeHandle WindowFramework::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::Constructor
//       Access: Protected
//  Description: 
////////////////////////////////////////////////////////////////////
WindowFramework::
WindowFramework(PandaFramework *panda_framework) :
  _panda_framework(panda_framework)
{
  _alight = (AmbientLight *)NULL;
  _dlight = (DirectionalLight *)NULL;
  _got_keyboard = false;
  _got_trackball = false;
  _got_lights = false;
  _wireframe_enabled = false;
  _texture_enabled = true;
  _two_sided_enabled = false;
  _one_sided_reverse_enabled = false;
  _lighting_enabled = false;
  _background_type = BT_default;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::Copy Constructor
//       Access: Protected
//  Description: 
////////////////////////////////////////////////////////////////////
WindowFramework::
WindowFramework(const WindowFramework &copy, DisplayRegion *display_region) :
  _panda_framework(copy._panda_framework),
  _window(copy._window),
  _display_region_3d(display_region)
{
  _alight = (AmbientLight *)NULL;
  _dlight = (DirectionalLight *)NULL;
  _got_keyboard = false;
  _got_trackball = false;
  _got_lights = false;
  _wireframe_enabled = false;
  _texture_enabled = true;
  _two_sided_enabled = false;
  _one_sided_reverse_enabled = false;
  _lighting_enabled = false;
  _background_type = BT_default;

  set_background_type(copy._background_type);
  // Set up a 3-d camera for the window by default.
  make_camera();
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::Destructor
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
WindowFramework::
~WindowFramework() {
  close_window();
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::open_window
//       Access: Protected
//  Description: Opens the actual window.  This is normally called
//               only from PandaFramework::open_window().
////////////////////////////////////////////////////////////////////
GraphicsWindow *WindowFramework::
open_window(const WindowProperties &props, GraphicsEngine *engine,
            GraphicsPipe *pipe, GraphicsStateGuardian *gsg) {
  nassertr(_window == (GraphicsWindow *)NULL, _window);

  PT(GraphicsStateGuardian) ptgsg = gsg;

  // If we were not given a gsg in the arguments, create a new one
  // just for this window.
  if (ptgsg == (GraphicsStateGuardian *)NULL) {
    ptgsg = engine->make_gsg(pipe);
    if (ptgsg == (GraphicsStateGuardian *)NULL) {
      // No GSG, no window.
      framework_cat.fatal() << "open_window: failed to create gsg object!\n";
      return NULL;
    }
  }

  static int next_window_index = 1;
  ostringstream stream;
  stream << "window" << next_window_index;
  next_window_index++;
  string name = stream.str();

  _window = engine->make_window(ptgsg, name, 0);
  if (_window != (GraphicsWindow *)NULL) {
    _window->request_properties(props);
    
    // Create a display region that covers the entire window.
    _display_region_3d = _window->make_display_region();

    // Make sure the DisplayRegion does the clearing, not the window,
    // so we can have multiple DisplayRegions of different colors.
    _window->set_clear_color_active(false);
    _window->set_clear_depth_active(false);
    set_background_type(_background_type);

    // Set up a 3-d camera for the window by default.
    make_camera();
    
    if (show_frame_rate_meter) {
      _frame_rate_meter = new FrameRateMeter("frame_rate_meter");
      _frame_rate_meter->setup_window(_window);
    }
  }

  return _window;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::close_window
//       Access: Protected
//  Description: Closes the window.  This is normally called
//               from PandaFramework::close_window().
////////////////////////////////////////////////////////////////////
void WindowFramework::
close_window() {
  _window.clear();
  _camera_group.remove_node();
  _render.remove_node();
  _render_2d.remove_node();
  _mouse.remove_node();

  _alight = (AmbientLight *)NULL;
  _dlight = (DirectionalLight *)NULL;
  _got_keyboard = false;
  _got_trackball = false;
  _got_lights = false;

  _wireframe_enabled = false;
  _texture_enabled = true;
  _two_sided_enabled = false;
  _one_sided_reverse_enabled = false;
  _lighting_enabled = false;

  _frame_rate_meter = (FrameRateMeter *)NULL;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::get_camera_group
//       Access: Public
//  Description: Returns the node above the collection of 3-d cameras
//               in the scene graph.  This node may be moved around to
//               represent the viewpoint.
////////////////////////////////////////////////////////////////////
const NodePath &WindowFramework::
get_camera_group() {
  if (_camera_group.is_empty()) {
    _camera_group = get_render().attach_new_node("camera_group");
  }
  return _camera_group;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::get_render
//       Access: Public
//  Description: Returns the root of the 3-d scene graph.
////////////////////////////////////////////////////////////////////
const NodePath &WindowFramework::
get_render() {
  if (_render.is_empty()) {
    _render = NodePath("render");

    _render.node()->set_attrib(RescaleNormalAttrib::make_default());

    // This is maybe here temporarily, and maybe not.
    _render.set_two_sided(0);
  }
  return _render;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::get_render_2d
//       Access: Public
//  Description: Returns the root of the 2-d scene graph.
////////////////////////////////////////////////////////////////////
const NodePath &WindowFramework::
get_render_2d() {
  if (_render_2d.is_empty()) {
    _render_2d = NodePath("render_2d");

    // Some standard properties for the 2-d display.

    // It's particularly important to turn off the depth test,
    // since we'll be keeping the same depth buffer already filled
    // by the previously-drawn 3-d scene--we don't want to pay for
    // a clear operation, but we also don't want to collide with
    // that depth buffer.
    CPT(RenderAttrib) dt = DepthTestAttrib::make(DepthTestAttrib::M_none);
    CPT(RenderAttrib) dw = DepthWriteAttrib::make(DepthWriteAttrib::M_off);
    _render_2d.node()->set_attrib(dt, 1);
    _render_2d.node()->set_attrib(dw, 1);

    _render_2d.set_material_off(1);
    _render_2d.set_two_sided(1, 1);

    // Now set up a 2-d camera to view render_2d.
    
    // Create a display region that matches the size of the 3-d
    // display region.
    float l, r, b, t;
    _display_region_3d->get_dimensions(l, r, b, t);
    _display_region_2d = _window->make_display_region(l, r, b, t);
    
    // Finally, we need a camera to associate with the display region.
    PT(Camera) camera = new Camera("camera2d");
    NodePath camera_np = _render_2d.attach_new_node(camera);
    
    PT(Lens) lens = new OrthographicLens;

    static const float left = -1.0f;
    static const float right = 1.0f;
    static const float bottom = -1.0f;
    static const float top = 1.0f;
    lens->set_film_size(right - left, top - bottom);
    lens->set_film_offset((right + left) * 0.5, (top + bottom) * 0.5);
    lens->set_near_far(-1000, 1000);

    camera->set_lens(lens);
    _display_region_2d->set_camera(camera_np);
  }

  return _render_2d;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::get_aspect_2d
//       Access: Public
//  Description: Returns the node under the 2-d scene graph that is
//               scaled to suit the window's aspect ratio.
////////////////////////////////////////////////////////////////////
const NodePath &WindowFramework::
get_aspect_2d() {
  if (_aspect_2d.is_empty()) {
    PGTop *top = new PGTop("aspect_2d");
    _aspect_2d = get_render_2d().attach_new_node(top);

    // Tell the PGTop about our MouseWatcher object, so the PGui
    // system can operate.
    PandaNode *mouse_node = get_mouse().node();
    if (mouse_node->is_of_type(MouseWatcher::get_class_type())) {
      top->set_mouse_watcher(DCAST(MouseWatcher, mouse_node));
    }

    float this_aspect_ratio = aspect_ratio;
    if (this_aspect_ratio == 0.0f) {
      // An aspect ratio of 0.0 means to try to infer it.
      this_aspect_ratio = 1.0f;

      WindowProperties properties = _window->get_properties();
      if (!properties.has_size()) {
        properties = _window->get_requested_properties();
      }
      if (properties.has_size() && properties.get_y_size() != 0.0f) {
        this_aspect_ratio = 
          (float)properties.get_x_size() / (float)properties.get_y_size();
      }
    }

    _aspect_2d.set_scale(1.0f / this_aspect_ratio, 1.0f, 1.0f);
  }

  return _aspect_2d;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::get_mouse
//       Access: Public
//  Description: Returns the node in the data graph corresponding to
//               the mouse associated with this window.
////////////////////////////////////////////////////////////////////
const NodePath &WindowFramework::
get_mouse() {
  if (_mouse.is_empty()) {
    NodePath mouse = _panda_framework->get_mouse(_window);

    // Create a MouseWatcher to filter the mouse input.  We do this
    // mainly so we can constrain the mouse input to our particular
    // display region, if we have one.  This means the node we return
    // from get_mouse() is actually a MouseWatcher, but since it
    // presents the same interface as a Mouse, no one should mind.

    // Another advantage to using a MouseWatcher is that it the PGTop
    // of aspect2d likes it better.
    PT(MouseWatcher) mw = new MouseWatcher("watcher");
    mw->set_display_region(_display_region_3d);
    _mouse = mouse.attach_new_node(mw);
  }
  return _mouse;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::enable_keyboard
//       Access: Public
//  Description: Creates a ButtonThrower to listen to button presses
//               and throw them as events.
////////////////////////////////////////////////////////////////////
void WindowFramework::
enable_keyboard() {
  if (_got_keyboard) {
    return;
  }

  if (_window->get_num_input_devices() > 0) {
    NodePath mouse = get_mouse();

    // Create a button thrower to listen for our keyboard events and
    // associate this WindowFramework pointer with each one.
    PT(ButtonThrower) bt = new ButtonThrower("kb-events");
    bt->add_parameter(EventParameter(this));
    ModifierButtons mods;
    mods.add_button(KeyboardButton::shift());
    mods.add_button(KeyboardButton::control());
    mods.add_button(KeyboardButton::alt());
    bt->set_modifier_buttons(mods);
    mouse.attach_new_node(bt);
  }

  _got_keyboard = true;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::setup_trackball
//       Access: Public
//  Description: Sets up the mouse to trackball around the camera.
////////////////////////////////////////////////////////////////////
void WindowFramework::
setup_trackball() {
  if (_got_trackball) {
    return;
  }

  if (_window->get_num_input_devices() > 0) {
    NodePath mouse = get_mouse();
    NodePath camera = get_camera_group();

    _trackball = new Trackball("trackball");
    _trackball->set_pos(LVector3f::forward() * 50.0);
    mouse.attach_new_node(_trackball);
    
    PT(Transform2SG) tball2cam = new Transform2SG("tball2cam");
    tball2cam->set_node(camera.node());
    _trackball->add_child(tball2cam);
  }

  _got_trackball = true;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::center_trackball
//       Access: Public
//  Description: Centers the trackball on the indicated object, and
//               scales the trackball motion suitably.
////////////////////////////////////////////////////////////////////
void WindowFramework::
center_trackball(const NodePath &object) {
  if (_trackball == (Trackball *)NULL) {
    return;
  }

  PT(BoundingVolume) volume = object.get_bounds();
  // We expect at least a geometric bounding volume around the world.
  nassertv(volume != (BoundingVolume *)NULL);
  nassertv(volume->is_of_type(GeometricBoundingVolume::get_class_type()));
  GeometricBoundingVolume *gbv = DCAST(GeometricBoundingVolume, volume);
  
  // Determine the bounding sphere around the world.  The topmost
  // BoundingVolume might itself be a sphere (it's likely), but since
  // it might not, we'll take no chances and make our own sphere.
  PT(BoundingSphere) sphere = new BoundingSphere;
  if (!sphere->extend_by(gbv)) {
    framework_cat.warning()
      << "Cannot determine bounding volume of " << object << "\n";
    return;
  }

  if (sphere->is_infinite()) {
    framework_cat.warning()
      << "Infinite bounding volume for " << object << "\n";
    return;
  }

  if (sphere->is_empty()) {
    framework_cat.warning()
      << "Empty bounding volume for " << object << "\n";
    return;
  }

  LPoint3f center = sphere->get_center();
  float radius = sphere->get_radius();

  float distance = 50.0f;

  // Choose a suitable distance to view the whole volume in our frame.
  // This is based on the camera lens in use.  Determine the lens
  // based on the first camera; this will be the default camera.
  Lens *lens = (Lens *)NULL;
  if (!_cameras.empty()) {
    Cameras::const_iterator ci;
    for (ci = _cameras.begin();
         ci != _cameras.end() && lens == (Lens *)NULL;
         ++ci) {
      lens = (*ci)->get_lens();
    }
  }

  if (lens != (Lens *)NULL) {
    LVecBase2f fov = lens->get_fov();
    distance = radius / ctan(deg_2_rad(min(fov[0], fov[1]) / 2.0f));

    // Ensure the far plane is far enough back to see the entire object.
    float ideal_far_plane = distance + radius;
    lens->set_far(max(lens->get_default_far(), ideal_far_plane)); 

    // And that the near plane is far enough forward.
    float ideal_near_plane = distance - radius;
    lens->set_near(min(lens->get_default_near(), ideal_near_plane)); 
  }

  _trackball->set_origin(center);
  _trackball->set_pos(LVector3f::forward() * distance);

  // Also set the movement scale on the trackball to be consistent
  // with the size of the model and the lens field-of-view.
  _trackball->set_forward_scale(distance * 0.006);
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::load_models
//       Access: Public
//  Description: Loads up all the model files listed in the indicated
//               argument list.  If first_arg is supplied, it is the
//               first argument in the list to consider.
//
//               Returns true if all models loaded successfully, or
//               false if at least one of them had an error.
////////////////////////////////////////////////////////////////////
bool WindowFramework::
load_models(const NodePath &parent, int argc, char *argv[], int first_arg) {
  pvector<Filename> files;

  for (int i = first_arg; i < argc && argv[i] != (char *)NULL; i++) {
    files.push_back(Filename::from_os_specific(argv[i]));
  }

  return load_models(parent, files);
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::load_models
//       Access: Public
//  Description: Loads up all the model files listed in the indicated
//               argument list.
//
//               Returns true if all models loaded successfully, or
//               false if at least one of them had an error.
////////////////////////////////////////////////////////////////////
bool WindowFramework::
load_models(const NodePath &parent, const pvector<Filename> &files) {
  bool all_ok = true;

  pvector<Filename>::const_iterator fi;
  for (fi = files.begin(); fi != files.end(); ++fi) {
    const Filename &filename = (*fi);
    NodePath model = load_model(parent, filename);
    if (model.is_empty()) {
      all_ok = false;
    }
  }

  return all_ok;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::load_model
//       Access: Public
//  Description: Loads up the indicated model and returns the new
//               NodePath, or the empty NodePath if the model could
//               not be loaded.
////////////////////////////////////////////////////////////////////
NodePath WindowFramework::
load_model(const NodePath &parent, Filename filename) {
  nout << "Loading " << filename << "\n";

  // If the filename already exists where it is, or if it is fully
  // qualified, don't search along the model path for it.
  bool search = !(filename.is_fully_qualified() || filename.exists());

  // We allow loading image files here.  Check to see if it might be
  // an image file, based on the filename extension.
  bool is_image = false;
  string extension = filename.get_extension();
  if (!extension.empty()) {
    LoaderFileTypeRegistry *reg = LoaderFileTypeRegistry::get_global_ptr();
    LoaderFileType *model_type =
      reg->get_type_from_extension(extension);
    if (model_type == (LoaderFileType *)NULL) {
      // The extension isn't a known model file type, is it a known
      // image extension?
      PNMFileTypeRegistry *reg = PNMFileTypeRegistry::get_global_ptr();
      PNMFileType *image_type =
        reg->get_type_from_extension(extension);
      if (image_type != (PNMFileType *)NULL) {
        // It is a known image extension.
        is_image = true;
      }
    }
  }
  
  Loader loader;
  PT(PandaNode) node;
  if (is_image) {
    node = load_image_as_model(filename);
  } else {
    node = loader.load_sync(filename, search);
  }

  if (node == (PandaNode *)NULL) {
    nout << "Unable to load " << filename << "\n";
    return NodePath::not_found();
  }    

  return parent.attach_new_node(node);
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::load_default_model
//       Access: Public
//  Description: Loads our favorite blue triangle.  This is intended
//               to provide some default geometry to have *something*
//               to look at for testing, when no other models are
//               provided.
////////////////////////////////////////////////////////////////////
NodePath WindowFramework::
load_default_model(const NodePath &parent) {
  PTA_Vertexf coords;
  PTA_TexCoordf uvs;
  PTA_Normalf norms;
  PTA_Colorf colors;
  PTA_ushort cindex;
  
  coords.push_back(Vertexf::rfu(0.0, 0.0, 0.0));
  coords.push_back(Vertexf::rfu(1.0, 0.0, 0.0));
  coords.push_back(Vertexf::rfu(0.0, 0.0, 1.0));
  uvs.push_back(TexCoordf(0.0, 0.0));
  uvs.push_back(TexCoordf(1.0, 0.0));
  uvs.push_back(TexCoordf(0.0, 1.0));
  norms.push_back(Normalf::back());
  colors.push_back(Colorf(0.5, 0.5, 1.0, 1.0));
  cindex.push_back(0);
  cindex.push_back(0);
  cindex.push_back(0);
  
  PT(GeomTri) geom = new GeomTri;
  geom->set_num_prims(1);
  geom->set_coords(coords);
  geom->set_texcoords(uvs, G_PER_VERTEX);
  geom->set_normals(norms, G_PER_PRIM);
  geom->set_colors(colors, G_PER_VERTEX, cindex);

  CPT(RenderState) state = RenderState::make_empty();

  // Get the default texture to apply to the triangle; it's compiled
  // into the code these days.
  string rock_floor_string((const char *)rock_floor, rock_floor_len);
  istringstream rock_floor_strm(rock_floor_string);
  PNMImage rock_floor_pnm;
  if (rock_floor_pnm.read(rock_floor_strm, "rock-floor.rgb")) {
    PT(Texture) tex = new Texture;
    tex->set_name("rock-floor.rgb");
    tex->load(rock_floor_pnm);
    tex->set_minfilter(Texture::FT_linear);
    tex->set_magfilter(Texture::FT_linear);
    state = state->add_attrib(TextureAttrib::make(tex));
  }
  
  GeomNode *geomnode = new GeomNode("tri");
  geomnode->add_geom(geom, state);

  return parent.attach_new_node(geomnode);
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::loop_animations
//       Access: Public
//  Description: Looks for characters and their matching animation
//               files in the scene graph; binds and loops any
//               matching animations found.
////////////////////////////////////////////////////////////////////
void WindowFramework::
loop_animations() {
  // If we happened to load up both a character file and its matching
  // animation file, attempt to bind them together now and start the
  // animations looping.
  auto_bind(get_render().node(), _anim_controls, ~0);
  _anim_controls.loop_all(true);
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::split_window
//       Access: Public
//  Description: Divides the window into two display regions, each of
//               which gets its own trackball and keyboard events.
//               The new window pointer is returned.
//
//               There is not an interface for recombining divided
//               windows.
////////////////////////////////////////////////////////////////////
WindowFramework *WindowFramework::
split_window(SplitType split_type) {
  DisplayRegion *new_region = NULL;

  if (split_type == ST_default) {
    // Choose either horizontal or vertical according to the largest
    // dimension.
    
    if (_display_region_3d->get_pixel_width() > 
        _display_region_3d->get_pixel_height()) {
      split_type = ST_horizontal;
    } else {
      split_type = ST_vertical;
    }
  }
  
  float left, right, bottom, top;
  _display_region_3d->get_dimensions(left, right, bottom, top);
  new_region = _display_region_3d->get_window()->make_display_region();

  if (split_type == ST_vertical) {
    _display_region_3d->set_dimensions(left, right, bottom, (top + bottom) / 2.0f);
    if (_display_region_2d != (DisplayRegion *)NULL) {
      _display_region_2d->set_dimensions(left, right, bottom, (top + bottom) / 2.0f);
    }
      
    new_region->set_dimensions(left, right, (top + bottom) / 2.0f, top);
    
  } else {
    _display_region_3d->set_dimensions(left, (left + right) / 2.0f, bottom, top);
    if (_display_region_2d != (DisplayRegion *)NULL) {
      _display_region_2d->set_dimensions(left, (left + right) / 2.0f, bottom, top);
    }

    new_region->set_dimensions((left + right) / 2.0f, right, bottom, top);
  }

  PT(WindowFramework) wf = new WindowFramework(*this, new_region);
  _panda_framework->_windows.push_back(wf);

  return wf;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::set_wireframe
//       Access: Public
//  Description: Forces wireframe state (true) or restores default
//               rendering (false).
////////////////////////////////////////////////////////////////////
void WindowFramework::
set_wireframe(bool enable) {
  if (enable == _wireframe_enabled) {
    return;
  }

  NodePath render = get_render();

  if (enable) {
    render.set_render_mode_wireframe(override_priority);
    render.set_two_sided(true, override_priority);
  } else {
    render.clear_render_mode();
    if (!_two_sided_enabled) {
      render.clear_two_sided();
    }
    if (_one_sided_reverse_enabled) {
      CPT(RenderAttrib) attrib = CullFaceAttrib::make_reverse();
      render.node()->set_attrib(attrib);
    }
  }

  _wireframe_enabled = enable;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::set_texture
//       Access: Public
//  Description: Forces textures off (false) or restores default
//               rendering (true).
////////////////////////////////////////////////////////////////////
void WindowFramework::
set_texture(bool enable) {
  if (enable == _texture_enabled) {
    return;
  }

  NodePath render = get_render();

  if (!enable) {
    render.set_texture_off(override_priority);
  } else {
    render.clear_texture();
  }

  _texture_enabled = enable;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::set_two_sided
//       Access: Public
//  Description: Forces two-sided rendering (true) or restores default
//               rendering (false).
////////////////////////////////////////////////////////////////////
void WindowFramework::
set_two_sided(bool enable) {
  if (enable == _two_sided_enabled) {
    return;
  }

  NodePath render = get_render();

  if (enable) {
    render.set_two_sided(true, override_priority);
  } else {
    if (!_wireframe_enabled) {
      render.clear_two_sided();
    }
  }

  _two_sided_enabled = enable;
  _one_sided_reverse_enabled = false;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::set_one_sided_reverse
//       Access: Public
//  Description: Toggles one-sided reverse mode.  In this mode, the
//               front sides of one-sided polygons are culled instead
//               of the back side.
////////////////////////////////////////////////////////////////////
void WindowFramework::
set_one_sided_reverse(bool enable) {
  if (enable == _one_sided_reverse_enabled) {
    return;
  }

  NodePath render = get_render();

  if (!_wireframe_enabled) {
    if (enable) {
      CPT(RenderAttrib) attrib = CullFaceAttrib::make_reverse();
      render.node()->set_attrib(attrib);
    } else {
      render.clear_two_sided();
    }
  }

  _two_sided_enabled = false;
  _one_sided_reverse_enabled = enable;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::set_lighting
//       Access: Public
//  Description: Turns lighting on (true) or off (false).
////////////////////////////////////////////////////////////////////
void WindowFramework::
set_lighting(bool enable) {
  if (enable == _lighting_enabled) {
    return;
  }

  NodePath render = get_render();

  if (enable) {
    if (!_got_lights) {
      setup_lights();
    }
    render.node()->set_attrib(LightAttrib::make(LightAttrib::O_add, 
                                                _alight, _dlight));
  } else {
    render.node()->clear_attrib(LightAttrib::get_class_type());
  }

  _lighting_enabled = enable;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::set_background_type
//       Access: Public
//  Description: Sets the background of the window to one of the
//               pre-canned background types (or to BT_other, which
//               indicates the user intends to set up his own special
//               background mode).
////////////////////////////////////////////////////////////////////
void WindowFramework::
set_background_type(WindowFramework::BackgroundType type) {
  _background_type = type;

  if (_display_region_3d == (DisplayRegion *)NULL) {
    return;
  }

  switch (_background_type) {
  case BT_other:
    break;

  case BT_default:
    _display_region_3d->set_clear_color_active(true);
    _display_region_3d->set_clear_depth_active(true);
    _display_region_3d->set_clear_color(_window->get_clear_color());
    _display_region_3d->set_clear_depth(_window->get_clear_depth());
    break;
    
  case BT_black:
    _display_region_3d->set_clear_color_active(true);
    _display_region_3d->set_clear_depth_active(true);
    _display_region_3d->set_clear_color(Colorf(0.0f, 0.0f, 0.0f, 1.0f));
    _display_region_3d->set_clear_depth(1.0f);
    break;
    
  case BT_gray:
    _display_region_3d->set_clear_color_active(true);
    _display_region_3d->set_clear_depth_active(true);
    _display_region_3d->set_clear_color(Colorf(0.3f, 0.3f, 0.3f, 1.0f));
    _display_region_3d->set_clear_depth(1.0f);
    break;
    
  case BT_white:
    _display_region_3d->set_clear_color_active(true);
    _display_region_3d->set_clear_depth_active(true);
    _display_region_3d->set_clear_color(Colorf(1.0f, 1.0f, 1.0f, 1.0f));
    _display_region_3d->set_clear_depth(1.0f);
    break;

  case BT_none:
    _display_region_3d->set_clear_color_active(false);
    _display_region_3d->set_clear_depth_active(false);
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::make_camera
//       Access: Protected
//  Description: Makes a new 3-d camera for the window.
////////////////////////////////////////////////////////////////////
PT(Camera) WindowFramework::
make_camera() {
  // Finally, we need a camera to associate with the display region.
  PT(Camera) camera = new Camera("camera");
  NodePath camera_np = get_camera_group().attach_new_node(camera);
  _cameras.push_back(camera);

  PT(Lens) lens = new PerspectiveLens;

  if (aspect_ratio != 0.0f) {
    // If we're given an explict aspect ratio, use it
    lens->set_aspect_ratio(aspect_ratio);

  } else {
    // Otherwise, infer the aspect ratio from the window size.  This
    // does assume we have square pixels on our output device.
    WindowProperties properties = _window->get_properties();
    if (!properties.has_size()) {
      properties = _window->get_requested_properties();
    }
    if (properties.has_size()) {
      lens->set_film_size(properties.get_x_size(), properties.get_y_size());
    }
  }

  camera->set_lens(lens);
  _display_region_3d->set_camera(camera_np);

  return camera;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::setup_lights
//       Access: Protected
//  Description: Makes light nodes and attaches them to the camera for
//               viewing the scene.
////////////////////////////////////////////////////////////////////
void WindowFramework::
setup_lights() {
  if (_got_lights) {
    return;
  }

  NodePath camera_group = get_camera_group();
  NodePath light_group = camera_group.attach_new_node("lights");

  _alight = new AmbientLight("ambient");
  _alight->set_color(Colorf(0.2f, 0.2f, 0.2f, 1.0f));
  _dlight = new DirectionalLight("directional");
  light_group.attach_new_node(_alight);
  light_group.attach_new_node(_dlight);
  
  _got_lights = true;
}

////////////////////////////////////////////////////////////////////
//     Function: WindowFramework::load_image_as_model
//       Access: Private
//  Description: Loads the indicated image file as a texture, and
//               creates a polygon to render it.  Returns the new
//               model.
////////////////////////////////////////////////////////////////////
PT(PandaNode) WindowFramework::
load_image_as_model(const Filename &filename) {
  PNMImageHeader header;
  if (!header.read_header(filename)) {
    return NULL;
  }

  int x_size = header.get_x_size();
  int y_size = header.get_y_size();
  bool has_alpha = header.has_alpha();

  // Yes, it is an image file; make a texture out of it.
  PT(Texture) tex = new Texture;
  if (!tex->read(filename)) {
    return NULL;
  }
  tex->set_minfilter(Texture::FT_linear_mipmap_linear);
  tex->set_magfilter(Texture::FT_linear);

  // Ok, now make a polygon to show the texture.
  PT(GeomNode) card_geode = new GeomNode("card");

  // Choose the dimensions of the polygon appropriately.
  float left = -x_size / 2.0;
  float right = x_size / 2.0;
  float bottom = -y_size / 2.0;
  float top = y_size / 2.0;

  GeomTristrip *geoset = new GeomTristrip;
  PTA_int lengths=PTA_int::empty_array(0);
  lengths.push_back(4);

  PTA_Vertexf verts;
  verts.push_back(Vertexf::rfu(left, 0.02f, top));
  verts.push_back(Vertexf::rfu(left, 0.02f, bottom));
  verts.push_back(Vertexf::rfu(right, 0.02f, top));
  verts.push_back(Vertexf::rfu(right, 0.02f, bottom));

  geoset->set_num_prims(1);
  geoset->set_lengths(lengths);

  geoset->set_coords(verts);

  PTA_TexCoordf uvs;
  uvs.push_back(TexCoordf(0.0f, 1.0f));
  uvs.push_back(TexCoordf(0.0f, 0.0f));
  uvs.push_back(TexCoordf(1.0f, 1.0f));
  uvs.push_back(TexCoordf(1.0f, 0.0f));
  
  geoset->set_texcoords(uvs, G_PER_VERTEX);

  card_geode->add_geom(geoset);
  card_geode->set_attrib(TextureAttrib::make(tex));
  if (has_alpha) {
    card_geode->set_attrib(TransparencyAttrib::make(TransparencyAttrib::M_alpha));
  }

  return card_geode.p();
}
