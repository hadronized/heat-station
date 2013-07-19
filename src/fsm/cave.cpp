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
  float const CAVE_W    = 10.f;
  float const CAVE_H    = 10.f;
  float const CAVE_TW   = 512.f;
  float const CAVE_TH   = 512.f;
  float const CAVE_TRES = CAVE_TW * CAVE_TH;
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

  vs.source(from_file("../../src/fsm/cave-vs.glsl").c_str());
  vs.compile("cave vertex shader");
  fs.source(from_file("../../src/fsm/cave-fs.glsl").c_str());
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
  _plane.va.indexed_render(primitive::TRIANGLE, CAVE_TRES*6, GLT_UINT);
  _sp.unuse();
}

