#include <misc/from_file.hpp>
#include <fsm/liquid.hpp>

using namespace sky;
using namespace core;
using namespace math;
using namespace misc;

namespace {
  char const *LIQUID_VS_SRC =
"#version 330 core\n"

"precision highp float;"

"layout(location=0)in vec3 co;"

"out vec3 vco;"
"out vec3 vno;"

"uniform mat4 proj;"
"uniform mat4 view;"
"uniform float time;"

"const float h=0.00001;"
"const float a=0.5;"

"float water(vec2 xy){"
  "float w="
      "sin(xy.x*8.+time*3.)"
    "+sin(xy.y*8.)"
    "+sin(length(xy+vec2(cos(time*0.5),sin(time*0.5))+1.)*10.+time*6.)"
    "+sin(xy.x*10.+time*6.)*sin(xy.y*6)"
    ";"
  "return w/4.*a;"
"}"

"vec3 deriv_no(vec2 xz,float h){"
  "float wxz=water(xz);"
  "float dx=water(vec2(xz.x+h,xz.y))-wxz;"
  "float dz=water(vec2(xz.x,xz.y+h))-wxz;"

  "return normalize(vec3(-dx,h,-dz));"
"}"

"void main(){"
  "vec2 lookup=co.xy*0.2;"
  "vco=vec3(co.x,water(lookup)-3.,co.y);"
  "vno=deriv_no(lookup,h);"

  "gl_Position=proj*view*vec4(vco,1.);"
"}";
  char const *LIQUID_FS_SRC =
"#version 330 core\n"

"in vec3 vco;"
"in vec3 vno;"

"uniform mat4 view;"

"layout(location=0)out vec3 nofrag;"
"layout(location=1)out uvec2 matfrag;"

"void main() {"
  "nofrag=vno;"
  "matfrag=uvec2(1,1);"
"}";
}

Liquid::Liquid(uint width, uint height, uint twidth, uint theight) :
    _plane(width, height, twidth, theight) {
  _init_program();
  _init_uniforms();
}

void Liquid::_init_program() {
  Shader vs(Shader::VERTEX);
  Shader fs(Shader::FRAGMENT);

  vs.source(LIQUID_VS_SRC);
  vs.compile("water vertex shader");
  fs.source(LIQUID_FS_SRC);
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

