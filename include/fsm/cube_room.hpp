#ifndef __FSM_CUBE_ROOM_HPP
#define __FSM_CUBE_ROOM_HPP

#include <core/buffer.hpp>
#include <core/framebuffer.hpp>
#include <core/renderbuffer.hpp>
#include <core/shader.hpp>
#include <core/texture.hpp>
#include <core/vertex_array.hpp>
#include <lang/primtypes.hpp>
#include <sync/parts_fsm.hpp>
#include <tech/framebuffer_copy.hpp>
#include <tech/post_process.hpp>

class CubeRoom : public sky::sync::FinalPartState {
  /* common */
  sky::ushort _width, _height;
  sky::tech::DefaultFramebufferCopy _fbCopier;

  /* laser */
  sky::core::VertexArray _laser;
  sky::core::Program _laserSP;
  void _init_laser_program(sky::ushort width, sky::ushort height);
  sky::core::Program::Uniform _laserTimeIndex;
  void _init_laser_uniforms(sky::ushort width, sky::ushort height);

  /* laser blur */
  sky::tech::PostProcess _laserHBlur;
  sky::tech::PostProcess _laserVBlur;
  sky::core::Framebuffer _laserBlurFB[2];
  sky::core::Renderbuffer _laserBlurRB;
  sky::core::Texture _laserBlurOfftex[2];
  void _init_laser_blur(sky::ushort width, sky::ushort height);

  /* laser move */
  sky::tech::TemporalPostProcess _laserMove;

  /* laser billboards */
  sky::core::VertexArray _laserBillboard;
  sky::core::Program _laserBillboardSP;

  void _render_laser(float time) const;

  /* room */
  sky::core::VertexArray _slab;
  sky::core::Buffer _slabIBO;
  sky::core::Program _slabSP;
  sky::core::Program::Uniform _slabSizeIndex;
  sky::core::Program::Uniform _slabThicknessIndex;
  void _init_room(void);
  void _init_room_program(sky::ushort width, sky::ushort height);
  void _init_room_uniforms(sky::ushort width, sky::ushort height);
  
  void _render_room(float time) const;

public :
  CubeRoom(sky::ushort width, sky::ushort height);
  ~CubeRoom(void);

  void run(float time) const override;
};

#endif /* guard */

