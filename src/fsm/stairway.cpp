#include <core/state.hpp>
#include <fsm/common.hpp>
#include <fsm/stairway.hpp>
#include <misc/log.hpp>
#include <scene/common.hpp>

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
  , _stringRenderer(common.stringRenderer)
  /*, _fogEffect("fog effect", from_file("../../src/fsm/fog-fs.glsl").c_str(), width, height)*/ {
  _init_materials();
}

void Stairway::_init_materials() {
  _matmgrProjIndex   = _matmgr.postprocess().program().map_uniform("proj");
  _matmgrViewIndex   = _matmgr.postprocess().program().map_uniform("view");
  _matmgrLColorIndex = _matmgr.postprocess().program().map_uniform("lightColor");
  _matmgrLPosIndex   = _matmgr.postprocess().program().map_uniform("lightPos");
}

void Stairway::_draw_texts(float t) const {
  state::enable(state::BLENDING);
  Framebuffer::blend_func(blending::ONE, blending::ONE);
  _stringRenderer.start_draw();

  if (t < 100.8f) {
  } else if (t < 161.3f) {
    _stringRenderer.draw_string("I WOULD ALSO LIKE TO THANK ALL FRIENDS OF MINE", 1.f-(t-100.8f)*0.5f, -0.2f, 0.08f); 
    _stringRenderer.draw_string("WHO GAVE ME THEIR GREAT SUPPORT", 1.f-(t-105.f)*0.5f, -0.35f, 0.08f);
    _stringRenderer.draw_string("LUV YOU GUYS!", 1.f-(t-110.f)*0.5f, -0.35f, 0.08f);
  } else {
    state::disable(state::DEPTH_TEST);
    _stringRenderer.draw_string("YOU IZ AWESOME EVOKE!", -0.7f, 0.3f, 0.08f);
    _stringRenderer.draw_string("CHEERS!", -0.25f, 0.f, 0.1f);
    state::enable(state::DEPTH_TEST);
  }
  _stringRenderer.end_draw();
  state::disable(state::BLENDING);
}

void Stairway::run(float time) {
  if (time <= 81.50f) return;

  auto proj = Mat44::perspective(FOVY, 1.f * _width / _height, ZNEAR, ZFAR);
  auto view = Mat44::trslt(-Position(cosf(time*0.1f)*10.f, sinf(time*0.8f), (81.50f+50.f-time))) * Orient(Axis3(0.f, 0.f, 1.f), PI_2+sinf(time*0.5f)).to_matrix() *
              Orient(Axis3(0.f, 1.f, 0.f), PI_2*sinf(time*0.5f) / 3.f).to_matrix() *
              Orient(Axis3(1.f, 0.f, 0.f), PI_4*cosf(time*0.1f)*0.5f).to_matrix();

  gFBH.unbind();

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
  auto colors = _fireflies.colors();
  for (int i = 0; i < _fireflies.FIREFLIES_NB; ++i) {
    auto p = lights[i];
    auto l = colors[i];
    _matmgrLColorIndex.push(l.x, l.y, l.z);
    _matmgrLPosIndex.push(p.x, p.y, p.z);
    state::clear(state::DEPTH_BUFFER);
    _matmgr.render();
  }
  _matmgr.end();
  _drenderer.end_shading();

  Framebuffer::blend_func(blending::SRC_ALPHA, blending::ONE_MINUS_SRC_ALPHA);
  _fireflies.render(proj, view);
  state::disable(state::BLENDING);


#if 0 /* shitty fog */
  state::disable(state::DEPTH_TEST);
  Framebuffer::blend_func(blending::DST_COLOR, blending::ZERO);
  _fogEffect.start();
  _fogEffect.apply(0.f);
  _fogEffect.end();
#endif

  _draw_texts(time);

  _fireflies.animate(time);
}

