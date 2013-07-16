#include <fsm/cube_room.hpp>
#include <fsm/init.hpp>
#include <fsm/stairway.hpp>

using namespace sky;
using namespace scene;
using namespace sync;

PartState * init_sync(ushort width, ushort height, Freefly const &freefly) {
  auto cubeRoom = new CubeRoom(width, height, freefly);
  auto stairway = new Stairway(width, height, freefly);
  return stairway;
  return cubeRoom;
}

