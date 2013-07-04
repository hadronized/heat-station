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
  float  const ZNEAR            = 0.001f;
  float  const ZFAR             = 100.f;
  ushort const LASER_TESS_LEVEL = 13;
  float  const LASER_HHEIGHT    = 0.15;
  ushort const BLUR_PASSES      = 3;
  float  const SLAB_SIZE        = 1.f;
  float  const SLAB_THICKNESS   = 0.5f;
  uint   const SLAB_INSTANCES   = 600;
  ushort const LIQUID_WIDTH      = 10;
  ushort const LIQUID_HEIGHT     = 10;
  ushort const LIQUID_TWIDTH     = 80;
  ushort const LIQUID_THEIGHT    = 80;
  ushort const LIQUID_RES        = LIQUID_TWIDTH * LIQUID_THEIGHT;
}

/* ============
 * [ CubeRoom ]
 * ============ */
CubeRoom::CubeRoom(ushort width, ushort height, Freefly const &freefly) :
    /* common */
    _width(width)
  , _height(height)
  , _fbCopier(width, height)
  , _freefly(freefly)
  , _drenderer(width, height, _depthmap, _normalmap, _materialmap)
  , _slab(width, height, SLAB_SIZE, SLAB_THICKNESS)
  , _liquid(LIQUID_WIDTH, LIQUID_HEIGHT, LIQUID_TWIDTH, LIQUID_THEIGHT)
  , _laser(width, height, LASER_TESS_LEVEL, LASER_HHEIGHT) {
}

CubeRoom::~CubeRoom() {
}

void CubeRoom::_init_materials() {
#if 0
  _matPlastic = _drenderer.matmgr.register_material(
"");
#endif
}

/* ============
 * [ CubeRoom ]
 * ============ */
void CubeRoom::run(float time) const {
  /* projection & view */
  auto proj = Mat44::perspective(FOVY, 1.f * _width / _height, ZNEAR, ZFAR);
  auto view = _freefly.view();

  /* viewport */
  //viewport(0, _height / 2, _width / 2, _height / 2);

  /* walls */
  state::enable(state::DEPTH_TEST);
  state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);
  _slab.render(time, proj, view, SLAB_INSTANCES);

  /* liquid */
  //state::enable(state::BLENDING);
  Framebuffer::blend_func(blending::ONE, blending::DST_ALPHA);
  _liquid.render(time, proj, view, LIQUID_RES);

  /* laser */
  state::enable(state::BLENDING);
  Framebuffer::blend_func(blending::ONE, blending::ONE);
  _laser.render(time, proj, view, LASER_TESS_LEVEL);
  state::disable(state::BLENDING);
}

