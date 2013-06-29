#include <core/context.hpp>
#include <lang/primtypes.hpp>
#include <fsm/init.hpp>
#include <math/common.hpp>
#include <misc/cli.hpp>
#include <misc/log.hpp>
#include <scene/freefly.hpp>
#include <track/synthesizer.hpp>

using namespace sky;
using namespace std;
using namespace misc;

namespace {
  char const TITLE[] = "evoke2013_64k";
}

int main(int argc, char **argv) {
  ushort width, height;
  bool full;
  bool loop;

  if (!scan_cli(argc, argv, width, height, full)) {
    misc::log << error << "CLI misformed" << endl;
    return 1;
  }

  core::Context gl(width, height, full, TITLE);
  track::Synthesizer synth;
  scene::Freefly freefly;
  sync::PartsFSM synchronizer(init_sync(width, height, freefly));

  synth.play("CentralStation.xm");

  loop = true;
  SDL_EnableKeyRepeat(10, 10);
  bool leftClick = false;
  //math::Quat foo(math::Vec3<float>(0.f, 0.f, -1.f), 0.f);
  //foo.normalize();
  //freefly.orientation(foo);
  for (auto time = 0.f; time <= synth.length() && loop; time = synth.cursor()) {
#ifdef SKY_DEBUG /* freefly management */
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_MOUSEMOTION :
          if (leftClick) {
            misc::log << debug << event.motion.xrel << endl;
            freefly.look_around(scene::Orient(math::Vec3<float>(0.f, 1.f, 0.f), 0.5f*event.motion.xrel*math::PI/180.f));
            //freefly.look_around(scene::Orient(math::Vec3<float>(1.f, 0.f, 0.f), 0.5f*event.motion.yrel*math::PI/180.f));
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
              freefly.ward(0.1f, 0.f, 0.f);
              break;

            case SDLK_DOWN :
              freefly.ward(-0.1f, 0.f, 0.f);
              break;

            case SDLK_LEFT :
              freefly.ward(0.f, 0.1f, 0.f);
              break;

            case SDLK_RIGHT :
              freefly.ward(0.f, -0.1f, 0.f);
              break;

            default :;
          }
          break;

        default :;
      }
    }
#endif

    synchronizer.exec(time);
    gl.swap_buffers();
  }

  return 0;
}
