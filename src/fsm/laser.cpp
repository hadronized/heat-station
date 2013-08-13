#include <core/renderbuffer.hpp>
#include <fsm/laser.hpp>
#include <misc/log.hpp>

using namespace sky;
using namespace core;
using namespace math;
using namespace misc;
using namespace tech;

namespace {
  ushort const TEXTURE_WIDTH  = 256;
  ushort const TEXTURE_HEIGHT = 256;
  ushort const BLUR_PASSES    = 1;
  char const *LASER_HBLUR_FS_SRC =
"#version 330 core\n"

"uniform sampler2D srctex;"
"uniform vec4 res;"

"out vec4 frag;"

"void main(){"
  "vec2 uv=gl_FragCoord.xy*res.zw;"
  "vec2 st=vec2(res.z*3.,0.);"

  "frag=texture2D(srctex,uv-12.*st)*0.15"
       "+texture2D(srctex,uv-9.*st)*0.25"
       "+texture2D(srctex,uv-st)*0.35"
       "+texture2D(srctex,uv)*0.50"
       "+texture2D(srctex,uv+st)*0.35"
       "+texture2D(srctex,uv+9.*st)*0.25"
       "+texture2D(srctex,uv+12.*st)*0.15"
       ";"
"}";
  char const *LASER_VBLUR_FS_SRC =
"#version 330 core\n"

"uniform sampler2D srctex;"
"uniform vec4 res;"

"out vec4 frag;"

"void main(){"
  "vec2 uv=gl_FragCoord.xy*res.zw;"
  "vec2 st=vec2(0.,res.z*3.);"

  "frag=texture2D(srctex,uv-12.*st)*0.15"
       "+texture2D(srctex,uv-9.*st)*0.25"
       "+texture2D(srctex,uv-st)*0.35"
       "+texture2D(srctex,uv)*0.50"
       "+texture2D(srctex,uv+st)*0.35"
       "+texture2D(srctex,uv+9.*st)*0.25"
       "+texture2D(srctex,uv+12.*st)*0.15"
       ";"
"}";
  char const *LASER_VS_SRC =
"#version 330 core\n"

"out vec3 vCo;"

"uniform vec2 vnb;" /* vertices nb; 1 / vertices nb */
"uniform float t;"

"const float PI=3.141592;"

"void main(){"
  /* laser */
  "float d=10.;"
  "float d2=d*0.5;"
  "vCo=vec3(0.,0.,gl_VertexID*vnb.y*d-d2);"

  /* displacement */
  "vCo.y+=sin(vCo.z*2.*PI+t*20.)/30.;"
  "vCo.y+=clamp(tan(vCo.z+t)/300.,-2.,2.);"
"}";
  char const *LASER_GS_SRC =
"#version 330 core\n"

"layout(lines)in;"
"layout(triangle_strip,max_vertices=16)out;"

"in vec3 vCo[];" /* vertex shader coordinates */

"out vec2 gUV;" /* geometry shader texture UV */
"out float gHeight;"

"uniform float hheight;" /* half height of each plane */
"uniform mat4 proj;"
"uniform mat4 view;"

"void emit(vec3 a){"
  "gl_Position=proj*view*vec4(a,1.);"
  "gHeight=a.y;"
  "EmitVertex();"
"}"

"void emit_plane(vec3 a,vec3 b,vec3 c,vec3 d){"
  "gUV=vec2(0.,0.);"
  "emit(a);"

  "gUV=vec2(1.,0.);"
  "emit(b);"

  "gUV=vec2(0.,1.);"
  "emit(c);"

  "gUV=vec2(1.,1.);"
  "emit(d);"

  "EndPrimitive();"
"}"

"void main(){"
  /* hplane */
  "vec3 a=vec3(vCo[0].x-hheight,vCo[0].yz);"
  "vec3 b=vec3(vCo[0].x+hheight,vCo[0].yz);"
  "vec3 c=vec3(vCo[1].x-hheight,vCo[1].yz);"
  "vec3 d=vec3(vCo[1].x+hheight,vCo[1].yz);"
  "emit_plane(a,b,c,d);"

  /* vplane */
  "a=vec3(vCo[0].x,vCo[0].y-hheight,vCo[0].z);"
  "b=vec3(vCo[0].x,vCo[0].y+hheight,vCo[0].z);"
  "c=vec3(vCo[1].x,vCo[1].y-hheight,vCo[1].z);"
  "d=vec3(vCo[1].x,vCo[1].y+hheight,vCo[1].z);"
  "emit_plane(a,b,c,d);"

  /* lplane */
  "a=vec3(vCo[0].x-hheight,vCo[0].y+hheight,vCo[0].z);"
  "b=vec3(vCo[0].x+hheight,vCo[0].y-hheight,vCo[0].z);"
  "c=vec3(vCo[1].x-hheight,vCo[1].y+hheight,vCo[1].z);"
  "d=vec3(vCo[1].x+hheight,vCo[1].y-hheight,vCo[1].z);"
  "emit_plane(a,b,c,d);"

