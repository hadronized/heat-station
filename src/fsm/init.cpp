#include <fsm/cube_room.hpp>
#include <fsm/init.hpp>

using namespace sky;
using namespace sync;

PartState * init_sync(ushort width, ushort height) {
  auto cubeRoom = new CubeRoom(width, height);
  return nullptr;
}

