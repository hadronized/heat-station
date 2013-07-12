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
  float const CAVE_W  = 512.f;
  float const CAVE_H  = 512.f;
  float const CAVE_TW = 1024.f;
  float const CAVE_TH = 1024.f;
}

Cave::Cave() :
    _plane(CAVE_W, CAVE_H, CAVE_TW, CAVE_TH) {
  _init_textures(CAVE_TW, CAVE_TH);
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
        "float pers = 0.75;\n"
        "for (short i = 0; i < 4; ++i, f = pow(f, 2.)) {\n"
          "frag += perlin_noise(uv*f+rand2(uv)) * pow(2., pers);\n"
        "}\n"
      "}\n"
    );

  for (short i = 0; i < 5; ++i)
    _pTexture[i] = png.gen(i);
}

void Cave::_init_program(uint width, uint height) {
  Shader vs(Shader::VERTEX);
  Shader fs(Shader::FRAGMENT);

  vs.source(from_file("../../fsm/cave-vs.glsl").c_str());
  vs.compile("cave vertex shader");
  fs.source(from_file("../../fsm/cave-fs.glsl").c_str());
  fs.compile("cave fragment shader");

  _sp.attach(vs);
  _sp.attach(fs);
  _sp.link();

  _init_uniforms(width, height);
}

void Cave::_init_uniforms(uint width, uint height) {
}

