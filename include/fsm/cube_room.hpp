#ifndef __FSM_CUBE_ROOM_HPP
#define __FSM_CUBE_ROOM_HPP

#include <lang/primtypes.hpp>
#include <sync/parts_fsm.hpp>
#include <tech/post_process.hpp>

class CubeRoom : public sky::sync::PartState {
  sky::tech::TemporalPostProcess _room;

public :
  CubeRoom(sky::ushort width, sky::ushort height);
  ~CubeRoom(void);

  void run(float time) const;
};

#endif /* guard */

