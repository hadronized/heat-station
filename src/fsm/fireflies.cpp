#include <fsm/fireflies.hpp>
#include <misc/from_file.hpp>

using namespace sky;
using namespace core;
using namespace math;
using namespace misc;
using namespace scene;

Fireflies::Fireflies() {
  _init_fireflies();
  _init_shader();
}

void Fireflies::_init_fireflies() {
  for (int i = 0; i < FIREFLIES_NB; ++i) {
    int j = i - FIREFLIES_NB / 2.f;
    _[i] = Position(sinf(j), sinf(j), cosf(j)*10.);
  }

  /* init buffer */
  gBH.bind(Buffer::ARRAY, _vbo);
  gBH.data(FIREFLIES_NB*sizeof(Position), Buffer::STATIC_DRAW, &_);
  gBH.unbind();

  /* init va */
  Program::In co(semantic::CO);
  _va.bind();
  co.enable();
  gBH.bind(Buffer::ARRAY, _vbo);
  co.pointer(3, GLT_FLOAT, false);
  _va.unbind();
}

void Fireflies::_init_shader() {
  Shader vs(Shader::VERTEX);
  Shader gs(Shader::GEOMETRY);
  Shader fs(Shader::FRAGMENT);

  vs.source(from_file("../../src/fsm/fireflies-vs.glsl").c_str());
  vs.compile("fireflies vertex shader");
  gs.source(from_file("../../src/fsm/fireflies-gs.glsl").c_str());
  gs.compile("fireflies geometry shader");
  fs.source(from_file("../../src/fsm/fireflies-fs.glsl").c_str());
  fs.compile("fireflies fragement shader");
  
  _sp.attach(vs);
  _sp.attach(gs);
  _sp.attach(fs);
  _sp.link();

  _init_uniforms();
}

void Fireflies::_init_uniforms() {
  _projIndex  = _sp.map_uniform("proj");
  _viewIndex  = _sp.map_uniform("view");
}

void Fireflies::render(Mat44 const &proj, Mat44 const &view) const {
  _sp.use();

  _projIndex.push(proj);
  _viewIndex.push(view);

  _va.bind();
  _va.render(primitive::POINT, 0, FIREFLIES_NB);
  _va.unbind();

  _sp.unuse();
}

