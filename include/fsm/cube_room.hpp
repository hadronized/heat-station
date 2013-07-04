#ifndef __FSM_CUBE_ROOM_HPP
#define __FSM_CUBE_ROOM_HPP

#include <fsm/laser.hpp>
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

  Laser _laser;
  Slab _slab;
  Liquid _liquid;

public :
  CubeRoom(sky::ushort width, sky::ushort height, sky::scene::Freefly const &freefly);
  ~CubeRoom(void);

  void run(float time) const override;
};

#endif /* guard */

