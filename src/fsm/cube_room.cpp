#include <core/state.hpp>
#include <fsm/cube_room.hpp>
#include <math/common.hpp>
#include <math/matrix.hpp>
#include <math/quaternion.hpp>
#include <misc/from_file.hpp>

using namespace sky;
using namespace core;
using namespace math;
using namespace misc;
using namespace tech;

namespace {
  float  const FOVY             = PI*70.f/180.f; /* 90 degrees */
  float  const ZNEAR            = 0.001f;
  float  const ZFAR             = 100.f;
  ushort const TESS_LASER_LEVEL = 13;
  ushort const BLUR_PASSES      = 3;
  float  const LASER_HHEIGHT    = 0.15;
}

/* ============
 * [ CubeRoom ]
 * ============ */
CubeRoom::CubeRoom(ushort width, ushort height) :
    /* common */
    _width(width)
  , _height(height)
  , _fbCopier(width, height)
    /* laser blur */
  , _laserHBlur("laser hblur", from_file("../../src/fsm/laser_hblur-fs.glsl").c_str(), width, height)
  , _laserVBlur("laser vblur", from_file("../../src/fsm/laser_vblur-fs.glsl").c_str(), width, height)
  , _laserMove("laser move", from_file("../../src/fsm/laser_move-fs.glsl").c_str(), width, height)
    /* water */
  , _water("water", from_file("../../src/fsm/water-fs.glsl").c_str(), width, height) {
  /* laser */
  _init_laser_program(width, height);
  _laser.bind();
  _laser.unbind();
  _init_laser_texture(width, height);
  /* room */
  _init_room();
  _init_room_program(width, height);
  _init_room_texture(256, 256);
}

CubeRoom::~CubeRoom() {
}

/* =========
 * [ Laser ]
 * ========= */
void CubeRoom::_init_laser_program(ushort width, ushort height) {
  Shader vs(Shader::VERTEX);
  Shader gs(Shader::GEOMETRY);
  Shader fs(Shader::FRAGMENT);

  /* sources compilation */
  vs.source(from_file("../../src/fsm/laser-vs.glsl").c_str());
  vs.compile("laser VS");
  gs.source(from_file("../../src/fsm/laser-gs.glsl").c_str());
  gs.compile("laser GS");
  fs.source(from_file("../../src/fsm/laser-fs.glsl").c_str());
  fs.compile("laser FS");

  /* program link */
  _laserSP.attach(vs);
  _laserSP.attach(gs);
  _laserSP.attach(fs);
  _laserSP.link();

  _init_laser_uniforms(width, height);
  _init_laser_blur(width, height); 
}

void CubeRoom::_init_laser_uniforms(ushort width, ushort height) {
  auto vnbIndex     = _laserSP.map_uniform("vnb");
  auto hheightIndex = _laserSP.map_uniform("hheight");
  auto laserTex     = _laserSP.map_uniform("lasertex");
  _laserProjIndex   = _laserSP.map_uniform("proj");
  _laserViewIndex   = _laserSP.map_uniform("view");
  _laserTimeIndex   = _laserSP.map_uniform("t");

  _laserSP.use();

  vnbIndex.push(1.f * TESS_LASER_LEVEL, 1.f / TESS_LASER_LEVEL);
  hheightIndex.push(LASER_HHEIGHT);
  laserTex.push(0);

  _laserSP.unuse();
}

void CubeRoom::_init_laser_texture(ushort width, ushort height) {
  Framebuffer fb;
  Renderbuffer rb;
  PostProcess generator("laser texture generator", from_file("../../src/fsm/laser_texgen-fs.glsl").c_str(), width, height);

  gRBH.bind(Renderbuffer::RENDERBUFFER, rb);
  gRBH.store(width, height, Texture::IF_DEPTH_COMPONENT);
  gRBH.unbind();

  gTH1.bind(Texture::T_2D, _laserTexture);
  gTH1.parameter(Texture::P_WRAP_S, Texture::PV_CLAMP_TO_EDGE);
  gTH1.parameter(Texture::P_WRAP_T, Texture::PV_CLAMP_TO_EDGE);
  gTH1.parameter(Texture::P_MIN_FILTER, Texture::PV_LINEAR);
  gTH1.parameter(Texture::P_MAG_FILTER, Texture::PV_LINEAR);
  gTH1.image_2D(width, height, 0, Texture::F_RGB, Texture::IF_RGB, GLT_FLOAT, 0, nullptr);
  gTH1.unbind();

  gFBH.bind(Framebuffer::DRAW, fb);
  gFBH.attach_2D_texture(_laserTexture, Framebuffer::COLOR_ATTACHMENT);
  gFBH.attach_renderbuffer(rb, Framebuffer::DEPTH_ATTACHMENT);

  /* make the texture */
  generator.start();
  generator.apply(0.f);
  generator.end();

  gFBH.unbind();
}

