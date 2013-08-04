#ifndef __FSM_STAIRWAY_HPP
#define __FSM_STAIRWAY_HPP

#include <core/shader.hpp>
#include <core/texture.hpp>
#include <fsm/cave.hpp>
#include <fsm/common.hpp>
#include <fsm/fireflies.hpp>
#include <scene/freefly.hpp>
#include <scene/material_manager.hpp>
#include <sync/parts_fsm.hpp>
#include <tech/deferred_renderer.hpp>
#include <tech/post_process.hpp>

class Stairway : public sky::sync::PartState {
  sky::ushort _width, _height;
  sky::scene::Freefly const &_freefly;
  sky::tech::DeferredRenderer &_drenderer;
  sky::scene::MaterialManager &_matmgr;
  sky::core::Program::Uniform _matmgrProjIndex;
  sky::core::Program::Uniform _matmgrViewIndex;
  sky::core::Program::Uniform _matmgrLColorIndex;
  sky::core::Program::Uniform _matmgrLPosIndex;

  Cave _cave;
  Fireflies _fireflies;

  sky::tech::PostProcess _fogEffect;

  void _init_materials(void);
  void _init_fog_uniforms(void);

public :
  Stairway(sky::ushort width, sky::ushort height, Common &common, sky::scene::Freefly const &freefly);
  ~Stairway(void) = default;

  void run(float time) override;
};

#endif /* guard */

