#include <core/state.hpp>
#include <core/viewport.hpp>
#include <fsm/cube_room.hpp>
#include <math/common.hpp>
#include <math/matrix.hpp>
#include <math/quaternion.hpp>
#include <misc/log.hpp>
#include <misc/from_file.hpp>

using namespace std;
using namespace sky;
using namespace core;
using namespace math;
using namespace misc;
using namespace scene;
using namespace tech;

namespace {
  float  const FOVY             = PI*70.f/180.f; /* 90 degrees */
  float  const ZNEAR            = 0.0001f;
  float  const ZFAR             = 100.f;
  ushort const LASER_TESS_LEVEL = 13;
  float  const LASER_HHEIGHT    = 0.15f;
  ushort const BLUR_PASSES      = 3;
  float  const SLAB_SIZE        = 1.f;
  float  const SLAB_THICKNESS   = 0.5f;
  uint   const SLAB_INSTANCES   = 600;
  ushort const LIQUID_WIDTH     = 10;
  ushort const LIQUID_HEIGHT    = 10;
  ushort const LIQUID_TWIDTH    = 80;
  ushort const LIQUID_THEIGHT   = 80;
  ushort const LIQUID_RES       = LIQUID_TWIDTH * LIQUID_THEIGHT;
  char   const *VIEWER_FS_SRC   =
    "#version 330 core\n"
    
    "out vec4 frag;\n"
    "uniform vec4 res;\n"
    "uniform usampler2D srctex;\n"

    "void main() {\n"
      "frag=texture(srctex,gl_FragCoord.xy*res.zw);"
    "}";
}

CubeRoom::CubeRoom(ushort width, ushort height, Freefly const &freefly) :
    /* common */
    _width(width)
  , _height(height)
  , _fbCopier(width, height)
  , _freefly(freefly)
  , _laserLight(
        Light::AmbientColor(0.6f, 0.6f, 0.6f)
      , Light::DiffuseColor(0.75f, 0.f, 0.f)
      , Light::SpecularColor(0.75f, 0.f, 0.f)
      , 10.f
      , 1.f
      , 1.f
      , 1.f
      )
  , _drenderer(width, height, _depthmap, _normalmap, _materialmap)
  , _slab(width, height, SLAB_SIZE, SLAB_THICKNESS)
  , _liquid(LIQUID_WIDTH, LIQUID_HEIGHT, LIQUID_TWIDTH, LIQUID_THEIGHT)
  , _laser(width, height, LASER_TESS_LEVEL, LASER_HHEIGHT)
  , _viewer("DR viewer", VIEWER_FS_SRC, width, height) {
  _init_materials(width, height);
}

CubeRoom::~CubeRoom() {
}

void CubeRoom::_init_materials(ushort width, ushort height) {
  _matPlastic = _drenderer.matmgr.register_material(
"frag = texture(normalmap, gl_FragCoord.xy*res.zw);");
  _drenderer.matmgr.register_material(
"frag = texture(depthmap, gl_FragCoord.xy*res.zw);");

  _drenderer.matmgr.commit_materials(width, height);
}

void CubeRoom::run(float time) const {
  /* projection & view */
  auto proj = Mat44::perspective(FOVY, 1.f * _width / _height, ZNEAR, ZFAR);
  auto const &view = _freefly.view();

  /* viewport */
  //viewport(0, _height / 2, _width / 2, _height / 2);

  _drenderer.start_geometry();
  state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);
  _slab.render(time, proj, view, SLAB_INSTANCES);
  _liquid.render(time, proj, view, LIQUID_RES);
  _drenderer.end_geometry();

  _drenderer.start_shading();
  state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);
  _drenderer.shade(_laserLight);
  _drenderer.end_shading();
#if 0
  state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);
  _viewer.start();
  gTH1.bind(Texture::T_2D, _materialmap);
  _viewer.apply(0.f);
  gTH1.unbind();
  _viewer.end();
#endif
  //_slab.render(time, proj, view, SLAB_INSTANCES);
  //_liquid.render(time, proj, view, LIQUID_RES);
  //_laser.render(time, proj, view, LASER_TESS_LEVEL);
}

