#include <cstdlib>
#include <ctime>
#include <fsm/fireflies.hpp>
#include <misc/from_file.hpp>

using namespace sky;
using namespace core;
using namespace math;
using namespace misc;
using namespace scene;

namespace {
  char const *FIREFLIES_VS_SRC =
"#version 330 core\n"

"layout(location=0)in vec3 co;"
"layout(location=1)in vec3 color;"

"out vec3 vco;"
"out vec3 vcolor;"

"void main(){"
  "vco=co;"
  "vcolor=color;"
"}";
  char const *FIREFLIES_GS_SRC =
"#version 330 core\n"

"layout(points)in;"
"layout(triangle_strip,max_vertices=6)out;"

"in vec3 vco[];"
"in vec3 vcolor[];"
"out vec2 gco;"
"flat out vec3 gcolor;"

"uniform mat4 proj;"
"uniform mat4 view;"

"const float s=0.6;"

"const vec2[4] tile=vec2[]("
    "vec2(-s,s)"
  ",vec2(s,s)"
  ",vec2(-s-s)"
  ",vec2(s,-s)"
");"

"void emit_vertex(uint i){"
  "gl_Position=proj*(view*vec4(vco[0],1.)+vec4(tile[i],0.,1.));"
  "gco=tile[i]/s;"
  "gcolor=vcolor[0];"
  "EmitVertex();"
"}"

"void main(){"
  "for (uint i=0u;i<4u;++i)"
    "emit_vertex(i);"
"}";
  char const *FIREFLIES_FS_SRC =
"#version 330 core\n"

"flat in vec3 gcolor;"
"in vec2 gco;"
"out vec4 frag;"

"void main(){"
  "float d=(1.-sqrt(length(gco)));"
  "frag=vec4(gcolor,1.)*d*2.5;" //vec4(2.4, 2., 3.6, 4.);
"}";
}

Fireflies::Fireflies() {
  _init_fireflies();
  _init_shader();
  srand(time(nullptr));
}

void Fireflies::_init_fireflies() {
  for (int i = 0; i < FIREFLIES_NB; ++i) {
    int j = i - FIREFLIES_NB / 2.f;
    _pos[i] = Position(sinf(i)*j*10.f, (1. - sinf(i)), cosf(i)*j*10.f);
    _colors[i] = Vec3<float>(0.5f + j/20.f, 1.f - 0.5*(1.+ tanf(j*10.f)), 1.f - pow(j % 5, 2)/30.f);
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

  vs.source(FIREFLIES_VS_SRC);
  vs.compile("fireflies vertex shader");
  gs.source(FIREFLIES_GS_SRC);
  gs.compile("fireflies geometry shader");
  fs.source(FIREFLIES_FS_SRC);
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

