#include "AliceState.hpp"
namespace th075 {void advance_alice_frame(FighterActor& a,FighterActor& other,AliceState& state,FighterActionEvents& events){const auto consumeSpirit=[&](i16 amount){if(a.combat.spiritBreak<1){a.combat.spirit=i16(u16(a.combat.spirit)-u16(amount));if(a.combat.spirit<1&&a.combat.spiritBreak<1){events.system_sound(43);a.combat.spirit=0;a.combat.spiritBreak=1000;}}else a.combat.spiritBreak=1000;};
const auto ownedVisual=[&](i16 id,float x,float y,i8 facing,u8 layer,u8 visual){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters,0,visual?&a.effectVisual:nullptr);};

  if ((double(a.motion.velocityX) == double(0.0f)) && ('\0' < i8(a.carryMomentum))) {
    a.carryMomentum = 0;
  }
  if ((double(540.0f) < double(a.motion.x) - double(other.motion.x)) ||
     (double(a.motion.x) - double(other.motion.x) < -double(540.0f))) {
    state.farRangeFrames = 0x3c;
  }
  if ((double(320.0f) < double(a.motion.x) - double(other.motion.x)) ||
     (double(a.motion.x) - double(other.motion.x) < -double(320.0f))) {
    state.farRangeFrames = i16(state.farRangeFrames) + 1;
    if (0x3c < i16(state.farRangeFrames)) {
      state.farRangeFrames = 0x3c;
    }
  }
  else {
    state.farRangeFrames = 0;
  }
  return;
}}
