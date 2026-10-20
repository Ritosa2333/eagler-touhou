#include "SuikaState.hpp"
namespace th075 {void advance_suika_frame(FighterActor& a,FighterActor& other,SuikaState& state,ReplayStreams& random,FighterActionEvents& events){const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters);};
const auto consumeSpirit=[&](FighterActor& receiver,i16 amount){auto& a=receiver;if(a.combat.spiritBreak<1){a.combat.spirit=i16(u16(a.combat.spirit)-u16(amount));if(a.combat.spirit<1&&a.combat.spiritBreak<1){events.system_sound(43);a.combat.spirit=0;a.combat.spiritBreak=1000;}}else a.combat.spiritBreak=1000;};
const auto ownedVisual=[&](i16 id,float x,float y,i8 facing,u8 layer,u8 visual){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters,0,visual?&a.effectVisual:nullptr);};

  float fVar1;
  int iVar2;
  u32 uVar3;
  float fVar4;
  u32 uVar5;
  
  if ((double(a.motion.velocityX) == double(0.0f)) && ('\0' < i8(a.carryMomentum))) {
    a.carryMomentum = 0;
  }
  if (0 < i16(state.summonTimer)) {
    state.summonTimer = i16(state.summonTimer) + -1;
    consumeSpirit(other,3);
    if ((i8(a.combat.spellPhase) == '\x02') && (iVar2 = random.range(0,100), iVar2 < 0x19)) {
      uVar5 = 1;
      uVar3 = (u32)u8(a.motion.facing);
      fVar1 = double(other.motion.y);
      iVar2 = random.range(0,100);
      fVar4 = double(iVar2)+float(double(fVar1)+40.0);
      fVar1 = double(other.motion.x);
      iVar2 = random.range(0,0x28);
      owned(0x336,double(float(double(fVar1)+20.0))-iVar2,fVar4,uVar3,uVar5);
    }
    if (i8(a.combat.spellPhase) == '\x01') {
      iVar2 = random.range(0,100);
      if (iVar2 < 0x19) {
        uVar5 = 1;
        uVar3 = (u32)u8(a.motion.facing);
        fVar1 = double(other.motion.y);
        iVar2 = random.range(0,100);
        fVar4 = double(iVar2)+float(double(fVar1)+40.0);
        fVar1 = double(other.motion.x);
        iVar2 = random.range(0,0x28);
        owned(0x337,double(float(double(fVar1)+20.0))-iVar2,fVar4,uVar3,uVar5);
      }
      if (0 < i16(a.combat.spiritBreak)) {
        if (3 < i16(a.combat.spiritBreak)) {
          a.combat.spiritBreak = i16(a.combat.spiritBreak) + -3;
          return;
        }
        a.combat.spiritBreak = 0;
        a.combat.spirit = 1000;
        return;
      }
      if (i16(a.combat.spirit) < 0x3e5) {
        a.combat.spirit = i16(a.combat.spirit) + 3;
      }
      else {
        a.combat.spirit = 1000;
      }
    }
  }
  if (0 < i16(state.giantMode)) {
    if (i16(a.combat.spirit) < 5) {
      consumeSpirit(a,i16(a.combat.spirit) + -1);
      state.giantMode = 0;
    }
    else {
      consumeSpirit(a,4);
    }
    if ((i16(other.combat.health) < 1) || (i16(a.combat.spellTimer) < 1)) {
      state.giantMode = 0;
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
