#include <misc/from_file.hpp>
#include <fsm/liquid.hpp>

using namespace sky;
using namespace core;
using namespace math;
using namespace misc;

namespace {
}

Liquid::Liquid(uint width, uint height, uint twidth, uint theight) :
    _plane(width, height, twidth, theight) {
  _init_program();
  _init_uniforms();
}

void Liquid::_init_program() {
  Shader vs(Shader::VERTEX);
  Shader fs(Shader::FRAGMENT);

  vs.source(from_file("../../src/fsm/water-vs.glsl").c_str());
  vs.compile("water vertex shader");
  fs.source(from_file("../../src/fsm/water-fs.glsl").c_str());
  fs.compile("water fragment shader");

  _sp.attach(vs);
  _sp.attach(fs);
  _sp.link();
}

void Liquid::_init_uniforms() {
  _projIndex = _sp.map_uniform("proj");
  _viewIndex = _sp.map_uniform("view");
  _timeIndex = _sp.map_uniform("time");
}

void Liquid::render(float time, Mat44 const &proj, Mat44 const &view, uint n) const {
  _sp.use();

  _projIndex.push(proj);
  _viewIndex.push(view);
  _timeIndex.push(time);

  _plane.va.indexed_render(primitive::TRIANGLE, n*6, GLT_UINT);
  
  _sp.unuse();
}

