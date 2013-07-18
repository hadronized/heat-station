#ifndef __FSM_STAIRWAY_HPP
#define __FSM_STAIRWAY_HPP

#include <core/shader.hpp>
#include <core/texture.hpp>
#include <fsm/cave.hpp>
#include <fsm/common.hpp>
#include <scene/freefly.hpp>
#include <scene/material_manager.hpp>
#include <sync/parts_fsm.hpp>
#include <tech/deferred_renderer.hpp>

class Stairway : public sky::sync::FinalPartState {
  sky::ushort _width, _height;
  sky::scene::Freefly const &_freefly;
  sky::tech::DeferredRenderer &_drenderer;
  sky::scene::MaterialManager &_matmgr;
  sky::core::Program::Uniform _matmgrProjIndex;
  sky::core::Program::Uniform _matmgrViewIndex;
  sky::core::Program::Uniform _matmgrLColorIndex;
  sky::core::Program::Uniform _matmgrLPosIndex;
  Cave _cave;

  void _init_materials(void);
public :
  Stairway(sky::ushort width, sky::ushort height, Common &common, sky::scene::Freefly const &freefly);
  ~Stairway(void) = default;

  void run(float time) const override;
};

#endif /* guard */

