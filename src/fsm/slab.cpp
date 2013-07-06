#include <core/framebuffer.hpp>
#include <core/renderbuffer.hpp>
#include <fsm/slab.hpp>
#include <misc/from_file.hpp>
#include <tech/post_process.hpp>

using namespace sky;
using namespace core;
using namespace math;
using namespace misc;
using namespace tech;

namespace {
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
}

Slab::Slab(uint width, uint height, float size, float thickness) {
  _init_ibo();
  _init_va();
  _init_texture(width, height);
  _init_program();
  _init_uniforms(size, thickness);
}

void Slab::_init_ibo() {
  gBH.bind(Buffer::ELEMENT_ARRAY, _ibo);
  gBH.data(sizeof(uint)*36, Buffer::STATIC_DRAW, ids);
  gBH.unbind();
}

void Slab::_init_va() {
  _va.bind();
  gBH.bind(Buffer::ELEMENT_ARRAY, _ibo);
  _va.unbind(); /* attribute-less render */
  gBH.unbind();
}

void Slab::_init_texture(uint width, uint height) {
  Framebuffer fb;
  Renderbuffer rb;
  PostProcess generator("slab texture generator", from_file("../../src/fsm/slab_texgen-fs.glsl").c_str(), width, height);

  gRBH.bind(Renderbuffer::RENDERBUFFER, rb);
  gRBH.store(width, height, Texture::IF_DEPTH_COMPONENT);
  gRBH.unbind();

  gTH1.bind(Texture::T_2D, _texture);
  gTH1.parameter(Texture::P_WRAP_S, Texture::PV_CLAMP_TO_EDGE);
  gTH1.parameter(Texture::P_WRAP_T, Texture::PV_CLAMP_TO_EDGE);
  gTH1.parameter(Texture::P_MIN_FILTER, Texture::PV_LINEAR);
  gTH1.parameter(Texture::P_MAG_FILTER, Texture::PV_LINEAR);
  gTH1.image_2D(width, height, 0, Texture::F_RGB, Texture::IF_RGB, GLT_FLOAT, 0, nullptr);
  gTH1.unbind();

  gFBH.bind(Framebuffer::DRAW, fb);
  gFBH.attach_2D_texture(_texture, Framebuffer::COLOR_ATTACHMENT);
  gFBH.attach_renderbuffer(rb, Framebuffer::DEPTH_ATTACHMENT);

  /* make the texture */
  generator.start();
  generator.apply(0.f);
  generator.end();

  gFBH.unbind();
}

void Slab::_init_program() {
  Shader vs(Shader::VERTEX);
  Shader gs(Shader::GEOMETRY);
  Shader fs(Shader::FRAGMENT);

  vs.source(from_file("../../src/fsm/room-vs.glsl").c_str());
  vs.compile("room vertex shader");
  gs.source(from_file("../../src/fsm/room-gs.glsl").c_str());
  gs.compile("room geometry shader");
  fs.source(from_file("../../src/fsm/room-fs.glsl").c_str());
  fs.compile("room fragment shader");

  _sp.attach(vs);
  _sp.attach(gs);
  _sp.attach(fs);
  _sp.link();
}

void Slab::_init_uniforms(float size, float thickness) {
  _projIndex          = _sp.map_uniform("proj");
  _viewIndex          = _sp.map_uniform("view");
  auto sizeIndex      = _sp.map_uniform("size");
  auto thicknessIndex = _sp.map_uniform("thickness");

  _sp.use();
  sizeIndex.push(size);
  thicknessIndex.push(thickness);
  _sp.unuse();
}

void Slab::render(float time, Mat44 const &proj, Mat44 const &view, uint n) const {
  state::enable(state::DEPTH_TEST);

  _sp.use();

  _projIndex.push(proj);
  _viewIndex.push(view);

  gTH1.bind(Texture::T_2D, _texture);
  _va.bind();
  _va.inst_indexed_render(primitive::TRIANGLE, 36, GLT_UINT, n);
  _va.unbind();
  gTH1.unbind();

  _sp.unuse();
}
