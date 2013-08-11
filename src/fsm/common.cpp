#include <font.hpp>
#include <fsm/common.hpp>

using namespace sky;
using namespace core;
using namespace scene;
using namespace tech;

Common::Common(ushort width, ushort height) :
    drenderer(width, height)
  , stringRenderer(width, height, GLPH_index, 75, '0') {
}
