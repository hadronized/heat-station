#ifndef __FSM_LIQUID_HPP
#define __FSM_LIQUID_HPP

#include <core/shader.hpp>
#include <data/subplane.hpp>

class Liquid {
  sky::data::SubPlane _plane;
  sky::core::Program _sp;
  sky::core::Program::Uniform _projIndex;
  sky::core::Program::Uniform _viewIndex;
  sky::core::Program::Uniform _timeIndex;

  void _init_program(void);
  void _init_uniforms(void);

public :
  Liquid(sky::uint width, sky::uint height, sky::uint twidth, sky::uint theight);
  ~Liquid(void) = default;

  void render(float time, sky::math::Mat44 const &proj, sky::math::Mat44 const &view, sky::uint n) const;
};

#endif /* guard */

