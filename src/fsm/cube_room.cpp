#include <core/state.hpp>
#include <core/viewport.hpp>
#include <fsm/common.hpp>
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
  ushort const LASER_TESS_LEVEL = 13;
  float  const LASER_HHEIGHT    = 0.15f;
  float  const SLAB_SIZE        = 1.f;
  float  const SLAB_THICKNESS   = 0.5f;
  uint   const SLAB_INSTANCES   = 600;
  ushort const LIQUID_WIDTH     = 10;
  ushort const LIQUID_HEIGHT    = 10;
  ushort const LIQUID_TWIDTH    = 80;
  ushort const LIQUID_THEIGHT   = 80;
  ushort const LIQUID_RES       = LIQUID_TWIDTH * LIQUID_THEIGHT;
}

CubeRoom::CubeRoom(ushort width, ushort height, Common &common, Freefly const &freefly) :
    /* common */
    _width(width)
  , _height(height)
  , _fbCopier(width, height)
  , _freefly(freefly)
  , _drenderer(common.drenderer)
  , _matmgr(common.matmgr)
  , _slab(width, height, SLAB_SIZE, SLAB_THICKNESS)
  , _liquid(LIQUID_WIDTH, LIQUID_HEIGHT, LIQUID_TWIDTH, LIQUID_THEIGHT)
  , _laser(width, height, LASER_TESS_LEVEL, LASER_HHEIGHT) {
  _init_materials(width, height);
  _init_offscreen(width, height);
}

void CubeRoom::_init_materials(ushort width, ushort height) {
  _matmgrProjIndex   = _matmgr.postprocess().program().map_uniform("proj");
  _matmgrViewIndex   = _matmgr.postprocess().program().map_uniform("view");
  _matmgrLColorIndex = _matmgr.postprocess().program().map_uniform("lightColor");
  _matmgrLPosIndex   = _matmgr.postprocess().program().map_uniform("lightPos");
}

void CubeRoom::_init_offscreen(ushort width, ushort height) {
  /* offscreen texture */
  gTH.bind(Texture::T_2D, _offTex);
  gTH.parameter(Texture::P_WRAP_S, Texture::PV_CLAMP);
  gTH.parameter(Texture::P_WRAP_T, Texture::PV_CLAMP);
  gTH.parameter(Texture::P_MIN_FILTER, Texture::PV_NEAREST);
  gTH.parameter(Texture::P_MAG_FILTER, Texture::PV_NEAREST);
  gTH.parameter(Texture::P_MAX_LEVEL, 0);
  gTH.image_2D(width, height, 0, Texture::F_RGB, Texture::IF_RGB32F, GLT_FLOAT, 0, nullptr);
  gTH.unbind();

  /* offscreen renderbuffer */
  gRBH.bind(Renderbuffer::RENDERBUFFER, _offRB);
  gRBH.store(width, height, Texture::IF_DEPTH_COMPONENT32F);
  gRBH.unbind();

  /* offscreen framebuffer */
  gFBH.bind(Framebuffer::DRAW, _offFB);
  gFBH.attach_renderbuffer(_offRB, Framebuffer::DEPTH_ATTACHMENT);
  gFBH.attach_2D_texture(_offTex, Framebuffer::color_attachment(0));
  gFBH.unbind();
}

void CubeRoom::run(float time) {
  /* projection & view */
  auto proj = Mat44::perspective(FOVY, 1.f * _width / _height, ZNEAR, ZFAR);
  //static auto yaw = Orient(Axis3(0.f, 1.f, 0.f), PI_2).to_matrix();
  //static auto pitch = Orient(Axis3(1.f, 0.f, 0.f), -PI_2).to_matrix();
  //Mat44 view;
  
  misc::log << debug << "time: " << time << std::endl;
#if 0
  if (time < 5.f) {
    view = Mat44::trslt(-Position(1.f, 0.f, 0.f)) * yaw;
    //viewport(0., _height * 0.5f, _width * 0.5f, _height * 0.5f);
  } else if (time < 10.f) {
    view = Mat44::trslt(-Position(0.f, 1.f, 0.f)) * pitch;
    //viewport(_width * 0.5f, _height * 0.5f, _width * 0.5f, _height * 0.5f);
  } else if (time < 15.f) {
    view = Mat44::trslt(-Position(1.f, 1.f, 1.f)) * Orient(Axis3(0.f, 1.f, 0.f), PI_2 / 3.).to_matrix();// * Orient(Axis3(1.f, 0.f, 0.f), -PI_4).to_matrix();
    //viewport(_width * 0.5f, 0.f, _width * 0.5f, _height * 0.5f);
  }
#endif
  auto view = _freefly.view();

  _drenderer.start_geometry();
  state::enable(state::DEPTH_TEST);
  state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);
  _slab.render(time, proj, view, SLAB_INSTANCES);
  _liquid.render(time, proj, view, LIQUID_RES);
  _drenderer.end_geometry();

  gFBH.bind(Framebuffer::DRAW, _offFB);
  state::disable(state::DEPTH_TEST);
  state::enable(state::BLENDING);
  state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);

  _drenderer.start_shading();
  _matmgr.start();

  _matmgrProjIndex.push(proj);
  _matmgrViewIndex.push(view);
  _matmgrLColorIndex.push(0.75f, 0.f, 0.f);
  _matmgrLPosIndex.push(0.f, 0.f, 0.f);
  _matmgr.render();

  _matmgr.end();
  _drenderer.end_shading();

  state::clear(state::DEPTH_BUFFER);

  _laser.render(time, proj, view, LASER_TESS_LEVEL);
  gFBH.unbind();

  state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);

  _fbCopier.copy(_offTex);
}

