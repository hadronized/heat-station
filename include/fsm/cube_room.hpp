#ifndef __FSM_CUBE_ROOM_HPP
#define __FSM_CUBE_ROOM_HPP

#include <core/vertex_array.hpp>
#include <core/shader.hpp>
#include <lang/primtypes.hpp>
#include <sync/parts_fsm.hpp>
#include <tech/post_process.hpp>

class CubeRoom : public sky::sync::FinalPartState {
  /* laser */
  sky::core::VertexArray _laser;
  sky::core::Program _laserSP;
  void _init_laser_program(void);
  void _init_laser_uniforms(sky::ushort width, sky::ushort height);
  void _render_laser(void) const;

public :
  CubeRoom(sky::ushort width, sky::ushort height);
  ~CubeRoom(void);

  void run(float time) const;
};

#endif /* guard */

