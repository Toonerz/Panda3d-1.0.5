// Filename: dxGraphicsStateGuardian8.I
// Created by:   masad (02Jan04)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2004, Disney Enterprises, Inc.  All rights reserved
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

INLINE DWORD
Colorf_to_D3DCOLOR(const Colorf &cColorf) {
// MS VC defines _M_IX86 for x86.  gcc should define _X86_
#if defined(_M_IX86) || defined(_X86_)
    DWORD d3dcolor,tempcolorval=255;

    // note the default FPU rounding mode will give 255*0.5f=0x80, not 0x7F as VC would force it to by resetting rounding mode
    // dont think this makes much difference

    __asm {
        push ebx   ; want to save this in case this fn is inlined
        push ecx
        mov ecx, cColorf
        fild tempcolorval
        fld DWORD PTR [ecx]
        fmul ST(0),ST(1)
        fistp tempcolorval  ; no way to store directly to int register
        mov eax, tempcolorval
        shl eax, 16

        fld DWORD PTR [ecx+4]  ;grn
        fmul ST(0),ST(1)
        fistp tempcolorval
        mov ebx,tempcolorval
        shl ebx, 8
        or eax,ebx

        fld DWORD PTR [ecx+8]  ;blue
        fmul ST(0),ST(1)
        fistp tempcolorval
        or eax,tempcolorval

        fld DWORD PTR [ecx+12] ;alpha
        fmul ST(0),ST(1)
        fistp tempcolorval
        ; simulate pop 255.0 off FP stack w/o store, mark top as empty and increment stk ptr
        ffree ST(0)
        fincstp
        mov ebx,tempcolorval
        shl ebx, 24
        or eax,ebx
        mov d3dcolor,eax
        pop ecx
        pop ebx
    }

   //   dxgsg9_cat.debug() << (void*)d3dcolor << endl;
   return d3dcolor;
#else //!_X86_
   return MY_D3DRGBA(cColorf[0], cColorf[1], cColorf[2], cColorf[3]);
#endif //!_X86_
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::enable_line_smooth
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
enable_line_smooth(bool val) {
  if(_line_smooth_enabled != val) {
    _line_smooth_enabled = val;
  #ifndef NDEBUG
    {
        if(val && (_pScrn->d3dcaps.LineCaps & D3DLINECAPS_ANTIALIAS))
           dxgsg9_cat.error() << "no HW support for line smoothing!!\n";
    }
  #endif

    _pD3DDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, (DWORD)val);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::enable_dither
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
enable_dither(bool val) {
  if (_dither_enabled != val) {
    _dither_enabled = val;

  #ifndef NDEBUG
    {
        if(val && !(_pScrn->d3dcaps.RasterCaps & D3DPRASTERCAPS_DITHER))
           dxgsg9_cat.error() << "no HW support for color dithering!!\n";
    }
  #endif

    _pD3DDevice->SetRenderState(D3DRS_DITHERENABLE, (DWORD)val);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::enable_stencil_test
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
enable_stencil_test(bool val) {
  if (_stencil_test_enabled != val) {
    _stencil_test_enabled = val;
    _pD3DDevice->SetRenderState(D3DRS_STENCILENABLE, (DWORD)val);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::enable_color_material
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
enable_color_material(bool val) {
  if (_color_material_enabled != val) {
    _color_material_enabled = val;
  }
}


////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::enable_blend
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
enable_blend(bool val) {
  if (_blend_enabled != val) {
    _blend_enabled = val;
    _pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)val);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::set_color_writemask
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
set_color_writemask(UINT color_writemask) {
  if (_color_writemask != color_writemask) {
    _color_writemask = color_writemask;
    if(_pScrn->bCanDirectDisableColorWrites) {
        // only newer HW supports this rstate
        _pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)color_writemask);
    } else {
        // blending can only handle on/off
        assert((color_writemask==0x0)||(color_writemask==0xFFFFFFFF));
        set_blend_mode(_color_write_mode, _color_blend_mode, _transparency_mode);        
    }
  }
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::enable_blend
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
enable_gouraud_shading(bool val) {
  if (_bGouraudShadingOn != val) {
    _bGouraudShadingOn = val;
    _pD3DDevice->SetRenderState(D3DRS_SHADEMODE, (val ? D3DSHADE_GOURAUD : D3DSHADE_FLAT));
  }
}

INLINE void DXGraphicsStateGuardian9::
enable_primitive_clipping(bool val) {
  if (_clipping_enabled != val) {
    _clipping_enabled = val;
    _pD3DDevice->SetRenderState(D3DRS_CLIPPING, (DWORD)val);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::enable_fog
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
enable_fog(bool val) {
  if ((_fog_enabled != val) && (_doFogType!=None)) {
    _fog_enabled = val;
    _pD3DDevice->SetRenderState(D3DRS_FOGENABLE, (DWORD)val);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::set_vertex_format
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
set_vertex_format(DWORD NewFvfType) {
#ifdef USE_VERTEX_SHADERS
  if(_CurVertexShader!=NULL) {
    // this needs optimization
    HRESULT hr = _pD3DDevice->SetVertexShader(_CurVertexShader);
    #ifndef NDEBUG
    if(FAILED(hr)) {
           dxgsg9_cat.error() << "SetVertexShader for custom vtx shader failed" << D3DERRORSTRING(hr);
           exit(1);
    }
    #endif
    _CurFVFType = NewFvfType;    
    return;
  }
#endif

  if (_CurFVFType != NewFvfType) {
    _CurFVFType = NewFvfType;

 HRESULT hr = _pD3DDevice->SetVertexShader(NULL);
 hr = _pD3DDevice->SetFVF(NewFvfType);

 //    HRESULT hr = _pD3DDevice->SetVertexShader((IDirect3DVertexShader9*)NewFvfType);
 #ifndef NDEBUG
    if(FAILED(hr)) {
           dxgsg9_cat.error() << "SetVertexShader(0x" << (void*)NewFvfType<<") failed" << D3DERRORSTRING(hr);
           exit(1);
    }
 #endif
  }
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::enable_alpha_test
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
enable_alpha_test(bool val )
{
  if (_alpha_test_enabled != val) {
    _alpha_test_enabled = val;
    _pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, (DWORD)val);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::call_dxLightModelAmbient
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
call_dxLightModelAmbient( const Colorf& color)
{
  if (_lmodel_ambient != color) {
    _lmodel_ambient = color;
#ifdef GSG_VERBOSE
    dxgsg9_cat.debug() << "dxLightModel(LIGHT_MODEL_AMBIENT, " << color << ")" << endl;
#endif
    _pD3DDevice->SetRenderState( D3DRS_AMBIENT,
                D3DCOLOR_COLORVALUE(color[0], color[1], color[2], color[3]));
  }
}


////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::call_dxAlphaFunc
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
call_dxAlphaFunc(D3DCMPFUNC func, float reference_alpha) {
  if (_alpha_func != func) {
    _alpha_func = func;
#ifdef GSG_VERBOSE
    dxgsg9_cat.debug() << "dxAlphaFunc(";
    switch (func) {
    case D3DCMP_NEVER:
      dxgsg9_cat.debug(false) << "D3DCMP_NEVER";
      break;
    case D3DCMP_LESS:
      dxgsg9_cat.debug(false) << "D3DCMP_LESS";
      break;
    case D3DCMP_EQUAL:
      dxgsg9_cat.debug(false) << "D3DCMP_EQUAL";
      break;
#ifdef D3DCMP_LEQUAL
    case D3DCMP_LEQUAL:
      dxgsg9_cat.debug(false) << "D3DCMP_LEQUAL";
      break;
#endif
    case D3DCMP_GREATER:
      dxgsg9_cat.debug(false) << "D3DCMP_GREATER";
      break;
    case D3DCMP_NOTEQUAL:
      dxgsg9_cat.debug(false) << "D3DCMP_NOTEQUAL";
      break;
#ifdef D3DCMP_GEQUAL
    case D3DCMP_GEQUAL:
      dxgsg9_cat.debug(false) << "D3DCMP_GEQUAL";
      break;
#endif
    case D3DCMP_ALWAYS:
      dxgsg9_cat.debug(false) << "D3DCMP_ALWAYS";
      break;
    }
    dxgsg9_cat.debug() << " , " << reference_alpha << ")" << endl;
#endif
    _pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, func);
  }

  if(_alpha_func_refval != reference_alpha) {
      _alpha_func_refval = reference_alpha;
      _pD3DDevice->SetRenderState(D3DRS_ALPHAREF, (UINT) (reference_alpha*255.0f));  //d3d uses 0x0-0xFF, not a float
  }
}


INLINE void DXGraphicsStateGuardian9::
call_dxBlendFunc(D3DBLEND sfunc, D3DBLEND dfunc )
{
  if (_blend_source_func != sfunc)
    {
    _blend_source_func = sfunc;
    _pD3DDevice->SetRenderState(D3DRS_SRCBLEND, sfunc);
#ifdef GSG_VERBOSE
    dxgsg9_cat.debug() << "dxSrcBlendFunc(";
    switch (sfunc)
        {
        case D3DBLEND_ZERO:
          dxgsg9_cat.debug(false) << "ZERO, ";
          break;
        case D3DBLEND_ONE:
          dxgsg9_cat.debug(false) << "ONE, ";
          break;
        case D3DBLEND_DESTCOLOR:
          dxgsg9_cat.debug(false) << "DESTCOLOR, ";
          break;
        case D3DBLEND_INVDESTCOLOR:
          dxgsg9_cat.debug(false) << "INVDESTCOLOR, ";
          break;
        case D3DBLEND_SRCALPHA:
          dxgsg9_cat.debug(false) << "SRCALPHA, ";
          break;
        case D3DBLEND_INVSRCALPHA:
          dxgsg9_cat.debug(false) << "INVSRCALPHA, ";
          break;
        case D3DBLEND_DESTALPHA:
          dxgsg9_cat.debug(false) << "DESTALPHA, ";
          break;
        case D3DBLEND_INVDESTALPHA:
          dxgsg9_cat.debug(false) << "INVDESTALPHA, ";
          break;
        case D3DBLEND_SRCALPHASAT:
          dxgsg9_cat.debug(false) << "SRCALPHASAT, ";
          break;
        default:
          dxgsg9_cat.debug(false) << "unknown, ";
          break;
        }
    dxgsg9_cat.debug(false) << endl;
#endif
    }
  if ( _blend_dest_func != dfunc)
    {
    _blend_dest_func = dfunc;
    _pD3DDevice->SetRenderState(D3DRS_DESTBLEND, dfunc);
#ifdef GSG_VERBOSE
    dxgsg9_cat.debug() << "dxDstBlendFunc(";
    switch (dfunc)
        {
        case D3DBLEND_ZERO:
          dxgsg9_cat.debug(false) << "ZERO, ";
          break;
        case D3DBLEND_ONE:
          dxgsg9_cat.debug(false) << "ONE, ";
          break;
        case D3DBLEND_DESTCOLOR:
          dxgsg9_cat.debug(false) << "DESTCOLOR, ";
          break;
        case D3DBLEND_INVDESTCOLOR:
          dxgsg9_cat.debug(false) << "INVDESTCOLOR, ";
          break;
        case D3DBLEND_SRCALPHA:
          dxgsg9_cat.debug(false) << "SRCALPHA, ";
          break;
        case D3DBLEND_INVSRCALPHA:
          dxgsg9_cat.debug(false) << "INVSRCALPHA, ";
          break;
        case D3DBLEND_DESTALPHA:
          dxgsg9_cat.debug(false) << "DESTALPHA, ";
          break;
        case D3DBLEND_INVDESTALPHA:
          dxgsg9_cat.debug(false) << "INVDESTALPHA, ";
          break;
        case D3DBLEND_SRCALPHASAT:
          dxgsg9_cat.debug(false) << "SRCALPHASAT, ";
          break;
        }
    dxgsg9_cat.debug(false) << endl;
#endif
    }
}

INLINE void DXGraphicsStateGuardian9::
enable_zwritemask(bool val) {
    if (_depth_write_enabled != val) {
        _depth_write_enabled = val;
        _pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, val);
    }
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::add_to_FVFBuf
//       Access: Private
//  Description: This adds data to the flexible vertex format
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
add_to_FVFBuf(void *data,  size_t bytes) {
    memcpy(_pCurFvfBufPtr, data, bytes);
    _pCurFvfBufPtr += bytes;
}

INLINE void DXGraphicsStateGuardian9::
transform_color(Colorf &InColor,D3DCOLOR &OutRGBAColor) {
  Colorf transformed
    ((InColor[0] * _current_color_scale[0]) + _current_color_offset[0],
     (InColor[1] * _current_color_scale[1]) + _current_color_offset[1],
     (InColor[2] * _current_color_scale[2]) + _current_color_offset[2],
     (InColor[3] * _current_color_scale[3]) + _current_color_offset[3]);
  OutRGBAColor = Colorf_to_D3DCOLOR(transformed);
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::enable_texturing
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
enable_texturing(bool val) {
  _texturing_enabled = val;
  
  if (!val) {
    _pD3DDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_DISABLE);

  } else {
    nassertv(_pCurTexContext!=NULL);
    SetTextureBlendMode(_CurTexBlendMode,true);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::wants_texcoords
//       Access: Public, Virtual
//  Description:
////////////////////////////////////////////////////////////////////
INLINE bool DXGraphicsStateGuardian9::
wants_texcoords() const {
    return _texturing_enabled;
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::compute_distance_to
//       Access: Public, Virtual
//  Description: This function may only be called during a render
//               traversal; it will compute the distance to the
//               indicated point, assumed to be in modelview
//               coordinates, from the camera plane.
////////////////////////////////////////////////////////////////////
INLINE float DXGraphicsStateGuardian9::
compute_distance_to(const LPoint3f &point) const {
    // In the case of a DXGraphicsStateGuardian9, we know that the
    // modelview matrix already includes the relative transform from the
    // camera, as well as a to-y-up conversion.  Thus, the distance to
    // the camera plane is simply the +z distance.  (negative of gl compute_distance_to,
    // since d3d uses left-hand coords)

    return point[2];
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::get_texture_wrap_mode
//       Access: Protected
//  Description: Maps from the Texture's internal wrap mode symbols to
//               GL's.
////////////////////////////////////////////////////////////////////
INLINE D3DTEXTUREADDRESS DXGraphicsStateGuardian9::
get_texture_wrap_mode(Texture::WrapMode wm) const {
  static D3DTEXTUREADDRESS PandaTexWrapMode_to_D3DTexWrapMode[Texture::WM_invalid] = {
    D3DTADDRESS_CLAMP,D3DTADDRESS_WRAP,D3DTADDRESS_MIRROR,D3DTADDRESS_MIRRORONCE,D3DTADDRESS_BORDER};

    return PandaTexWrapMode_to_D3DTexWrapMode[wm];
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::get_fog_mode_type
//       Access: Protected
//  Description: Maps from the fog types to gl version
////////////////////////////////////////////////////////////////////
INLINE D3DFOGMODE DXGraphicsStateGuardian9::
get_fog_mode_type(Fog::Mode m) const {
  switch (m) {
  case Fog::M_linear:
    return D3DFOG_LINEAR;
  case Fog::M_exponential:
    return D3DFOG_EXP;
  case Fog::M_exponential_squared:
    return D3DFOG_EXP2;
  }
  dxgsg9_cat.error() << "Invalid Fog::Mode value" << endl;
  return D3DFOG_EXP;
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::enable_clip_plane
//       Access: Protected, Virtual
//  Description: Intended to be overridden by a derived class to
//               enable the indicated clip_plane id.  A specific
//               PlaneNode will already have been bound to this id via
//               bind_clip_plane().
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
enable_clip_plane(int plane_id, bool enable) {
  assert(plane_id < D3DMAXUSERCLIPPLANES);

  DWORD bitflag = ((DWORD)1 << plane_id);
  if (enable) {
    _clip_plane_bits |= bitflag;
  } else {
    _clip_plane_bits &= ~bitflag;
  }

  _pD3DDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, _clip_plane_bits);
}

/**  unimplemented

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::enable_multisample
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
enable_multisample(bool val) {
  _multisample_enabled = val;
  #ifdef NDEBUG
    dxgsg9_cat.error() << "dx multisample unimplemented!!\n";
  #endif
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::enable_multisample_alpha_one
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
enable_multisample_alpha_one(bool val) {
  if (_multisample_alpha_one_enabled != val) {
    _multisample_alpha_one_enabled = val;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::enable_multisample_alpha_mask
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
enable_multisample_alpha_mask(bool val) {
  if (_multisample_alpha_mask_enabled != val) {
    _multisample_alpha_mask_enabled = val;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: DXGraphicsStateGuardian9::enable_point_smooth
//       Access:
//  Description:
////////////////////////////////////////////////////////////////////
INLINE void DXGraphicsStateGuardian9::
enable_point_smooth(bool val) {
  // _point_smooth_enabled = val;

  #ifdef NDEBUG
    dxgsg9_cat.error() << "dx point smoothing unimplemented!!\n";
  #endif
}
*/

