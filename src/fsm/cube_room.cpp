#include <core/state.hpp>
#include <fsm/cube_room.hpp>
#include <math/common.hpp>
#include <math/matrix.hpp>
#include <misc/from_file.hpp>

using namespace sky;

namespace {
  ushort const TESS_LASER_LEVEL = 18;
  float const FOVY              = math::PI_4; /* 90 degrees */
  float const ZNEAR             = 0.01f;
  float const ZFAR              = 100.f;
}

CubeRoom::CubeRoom(ushort width, ushort height) {
  _init_laser_program();
  _init_laser_uniforms(width, height);
  _laser.bind();
  _laser.unbind();
}

CubeRoom::~CubeRoom() {
}

void CubeRoom::_init_laser_program() {
  core::Shader vs(core::Shader::VERTEX);
  core::Shader fs(core::Shader::FRAGMENT);

  /* sources compilation */
  vs.source(misc::from_file("../../src/fsm/laser-vs.glsl").c_str());
  vs.compile("laser VS");
  fs.source(misc::from_file("../../src/fsm/laser-fs.glsl").c_str());
  fs.compile("laser FS");

  /* program link */
  _laserSP.attach(vs);
  _laserSP.attach(fs);
  _laserSP.link();
}

void CubeRoom::_init_laser_uniforms(ushort width, ushort height) {
  auto rvnbIndex  = _laserSP.map_uniform("rvnb");
  auto projIndex  = _laserSP.map_uniform("proj");
  _laserTimeIndex = _laserSP.map_uniform("t");

  _laserSP.use();
  rvnbIndex.push(1.f / TESS_LASER_LEVEL);
  projIndex.push(math::Mat44::perspective(FOVY, 1.f * width / height, ZNEAR, ZFAR));

  _laserSP.unuse();
}

void CubeRoom::_render_laser(float time) const {
  core::state::clear(core::state::COLOR_BUFFER | core::state::DEPTH_BUFFER);
  
  /* first render the lined laser */
  _laserSP.use();
  _laserTimeIndex.push(time);
  _laser.render(core::primitive::LINE_STRIP, 0, TESS_LASER_LEVEL+1);
  _laserSP.unuse();

  /* blur the lined laser */

  /* add a moving effect on the blured area */

  /* then render the extremity with billboards */
}

void CubeRoom::run(float time) const {
  _render_laser(time);
}

