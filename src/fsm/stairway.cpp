#include <core/state.hpp>
#include <scene/common.hpp>
#include <fsm/common.hpp>
#include <fsm/stairway.hpp>

using namespace sky;
using namespace core;
using namespace math;
using namespace scene;

Stairway::Stairway(ushort width, ushort height, Common &common, Freefly const &freefly) :
    _width(width)
  , _height(height)
  , _freefly(freefly)
  , _drenderer(common.drenderer)
  , _matmgr(common.matmgr) {
  _init_materials();
}

void Stairway::_init_materials() {
  _matmgrProjIndex   = _matmgr.postprocess().program().map_uniform("proj");
  _matmgrViewIndex   = _matmgr.postprocess().program().map_uniform("view");
  _matmgrLColorIndex = _matmgr.postprocess().program().map_uniform("lightColor");
  _matmgrLPosIndex   = _matmgr.postprocess().program().map_uniform("lightPos");
}

void Stairway::run(float time) const {
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
  _matmgrLColorIndex.push(1.f, 1.f, 1.f);
  _matmgrLPosIndex.push(0.f, 5.f, 0.f);
  _matmgr.render();
  _matmgr.end();
  _drenderer.end_shading();
}