void CubeRoom::_init_laser_blur(ushort width, ushort height) {
  FramebufferHandler fbh;
  RenderbufferHandler rbh;
  TextureHandler<1> texh;

  rbh.bind(Renderbuffer::RENDERBUFFER, _laserBlurRB);
  rbh.store(width, height, Texture::IF_DEPTH_COMPONENT);
  rbh.unbind();

  for (int i = 0; i < 2; ++i) {
    texh.bind(Texture::T_2D, _laserBlurOfftex[i]);
    texh.parameter(Texture::P_WRAP_S, Texture::PV_CLAMP_TO_BORDER);
    texh.parameter(Texture::P_WRAP_T, Texture::PV_CLAMP_TO_BORDER);
    texh.parameter(Texture::P_MIN_FILTER, Texture::PV_LINEAR);
    texh.parameter(Texture::P_MAG_FILTER, Texture::PV_LINEAR);
    texh.image_2D(width, height, 0, Texture::F_RGB, Texture::IF_RGB, GLT_FLOAT, 0, nullptr);
    texh.unbind();

    fbh.bind(Framebuffer::DRAW, _laserBlurFB[i]);
    fbh.attach_renderbuffer(_laserBlurRB, Framebuffer::DEPTH_ATTACHMENT);
    fbh.attach_2D_texture(_laserBlurOfftex[i], Framebuffer::COLOR_ATTACHMENT);
  }
}

void CubeRoom::_render_laser(float time, Mat44 const &proj, Mat44 const &view) const {
  static FramebufferHandler fbh;
  static TextureHandler<1> texh;
  int offtexid = 0;
 
  /* first, render the lined laser into a framebuffer */
  _laserSP.use();

  _laserProjIndex.push(proj);
  _laserViewIndex.push(view);

  _laserTimeIndex.push(time);
  fbh.bind(Framebuffer::DRAW, _laserBlurFB[0]);
  texh.bind(Texture::T_2D, _laserTexture);
  state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);
  state::disable(state::DEPTH_TEST);
  _laser.render(primitive::LINE_STRIP, 0, TESS_LASER_LEVEL+1);
  texh.unbind();
  fbh.unbind();
  _laserSP.unuse();

  /* then, blur the lined laser */
  for (int i = 0; i < BLUR_PASSES; ++i) {
    /* first hblur */
    texh.bind(Texture::T_2D, _laserBlurOfftex[0]);
    fbh.bind(Framebuffer::DRAW, _laserBlurFB[1]);
    state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);
    _laserHBlur.start();
    _laserHBlur.apply(0.);
    _laserHBlur.end();
    /* then vblur */
    texh.bind(Texture::T_2D, _laserBlurOfftex[1]);
    fbh.bind(Framebuffer::DRAW, _laserBlurFB[0]);
    state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);
    _laserVBlur.start();
    _laserVBlur.apply(0.);
    _laserVBlur.end();
  }
  fbh.unbind();
  texh.unbind();

  /* add a moving effect on the blurred area
   * hint: the final blurred framebuffer id is 0 */
#if 0
  fbh.bind(Framebuffer::DRAW, _laserBlurFB[1]);
  texh.bind(Texture::T_2D, _laserBlurOfftex[0]);
  _laserMove.start();
  _laserMove.apply(time);
  _laserMove.end();
  fbh.unbind();
#endif

  /* then render the extremity with billboards */
  
  /* combine the blurred lined moving laser and billboards */
  _fbCopier.copy(_laserBlurOfftex[0]);
}

/* ========
 * [ Room ]
 * ======== */
void CubeRoom::_init_room() {
  uint const ids[] = {
    /* front face */
      0, 1, 2
    , 0, 2, 3
    /* back face */
    , 4, 5, 6
    , 4, 6, 7
    /* up face */
    , 0, 1, 4 
    , 1, 4, 5
    /* bottom face */
    , 2, 3, 7
    , 2, 7, 6
    /* left face */
    , 1, 2, 5
    , 2, 5, 6
    /* right face */
    , 0, 3, 4
    , 3, 4, 7
  };

  /* IBO */
  gBH.bind(Buffer::ELEMENT_ARRAY, _slabIBO);
  gBH.data(sizeof(uint)*36, Buffer::STATIC_DRAW, ids);
  gBH.unbind();

  /* VA */
  _slab.bind();
  gBH.bind(Buffer::ELEMENT_ARRAY, _slabIBO);
  _slab.unbind(); /* attribute-less render */
  gBH.unbind();
}

