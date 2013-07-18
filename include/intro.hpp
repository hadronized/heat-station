#ifndef __INTRO_HPP
#define __INTRO_HPP

#include <core/context.hpp>
#include <fsm/common.hpp>
#include <lang/primtypes.hpp>
#include <scene/freefly.hpp>
#include <sync/parts_fsm.hpp>
#include <track/synthesizer.hpp>

class Intro {
  sky::core::Context _cntxt;
  sky::track::Synthesizer _synth;
  Common  _com;
  /* debug part */
  sky::scene::Freefly _freefly;
  /* fsm part */
  sky::sync::PartsFSM *_pFSM;

  void _init_materials(sky::ushort width, sky::ushort height);

public :
  Intro(sky::ushort width, sky::ushort height, bool full, char const *title);
  ~Intro(void);

  void run(void);
};

#endif /* guard */

