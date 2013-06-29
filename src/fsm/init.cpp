#include <fsm/cube_room.hpp>
#include <fsm/init.hpp>

using namespace sky;
using namespace scene;
using namespace sync;

PartState * init_sync(ushort width, ushort height, Freefly const &freefly) {
  auto cubeRoom = new CubeRoom(width, height, freefly);
  return cubeRoom;
}

