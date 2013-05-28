#include <fsm/cube_room.hpp>
#include <misc/from_file.hpp>

using namespace sky;

CubeRoom::CubeRoom(ushort width, ushort height) :
    _room("CubeRoom", misc::from_file("../../shaders/cuberoom-fs.glsl").c_str(), width, height) {
}

CubeRoom::~CubeRoom() {
}

void CubeRoom::run(float time) const {
  _room.start();
  _room.apply(time);
  _room.start();
}
