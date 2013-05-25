#include <lang/primtypes.hpp>
#include <misc/cli.hpp>
#include <misc/log.hpp>
#include <track/synthesizer.hpp>

using namespace sky;
using namespace std;
using namespace misc;
using namespace track;

int main(int argc, char **argv) {
  ushort width, height;
  bool full;

  if (!scan_cli(argc, argv, width, height, full)) {
    log << error << "CLI misformed" << endl;
    return 1;
  }

  /* allocate the synthesizer */
  Synthesizer synth;
  synth.play("CentralStation.xm");

  while (1) {};


  return 0;
}
