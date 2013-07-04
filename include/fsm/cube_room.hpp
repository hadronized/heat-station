#ifndef __FSM_CUBE_ROOM_HPP
#define __FSM_CUBE_ROOM_HPP

#include <fsm/liquid.hpp>
#include <fsm/slab.hpp>

#include <core/buffer.hpp>
#include <core/framebuffer.hpp>
#include <core/renderbuffer.hpp>
#include <core/shader.hpp>
#include <core/texture.hpp>
#include <core/vertex_array.hpp>
#include <data/subplane.hpp>
#include <lang/primtypes.hpp>
#include <scene/freefly.hpp>
#include <sync/parts_fsm.hpp>
#include <tech/framebuffer_copy.hpp>
#include <tech/post_process.hpp>
#include <tech/deferred_renderer.hpp>

class CubeRoom : public sky::sync::FinalPartState {
  /* common */
  sky::ushort _width, _height;
  sky::tech::DefaultFramebufferCopy _fbCopier;
  sky::scene::Freefly const &_freefly;
  sky::tech::DeferredRenderer _drenderer;
  sky::core::Texture _depthmap, _normalmap, _materialmap;
  sky::scene::Material _matPlastic;

  void _init_materials(void);

  /* laser */
  sky::core::VertexArray _laser;
  sky::core::Program _laserSP;
  void _init_laser_program(sky::ushort width, sky::ushort height);
  sky::core::Program::Uniform _laserTimeIndex;
  sky::core::Program::Uniform _laserProjIndex;
  sky::core::Program::Uniform _laserViewIndex;
  void _init_laser_uniforms(sky::ushort width, sky::ushort height);
  sky::core::Texture _laserTexture;
  void _init_laser_texture(sky::ushort height, sky::ushort width);

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

  void _render_laser(float time, sky::math::Mat44 const &proj, sky::math::Mat44 const &view) const;

  /* room */
  Slab _slab;

  /* liquid */
  Liquid _liquid;

public :
  CubeRoom(sky::ushort width, sky::ushort height, sky::scene::Freefly const &freefly);
  ~CubeRoom(void);

  void run(float time) const override;
};

#endif /* guard */

