#ifndef __FSM_COMMON_HPP
#define __FSM_COMMON_HPP

#include <glyph/string_renderer.hpp>
#include <math/common.hpp>
#include <scene/material_manager.hpp>
#include <tech/deferred_renderer.hpp>

float  const FOVY             = sky::math::PI*70.f/180.f; /* 90 degrees */
float  const ZNEAR            = 0.0001f;
float  const ZFAR             = 10.f;

struct Common {
  sky::tech::DeferredRenderer drenderer;
  sky::scene::MaterialManager matmgr;
  sky::glyph::StringRenderer stringRenderer;

  Common(sky::ushort width, sky::ushort height);
  ~Common(void) = default;
};

#endif /* guard */

