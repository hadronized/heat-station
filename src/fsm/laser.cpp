#include <core/renderbuffer.hpp>
#include <fsm/laser.hpp>
#include <misc/from_file.hpp>
#include <misc/log.hpp>

using namespace sky;
using namespace core;
using namespace math;
using namespace misc;
using namespace tech;

namespace {
  ushort const TEXTURE_WIDTH  = 256;
  ushort const TEXTURE_HEIGHT = 256;
}

Laser::Laser(ushort width, ushort height, ushort tessLvl, float hheight) :
    _fbCopier(width, height) {
  _init_va();
  _init_program();
  _init_uniforms(tessLvl, hheight);
  _init_texture(width, height);
}

void Laser::_init_va() {
  _va.bind();
  _va.unbind();
}

void Laser::_init_program() {
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
  _sp.attach(vs);
  _sp.attach(gs);
  _sp.attach(fs);
  _sp.link();
}

void Laser::_init_uniforms(ushort tessLvl, float hheight) {
  auto vnbIndex     = _sp.map_uniform("vnb");
  auto hheightIndex = _sp.map_uniform("hheight");
  auto laserTex     = _sp.map_uniform("lasertex");
  _projIndex        = _sp.map_uniform("proj");
  _viewIndex        = _sp.map_uniform("view");
  _timeIndex        = _sp.map_uniform("t");

  _sp.use();

  vnbIndex.push(1.f * tessLvl, 1.f / tessLvl);
  hheightIndex.push(hheight);
  laserTex.push(0);

  _sp.unuse();
}

void Laser::_init_texture(ushort width, ushort height) {
  Framebuffer fb; /* FIXME: it may not be needed at all */
  Renderbuffer rb;
  PostProcess generator("laser texture generator", from_file("../../src/fsm/laser_texgen-fs.glsl").c_str(), TEXTURE_WIDTH, TEXTURE_HEIGHT);

  /* laser texture */
  gRBH.bind(Renderbuffer::RENDERBUFFER, rb);
  gRBH.store(TEXTURE_WIDTH, TEXTURE_HEIGHT, Texture::IF_DEPTH_COMPONENT);
  gRBH.unbind();

  gTH.bind(Texture::T_2D, _laserTexture);
  gTH.parameter(Texture::P_WRAP_S, Texture::PV_CLAMP_TO_EDGE);
  gTH.parameter(Texture::P_WRAP_T, Texture::PV_CLAMP_TO_EDGE);
  gTH.parameter(Texture::P_MIN_FILTER, Texture::PV_LINEAR);
  gTH.parameter(Texture::P_MAG_FILTER, Texture::PV_LINEAR);
  gTH.image_2D(TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, Texture::F_RGB, Texture::IF_RGB, GLT_FLOAT, 0, nullptr);
  gTH.unbind();

  gFBH.bind(Framebuffer::DRAW, fb);
  gFBH.attach_renderbuffer(rb, Framebuffer::DEPTH_ATTACHMENT);
  gFBH.attach_2D_texture(_laserTexture, Framebuffer::COLOR_ATTACHMENT);

  generator.start();
  generator.apply(0.f);
  generator.end();

  /* ping pong render textures */
  gRBH.bind(Renderbuffer::RENDERBUFFER, _rb);
  gRBH.store(width, height, Texture::IF_DEPTH_COMPONENT);
  gRBH.unbind();

  for (short i = 0; i < 2; ++i) {
    gTH.bind(Texture::T_2D, _offtexture[i]);
    gTH.parameter(Texture::P_WRAP_S, Texture::PV_CLAMP_TO_BORDER);
    gTH.parameter(Texture::P_WRAP_T, Texture::PV_CLAMP_TO_BORDER);
    gTH.parameter(Texture::P_MIN_FILTER, Texture::PV_LINEAR);
    gTH.parameter(Texture::P_MAG_FILTER, Texture::PV_LINEAR);
    gTH.image_2D(width, height, 0, Texture::F_RGB, Texture::IF_RGB, GLT_FLOAT, 0, nullptr);
    gTH.unbind();

    gFBH.bind(Framebuffer::DRAW, _pingpong[i]);
    gFBH.attach_renderbuffer(_rb, Framebuffer::DEPTH_ATTACHMENT);
    gFBH.attach_2D_texture(_offtexture[i], Framebuffer::COLOR_ATTACHMENT);
  }

  gFBH.unbind();
}

void Laser::render(float time, Mat44 const &proj, Mat44 const &view, ushort n) const {
  int offtexid = 0;
 
  state::disable(state::DEPTH_TEST);
  state::enable(state::BLENDING);
  Framebuffer::blend_func(blending::ONE, blending::ONE);

  _sp.use();

  _projIndex.push(proj);
  _viewIndex.push(view);
  _timeIndex.push(time);

  gFBH.bind(Framebuffer::DRAW, _pingpong[0]);
  gTH.unit(0);
  gTH.bind(Texture::T_2D, _laserTexture);
  state::clear(state::COLOR_BUFFER);
  _va.render(primitive::LINE_STRIP, 0, n+1);
  gTH.unbind();
  gFBH.unbind();

  _sp.unuse();

#if 0
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
#endif

  /* combine the blurred lined moving laser and billboards */
  _fbCopier.copy(_offtexture[0]);
  state::disable(state::BLENDING);
  state::enable(state::DEPTH_TEST);
}

#if 0
, _laserHBlur("laser hblur", from_file("../../src/fsm/laser_hblur-fs.glsl").c_str(), width, height)
  , _laserVBlur("laser vblur", from_file("../../src/fsm/laser_vblur-fs.glsl").c_str(), width, height)
  , _laserMove("laser move", from_file("../../src/fsm/laser_move-fs.glsl").c_str(), width, height)
#endif

