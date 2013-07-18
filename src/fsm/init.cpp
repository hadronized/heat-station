#include <fsm/cube_room.hpp>
#include <fsm/init.hpp>
#include <fsm/stairway.hpp>

using namespace sky;
using namespace scene;
using namespace sync;

void init_materials(MaterialManager &matmgr) {
}

PartState * init_sync(ushort width, ushort height, Common &common, Freefly const &freefly) {
  init_materials(common.matmgr);

  //auto cubeRoom = new CubeRoom(width, height, freefly);
  auto stairway = new Stairway(width, height, common, freefly);
  return stairway;
}

