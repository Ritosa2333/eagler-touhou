#include "SakuyaState.hpp"
namespace th075 {void advance_sakuya_frame(FighterActor& a,FighterActor& other,SakuyaState& state,FighterActionEvents& events){const auto consumeSpirit=[&](i16 amount){if(a.combat.spiritBreak<1){a.combat.spirit=i16(u16(a.combat.spirit)-u16(amount));if(a.combat.spirit<1&&a.combat.spiritBreak<1){events.system_sound(43);a.combat.spirit=0;a.combat.spiritBreak=1000;}}else a.combat.spiritBreak=1000;};
const auto ownedVisual=[&](i16 id,float x,float y,i8 facing,u8 layer,u8 visual){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters,0,visual?&a.effectVisual:nullptr);};

  if ((double(a.motion.velocityX) == double(0.0f)) && ('\0' < i8(a.carryMomentum))) {
    a.carryMomentum = 0;
  }
  if (i16(state.clockReserve) < 1) {
    a.bodyPower = 1.0f;
  }
  else {
    if (i16(state.clockPulse) < 1) {
      a.motion.set_hit_freeze(1);
      state.clockPulse = 1;
    }
    else {
      state.clockPulse = i16(state.clockPulse) + -1;
    }
    a.bodyPower = 0.75f;
    state.clockReserve = a.combat.spirit;
    if ((i16(a.animation.cursor.action) != 0x2c1) && (i16(a.animation.cursor.action) != 0x2c3)) {
      if (i8(a.combat.spellPhase) == '\x02') {
        if (i16(a.combat.spirit) < 9) {
          state.clockReserve = 0;
        }
        else {
          consumeSpirit(8);
        }
      }
      if (i8(a.combat.spellPhase) == '\x01') {
        if (i16(a.combat.spirit) < 5) {
          state.clockReserve = 0;
        }
        else {
          consumeSpirit(4);
        }
      }
    }
  }
  if (0 < i16(state.timeActive)) {
    a.combat.recoveryTimers[0] = 2;
    a.combat.recoveryTimers[2] = 2;
    if (i16(state.timeReserve) < 1) {
      state.timeReserve = 0;
      state.timeActive = 0;
      other.combat.recoveryTimers[3] = 0;
      other.pose.flags = 0;
    }
    if (((0 < i16(state.timeReserve)) && (i16(a.animation.cursor.action) != 0x2c0)) &&
       (i16(a.animation.cursor.action) != 0x2c2)) {
      state.timeReserve = a.combat.spirit;
      a.motion.hitFreeze = 2;
      if (i8(a.combat.spellPhase) == '\x02') {
        if (i16(a.combat.spirit) < 9) {
          state.timeReserve = 0;
        }
        else {
          consumeSpirit(8);
        }
      }
      if (i8(a.combat.spellPhase) == '\x01') {
        if (i16(a.combat.spirit) < 6) {
          state.timeReserve = 0;
        }
        else {
          consumeSpirit(5);
        }
      }
      other.begin_pose(0,0,0);
      other.pose.flags = 0x82;
    }
  }
  if (((0x95 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 0x9a)) &&
     ((i16(a.animation.cursor.index) == 0 &&
      ((i16(a.animation.cursor.elapsed) == 0 && (i16(state.blockEffectSent) == 0)))))) {
    a.effectVisual.projection[0] = 0;
    a.effectVisual.projection[1] = 0;
    a.effectVisual.projection[2] = 0;
    a.effectVisual.scale[0] = 1.0f;
    a.effectVisual.scale[1] = 1.0f;
    a.effectVisual.alpha = 0xff;
    a.effectVisual.red = 0xff;
    a.effectVisual.green = 0xff;
    a.effectVisual.blue = 0xff;
    a.effectVisual.rotation[0] = 0.0f;
    a.effectVisual.rotation[1] = 50.0f;
    a.effectVisual.rotation[2] = 0.0f;
    ownedVisual(0x331,(float)(i8(a.motion.facing) * 0x1e) + double(a.motion.x),
                 double(a.motion.y) + double(94.0f),u8(a.motion.facing),1,1);
    state.blockEffectSent = 1;
  }
  if ((((0x99 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 0x9e)) &&
      (i16(a.animation.cursor.index) == 0)) &&
     ((i16(a.animation.cursor.elapsed) == 0 && (i16(state.blockEffectSent) == 0)))) {
    a.effectVisual.projection[0] = 0;
    a.effectVisual.projection[1] = 0;
    a.effectVisual.projection[2] = 0;
    a.effectVisual.scale[0] = 1.0f;
    a.effectVisual.scale[1] = 1.0f;
    a.effectVisual.alpha = 0xff;
    a.effectVisual.red = 0xff;
    a.effectVisual.green = 0xff;
    a.effectVisual.blue = 0xff;
    a.effectVisual.rotation[0] = 0.0f;
    a.effectVisual.rotation[1] = 50.0f;
    a.effectVisual.rotation[2] = 0.0f;
    ownedVisual(0x331,(float)(i8(a.motion.facing) * 0x46) + double(a.motion.x),
                 double(a.motion.y) + double(60.0f),u8(a.motion.facing),1,1);
    state.blockEffectSent = 1;
  }
  if (((i16(a.animation.cursor.action) == 0x9e) && (i16(a.animation.cursor.index) == 0)) &&
     ((i16(a.animation.cursor.elapsed) == 0 && (i16(state.blockEffectSent) == 0)))) {
    a.effectVisual.projection[0] = 0;
    a.effectVisual.projection[1] = 0;
    a.effectVisual.projection[2] = 0;
    a.effectVisual.scale[0] = 1.0f;
    a.effectVisual.scale[1] = 1.0f;
    a.effectVisual.alpha = 0xff;
    a.effectVisual.red = 0xff;
    a.effectVisual.green = 0xff;
    a.effectVisual.blue = 0xff;
    a.effectVisual.rotation[0] = 0.0f;
    a.effectVisual.rotation[1] = 50.0f;
    a.effectVisual.rotation[2] = 0.0f;
    ownedVisual(0x331,(float)(i8(a.motion.facing) * 0x39) + double(a.motion.x),
                 double(a.motion.y) + double(100.0f),u8(a.motion.facing),1,1);
    state.blockEffectSent = 1;
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
