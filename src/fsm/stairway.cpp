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
}

void Stairway::run(float time) const {
  auto proj = Mat44::perspective(FOVY, 1.f * _width / _height, ZNEAR, ZFAR);
  auto const &view = _freefly.view();

  _drenderer.start_geometry();
  state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);
  _cave.render(time, proj, view, 2);
  _drenderer.end_geometry();

  _drenderer.start_shading();
  _matmgr.start();
  _matmgr.render();
  _matmgr.end();
  _drenderer.end_shading();
}

