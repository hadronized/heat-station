#include <core/state.hpp>
#include <core/viewport.hpp>
#include <fsm/cube_room.hpp>
#include <math/common.hpp>
#include <math/matrix.hpp>
#include <math/quaternion.hpp>
#include <misc/log.hpp>
#include <misc/from_file.hpp>

using namespace std;
using namespace sky;
using namespace core;
using namespace math;
using namespace misc;
using namespace scene;
using namespace tech;

namespace {
  float  const FOVY             = PI*70.f/180.f; /* 90 degrees */
  float  const ZNEAR            = 0.0001f;
  float  const ZFAR             = 100.f;
  ushort const LASER_TESS_LEVEL = 13;
  float  const LASER_HHEIGHT    = 0.15f;
  float  const SLAB_SIZE        = 1.f;
  float  const SLAB_THICKNESS   = 0.5f;
  uint   const SLAB_INSTANCES   = 600;
  ushort const LIQUID_WIDTH     = 10;
  ushort const LIQUID_HEIGHT    = 10;
  ushort const LIQUID_TWIDTH    = 80;
  ushort const LIQUID_THEIGHT   = 80;
  ushort const LIQUID_RES       = LIQUID_TWIDTH * LIQUID_THEIGHT;
}

CubeRoom::CubeRoom(ushort width, ushort height, Freefly const &freefly) :
    /* common */
    _width(width)
  , _height(height)
  , _fbCopier(width, height)
  , _freefly(freefly)
  , _drenderer(width, height, _depthmap, _normalmap, _materialmap)
  , _slab(width, height, SLAB_SIZE, SLAB_THICKNESS)
  , _liquid(LIQUID_WIDTH, LIQUID_HEIGHT, LIQUID_TWIDTH, LIQUID_THEIGHT)
  , _laser(width, height, LASER_TESS_LEVEL, LASER_HHEIGHT) {
  _init_materials(width, height);
}

void CubeRoom::_init_materials(ushort width, ushort height) {
  char const *matHeader =
    "uniform mat4 proj;\n"
    "uniform mat4 view;\n"
    "uniform vec3 lightColor;\n"
    "uniform vec3 lightPos;\n"
    
    "vec2 get_uv() {\n"
      "return gl_FragCoord.xy*res.zw;\n"
    "}\n"
    "vec3 get_co() {\n"
      "vec2 uv = get_uv();\n"
      "vec4 p = inverse(proj * view) * vec4(2. * vec3(uv, texture(depthmap, uv).r) - 1., 1.);\n"
      "return p.xyz/p.w;\n"
    "}\n"
    "vec3 get_eye() {\n"
      "return inverse(proj)[3].xyz;\n"
    "}\n";
  _matmgr.register_material(
    "vec3 no = normalize(texture(normalmap, get_uv()).xyz);\n"
    "vec3 co = get_co();\n"
    "vec4 matColor;// = texture(propmap, get_uv());\n"
    "matColor = vec4(.4);\n"
    "vec3 ldir = vec3((lightPos - co).xy, 0.);\n"
    "vec3 nldir = normalize(ldir);\n"
    "vec3 eyedir = normalize(get_eye() - co);\n"
    "vec3 h = ldir + eyedir;\n"
    "vec3 nh = normalize(h);\n"
    "float diffk = max(0., dot(nldir, no));\n"
    "float bspeck = pow(dot(nh,no), 10.);\n" /* blinn-phong */
    
    "vec4 mixedColor = matColor + vec4(lightColor, 1.);\n"
    "vec4 f = mixedColor * diffk;\n"
    "f += mixedColor * bspeck;\n"
    "f /= pow(length(ldir)*0.5, 2.);\n"
    "return f;\n"
  , _matPlastic);

  _matmgr.commit_materials(width, height, matHeader);
  
  _matmgrProjIndex   = _matmgr.postprocess().program().map_uniform("proj");
  _matmgrViewIndex   = _matmgr.postprocess().program().map_uniform("view");
  _matmgrLColorIndex = _matmgr.postprocess().program().map_uniform("lightColor");
  _matmgrLPosIndex   = _matmgr.postprocess().program().map_uniform("lightPos");
}

void CubeRoom::run(float time) const {
  /* projection & view */
  auto proj = Mat44::perspective(FOVY, 1.f * _width / _height, ZNEAR, ZFAR);
  auto const &view = _freefly.view();

  /* viewport */
  //viewport(0, _height / 2, _width / 2, _height / 2);

  _drenderer.start_geometry();
  state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);
  _slab.render(time, proj, view, SLAB_INSTANCES);
  _liquid.render(time, proj, view, LIQUID_RES);
  _drenderer.end_geometry();

  state::clear(state::COLOR_BUFFER | state::DEPTH_BUFFER);

  _drenderer.start_shading();
  _matmgr.start();

  _matmgrProjIndex.push(proj);
  _matmgrViewIndex.push(view);
  _matmgrLColorIndex.push(0.75f, 0.f, 0.f);
  _matmgrLPosIndex.push(0.f, 0.f, 0.f);
  _matmgr.render();

  _matmgr.end();
  _drenderer.end_shading();

  _laser.render(time, proj, view, LASER_TESS_LEVEL);
}

