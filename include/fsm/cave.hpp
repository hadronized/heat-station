#ifndef __FSM_CAVE_HPP
#define __FSM_CAVE_HPP

#include <core/shader.hpp>
#include <core/texture.hpp>
#include <data/subplane.hpp>
#include <lang/primtypes.hpp>

class Cave {
  sky::data::SubPlane _plane;
  sky::core::Texture *_pTexture[2];
  sky::core::Program _sp;
  sky::core::Program::Uniform _projIndex;
  sky::core::Program::Uniform _viewIndex;
  sky::core::Program::Uniform _timeIndex;

  void _init_textures(sky::uint width, sky::uint height);
  void _init_program(void);
  void _init_uniforms(void);

public :
  Cave(void);
  ~Cave(void);

  void render(float time, sky::math::Mat44 const &proj, sky::math::Mat44 const &view) const;
};

#endif

