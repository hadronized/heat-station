#ifndef __FSM_FIREFLIES_HPP
#define __FSM_FIREFLIES_HPP

#include <core/buffer.hpp>
#include <core/shader.hpp>
#include <core/vertex_array.hpp>
#include <math/matrix.hpp>
#include <scene/common.hpp>

class Fireflies {
public :
  static int const FIREFLIES_NB = 20;

private :
  sky::scene::Position _pos[FIREFLIES_NB];
  sky::math::Vec3<float> _colors[FIREFLIES_NB];
  sky::core::Buffer _vbo;
  sky::core::VertexArray _va;
  sky::core::Program _sp;
  sky::core::Program::Uniform _projIndex;
  sky::core::Program::Uniform _viewIndex;

  void _init_fireflies(void);
  void _init_shader(void);
  void _init_uniforms(void);

public :
  Fireflies(void);
  ~Fireflies(void) = default;

  sky::scene::Position const * positions(void) const;
  sky::math::Vec3<float> const * colors(void) const;
  void render(sky::math::Mat44 const &proj, sky::math::Mat44 const &view) const;
  void animate(float time);
};

#endif /* guard */

