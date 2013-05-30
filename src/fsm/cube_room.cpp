#include <core/state.hpp>
#include <fsm/cube_room.hpp>
#include <math/common.hpp>
#include <math/matrix.hpp>
#include <misc/from_file.hpp>

using namespace sky;

namespace {
  float  const FOVY             = math::PI_4; /* 90 degrees */
  float  const ZNEAR            = 0.001f;
  float  const ZFAR             = 100.f;
  ushort const TESS_LASER_LEVEL = 18;
  ushort const BLUR_PASSES      = 1;
}

CubeRoom::CubeRoom(ushort width, ushort height) :
    /* laser blur */
    _laserHBlur("laser hblur", misc::from_file("../../src/fsm/laser_hblur-fs.glsl").c_str(), width, height)
  , _laserVBlur("laser vblur", misc::from_file("../../src/fsm/laser_vblur-fs.glsl").c_str(), width, height) {
  _init_laser_program(width, height);
  _laser.bind();
  _laser.unbind();
}

CubeRoom::~CubeRoom() {
}

void CubeRoom::_init_laser_program(ushort width, ushort height) {
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

  _init_laser_uniforms(width, height);
  _init_laser_blur(width, height); 
}

void CubeRoom::_init_laser_blur(ushort width, ushort height) {
  core::FramebufferHandler fbh;
  core::RenderbufferHandler rbh;
  core::TextureHandler<1> texh;

  rbh.bind(core::Renderbuffer::RENDERBUFFER, _laserBlurRB);
  rbh.store(width, height, core::Texture::IF_DEPTH_COMPONENT);
  rbh.unbind();

  for (int i = 0; i < 2; ++i) {
    texh.bind(core::Texture::T_2D, _laserBlurOfftex[i]);
    texh.parameter(core::Texture::P_WRAP_S, core::Texture::PV_CLAMP_TO_BORDER);
    texh.parameter(core::Texture::P_WRAP_T, core::Texture::PV_CLAMP_TO_BORDER);
    texh.parameter(core::Texture::P_MIN_FILTER, core::Texture::PV_NEAREST);
    texh.parameter(core::Texture::P_MAG_FILTER, core::Texture::PV_NEAREST);
    texh.image_2D(width, height, 0, core::Texture::F_RGB, core::Texture::IF_RGB, core::GLT_FLOAT, 0, nullptr);
    texh.unbind();

    fbh.bind(core::Framebuffer::DRAW, _laserBlurFB[i]);
    fbh.attach_renderbuffer(_laserBlurRB, core::Framebuffer::DEPTH_ATTACHMENT);
    fbh.attach_2D_texture(_laserBlurOfftex[i], core::Framebuffer::COLOR_ATTACHMENT);
  }
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
  static core::FramebufferHandler fbh;
  static core::TextureHandler<1> texh;
  static int offtexid = 0;

  core::state::clear(core::state::COLOR_BUFFER | core::state::DEPTH_BUFFER);
  
  /* first render the lined laser in a frambuffer */
  _laserSP.use();
  _laserTimeIndex.push(time);
  fbh.bind(core::Framebuffer::DRAW, _laserBlurFB[0]);
  _laser.render(core::primitive::LINE_STRIP, 0, TESS_LASER_LEVEL+1);
  fbh.unbind();
  _laserSP.unuse();

  /* blur the lined laser */
  for (int i = 0; i < BLUR_PASSES; ++i) {

    texh.bind(core::Texture::T_2D, _laserBlurOfftex[1-i]);
    _laserHBlur.start();
    _laserHBlur.apply(0.);
    _laserHBlur.end();
    texh.unbind();
  }


  /* add a moving effect on the blured area */

  /* then render the extremity with billboards */
}

void CubeRoom::run(float time) const {
  _render_laser(time);
}

