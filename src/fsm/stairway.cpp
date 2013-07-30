#include <core/state.hpp>
#include <scene/common.hpp>
#include <fsm/common.hpp>
#include <fsm/stairway.hpp>
#include <misc/from_file.hpp>

using namespace sky;
using namespace core;
using namespace math;
using namespace misc;
using namespace scene;

Stairway::Stairway(ushort width, ushort height, Common &common, Freefly const &freefly) :
    _width(width)
  , _height(height)
  , _freefly(freefly)
  , _drenderer(common.drenderer)
  , _matmgr(common.matmgr)
  , _fogEffect("fog effect", from_file("../../src/fsm/fog-fs.glsl").c_str(), width, height) {
  _init_materials();
}

void Stairway::_init_materials() {
  _matmgrProjIndex   = _matmgr.postprocess().program().map_uniform("proj");
  _matmgrViewIndex   = _matmgr.postprocess().program().map_uniform("view");
  _matmgrLColorIndex = _matmgr.postprocess().program().map_uniform("lightColor");
  _matmgrLPosIndex   = _matmgr.postprocess().program().map_uniform("lightPos");
}

void Stairway::_init_fog_uniforms() {
}

void Stairway::run(float time) {
  auto proj = Mat44::perspective(FOVY, 1.f * _width / _height, ZNEAR, ZFAR);
  auto const &view = _freefly.view();

  state::enable(state::DEPTH_TEST);
  _drenderer.start_geometry();
  state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);
  _cave.render(time, proj, view);
  _drenderer.end_geometry();

  state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);

  _drenderer.start_shading();
  _matmgr.start();
  _matmgrProjIndex.push(proj);
  _matmgrViewIndex.push(view);

  state::enable(state::BLENDING);
  Framebuffer::blend_func(blending::ONE, blending::ONE);

  auto lights = _fireflies.positions();
  for (int i = 0; i < _fireflies.FIREFLIES_NB; ++i) {
    auto p = lights[i];
    _matmgrLColorIndex.push(1.f * i/20.f, 1.f - sinf(i), 1.f - i/30.f);
    _matmgrLPosIndex.push(p.x, p.y, p.z);
    state::clear(state::DEPTH_BUFFER);
    _matmgr.render();
  }
  _matmgr.end();
  _drenderer.end_shading();

  _fireflies.render(proj, view);

#if 0 /* shitty fog */
  state::disable(state::DEPTH_TEST);
  Framebuffer::blend_func(blending::DST_COLOR, blending::ZERO);
  _fogEffect.start();
  _fogEffect.apply(0.f);
  _fogEffect.end();
#endif
  state::disable(state::BLENDING);
  state::enable(state::DEPTH_TEST);

  _fireflies.animate(time);
}

