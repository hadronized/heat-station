#include <lang/primtypes.hpp>
#include <intro.hpp>
#include <misc/cli.hpp>
#include <misc/log.hpp>

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

  SDL_EnableKeyRepeat(10, 10);

  Intro intro(width, height, full, TITLE);
  intro.run();

  return 0;
}
