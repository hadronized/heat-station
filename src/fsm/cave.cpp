#include <fsm/cave.hpp>
#include <misc/from_file.hpp>
#include <tech/perlin_noise_generator.hpp>

using namespace sky;
using namespace core;
using namespace data;
using namespace math;
using namespace misc;
using namespace tech;

namespace {
  float const CAVE_W    = 100.f;
  float const CAVE_H    = 100.f;
  float const CAVE_TW   = 600.f;
  float const CAVE_TH   = 600.f;
  float const CAVE_TRES = CAVE_TW * CAVE_TH;
  char const *CAVE_VS_SRC =
"#version 330 core\n"

"in vec3 co;"
"out vec3 vno;"

"uniform sampler2D heightmap;"
"uniform sampler2D heightmap2;"
"uniform mat4 proj;"
"uniform mat4 view;"
"uniform vec4 pres;"
"uniform float t;"

"vec2 h=vec2(1./512.);"
"float maxAmp=4.;" //4.*sin(t*0.1); /* max amplitude */

"float height(vec2 uv){"
  "return texture(heightmap,uv).r*maxAmp;"
"}"

"float height2(vec2 uv){"
  "return texture(heightmap2,uv).r*maxAmp;"
"}"

"vec3 deriv_no(vec2 xz,float first){"
  "float wxz=first;"
  "float dx=height(vec2(xz.x+h.x,xz.y))-wxz;"
  "float dz=height(vec2(xz.x,xz.y+h.y))-wxz;"

  "return normalize(vec3(-dx,h.y,-dz));"
"}"

"vec3 deriv_no2(vec2 xz,float first){"
  "float wxz=first;"
  "float dx=height2(vec2(xz.x+h.x,xz.y))-wxz;"
  "float dz=height2(vec2(xz.x,xz.y+h.y))-wxz;"

  "return normalize(vec3(-dx,h.y,-dz));"
"}"

"float mixer(float a){"
  "return clamp(pow(a,2.)*pow(-a+0.5,2.),0.,1.);"
"}"

"void main(){"
  /* compute space coordinates */
  "vec3 pos=co.xxy;"
  "vec2 lookup=(co.xy+pres.xy*0.5)*pres.zw;"
  "lookup=co.xy*0.1;"

  "float mixed=mixer(clamp(t-112.,0.,1.));"
  "pos.y=mix(height(lookup),height2(lookup),mixed);"
  "vno=mix(deriv_no(lookup,pos.y),deriv_no2(lookup,pos.y),mixed);"
  "vno*=(gl_InstanceID==0?1:-1);"
  "pos.y+= 4*(gl_InstanceID==0?1:-1);"

  "gl_Position=proj*view*vec4(pos,1.);"
"}";
  char const *CAVE_FS_SRC =
"#version 330 core\n"

"in vec3 vno;"

"layout(location=0)out vec3 nofrag;"
"layout(location=1)out ivec2 matfrag;"

"uniform sampler2D heightmap;"
"uniform vec4 pres;"

"void main(){"
  "nofrag=vno;"
  "matfrag=ivec2(2u,2u);"
"}";
}

Cave::Cave() :
    _plane(CAVE_W, CAVE_H, CAVE_TW, CAVE_TH) {
  _init_textures(CAVE_TW, CAVE_TH);
  _init_program();
}

Cave::~Cave() {
  for (short i = 0; i < 5; ++i)
    delete _pTexture[i];
}

void Cave::_init_textures(uint width, uint height) {
  PerlinNoiseGenerator png(width, height,
      "void main() {\n"
        "vec2 uv = gl_FragCoord.xy * res.zw;\n"
        "float f = 2.;\n"
        "float pers = 0.6;\n"
        "frag = vec4(0., 0., 0., 0.);\n"
        "for (int i = 0; i < 3; i++) {\n"
          "frag += perlin_noise(uv*pow(2., float(i+2))) * pow(pers, float(i));\n"
        "}\n"
      "}\n"
    );

  for (short i = 0; i < 5; ++i)
    _pTexture[i] = png.gen(i);
}

void Cave::_init_program() {
  Shader vs(Shader::VERTEX);
  Shader fs(Shader::FRAGMENT);

  vs.source(CAVE_VS_SRC);
  vs.compile("cave vertex shader");
  fs.source(CAVE_FS_SRC);
  fs.compile("cave fragment shader");

  _sp.attach(vs);
  _sp.attach(fs);
  _sp.link();

  _init_uniforms();
}

void Cave::_init_uniforms() {
  auto heightmapIndex = _sp.map_uniform("heightmap");
  auto heightmap2Index = _sp.map_uniform("heightmap2");
  auto presIndex      = _sp.map_uniform("pres");
  _projIndex          = _sp.map_uniform("proj");
  _viewIndex          = _sp.map_uniform("view");
  _timeIndex          = _sp.map_uniform("t");

  _sp.use();
  heightmapIndex.push(0);
  heightmap2Index.push(1);
  presIndex.push(CAVE_W, CAVE_H, 1.f / CAVE_W, 1.f / CAVE_H);
  _sp.unuse();
}

void Cave::render(float time, Mat44 const &proj, Mat44 const &view) const {
  _sp.use();

  _projIndex.push(proj);
  _viewIndex.push(view);
  _timeIndex.push(time);

  gTH.unit(0);
  gTH.bind(Texture::T_2D, *_pTexture[0]);
  gTH.unit(1);
  gTH.bind(Texture::T_2D, *_pTexture[1]);
  _plane.va.inst_indexed_render(primitive::TRIANGLE, CAVE_TRES*6, GLT_UINT, 2);
  _sp.unuse();
}

