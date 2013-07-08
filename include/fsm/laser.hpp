#ifndef __FSM_LASER_HPP
#define __FSM_LASER_HPP

#include <core/framebuffer.hpp>
#include <core/renderbuffer.hpp>
#include <core/shader.hpp>
#include <core/texture.hpp>
#include <core/vertex_array.hpp>
#include <tech/framebuffer_copy.hpp>
#include <tech/post_process.hpp>

class Laser {
  sky::core::VertexArray _va;
  sky::core::Program _sp;
  sky::core::Program::Uniform _projIndex;
  sky::core::Program::Uniform _viewIndex;
  sky::core::Program::Uniform _timeIndex;
  sky::core::Texture _offtexture[2];
  sky::core::Texture _laserTexture;
  sky::core::Framebuffer _pingpong[2];
  sky::core::Renderbuffer _rb;
  sky::tech::DefaultFramebufferCopy _fbCopier; /* TODO: to move away from here */
#if 0
  sky::tech::PostProcess _hblur;
  sky::tech::PostProcess _vblur;
#endif

  void _init_va(void);
  void _init_program(void);
  void _init_uniforms(sky::ushort tessLvl, float hheight);
  void _init_texture(sky::ushort width, sky::ushort height);

public :
  Laser(sky::ushort width, sky::ushort height, sky::ushort tessLvl, float hheight);
  ~Laser(void) = default;

  void render(float time, sky::math::Mat44 const &proj, sky::math::Mat44 const &view, sky::ushort n) const;
};

#endif /* guard */

