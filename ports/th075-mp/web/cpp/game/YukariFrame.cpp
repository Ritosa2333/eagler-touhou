#include "YukariState.hpp"
namespace th075 {void advance_yukari_frame(FighterActor& a,FighterActor& other,YukariState& state,ReplayStreams& random,FighterActionEvents& events){const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters);};
const auto consumeSpirit=[&](i16 amount){if(a.combat.spiritBreak<1){a.combat.spirit=i16(u16(a.combat.spirit)-u16(amount));if(a.combat.spirit<1&&a.combat.spiritBreak<1){events.system_sound(43);a.combat.spirit=0;a.combat.spiritBreak=1000;}}else a.combat.spiritBreak=1000;};
const auto ownedVisual=[&](i16 id,float x,float y,i8 facing,u8 layer,u8 visual){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters,0,visual?&a.effectVisual:nullptr);};

  if (i16(a.motion.hitFreeze) == 0) {
    state.orbitAngle = i16(state.orbitAngle) + 2;
  }
  if (0xb3 < i16(state.orbitAngle)) {
    state.orbitAngle = 0xff4c;
  }
  if ((double(a.motion.velocityX) == double(0.0f)) && ('\0' < i8(a.carryMomentum))) {
    a.carryMomentum = 0;
  }
  if (((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) &&
     (i16(state.guardEffectSent) == 0)) {
    if ((0x95 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 0x9a)) {
      ownedVisual(0x33e,(float)(i8(a.motion.facing) * 0x39) + double(a.motion.x),
                   double(a.motion.y) + double(94.0f),u8(a.motion.facing),1,1);
      state.guardEffectSent = 1;
    }
    if ((0x99 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 0x9e)) {
      ownedVisual(0x33e,(float)(i8(a.motion.facing) * 0x39) + double(a.motion.x),
                   double(a.motion.y) + double(60.0f),u8(a.motion.facing),1,1);
      state.guardEffectSent = 1;
    }
    if (i16(a.animation.cursor.action) == 0x9e) {
      ownedVisual(0x33e,(float)(i8(a.motion.facing) * 0x39) + double(a.motion.x),
                   double(a.motion.y) + double(100.0f),u8(a.motion.facing),1,1);
      state.guardEffectSent = 1;
    }
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
