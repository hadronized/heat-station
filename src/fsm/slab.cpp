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
  char const *ROOM_VS_SRC =
"#version 330 core\n"

"out vec3 vco;" /* space coordinates as vertex shader output */
"out vec3 vno;" /* normal as vertex shader output */

"uniform float size;"      /* size of the slab */
"uniform float thickness;" /* thickness of the slab: 0. = 0., 1. = size */
"uniform float t;"

/* slab vertices */
"const float margin=0.05;"
"float offset=size+margin;"
"float size_2=size/2.;"
"float depth_2=size_2*thickness;"
"vec3[8] v=vec3[]("
    "vec3(-size_2,size_2,depth_2)"
  ", vec3(size_2,size_2,depth_2)"
  ", vec3(size_2,-size_2,depth_2)"
  ", vec3(-size_2,-size_2,depth_2)"
  ", vec3(-size_2,size_2,-depth_2)"
  ", vec3(size_2,size_2,-depth_2)"
  ", vec3(size_2,-size_2,-depth_2)"
  ", vec3(-size_2,-size_2,-depth_2)"
");"

"void main(){"
  "float foo=offset*10.;"
  "float c=(10*size+9*margin)*0.5;"
  "vco=v[gl_VertexID];"

  "if(gl_InstanceID<100){"
    "vco+=vec3(mod(gl_InstanceID,10)*offset,floor(gl_InstanceID/10)*offset,0)-c;"
  "}else if(gl_InstanceID<200){"
    "vco+=vec3(mod(gl_InstanceID-100,10)*offset,floor((gl_InstanceID-100)/10)*offset,foo)-c;"
  "}else if(gl_InstanceID<300){"
    "vco+=vec3(0.,floor((gl_InstanceID-200)/10)*offset,mod(gl_InstanceID-200,10)*offset)-c;"
  "}else if(gl_InstanceID<400){"
    "vco+=vec3(foo,floor((gl_InstanceID-300)/10)*offset,mod(gl_InstanceID-300,10)*offset)-c;"
  "}else if(gl_InstanceID<500){"
    "vco+=vec3(mod(gl_InstanceID-400,10)*offset,0.,floor((gl_InstanceID-400)/10)*offset)-c;"
  "}else{"
    "vco+=vec3(mod(gl_InstanceID-500,10)*offset,foo,floor((gl_InstanceID-500)/10)*offset)-c;"
  "}"
"}";
  char const *ROOM_GS_SRC =
"#version 330 core\n"

"layout(triangles)in;"
"layout(triangle_strip,max_vertices=3)out;"

"in vec3 vco[];"
"in vec3 vno[];"

"out vec3 gco;"
"out vec3 gno;"
"out vec2 guv;"

"uniform mat4 proj;"
"uniform mat4 view;"

"void emit(int i){"
  "gco=vco[i];"
  "gl_Position=proj*view*vec4(gco,1.);"
  "EmitVertex();"
"}"

"void main(){"
  /* cube face ID */
  "int faceID=gl_PrimitiveIDIn/2;"

  /* compute normal and UV coordinates */
  "if(faceID<2){"
    "gno=vec3(0.,0.,1.);"
  "}else if(faceID<4){"
    "gno=vec3(0.,1.,0.);"
  "}else{"
    "gno=vec3(1.,0.,0.);"
  "}"
  "if((faceID&1)==1){"
    "gno=-gno;"
  "}"

  /* emit the vertices */
  "for(int i=0;i<3;++i)"
    "emit(i);"
"}";
  char const *ROOM_FS_SRC =
"#version 330 core\n"

"in vec3 gco;" /* vertex shader space coordinates */
"in vec3 gno;" /* vertex shader normal */

"layout(location=0)out vec3 nofrag;"
"layout(location=1)out uvec2 matfrag;"

"void main(){"
  "nofrag=gno;"
  "matfrag=uvec2(1,0);"
"}";
}

Slab::Slab(uint width, uint height, float size, float thickness) {
  _init_ibo();
  _init_va();
  //_init_texture(width, height);
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

#if 0
void Slab::_init_texture(uint width, uint height) {
  Framebuffer fb;
  Renderbuffer rb;
  PostProcess generator("slab texture generator", from_file("../../src/fsm/slab_texgen-fs.glsl").c_str(), width, height);

  gRBH.bind(Renderbuffer::RENDERBUFFER, rb);
  gRBH.store(width, height, Texture::IF_DEPTH_COMPONENT);
  gRBH.unbind();

  gTH.bind(Texture::T_2D, _texture);
  gTH.parameter(Texture::P_WRAP_S, Texture::PV_CLAMP_TO_EDGE);
  gTH.parameter(Texture::P_WRAP_T, Texture::PV_CLAMP_TO_EDGE);
  gTH.parameter(Texture::P_MIN_FILTER, Texture::PV_LINEAR);
  gTH.parameter(Texture::P_MAG_FILTER, Texture::PV_LINEAR);
  gTH.image_2D(width, height, 0, Texture::F_RGB, Texture::IF_RGB, GLT_FLOAT, 0, nullptr);
  gTH.unbind();

  gFBH.bind(Framebuffer::DRAW, fb);
  gFBH.attach_2D_texture(_texture, Framebuffer::COLOR_ATTACHMENT);
  gFBH.attach_renderbuffer(rb, Framebuffer::DEPTH_ATTACHMENT);

  /* make the texture */
  generator.start();
  generator.apply(0.f);
  generator.end();

  gFBH.unbind();
}
#endif

void Slab::_init_program() {
  Shader vs(Shader::VERTEX);
  Shader gs(Shader::GEOMETRY);
  Shader fs(Shader::FRAGMENT);

  vs.source(ROOM_VS_SRC);
  vs.compile("room vertex shader");
  gs.source(ROOM_GS_SRC);
  gs.compile("room geometry shader");
  fs.source(ROOM_FS_SRC);
  fs.compile("room fragment shader");

  _sp.attach(vs);
  _sp.attach(gs);
  _sp.attach(fs);
  _sp.link();
}

void Slab::_init_uniforms(float size, float thickness) {
  _projIndex          = _sp.map_uniform("proj");
  _viewIndex          = _sp.map_uniform("view");
  _timeIndex          = _sp.map_uniform("t");
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
  _timeIndex.push(time);

  gTH.bind(Texture::T_2D, _texture);
  _va.bind();
  _va.inst_indexed_render(primitive::TRIANGLE, 36, GLT_UINT, n);
  _va.unbind();
  gTH.unbind();

  _sp.unuse();
}
