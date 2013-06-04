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
  ushort const BLUR_PASSES      = 5;
}

CubeRoom::CubeRoom(ushort width, ushort height) :
    /* common */
    _width(width)
  , _height(height)
  , _fbCopier(width, height)
    /* laser blur */
  , _laserHBlur("laser hblur", misc::from_file("../../src/fsm/laser_hblur-fs.glsl").c_str(), width, height)
  , _laserVBlur("laser vblur", misc::from_file("../../src/fsm/laser_vblur-fs.glsl").c_str(), width, height)
  , _laserMove("laser move", misc::from_file("../../src/fsm/laser_move-fs.glsl").c_str(), width, height) {
  /* laser */
  _init_laser_program(width, height);
  _laser.bind();
  _laser.unbind();

  /* room */
  _init_room();
  _init_room_program(width, height);
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

void CubeRoom::_init_laser_uniforms(ushort width, ushort height) {
  auto rvnbIndex  = _laserSP.map_uniform("rvnb");
  auto projIndex  = _laserSP.map_uniform("proj");
  _laserTimeIndex = _laserSP.map_uniform("t");

  _laserSP.use();
  rvnbIndex.push(1.f / TESS_LASER_LEVEL);
  projIndex.push(math::Mat44::perspective(FOVY, 1.f * width / height, ZNEAR, ZFAR));

  _laserSP.unuse();
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
    texh.parameter(core::Texture::P_MIN_FILTER, core::Texture::PV_LINEAR);
    texh.parameter(core::Texture::P_MAG_FILTER, core::Texture::PV_LINEAR);
    texh.image_2D(width, height, 0, core::Texture::F_RGB, core::Texture::IF_RGB, core::GLT_FLOAT, 0, nullptr);
    texh.unbind();

    fbh.bind(core::Framebuffer::DRAW, _laserBlurFB[i]);
    fbh.attach_renderbuffer(_laserBlurRB, core::Framebuffer::DEPTH_ATTACHMENT);
    fbh.attach_2D_texture(_laserBlurOfftex[i], core::Framebuffer::COLOR_ATTACHMENT);
  }
}

void CubeRoom::_init_room() {
  core::BufferHandler bufh;

  uint ids[48] = {
    /* front face */
      0, 1, 2
    , 0, 2, 3
    /* back face */
    , 4, 5, 7
    , 5, 7, 6
    /* up face */
    , 0, 1, 4
    , 1, 4, 5
    /* bottom face */
    , 2, 3, 7
    , 2, 7, 6
    /* left face */

  /* IBO */
  bufh.bind(core::Buffer::ELEMENT_ARRAY);
  bufh.data(48, ids, core::Buffer::STATIC_DRAW);
  bufh.unbind();


  /* VA */
  _room.bind();
  bufh.bind(core::Buffer::ELEMENT_ARRAY);
  _room.unbind(); /* attribute-less render */
  bufh.unbind();
}

void CubeRoom::_init_room_program(ushort width, ushort height) {
  core::Shader vs(core::Shader::VERTEX);
  core::Shader fs(core::Shader::FRAGMENT);

  vs.source(misc::from_file("../../src/fsm/room-vs.glsl").c_str());
  vs.compile("room vertex shader");
  fs.source(misc::from_file("../../src/fsm/room-fs.glsl").c_str());
  fs.compile("room fragment shader");

  _roomSP.attach(vs);
  _roomSP.attach(fs);
  _roomSP.link();

  _init_room_uniforms(width, height);
}

void CubeRoom::_init_room_uniforms(ushort width, ushort height) {
  auto projIndex = _roomSP.map_uniform("proj");

  _roomSP.use();
  projIndex.push(math::Mat44::perspective(FOVY, 1.f * width / height, ZNEAR, ZFAR));
  _roomSP.unuse();
}

void CubeRoom::_render_laser(float time) const {
  static core::FramebufferHandler fbh;
  static core::TextureHandler<1> texh;
  int offtexid = 0;
 
  /* first, render the lined laser into a framebuffer */
  _laserSP.use();
  _laserTimeIndex.push(time);
  fbh.bind(core::Framebuffer::DRAW, _laserBlurFB[0]);
  core::state::clear(core::state::COLOR_BUFFER | core::state::DEPTH_BUFFER);
  _laser.render(core::primitive::LINE_STRIP, 0, TESS_LASER_LEVEL+1);
  fbh.unbind();
  _laserSP.unuse();

  /* then, blur the lined laser */
  for (int i = 0; i < BLUR_PASSES; ++i) {
    /* first hblur */
    texh.bind(core::Texture::T_2D, _laserBlurOfftex[0]);
    fbh.bind(core::Framebuffer::DRAW, _laserBlurFB[1]);
    core::state::clear(core::state::COLOR_BUFFER | core::state::DEPTH_BUFFER);
    _laserHBlur.start();
    _laserHBlur.apply(0.);
    _laserHBlur.end();
    /* then vblur */
    texh.bind(core::Texture::T_2D, _laserBlurOfftex[1]);
    fbh.bind(core::Framebuffer::DRAW, _laserBlurFB[0]);
    core::state::clear(core::state::COLOR_BUFFER | core::state::DEPTH_BUFFER);
    _laserVBlur.start();
    _laserVBlur.apply(0.);
    _laserVBlur.end();
  }
  fbh.unbind();
  texh.unbind();


  /* add a moving effect on the blurred area
   * hint: the final blurred framebuffer id is 0 */
  fbh.bind(core::Framebuffer::DRAW, _laserBlurFB[1]);
  texh.bind(core::Texture::T_2D, _laserBlurOfftex[0]);
  _laserMove.start();
  _laserMove.apply(time);
  _laserMove.end();
  fbh.unbind();

  /* then render the extremity with billboards */
  
  /* combine the blurred lined moving laser and billboards */
  _fbCopier.copy(_laserBlurOfftex[1]);
}

void CubeRoom::run(float time) const {
  _render_laser(time);
}

