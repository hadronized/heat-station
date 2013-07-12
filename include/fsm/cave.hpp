#ifndef __FSM_CAVE_HPP
#define __FSM_CAVE_HPP

#include <core/shader.hpp>
#include <core/texture.hpp>
#include <data/subplane.hpp>
#include <lang/primtypes.hpp>

class Cave {
  sky::data::SubPlane _plane;
  sky::core::Texture *_pTexture[5];
  sky::core::Program _sp;

  void _init_textures(sky::uint width, sky::uint height);
  void _init_program(sky::uint width, sky::uint height);
  void _init_uniforms(sky::uint width, sky::uint height);

public :
  Cave(void);
  ~Cave(void);

  void render(float time, sky::math::Mat44 const &proj, sky::math::Mat44 const &view, sky::uint n) const;
};

#endif

