#ifndef __FSM_INIT_HPP
#define __FSM_INIT_HPP

#include <fsm/common.hpp>
#include <lang/primtypes.hpp>
#include <scene/freefly.hpp>
#include <sync/parts_fsm.hpp>

sky::sync::PartState * init_sync(sky::ushort width, sky::ushort height, Common &common, sky::scene::Freefly const &freefly);

#endif /* guard */