void CubeRoom::_init_room_program(ushort width, ushort height) {
  Shader vs(Shader::VERTEX);
  Shader gs(Shader::GEOMETRY);
  Shader fs(Shader::FRAGMENT);

  vs.source(from_file("../../src/fsm/room-vs.glsl").c_str());
  vs.compile("room vertex shader");
  gs.source(from_file("../../src/fsm/room-gs.glsl").c_str());
  gs.compile("room geometry shader");
  fs.source(from_file("../../src/fsm/room-fs.glsl").c_str());
  fs.compile("room fragment shader");

  _slabSP.attach(vs);
  _slabSP.attach(gs);
  _slabSP.attach(fs);
  _slabSP.link();

  _init_room_uniforms(width, height);
}

void CubeRoom::_init_room_uniforms(ushort width, ushort height) {
  _slabProjIndex      = _slabSP.map_uniform("proj");
  _slabViewIndex      = _slabSP.map_uniform("view");
  _slabSizeIndex      = _slabSP.map_uniform("size");
  _slabThicknessIndex = _slabSP.map_uniform("thickness");
}

void CubeRoom::_init_room_texture(ushort width, ushort height) {
  Framebuffer fb;
  Renderbuffer rb;
  PostProcess generator("slab texture generator", from_file("../../src/fsm/slab_texgen-fs.glsl").c_str(), width, height);

  gRBH.bind(Renderbuffer::RENDERBUFFER, rb);
  gRBH.store(width, height, Texture::IF_DEPTH_COMPONENT);
  gRBH.unbind();

  gTH1.bind(Texture::T_2D, _slabTexture);
  gTH1.parameter(Texture::P_WRAP_S, Texture::PV_CLAMP_TO_EDGE);
  gTH1.parameter(Texture::P_WRAP_T, Texture::PV_CLAMP_TO_EDGE);
  gTH1.parameter(Texture::P_MIN_FILTER, Texture::PV_LINEAR);
  gTH1.parameter(Texture::P_MAG_FILTER, Texture::PV_LINEAR);
  gTH1.image_2D(width, height, 0, Texture::F_RGB, Texture::IF_RGB, GLT_FLOAT, 0, nullptr);
  gTH1.unbind();

  gFBH.bind(Framebuffer::DRAW, fb);
  gFBH.attach_2D_texture(_slabTexture, Framebuffer::COLOR_ATTACHMENT);
  gFBH.attach_renderbuffer(rb, Framebuffer::DEPTH_ATTACHMENT);

  /* make the texture */
  generator.start();
  generator.apply(0.f);
  generator.end();

  gFBH.unbind();
}

void CubeRoom::_render_room(float time, Mat44 const &proj, Mat44 const &view) const {
  _slabSP.use();
  
  /* move camera around */
  _slabProjIndex.push(proj);
  _slabViewIndex.push(view);

  /* push size and thickness */
  _slabSizeIndex.push(1.f);
  _slabThicknessIndex.push(1.f);
  
  /* render walls */
  /* bind slab texture */
  gTH1.bind(Texture::T_2D, _slabTexture);
  _slab.bind();
  _slab.inst_indexed_render(primitive::TRIANGLE, 36, GLT_UINT, 600);
  _slab.unbind();
  gTH1.unbind();
  
  _slabSP.unuse();
}

/* ============
 * [ CubeRoom ]
 * ============ */
void CubeRoom::run(float time) const {
  static FramebufferHandler fbh;

  /* projection */
  auto proj = Mat44::perspective(FOVY, 1.f * _width / _height, ZNEAR, ZFAR);
  auto view = Mat44::trslt(-Vec3<float>(1.f, cosf(time), sinf(time))*3.f) * Quat(Axis3(0.f, 1.f, 0.f), PI_2).to_matrix();

  /* view */
  fbh.unbind(); /* back to the default framebuffer */

  state::enable(state::DEPTH_TEST);
  state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);

  _render_room(time, proj, view);
  state::enable(state::BLENDING);
  Framebuffer::blend_func(blending::ONE, blending::ONE);
  _render_laser(time, proj, view);
  state::disable(state::BLENDING);
}

