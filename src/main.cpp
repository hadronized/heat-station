#include <core/context.hpp>
#include <lang/primtypes.hpp>
#include <fsm/init.hpp>
#include <misc/cli.hpp>
#include <misc/log.hpp>
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

  if (!scan_cli(argc, argv, width, height, full)) {
    log << error << "CLI misformed" << endl;
    return 1;
  }

  core::Context gl(width, height, full, TITLE);
  track::Synthesizer synth;
  sync::PartsFSM synchronizer(init_sync(width, height));

  synth.play("CentralStation.xm");

  for (auto time = 0.f; time <= synth.length() && gl.treat_events(); time = synth.cursor()) {
    synchronizer.exec(time);
    gl.swap_buffers();
  }

  return 0;
}
