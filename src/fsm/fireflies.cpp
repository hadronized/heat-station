#include <cstdlib>
#include <ctime>
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
  srand(time(nullptr));
}

void Fireflies::_init_fireflies() {
  for (int i = 0; i < FIREFLIES_NB; ++i) {
    int j = i - FIREFLIES_NB / 2.f;
    _pos[i] = Position(sinf(i)*j*10.f, (1. - sinf(i)), cosf(i)*j*10.f);
    _colors[i] = Vec3<float>(0.5f + j/20.f, 1.f - 0.5*(1.+ sinf(j*10.f)), 1.f - (j % 5)/30.f);
  }

  /* init buffer */
  gBH.bind(Buffer::ARRAY, _vbo);
  gBH.data(FIREFLIES_NB*(sizeof(Position)+sizeof(Vec3<float>)), Buffer::STATIC_DRAW, &_pos);
  gBH.unbind();

  /* init va */
  Program::In co(semantic::CO);
  Program::In color(1);
  _va.bind();
  co.enable();
  color.enable();
  gBH.bind(Buffer::ARRAY, _vbo);
  co.pointer(3, GLT_FLOAT, false);
  color.pointer(3, GLT_FLOAT, false, FIREFLIES_NB*sizeof(Position));
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

sky::scene::Position const * Fireflies::positions() const {
  return _pos;
}

sky::math::Vec3<float> const * Fireflies::colors() const {
  return _colors;
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

void Fireflies::animate(float time) {
  static float lastTime = 0.f;
  float elapsed = time - lastTime;
  float const UPDATE_TIME = 0.3f; /* update every 100ms */
  static Vec3<float> offset[FIREFLIES_NB];

  for (int i = 0; i < FIREFLIES_NB; ++i) {
    if (elapsed >= UPDATE_TIME) {
      float x = (1.f * (rand() % 1000) / 1000.f) - 0.5f;
      float y = (1.f * (rand() % 1000) / 1000.f) - 0.5f;
      float z = (1.f * (rand() % 1000) / 1000.f) - 0.5f;
      offset[i] = Vec3<float>(x, y, z)*0.08f;
    }

    _pos[i] += offset[i];
  }

  if (elapsed >= UPDATE_TIME)
    lastTime = time;

  gBH.bind(Buffer::ARRAY, _vbo);
  gBH.subdata(0, FIREFLIES_NB*sizeof(Position), &_pos);
  gBH.unbind();
}