  /* rplane */
  "a=vec3(vCo[0].x-hheight,vCo[0].y-hheight,vCo[0].z);"
  "b=vec3(vCo[0].x+hheight,vCo[0].y+hheight,vCo[0].z);"
  "c=vec3(vCo[1].x-hheight,vCo[1].y-hheight,vCo[1].z);"
  "d=vec3(vCo[1].x+hheight,vCo[1].y+hheight,vCo[1].z);"
  "emit_plane(a,b,c,d);"
"}";
  char const *LASER_FS_SRC =
"#version 330 core\n"

"in vec2 gUV;"
"in float gHeight;"

"out vec4 frag;"

"uniform sampler2D lasertex;"

"void main(){"
  "frag=texture(lasertex,gUV)+vec4(0.,0.,1.,1.)*gHeight/2.;"
"}";
  char const *LASER_TEX_GEN_FS_SRC =
"#version 330 core\n"

"out vec4 frag;"

"uniform vec4 res;"
"uniform float t;"

"const float PI=3.14159265359;"

"float laser_tex(vec2 uv,float i){"
  "return pow(sin(uv.x*PI),i);"
"}"

"float rand(vec2 a){"
  "return sin(dot(a.xy,vec2(12.9898,78.233)))*43758.5453;"
"}"

"void main(){"
  "vec2 uv=gl_FragCoord.xy*res.zw;"
  "vec4 laserColor=vec4(0.75,0.,0.,1.);"
  "vec4 laserCoreColor=vec4(1.);"

  "frag=laserColor*laser_tex(uv,4.);"
  "frag+=laserCoreColor*laser_tex(uv,24.)*0.42;"
"}";
}

Laser::Laser(ushort width, ushort height, ushort tessLvl, float hheight) :
    _fbCopier(width, height)
  , _hblur("laser hblur", LASER_HBLUR_FS_SRC, width, height)
  , _vblur("laser vblur", LASER_VBLUR_FS_SRC, width, height) {
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
  vs.source(LASER_VS_SRC);
  vs.compile("laser VS");
  gs.source(LASER_GS_SRC);
  gs.compile("laser GS");
  fs.source(LASER_FS_SRC);
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
  PostProcess generator("laser texture generator", LASER_TEX_GEN_FS_SRC, TEXTURE_WIDTH, TEXTURE_HEIGHT);

  /* laser texture */
  gRBH.bind(Renderbuffer::RENDERBUFFER, rb);
  gRBH.store(TEXTURE_WIDTH, TEXTURE_HEIGHT, Texture::IF_DEPTH_COMPONENT32F);
  gRBH.unbind();

  gTH.bind(Texture::T_2D, _laserTexture);
  gTH.parameter(Texture::P_WRAP_S, Texture::PV_CLAMP_TO_EDGE);
  gTH.parameter(Texture::P_WRAP_T, Texture::PV_CLAMP_TO_EDGE);
  gTH.parameter(Texture::P_MIN_FILTER, Texture::PV_LINEAR);
  gTH.parameter(Texture::P_MAG_FILTER, Texture::PV_LINEAR);
  gTH.image_2D(TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, Texture::F_RGB, Texture::IF_RGB32F, GLT_FLOAT, 0, nullptr);
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
    gTH.image_2D(width, height, 0, Texture::F_RGB, Texture::IF_RGB32F, GLT_FLOAT, 0, nullptr);
    gTH.unbind();

    gFBH.bind(Framebuffer::DRAW, _pingpong[i]);
    gFBH.attach_renderbuffer(_rb, Framebuffer::DEPTH_ATTACHMENT);
    gFBH.attach_2D_texture(_offtexture[i], Framebuffer::COLOR_ATTACHMENT);
    gFBH.unbind();
  }

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

  /* then, blur the lined laser */
  for (int i = 0; i < BLUR_PASSES; ++i) {
    /* first hblur */
    gTH.bind(Texture::T_2D, _offtexture[0]);
    gFBH.bind(Framebuffer::DRAW, _pingpong[1]);
    state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);
    _hblur.start();
    _hblur.apply(0.);
    _hblur.end();
    gFBH.lazy_unbind();
    /* then vblur */
    gTH.bind(Texture::T_2D, _offtexture[1]);
    gFBH.bind(Framebuffer::DRAW, _pingpong[0]);
    state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);
    _vblur.start();
    _vblur.apply(0.);
    _vblur.end();
    gFBH.unbind();
  }

  /* combine the blurred lined moving laser and billboards */
  _fbCopier.copy(_offtexture[0]);
  state::disable(state::BLENDING);
}

