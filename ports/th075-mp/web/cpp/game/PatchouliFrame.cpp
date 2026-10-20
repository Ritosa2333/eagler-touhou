#include "PatchouliState.hpp"
namespace th075 {void advance_patchouli_frame(FighterActor& a,FighterActor& other,PatchouliState& state,ReplayStreams& random,FighterActionEvents& events){const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters);};
const auto consumeSpirit=[&](i16 amount){if(a.combat.spiritBreak<1){a.combat.spirit=i16(u16(a.combat.spirit)-u16(amount));if(a.combat.spirit<1&&a.combat.spiritBreak<1){events.system_sound(43);a.combat.spirit=0;a.combat.spiritBreak=1000;}}else a.combat.spiritBreak=1000;};
const auto ownedVisual=[&](i16 id,float x,float y,i8 facing,u8 layer,u8 visual){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters,0,visual?&a.effectVisual:nullptr);};

  int iVar1;
  u32 uVar2;
  int iVar3;
  u32 uVar4;
  u32 uVar5;
  u32 local_10;
  
  if ((double(a.motion.velocityX) == double(0.0f)) && ('\0' < i8(a.carryMomentum))) {
    a.carryMomentum = 0;
  }
  if (i16(a.motion.hitFreeze) == 0) {
    state.spinAngle = i16(state.spinAngle) + i8(a.motion.facing) * 5;
  }
  if (i16(state.spinAngle) < 0) {
    state.spinAngle = 0x167;
  }
  if (0x167 < i16(state.spinAngle)) {
    state.spinAngle = 0;
  }
  if (i16(a.motion.hitFreeze) == 0) {
    state.orbitAngle = i16(state.orbitAngle) + 2;
  }
  if (i16(state.orbitAngle) < 0) {
    state.orbitAngle = 0x167;
  }
  if (0x167 < i16(state.orbitAngle)) {
    state.orbitAngle = 0;
  }
  if (0 < i16(state.windForceTime)) {
    local_10 = (u32)(double(0.0f) < (double(other.motion.x) -
                            double(a.motion.x)) * (float)(int)i8(a.motion.facing));
    a.motion.opponentPush =
         (float)(i16(state.windForceTime) + 0x14) * double(0.1f) *
         (float)((int)i8(a.motion.facing) * (int)i8(other.motion.facing))
         * (float)(int)(local_10 * 2 + -1);
    state.windForceTime = i16(state.windForceTime) + -2;
    iVar1 = random.range(0,100);
    if (iVar1 < 0x32) {
      uVar5 = 1;
      uVar2 = (u32)u8(a.motion.facing);
      uVar4 = 0;
      iVar1 = random.range(0,0x280);
      owned(0x38c,(float)(iVar1 * i8(a.motion.facing)) + double(a.motion.x),
                   uVar4,uVar2,uVar5);
    }
    else {
      uVar5 = 1;
      iVar3 = -(int)i8(a.motion.facing);
      uVar4 = 0;
      iVar1 = random.range(0,0x280);
      owned(0x38c,double(a.motion.x) - (float)(iVar1 * i8(a.motion.facing)),
                   uVar4,iVar3,uVar5);
    }
    if (i16(state.windForceTime) < 0) {
      state.windForceTime = 0;
    }
  }
  if (i16(state.blockEffectSent) != 1) {
    if ((0x95 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 0x9a)) {
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
                   double(a.motion.y) + double(94.0f),u8(a.motion.facing),1,1);
      state.blockEffectSent = 1;
    }
    if ((0x99 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 0x9e)) {
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
                   double(a.motion.y) + double(60.0f),u8(a.motion.facing),1,1);
      state.blockEffectSent = 1;
    }
    if (i16(a.animation.cursor.action) == 0x9e) {
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
