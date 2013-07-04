#ifndef __FSM_SLAB_HPP
#define __FSM_SLAB_HPP

#include <core/buffer.hpp>
#include <core/shader.hpp>
#include <core/texture.hpp>
#include <core/vertex_array.hpp>

class Slab {
  sky::core::Buffer _ibo;
  sky::core::VertexArray _va;
  sky::core::Texture _texture;
  sky::core::Program _sp;
  sky::core::Program::Uniform _projIndex;
  sky::core::Program::Uniform _viewIndex;

  void _init_ibo(void);
  void _init_va(void);
  void _init_texture(uint width, uint height);
  void _init_program(void);
  void _init_uniforms(float size, float thickness);

public :
  Slab(uint width, uint height, float size, float thickness);
  ~Slab(void) = default;

  void render(float time, sky::math::Mat44 const &proj, sky::math::Mat44 const &view, sky::uint n) const;
};

#endif /* guard */

