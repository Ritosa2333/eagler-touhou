#include "YoumuState.hpp"
namespace th075 {void advance_youmu_frame(FighterActor& a,FighterActor& other,YoumuState& state,GameMode mode,ReplayStreams& random,FighterActionEvents& events){const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters);};
const auto consumeSpirit=[&](i16 amount){if(a.combat.spiritBreak<1){a.combat.spirit=i16(u16(a.combat.spirit)-u16(amount));if(a.combat.spirit<1&&a.combat.spiritBreak<1){events.system_sound(43);a.combat.spirit=0;a.combat.spiritBreak=1000;}}else a.combat.spiritBreak=1000;};
const auto ownedVisual=[&](i16 id,float x,float y,i8 facing,u8 layer,u8 visual){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters,0,visual?&a.effectVisual:nullptr);};

  u32 uVar1;
  u16 local_14;
  u16 local_12;
  u32 local_10;
  u32 local_c;
  u8 local_8;
  u16 local_6;
  
  if(a.combat.freeze==0&&a.motion.hitFreeze==0){state.history.push_back({a.animation.cursor.action,a.animation.cursor.index,a.motion.x,a.motion.y,a.motion.facing,a.remainingHits});if(state.history.size()>10)state.history.pop_front();}
  if (0 < i16(state.phantomTime)) {
    state.phantomTime = i16(state.phantomTime) + -1;
  }
  if ((double(a.motion.velocityX) == double(0.0f)) && ('\0' < i8(a.carryMomentum))) {
    a.carryMomentum = 0;
  }
  if (i16(state.phantomSpawned) == 0) {
    a.effectVisual.projection[0] = 0;
    a.effectVisual.projection[1] = 0;
    a.effectVisual.projection[2] = 0;
    a.effectVisual.scale[0] = 1.0f;
    a.effectVisual.scale[1] = 1.0f;
    a.effectVisual.alpha = 0xff;
    a.effectVisual.red = 0xff;
    a.effectVisual.green = 0xff;
    a.effectVisual.blue = 0xff;
    if (u8(mode) == '\x03') {
      if (i16(state.phantomActivated) == 1) {
        ownedVisual(0x32a,double(a.motion.x),double(a.motion.y) + double(100.0f),
                     u8(a.motion.facing),1,1);
        state.phantomSpawned = i16(state.phantomSpawned) + 1;
      }
      if (((i8(a.combat.storyOpponent) != '\0') &&
          (i8(other.dialogueFinished) != '\0')) &&
         (i16(state.phantomActivated) == 0)) {
        ownedVisual(0x32a,double(a.motion.x),double(a.motion.y) + double(100.0f),
                     u8(a.motion.facing),1,1);
        state.phantomSpawned = i16(state.phantomSpawned) + 1;
        state.phantomActivated = 1;
      }
      if (((i8(a.combat.storyOpponent) == '\0') && (i8(a.dialogueFinished) != '\0')) &&
         (i16(state.phantomActivated) == 0)) {
        ownedVisual(0x32a,double(a.motion.x),double(a.motion.y) + double(100.0f),
                     u8(a.motion.facing),1,1);
        state.phantomSpawned = i16(state.phantomSpawned) + 1;
        state.phantomActivated = 1;
      }
    }
    else {
      ownedVisual(0x32a,double(a.motion.x),double(a.motion.y) + double(100.0f),
                   u8(a.motion.facing),1,1);
      state.phantomSpawned = i16(state.phantomSpawned) + 1;
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
