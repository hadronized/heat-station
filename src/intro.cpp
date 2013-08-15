#include <intro.hpp>
#ifdef SKY_DEBUG
# include <misc/clock.hpp>
# include <misc/log.hpp>
#endif
#include <scene/material.hpp>

/* include fsm parts here */
#include <fsm/cube_room.hpp>
#include <fsm/stairway.hpp>

using namespace sky;
using namespace core;
#ifdef SKY_DEBUG
using namespace misc;
#endif
using namespace scene;
using namespace sync;

Intro::Intro(ushort width, ushort height, bool full, char const *title) :
    _cntxt(width, height, full, title)
  , _com(width, height)
  , _pFSM(nullptr) {
  /* common initialization here */
  _init_materials(width, height);

  /* init parts FSM here */
  auto cubeRoom = new CubeRoom(width, height, _com, _freefly);
  auto stairway = new Stairway(width, height, _com, _freefly);
  cubeRoom->transition(stairway, 80.2f);
  stairway->transition(nullptr, 180.f);
  _pFSM = new PartsFSM(cubeRoom);
}

Intro::~Intro() {
  delete _pFSM;
}

void Intro::_init_materials(ushort width, ushort height) {
  Material matPlastic;

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
  _com.matmgr.register_material(
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
    "return clamp(f, 0., 1.);\n"
  , matPlastic);
  _com.matmgr.register_material( /* terrain material */
    //"return texture(normalmap, get_uv());\n"
    "vec4 terrainColor = vec4(0.4);\n"
    "vec3 no = texture(normalmap, get_uv()).xyz;\n"
    "vec3 co = get_co();\n"
    "vec3 ldir = normalize(lightPos - co);\n"
    "float atten = distance(co, lightPos);\n" /* FIXME: with above line */
    "atten = pow(atten*0.6, 2.);\n"
    "atten = 1. / atten;\n"
    "return (terrainColor+vec4(lightColor, 1.)) * max(0., dot(no, ldir)) * atten;\n"
  );

  _com.matmgr.commit_materials(width, height, matHeader);
}

void Intro::run() {
  bool loop = true;
#ifdef SKY_DEBUG
  bool leftClick = false;
  SDL_Event event;
#endif

  _synth.play("CentralStation.xm");
#ifdef SKY_DEBUG
  _synth.advance_cursor(150.f);
#endif


#ifdef SKY_DEBUG
  Clock clock;
  SDL_EnableKeyRepeat(10, 10);
#endif
  for (auto time = 0.f; !_pFSM->over() && time <= 163.5f && loop; time = _synth.cursor()) {
#ifdef SKY_DEBUG
    clock.reset();
    misc::log << debug << "time: " << time << std::endl;
#endif
    _pFSM->exec(time);
    _cntxt.swap_buffers();

#ifdef SKY_DEBUG /* freefly management */
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_MOUSEMOTION :
          if (leftClick) {
            _freefly.look_around(scene::Orient(math::Vec3<float>(0.f, 1.f, 0.f), 0.5f*event.motion.xrel*math::PI/180.f));
            _freefly.look_around(scene::Orient(math::Vec3<float>(1.f, 0.f, 0.f), 0.5f*event.motion.yrel*math::PI/180.f));
          }
          break;

        case SDL_QUIT :
          loop = false;
          break;

        case SDL_KEYUP :
          if (event.key.keysym.sym == SDLK_ESCAPE)
            loop = false;
          break;

        case SDL_MOUSEBUTTONDOWN :
          switch (event.button.button) {
            case SDL_BUTTON_LEFT :
              leftClick = true;
              break;
          }
          break;

        case SDL_MOUSEBUTTONUP :
          switch (event.button.button) {
            case SDL_BUTTON_LEFT :
              leftClick = false;
              break;
          }
          break;

        case SDL_KEYDOWN :
          switch (event.key.keysym.sym) {
            case SDLK_UP :
              _freefly.ward(0.1f, 0.f, 0.f);
              break;

            case SDLK_DOWN :
              _freefly.ward(-0.1f, 0.f, 0.f);
              break;

            case SDLK_LEFT :
              _freefly.ward(0.f, 0.1f, 0.f);
              break;

            case SDLK_RIGHT :
              _freefly.ward(0.f, -0.1f, 0.f);
              break;

            default :;
          }
          break;

        default :;
      }
    }
#endif
  }
}
