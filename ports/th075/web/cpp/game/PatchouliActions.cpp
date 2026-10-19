#include "PatchouliState.hpp"
#include <cmath>
namespace th075 {
void advance_patchouli_action(FighterActor& a,FighterActor& other,PatchouliState& state,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){

    double aimRadians=0;const auto atan=[](float y,float x){return std::atan2(double(y),double(x));};
    const auto selectFrame=[&](i16 frame){a.animation.set(a.animation.cursor.action,frame,0);};
    const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters);};
    const auto shared=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Shared,id,x,y,facing,layer,a.launchParameters);};
    const auto ownedVisual=[&](i16 id,float x,float y,i8 facing,u8 layer,u8 visual,u8 order){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters,order,visual?&a.effectVisual:nullptr);};
    const auto consumeSpirit=[&](i16 amount){if(a.combat.spiritBreak<1){a.combat.spirit=i16(u16(a.combat.spirit)-u16(amount));if(a.combat.spirit<1&&a.combat.spiritBreak<1){events.system_sound(43);a.combat.spirit=0;a.combat.spiritBreak=1000;}}else a.combat.spiritBreak=1000;};
    const auto trail=[&](i32 frames,i8 interval,u32 mask){a.combat.noticeTimer=frames;a.trails.interval=interval;a.trails.colorMask=mask;};
    const auto hitFreeze=[&](i16 frames){if(frames>=0)a.motion.hitFreeze=frames;};
    const auto fade=[&](u8 kind){a.combat.fade=240;a.fadeKind=kind;};

  float fVar1;
  i8 cVar2;
  u16 uVar3;
  i16 sVar4;
  int iVar5;
  u32 uVar6;
  
  int iVar7;
  float fVar8;
  u32 uVar9;
  u32 uVar10;
  u32 local_1e4;
  int local_1c;
  int emission;
  int volley;
  int spark;
  int burst;
  int dust;
  
  if (i16(a.combat.freeze) != 0) {
    return;
  }
  if (i16(other.combat.opponentStop) != 0) {
    return;
  }
  iVar5 = (int)i16(a.animation.cursor.action);
  if (0x32 < iVar5) {
    if (iVar5 < 0x8d) {
      if (iVar5 == 0x8c) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -11.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
          state.actionTicks = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (4 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(1);
        return;
      }
      switch(iVar5) {
      case 0x33:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -9.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x34:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -12.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x35:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.motion.velocityX = -15.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.actionTicks = 0;
          return;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (0 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(1);
        return;
      case 0x38:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -8.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x39:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -9.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x3a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -12.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x3b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.motion.velocityX = -15.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.actionTicks = 0;
          return;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (0 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(1);
        return;
      case 0x3e:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -8.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0x3f:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -9.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0x40:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -12.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0x41:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.motion.velocityX = -15.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.actionTicks = 0;
          return;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (0 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(1);
        return;
      case 0x46:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -double(a.impulseX);
          a.motion.velocityY = double(a.motion.velocityY) + double(a.impulseY);
          if (double(a.motion.velocityY) < double(6.0f)) {
            a.motion.velocityY = 6.0f;
          }
          a.motion.accelerationY = 0.800000011920929f;
          if (double(25.0f) < double(a.motion.velocityY)) {
            a.motion.velocityY = 25.0f;
          }
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 3) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(4);
          return;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          uVar3 = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = uVar3;
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(0x61);
          events.shake(2.0f);
          events.system_sound(0x16);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(7);
        return;
      case 0x47:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -double(a.impulseX);
          a.motion.velocityY = double(a.impulseY);
          a.motion.accelerationY = 0.800000011920929f;
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 3) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(4);
          return;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          uVar3 = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = uVar3;
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(0x61);
          events.shake(2.0f);
          events.system_sound(0x16);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(7);
        return;
      case 0x48:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -double(a.impulseX);
          a.motion.velocityY = double(a.impulseY);
          a.motion.accelerationY = 1.2999999523162842f;
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 3) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(4);
          return;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          uVar3 = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = uVar3;
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(0x61);
          events.shake(2.0f);
          events.system_sound(0x16);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(7);
        return;
      case 0x49:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -double(a.impulseX);
          a.motion.velocityY = double(a.impulseY);
          a.motion.accelerationY = 0.800000011920929f;
          a.response = 0;
        }
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          uVar3 = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = uVar3;
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(0x61);
          events.shake(2.0f);
          events.system_sound(0x16);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          selectFrame(i16(a.animation.cursor.index) + -1);
        }
        if (i16(a.animation.cursor.index) != 1) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (double(a.motion.velocityY) == double(0.0f)) {
          return;
        }
        if (double(a.motion.velocityX) == double(0.0f)) {
          selectFrame(3);
          return;
        }
        selectFrame(2);
        return;
      case 0x4a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -double(a.impulseX);
          a.motion.velocityY = double(a.impulseY);
          a.motion.accelerationY = 0.800000011920929f;
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 3) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(4);
          return;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') &&
           (double(a.motion.velocityX) < -double(15.0f) != (double(a.motion.velocityX) == -double(15.0f)))) {
          a.select_action(0x4c);
          a.motion.velocityX = -double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = 15.0f;
          a.motion.accelerationY = 0.5f;
          events.shake(2.0f);
          events.system_sound(0x16);
          state.actionTicks = 0;
          return;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          events.system_sound(0x16);
          uVar3 = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = uVar3;
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(0x61);
          events.shake(2.0f);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(7);
        return;
      case 0x4c:
        if (i16(state.actionTicks) == 0) {
          state.actionTicks = i16(state.actionTicks) + 1;
          a.begin_pose(0.0f,95.0f,0.0f);
        }
        a.pose.rotation[2] = double(a.pose.rotation[2]) + double(30.0f);
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          events.system_sound(0x16);
          uVar3 = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = uVar3;
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(0x61);
          a.pose.rotation[2] = 0.0f;
          events.shake(2.0f);
          (a.pose.active=false);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(0);
        return;
      case 0x58:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -double(a.impulseX);
          a.motion.velocityY = double(a.impulseY);
          a.motion.accelerationY = 0.800000011920929f;
          a.response = 0;
        }
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          uVar3 = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = uVar3;
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(0x59);
          events.shake(3.0f);
          events.system_sound(0x16);
          for (dust = 0; dust < 10; dust = dust + 1) {
            owned(0x3e4,double(a.motion.x),double(a.motion.y),1,1);
          }
          if (i16(a.knockdownTime) < 0x1a) {
            return;
          }
          a.knockdownTime = 0;
          owned(0x3e3,double(a.motion.x),double(a.motion.y),1,1);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          selectFrame(i16(a.animation.cursor.index) + -1);
        }
        if (i16(a.animation.cursor.index) != 1) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (double(a.motion.velocityY) == double(0.0f)) {
          return;
        }
        if (double(a.motion.velocityX) == double(0.0f)) {
          selectFrame(3);
          return;
        }
        selectFrame(2);
        return;
      case 0x59:
        if (2 < i16(a.animation.cursor.index)) {
          a.pose.rotation[2] = double(a.pose.rotation[2]) + double(40.0f);
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
              (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
            uVar3 = i16(i32(double(a.motion.velocityX)*double(100.0f)));
            state.actionTicks = uVar3;
            uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
            state.actionStage = uVar3;
            a.motion.clear();
            a.motion.y = 0.0f;
            a.select_action(0x61);
            a.pose.rotation[2] = 0.0f;
            events.shake(2.0f);
            (a.pose.active=false);
            events.system_sound(0x16);
            return;
          }
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          selectFrame(4);
          return;
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = (float)(int)i16(state.actionTicks) * double(0.0025f);
        a.motion.velocityY = (float)-(int)i16(state.actionStage) * double(0.01f);
        a.motion.accelerationY = 0.699999988079071f;
        a.begin_pose(0.0f,95.0f,0.0f);
        return;
      case 0x5d:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 11.0f;
        }
        uVar9 = 1;
        uVar6 = (u32)(u8)u8(a.motion.facing);
        iVar5 = random.range(0,0x14);
        fVar8 = double(a.motion.y) - (float)iVar5;
        fVar1 = double(a.motion.x);
        iVar5 = random.range(0,0x8c);
        owned(0x353,double(float(double(fVar1)+70.0))-iVar5,fVar8,uVar6,uVar9);
        sVar4 = a.animation.advance();
        if ((sVar4 == i16(a.animation.cursor.frameCount)) &&
           (a.select_action(0), 0 < a.controls.current.vertical)) {
          a.select_action(1);
        }
        if ((int)i16(a.animation.cursor.index) == i16(a.animation.cursor.frameCount) + -1) {
          if (double(other.motion.x) < double(a.motion.x)) {
            a.motion.facing = (u8)0xff;
          }
          if (double(a.motion.x) < double(other.motion.x)) {
            a.motion.facing = (u8)0x1;
          }
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 0.0f;
        return;
      case 0x5e:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -11.0f;
        }
        uVar9 = 1;
        uVar6 = (u32)(u8)u8(a.motion.facing);
        iVar5 = random.range(0,0x14);
        fVar8 = double(a.motion.y) - (float)iVar5;
        fVar1 = double(a.motion.x);
        iVar5 = random.range(0,0x8c);
        owned(0x353,double(float(double(fVar1)+70.0))-iVar5,fVar8,uVar6,uVar9);
        sVar4 = a.animation.advance();
        if ((sVar4 == i16(a.animation.cursor.frameCount)) &&
           (a.select_action(0), 0 < a.controls.current.vertical)) {
          a.select_action(1);
        }
        if ((int)i16(a.animation.cursor.index) == i16(a.animation.cursor.frameCount) + -1) {
          if (double(other.motion.x) < double(a.motion.x)) {
            a.motion.facing = (u8)0xff;
          }
          if (double(a.motion.x) < double(other.motion.x)) {
            a.motion.facing = (u8)0x1;
          }
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 0.0f;
        return;
      case 0x5f:
        sVar4 = a.animation.advance();
        if ((sVar4 == i16(a.animation.cursor.frameCount)) &&
           (a.select_action(0), 0 < a.controls.current.vertical)) {
          a.select_action(1);
        }
        if ((int)i16(a.animation.cursor.index) != i16(a.animation.cursor.frameCount) + -1) {
          return;
        }
        if (double(other.motion.x) < double(a.motion.x)) {
          a.motion.facing = (u8)0xff;
        }
        if (double(other.motion.x) <= double(a.motion.x)) {
          return;
        }
        a.motion.facing = (u8)0x1;
        return;
      case 0x60:
        if ((0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) &&
           (((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
            ((a.controls.current.buttons[2] != 0 || (a.controls.current.buttons[3] != 0)))))) {
          state.actionTicks = 1;
        }
        if (((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 0) &&
           (((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
            ((a.controls.current.buttons[2] != 0 || (a.controls.current.buttons[3] != 0)))))) {
          state.actionTicks = 2;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          for (burst = 0; burst < 10; burst = burst + 1) {
            owned(0x3e4,double(a.motion.x),double(a.motion.y),1,1);
          }
          if (0x19 < i16(a.knockdownTime)) {
            a.knockdownTime = 0;
            owned(0x3e3,double(a.motion.x),double(a.motion.y),1,1);
          }
        }
        if (((2 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 7)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(7);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x62);
          if (u8(a.transitionReady) != (u8)0x0) {
            return;
          }
          a.select_action(99);
          return;
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = (float)(int)i16(state.actionTicks) * double(0.005f);
        a.motion.velocityY = (float)-(int)i16(state.actionStage) * double(0.005f);
        a.motion.accelerationY = 2.0f;
        return;
      case 0x61:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          for (spark = 0; spark < 10; spark = spark + 1) {
            owned(0x3e4,double(a.motion.x),double(a.motion.y),1,1);
          }
          if (0x19 < i16(a.knockdownTime)) {
            a.knockdownTime = 0;
            owned(0x3e3,double(a.motion.x),double(a.motion.y),1,1);
          }
        }
        if (((2 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 7)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(7);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x62);
          if (u8(a.transitionReady) != (u8)0x0) {
            return;
          }
          a.select_action(99);
          return;
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = (float)(int)i16(state.actionTicks) * double(0.005f);
        a.motion.velocityY = (float)-(int)i16(state.actionStage) * double(0.005f);
        a.motion.accelerationY = 2.0f;
        return;
      case 0x62:
        if ((0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) &&
           ((((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
             (a.controls.current.buttons[2] != 0)) || (a.controls.current.buttons[3] != 0)))) {
          state.actionTicks = 1;
        }
        if (((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 0) &&
           (((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
            ((a.controls.current.buttons[2] != 0 || (a.controls.current.buttons[3] != 0)))))) {
          state.actionTicks = 2;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0x5f);
        if (i16(state.actionTicks) == 1) {
          a.select_action(0x5d);
        }
        if (i16(state.actionTicks) != 2) {
          return;
        }
        a.select_action(0x5e);
        return;
      case 99:
        if ((u8(mode) == '\x03') && (u8(a.combat.storyOpponent) != (u8)0x0)) {
          if ((((1 < i16(a.animation.cursor.index)) && ('\0' < (i8)u8(a.combat.spellPhase))) &&
              (i16(other.animation.cursor.action) < 0x96)) &&
             (a.phaseKinds[a.combat.spellPhase] == (u8)0x2)) {
            a.select_action(0x31c);
            return;
          }
          if (((0 < i16(a.animation.cursor.index)) && ('\0' < (i8)u8(a.combat.spellPhase))) &&
             ((i16(other.animation.cursor.action) < 0x96 &&
              (a.phaseKinds[a.combat.spellPhase] == (u8)0x1)))) {
            a.select_action(0x31c);
            return;
          }
          if (((1 < i16(a.animation.cursor.index)) && ('\0' < (i8)u8(a.combat.spellPhase))) &&
             ((i16(other.animation.cursor.action) < 0x96 &&
              (a.phaseKinds[a.combat.spellPhase] == (u8)0x0)))) {
            a.select_action(0x5f);
            a.transitionReady = (u8)0x1;
            return;
          }
        }
        else if ((i16(a.animation.cursor.index) == 2) &&
                (('\0' < (i8)u8(a.combat.spellPhase) &&
                 (i16(other.animation.cursor.action) < 0x96)))) {
          a.select_action(0x5f);
          a.transitionReady = (u8)0x1;
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(2);
        return;
      }
    }
    else if (iVar5 < 0x12d) {
      if (iVar5 == 300) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
        }
        if (('\0' < (i8)u8(a.carryMomentum)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1c);
        return;
      }
      switch(iVar5) {
      case 0x8f:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -12.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
          state.actionTicks = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (6 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(1);
        return;
      case 0x90:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -12.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
          state.actionTicks = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (6 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(1);
        return;
      case 0x91:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -10.0f;
          a.motion.velocityY = 14.0f;
          a.motion.accelerationY = 0.5f;
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(3);
          return;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          uVar3 = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = uVar3;
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(0x61);
          events.shake(2.0f);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(7);
        return;
      case 0x96:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.motion.velocityX = -9.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x97:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.motion.velocityX = -10.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x98:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.motion.velocityX = -13.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x9a:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.motion.velocityX = -9.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0x9b:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.motion.velocityX = -10.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0x9c:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.motion.velocityX = -13.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0x9e:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -6.0f;
          a.motion.velocityY = 8.0f;
          a.motion.accelerationY = 0.75f;
        }
        if ((double(0.0f) <= double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.clear();
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x9f:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.motion.velocityX = -9.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0xa0:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.motion.velocityX = -10.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0xa1:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.motion.velocityX = -13.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0xa3:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -9.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0xa4:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -10.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0xa5:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -13.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        cVar2 = a.motion.horizontal_limit(other.motion);
        if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0xa7:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0xfa);
          events.system_sound(0x2c);
          state.actionTicks = 0xb4;
          shared(0x38,(float)((i8)u8(a.motion.facing) * 0x1e) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        a.motion.opponentPush = (float)-(int)i16(state.actionTicks) * 0.1;
        if (0 < i16(state.actionTicks)) {
          state.actionTicks = i16(state.actionTicks) + -10;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0xa8:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0xfa);
          events.system_sound(0x2c);
          state.actionTicks = 0xb4;
          shared(0x38,(float)((i8)u8(a.motion.facing) * 0x1e) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        a.motion.opponentPush = (float)-(int)i16(state.actionTicks) * 0.1;
        if (0 < i16(state.actionTicks)) {
          state.actionTicks = i16(state.actionTicks) + -10;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0xb4:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.motion.velocityX = 8.0f;
          a.motion.velocityY = 12.5f;
          a.motion.accelerationY = 0.75f;
          a.pose.rotation[2] = 0.0f;
          (a.pose.active=false);
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.clear();
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(8);
        return;
      case 0xb5:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.motion.velocityX = -8.0f;
          a.motion.velocityY = 12.5f;
          a.motion.accelerationY = 0.75f;
          a.pose.rotation[2] = 0.0f;
          (a.pose.active=false);
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.clear();
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(10);
        return;
      case 200:
        a.carryMomentum = (u8)0x1;
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          state.actionTicks = 0;
        }
        if ((double(10.0f) < double(a.motion.velocityX)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
           double(a.motion.velocityX) < double(10.0f))) {
          a.motion.velocityX = 10.0f;
        }
        if (a.controls.current.vertical < 0) {
          if (0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) {
            a.select_action(0xcd);
            return;
          }
          a.select_action(0xcc);
          return;
        }
        if ((0 < i16(state.actionTicks)) &&
           ((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 1)) {
          a.select_action(0xcf);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          if ((0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) &&
             (i16(state.actionTicks) < 5)) {
            selectFrame(2);
            state.actionTicks = i16(state.actionTicks) + 1;
            return;
          }
          a.select_action(0xcf);
          return;
        }
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 8.5f;
        shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        shared(0x7e,double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        events.system_sound(0x1f);
        return;
      case 0xc9:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -10.0f;
          a.motion.velocityY = 3.0f;
          a.motion.accelerationY = 0.30000001192092896f;
          shared(0x7d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x50),double(a.motion.y) + double(80.0f),-(int)(i8)u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(80.0f),-(int)(i8)u8(a.motion.facing),1);
          events.system_sound(0x1f);
        }
        if ((((double(0.0f) < double(a.motion.y)) && (0 < i16(a.animation.cursor.index))) &&
            (i16(a.animation.cursor.index) < 9)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
           (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f)))) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.y = 0.0f;
          a.animation.cursor.elapsed = 0;
          selectFrame(9);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 9) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(8);
        return;
      case 0xca:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 8.0f;
          a.motion.velocityY = 3.0f;
          a.motion.accelerationY = 0.4000000059604645f;
          shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          events.system_sound(0x1f);
        }
        if (((double(0.0f) < double(a.motion.y)) && (1 < i16(a.animation.cursor.index))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
           (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f)))) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.y = 0.0f;
          a.animation.cursor.elapsed = 0;
          a.select_action(10);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0xcb:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
        }
        if (((double(0.0f) < double(a.motion.y)) && (0 < i16(a.animation.cursor.index))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
           (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f)))) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.y = 0.0f;
          a.animation.cursor.elapsed = 0;
          a.select_action(10);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 1) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = -8.0f;
        a.motion.velocityY = 3.0f;
        a.motion.accelerationY = 0.4000000059604645f;
        shared(0x7d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x50),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
        shared(0x7e,double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
        events.system_sound(0x1f);
        return;
      case 0xcc:
        if ((double(a.motion.y) == double(0.0f)) || (double(0.0f) < double(a.motion.velocityY))) {
          trail(3,4,0xff);
          a.combat.recoveryTimers[2] = 2;
        }
        if ((i16(a.animation.cursor.index) == 0) && (u8(a.carryMomentum) == (u8)0x0)) {
          a.motion.clear();
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.clear();
          return;
        }
        if ((i16(a.animation.cursor.index) == 2) && (double(a.motion.velocityY) < double(4.0f))) {
          selectFrame(3);
        }
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.elapsed) < i16(a.animation.cursor.duration)) {
          return;
        }
        a.animation.cursor.elapsed = 0;
        a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
        if (i16(a.animation.cursor.index) == 2) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 19.0f;
          a.motion.accelerationY = 0.6000000238418579f;
          a.carryMomentum = (u8)0x0;
          shared(0x3c,double(a.motion.x),double(a.motion.y),1,1);
        }
        if (i16(a.animation.cursor.frameCount) <= i16(a.animation.cursor.index)) {
          a.animation.cursor.index = i16(a.animation.cursor.index) + -2;
        }
        a.animation.set(a.animation.cursor.action,a.animation.cursor.index,a.animation.cursor.elapsed);
        return;
      case 0xcd:
        if ((double(a.motion.y) == double(0.0f)) || (double(0.0f) < double(a.motion.velocityY))) {
          trail(3,4,0xff);
          a.combat.recoveryTimers[2] = 2;
        }
        if ((i16(a.animation.cursor.index) == 0) && (u8(a.carryMomentum) == (u8)0x0)) {
          a.motion.clear();
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.clear();
          return;
        }
        if ((i16(a.animation.cursor.index) == 2) && (double(a.motion.velocityY) < double(4.0f))) {
          selectFrame(3);
        }
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.elapsed) < i16(a.animation.cursor.duration)) {
          return;
        }
        a.animation.cursor.elapsed = 0;
        a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
        if (i16(a.animation.cursor.index) == 2) {
          a.motion.velocityX = 8.5f;
          a.motion.velocityY = 15.0f;
          a.motion.accelerationY = 0.6000000238418579f;
          a.carryMomentum = (u8)0x0;
          shared(0x3c,double(a.motion.x),double(a.motion.y),1,1);
        }
        if (i16(a.animation.cursor.frameCount) <= i16(a.animation.cursor.index)) {
          a.animation.cursor.index = i16(a.animation.cursor.index) + -2;
        }
        a.animation.set(a.animation.cursor.action,a.animation.cursor.index,a.animation.cursor.elapsed);
        return;
      case 0xce:
        if ((double(a.motion.y) == double(0.0f)) || (double(0.0f) < double(a.motion.velocityY))) {
          trail(3,4,0xff);
          a.combat.recoveryTimers[2] = 2;
        }
        if ((i16(a.animation.cursor.index) == 0) && (u8(a.carryMomentum) == (u8)0x0)) {
          a.motion.clear();
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.clear();
          return;
        }
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.elapsed) < i16(a.animation.cursor.duration)) {
          return;
        }
        a.animation.cursor.elapsed = 0;
        a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
        if (i16(a.animation.cursor.index) == 2) {
          a.motion.velocityX = -8.5f;
          a.motion.velocityY = 15.0f;
          a.motion.accelerationY = 0.6000000238418579f;
          a.carryMomentum = (u8)0x0;
          shared(0x3c,double(a.motion.x),double(a.motion.y),1,1);
        }
        if (i16(a.animation.cursor.frameCount) <= i16(a.animation.cursor.index)) {
          a.animation.cursor.index = i16(a.animation.cursor.index) + -2;
        }
        a.animation.set(a.animation.cursor.action,a.animation.cursor.index,a.animation.cursor.elapsed);
        return;
      case 0xcf:
        a.carryMomentum = (u8)0x1;
        a.motion.velocityX = double(a.motion.velocityX) - double(0.5f);
        if (double(a.motion.velocityX) < double(0.0f)) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        a.carryMomentum = (u8)0x0;
        return;
      case 0xd0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.facing = (u8)-(i8)u8(a.motion.facing);
          a.motion.velocityX = -double(a.motion.velocityX);
        }
        if (double(a.motion.accelerationY) == double(0.0f)) {
          a.motion.accelerationY = 0.6000000238418579f;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.clear();
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(5);
        return;
      case 0xd1:
        if (i16(a.combat.spirit) < 2) {
          a.select_action(0);
          return;
        }
        a.combat.spirit = i16(a.combat.spirit) + -3;
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          events.system_sound(0x2c);
          owned(0x3e6,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        if (a.controls.current.buttons[3] != 0) {
          selectFrame(2);
          return;
        }
        a.select_action(0);
        return;
      case 0xd2:
        if (i16(a.combat.spirit) < 2) {
          a.select_action(9);
          return;
        }
        a.combat.spirit = i16(a.combat.spirit) + -3;
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          events.system_sound(0x2c);
          owned(0x3e6,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY) * double(0.25f);
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        if (a.controls.current.buttons[3] != 0) {
          selectFrame(2);
          return;
        }
        a.select_action(9);
        return;
      }
    }
    else if (iVar5 < 0x187) {
      if (iVar5 == 0x186) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = 8.0f;
        }
        if ((0 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1c);
        return;
      }
      switch(iVar5) {
      case 0x12d:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
        }
        if (('\0' < (i8)u8(a.carryMomentum)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.actionTicks = 0;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x32d,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1d);
        return;
      case 0x12e:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
        }
        if (('\0' < (i8)u8(a.carryMomentum)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(2);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1b);
        return;
      case 0x12f:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
        }
        if (('\0' < (i8)u8(a.carryMomentum)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(2);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1d);
        return;
      case 0x130:
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.velocityY = 0.0f;
          return;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          state.actionTicks = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1c);
        }
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x335,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x131:
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.velocityY = 0.0f;
          return;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (a.controls.current.buttons[1] != 0) {
            selectFrame(5);
            return;
          }
          events.system_sound(0x1d);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x32e,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          selectFrame(8);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1d);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x32c,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x133:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 0;
          a.response = 1;
        }
        if (('\0' < (i8)u8(a.carryMomentum)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.actionTicks = 0;
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x32f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        owned(0x333,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        events.sound(6);
        return;
      case 0x134:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 0;
          a.response = 1;
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if (('\0' < (i8)u8(a.carryMomentum)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.actionTicks = 0;
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x334,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        events.sound(6);
        return;
      case 0x135:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.carryMomentum = (u8)0x0;
          a.response = 0;
        }
        if ((double(0.0f) < double(a.motion.velocityX)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.9f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.actionTicks = 0;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 13.0f;
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1d);
        return;
      case 0x136:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.carryMomentum = (u8)0x0;
          a.response = 0;
        }
        if ((double(0.0f) < double(a.motion.velocityX)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.9f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 13.0f;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1d);
        return;
      case 0x137:
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.velocityY = 0.0f;
          return;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          state.actionTicks = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x32f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        owned(0x334,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        events.sound(6);
        return;
      case 0x138:
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.velocityY = 0.0f;
          return;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          state.actionTicks = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x336,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        events.sound(6);
        return;
      }
    }
    else if (iVar5 < 0x1f5) {
      if (iVar5 == 500) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 0;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          selectFrame(0x11);
          a.motion.y = 0.0f;
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(9);
          consumeSpirit(200);
          state.windForceTime = 0x5a;
          a.motion.velocityY = 6.0f;
          a.motion.accelerationY = 0.6000000238418579f;
          state.actionTicks = 100;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if (((i16(a.animation.cursor.index) == 0xd) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (double(12.0f) < double(a.motion.velocityY))) {
          selectFrame(6);
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 0x11) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xf);
        return;
      }
      switch(iVar5) {
      case 0x187:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = 10.0f;
        }
        if ((0 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.4f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x32d,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1d);
        return;
      case 0x188:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = 10.0f;
        }
        if ((0 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.4f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1d);
        return;
      case 0x189:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 3;
          a.response = 0;
          a.motion.velocityX = 10.0f;
          state.actionTicks = 0;
        }
        if ((0 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.3f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x32f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          owned(0x357,(float)((i8)u8(a.motion.facing) * 0x78) + double(a.motion.x),double(a.motion.y) + double(120.0f),u8(a.motion.facing),1);
          events.sound(6);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (1 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        a.response = 0;
        selectFrame(5);
        return;
      case 0x18a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = 15.0f;
          state.actionTicks = 0;
        }
        a.motion.velocityX = double(a.motion.velocityX) - double(0.6f);
        if (double(a.motion.velocityX) < double(0.0f)) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x327,(float)((i8)u8(a.motion.facing) * 0x97) + double(a.motion.x),double(a.motion.y) + double(104.0f),u8(a.motion.facing),1);
        return;
      case 0x18b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = 10.0f;
          state.actionTicks = 0;
        }
        if ((6 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(2);
        }
        if (((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) < 2)) {
          state.actionTicks = i16(state.actionTicks) + 1;
          selectFrame(6);
          a.remainingHits = 1;
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x328,(float)((int)(i8)u8(a.motion.facing) << 7) + double(a.motion.x),double(a.motion.y) + double(70.0f),u8(a.motion.facing),1);
        return;
      case 400:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(100);
          a.launchParameters[0] = 5.0f;
          owned(0x33b,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = -5.0f;
          owned(0x33b,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 20.0f;
          owned(0x33b,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = -20.0f;
          owned(0x33b,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          events.sound(0);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x191:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(100);
          a.launchParameters[0] = 45.0f;
          owned(0x33d,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = -25.0f;
          owned(0x33d,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 25.0f;
          owned(0x33d,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = -45.0f;
          owned(0x33d,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          events.sound(0);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x192:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(100);
          a.launchParameters[0] = 55.0f;
          owned(0x343,(float)((i8)u8(a.motion.facing) * 0x73) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 65.0f;
          owned(0x343,(float)((i8)u8(a.motion.facing) * 0x73) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 45.0f;
          owned(0x343,(float)((i8)u8(a.motion.facing) * 0x73) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 75.0f;
          owned(0x343,(float)((i8)u8(a.motion.facing) * 0x73) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1);
          events.sound(0);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0x194:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          a.response = 1;
        }
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 0.0f;
          a.motion.y = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(100);
          a.launchParameters[0] = 5.0f;
          owned(0x341,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = -5.0f;
          owned(0x341,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 20.0f;
          owned(0x341,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = -20.0f;
          owned(0x341,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          events.sound(0);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x195:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          a.response = 1;
        }
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 0.0f;
          a.motion.y = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(100);
          a.launchParameters[0] = 45.0f;
          owned(0x33f,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = -35.0f;
          owned(0x33f,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 25.0f;
          owned(0x33f,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = -45.0f;
          owned(0x33f,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          events.sound(0);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x196:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 0;
          a.response = 1;
        }
        if (('\0' < (i8)u8(a.carryMomentum)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.actionTicks = 0;
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        consumeSpirit(100);
        for (volley = 0; volley < 8; volley = volley + 1) {
          a.launchParameters[0] = (float)(volley * 0x2d);
          owned(0x35e,(float)((i8)u8(a.motion.facing) * 0x6a) + double(a.motion.x),double(a.motion.y) + double(144.0f),u8(a.motion.facing),1);
        }
        events.sound(2);
        return;
      case 0x197:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 0;
          a.response = 1;
        }
        if (('\0' < (i8)u8(a.carryMomentum)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.actionTicks = 0;
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        consumeSpirit(100);
        owned(0x32f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        owned(0x354,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        events.sound(6);
        return;
      case 0x198:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(100);
          owned(0x358,(float)((i8)u8(a.motion.facing) * 0x73) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1);
          events.sound(0x18);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0x19a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          a.response = 1;
        }
        if (8 < i16(a.animation.cursor.index)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        }
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 0.0f;
          a.motion.y = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(100);
          for (emission = 0; emission < 8; emission = emission + 1) {
            a.launchParameters[0] = (float)(emission * 0x2d);
            owned(0x35e,(float)((i8)u8(a.motion.facing) * 0x4e) + double(a.motion.x),double(a.motion.y) + double(105.0f),u8(a.motion.facing),1);
          }
          events.sound(2);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x19c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        if (((4 < i16(a.animation.cursor.index)) && (i16(state.actionTicks) < 1)) &&
           (i16(state.actionStage) < 10)) {
          consumeSpirit(10);
          events.sound(0x16);
          iVar5 = random.range(0,0x14);
          a.launchParameters[0] = (float)(10 - iVar5);
          owned(0x324,(float)((i8)u8(a.motion.facing) * 0x41) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          state.actionStage = i16(state.actionStage) + 1;
          state.actionTicks = 2;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x19e:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0x96);
          owned(0x355,(float)((i8)u8(a.motion.facing) * 0x73) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1);
          events.sound(0x18);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0x1a0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          a.response = 1;
        }
        if (8 < i16(a.animation.cursor.index)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        }
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 0.0f;
          a.motion.y = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0x96);
          owned(0x355,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          events.sound(0x18);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x1a1:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.2f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.2f);
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 0.0f;
          a.motion.y = 0.0f;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        if (((3 < i16(a.animation.cursor.index)) && (i16(state.actionTicks) < 1)) &&
           (i16(state.actionStage) < 10)) {
          consumeSpirit(10);
          events.sound(0x16);
          iVar5 = random.range(0,0x14);
          a.launchParameters[0] = (float)(iVar5 + -10);
          owned(0x324,(float)((i8)u8(a.motion.facing) * 0x4e) + double(a.motion.x),double(a.motion.y) + double(105.0f),u8(a.motion.facing),1);
          state.actionStage = i16(state.actionStage) + 1;
          state.actionTicks = 2;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (9 < i16(state.actionStage)) {
          return;
        }
        selectFrame(5);
        return;
      }
    }
    else if (iVar5 < 0x265) {
      if (iVar5 == 0x264) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.response = 1;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        consumeSpirit(0x96);
        events.sound(10);
        a.launchParameters[0] = 105.0f;
        owned(0x346,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 90.0f;
        owned(0x346,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 75.0f;
        owned(0x346,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 60.0f;
        owned(0x346,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -105.0f;
        owned(0x346,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -90.0f;
        owned(0x346,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -75.0f;
        owned(0x346,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -60.0f;
        owned(0x346,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        return;
      }
      switch(iVar5) {
      case 0x1f5:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 10;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          selectFrame(0x11);
          a.motion.y = 0.0f;
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(9);
          state.windForceTime = 0x78;
          consumeSpirit(0xfa);
          a.motion.velocityY = 10.0f;
          a.motion.accelerationY = 0.5f;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if (((i16(a.animation.cursor.index) == 0xd) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (double(4.0f) < double(a.motion.velocityY))) {
          selectFrame(6);
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 0x11) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xf);
        return;
      case 0x252:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.selectedSpell = (u8)((i8)u8(a.selectedSpell) + -1);
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
        }
        if ((i16(a.combat.spiritBreak) == 0) && (a.response == 1)) {
          a.combat.spirit = 1000;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        shared(0x37,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        events.system_sound(0x21);
        other.knockdownTime = 0;
        return;
      case 0x253:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.selectedSpell = (u8)((i8)u8(a.selectedSpell) + -1);
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
        }
        if ((i16(a.combat.spiritBreak) == 0) && (a.response == 1)) {
          a.combat.spirit = 1000;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        shared(0x37,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        events.system_sound(0x21);
        other.knockdownTime = 0;
        return;
      case 0x254:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.selectedSpell = (u8)((i8)u8(a.selectedSpell) + -1);
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        shared(0x37,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        events.system_sound(0x21);
        other.knockdownTime = 0;
        return;
      case 0x255:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x37,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (0 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(6);
        return;
      case 0x256:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        shared(0x37,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        return;
      case 599:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x37,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (0 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(6);
        return;
      case 600:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(100);
          owned(0x32a,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          owned(0x332,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          shared(0x7f,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x3c),0.0f,u8(a.motion.facing),1);
          events.sound(2);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x259:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(100);
          owned(0x329,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          owned(0x332,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          shared(0x7f,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x3c),0.0f,u8(a.motion.facing),1);
          events.sound(2);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x25a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(100);
          a.launchParameters[0] = (float)(int)i16(state.emissionIndex);
          owned(0x328,(float)((i8)u8(a.motion.facing) * 0x28) + double(a.motion.x),double(a.motion.y) + double(45.0f),u8(a.motion.facing),1);
          events.sound(2);
          a.motion.velocityX = -8.0f;
          a.motion.velocityY = 8.0f;
        }
        if ((2 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.6f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          a.select_action(10);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x25c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(100);
          owned(0x338,double(other.motion.x),0.0f,u8(a.motion.facing),1);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x260:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x35d,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),0.0f,u8(a.motion.facing),1);
        return;
      case 0x261:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x35d,(float)((i8)u8(a.motion.facing) * 300) + double(a.motion.x),0.0f,u8(a.motion.facing),1);
        return;
      case 0x262:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x35d,(float)((i8)u8(a.motion.facing) * 500) + double(a.motion.x),0.0f,u8(a.motion.facing),1);
        return;
      case 0x263:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.response = 1;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        consumeSpirit(100);
        events.sound(10);
        a.launchParameters[0] = 75.0f;
        owned(0x346,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -75.0f;
        owned(0x346,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 45.0f;
        a.launchParameters[0] = 60.0f;
        owned(0x346,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -60.0f;
        owned(0x346,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 45.0f;
        owned(0x346,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -45.0f;
        owned(0x346,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        return;
      }
    }
    else {
      switch(iVar5) {
      case 0x266:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if ((0xd < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 8.0f;
          owned(0x329,double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 0xd) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (0 < i16(state.actionTicks)) {
          return;
        }
        selectFrame(6);
        state.actionTicks = i16(state.actionTicks) + 1;
        return;
      case 700:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.clear();
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x73,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x21),double(a.motion.y) + double(170.0f),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          ownedVisual(800,(float)((i8)u8(a.motion.facing) * 0x42) + double(a.motion.x),double(a.motion.y) + double(67.0f),u8(a.motion.facing),1,0,1);
          events.sound(0x15);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        ownedVisual(0x321,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x48),double(a.motion.y) + double(273.0f),u8(a.motion.facing),1,0,1);
        consumeSpirit(0xfa);
        return;
      case 0x2bd:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          state.actionStage = 0x14;
          a.response = 1;
          a.motion.clear();
        }
        sVar4 = a.animation.advance();
        if ((sVar4 == i16(a.animation.cursor.frameCount)) &&
           (a.select_action(0), double(0.0f) < double(a.motion.y))) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x73,(float)((i8)u8(a.motion.facing) * 0x1e) + double(a.motion.x),double(a.motion.y) + double(177.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) {
            state.actionStage = i16(state.actionStage) + -1;
          }
          if ((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 0) {
            state.actionStage = i16(state.actionStage) + 1;
          }
          if (0x28 < i16(state.actionStage)) {
            state.actionStage = 0x28;
          }
          if (i16(state.actionStage) < 10) {
            state.actionStage = 10;
          }
          iVar5 = random.range(0,(int)i16(state.actionStage) << 1);
          a.launchParameters[0] = (float)(iVar5 - i16(state.actionStage));
          events.sound(0xc);
          owned(0x323,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          consumeSpirit(10);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (0xb < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        if (0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) {
          state.actionStage = i16(state.actionStage) + -1;
        }
        if ((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 0) {
          state.actionStage = i16(state.actionStage) + 1;
        }
        if (0x28 < i16(state.actionStage)) {
          state.actionStage = 0x28;
        }
        if (i16(state.actionStage) < 10) {
          state.actionStage = 10;
        }
        iVar5 = random.range(0,(int)i16(state.actionStage) << 1);
        a.launchParameters[0] = (float)(iVar5 - i16(state.actionStage));
        events.sound(0xc);
        owned(0x323,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        selectFrame(6);
        consumeSpirit(10);
        return;
      case 0x2be:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.clear();
          a.remainingHits = 99;
        }
        if (((6 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 9)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(9);
          return;
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0xd);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x73,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x3e),double(a.motion.y) + double(107.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 2.0f;
          a.motion.velocityY = 3.5f;
          a.motion.accelerationY = 0.05000000074505806f;
          consumeSpirit(0xfa);
          owned(0x349,double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),0);
          for (local_1c = 0; local_1c < 3; local_1c = local_1c + 1) {
            a.launchParameters[0] = (float)local_1c;
            owned(0x348,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          }
        }
        if (i16(a.animation.cursor.index) != 9) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.response = 0;
        selectFrame(7);
        return;
      case 0x2bf:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          state.actionStage = 0;
          a.response = 1;
          a.motion.clear();
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          return;
        }
        if ((6 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 9)) {
          if (i16(a.animation.cursor.index) < 9) {
            events.shake(10.0f);
          }
          if (i16(a.combat.spirit) < 6) {
            consumeSpirit(i16(a.combat.spirit) + -1);
          }
          else {
            consumeSpirit(5);
          }
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          hitFreeze(0x28);
          events.system_sound(0x17);
          events.sound(0xe);
          shared(0x74,(float)((i8)u8(a.motion.facing) * 0x14) + double(a.motion.x),double(a.motion.y) + double(240.0f),u8(a.motion.facing),1);
          ownedVisual(0x351,(float)((i8)u8(a.motion.facing) * 0x14) + double(a.motion.x),double(a.motion.y) + double(240.0f),u8(a.motion.facing),1,0,1);
        }
        if (((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (5 < i16(a.combat.spirit))) {
          selectFrame(7);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (9 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(9);
        return;
      case 0x2c0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          state.actionStage = 0;
          state.emissionIndex = 0;
          a.response = 1;
          a.motion.clear();
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x74,(float)((i8)u8(a.motion.facing) * 0xc) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          ownedVisual(0x34a,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),0,0,1);
        }
        state.actionStage = i16(state.actionStage) + -1;
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x35a,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),0);
        }
        if (((2 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 7)) &&
           (i16(state.actionStage) < 1)) {
          consumeSpirit(5);
          a.set_polar_velocity((float)(int)i16(state.emissionIndex),220.0f);
          state.emissionIndex = i16(state.emissionIndex) + 10;
          sVar4 = i16(state.emissionIndex);
          if (sVar4 == 0xb4) {
            state.emissionIndex = 0;
          }
          owned(0x34b,double(a.motion.x) + double(a.motion.velocityX),double(a.motion.velocityY) * double(0.13f),u8(a.motion.facing),double(a.motion.velocityY) < double(0.0f));
          fVar1 = double(a.motion.velocityY);
          owned(0x35c,double(a.motion.x) + double(a.motion.velocityX),double(a.motion.velocityY) * double(0.13f),u8(a.motion.facing),fVar1 < double(0.0f));
          fVar1 = double(a.motion.velocityY);
          owned(0x34b,double(a.motion.x) - double(a.motion.velocityX),-double(a.motion.velocityY) * double(0.13f),u8(a.motion.facing),fVar1 < double(0.0f));
          owned(0x35c,double(a.motion.x) - double(a.motion.velocityX),-double(a.motion.velocityY) * double(0.13f),u8(a.motion.facing),double(a.motion.velocityY) < double(0.0f));
          state.actionStage = 2;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          return;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0x11);
        }
        if (((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) < 7)) {
          state.actionTicks = i16(state.actionTicks) + 1;
          selectFrame(3);
        }
        a.motion.velocityX = 0.0f;
        a.motion.velocityY = 0.0f;
        return;
      case 0x2c1:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          state.actionStage = 0;
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x74,(float)((i8)u8(a.motion.facing) * 0xc) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          hitFreeze(0x28);
          events.system_sound(0x17);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0xfa);
          events.sound(0x12);
          a.launchParameters[0] = 0.0f;
          ownedVisual(0x34c,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
          a.launchParameters[0] = 1.0f;
          ownedVisual(0x34d,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
          a.launchParameters[0] = 2.0f;
          ownedVisual(0x34e,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
          a.launchParameters[0] = 3.0f;
          ownedVisual(0x34f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
          a.launchParameters[0] = 4.0f;
          ownedVisual(0x350,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x2d0:
        if ((i16(a.animation.cursor.index) == 0) && (u8(a.carryMomentum) == (u8)0x0)) {
          a.motion.clear();
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.clear();
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(10);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          iVar5 = random.range(0,4);
          a.motion.velocityX = (float)(iVar5 + 8);
          iVar5 = random.range(0,5);
          a.motion.velocityY = (float)(iVar5 + 9);
          a.motion.accelerationY = 0.30000001192092896f;
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(6);
        return;
      case 0x2d1:
        if ((i16(a.animation.cursor.index) == 0) && (u8(a.carryMomentum) == (u8)0x0)) {
          a.motion.clear();
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.clear();
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(10);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          iVar5 = random.range(0,4);
          a.motion.velocityX = (float)(-8 - iVar5);
          iVar5 = random.range(0,5);
          a.motion.velocityY = (float)(iVar5 + 9);
          a.motion.accelerationY = 0.30000001192092896f;
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(6);
        return;
      case 0x2db:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          uVar3 = random.range(0,0x24);
          state.actionTicks = uVar3;
          a.response = 1;
          a.motion.clear();
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(0,0x3c);
          a.commandTimers[2] = sVar4 + 0xb4 + state.difficulty * -0x1e;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          ownedVisual(800,(float)((i8)u8(a.motion.facing) * 0x42) + double(a.motion.x),double(a.motion.y) + double(67.0f),u8(a.motion.facing),1,0,1);
          events.sound(0x15);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.launchParameters[0] = (float)(i16(state.actionTicks) + 0x12);
        a.launchParameters[1] = -20.0f;
        ownedVisual(900,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x48),double(a.motion.y) + double(273.0f),u8(a.motion.facing),1,0,1);
        return;
      case 0x2e5:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
          state.emissionIndex = 0;
          state.emissionDelay = 0;
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (((i16(a.animation.cursor.index) < 4 || (0xb < i16(a.animation.cursor.index))) &&
            (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
            double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))))) {
          selectFrame(0x10);
          a.motion.y = 0.0f;
          a.motion.clear();
          return;
        }
        if ((((7 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
            ((int)i16(state.emissionIndex) < state.difficulty * 2 + 3)) &&
           (state.emissionDelay = i16(state.emissionDelay) + -1,
           i16(state.emissionDelay) < 1)) {
          owned(0x391,(float)((i8)u8(a.motion.facing) * 0x4d) + double(a.motion.x),double(a.motion.y) + double(118.0f),u8(a.motion.facing),1);
          events.sound(0x18);
          state.emissionDelay = 0xf;
          state.emissionIndex = i16(state.emissionIndex) + 1;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(0x3c,0x5a);
          a.commandTimers[2] = sVar4 + state.difficulty * -0x14;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -4.0f;
          a.motion.velocityY = 12.0f;
          a.motion.accelerationY = 0.30000001192092896f;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.2f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.2f);
        }
        if (((i16(a.animation.cursor.index) == 0xc) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionStage) < 5)) {
          state.actionStage = i16(state.actionStage) + 1;
          selectFrame(8);
        }
        if (i16(a.animation.cursor.index) != 0x10) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xe);
        return;
      case 0x2e6:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          state.actionStage = 0x14;
          a.response = 1;
          a.motion.clear();
        }
        if (((i16(a.animation.cursor.index) < 4) && (i16(a.animation.cursor.elapsed) < 0x19)) &&
           (state.actionStage = i16(state.actionStage) + 1,
           3 < i16(state.actionStage))) {
          shared(0x3e,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x1c),double(a.motion.y) + double(180.0f),1,1);
          state.actionStage = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          if (double(0.0f) < double(a.motion.y)) {
            a.select_action(9);
          }
          sVar4 = random.range(0x3c,0x5a);
          a.commandTimers[2] = sVar4 + state.difficulty * -0x1e;
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          iVar5 = random.range(0,0x28);
          a.launchParameters[0] = (float)(iVar5 + -0x14);
          events.sound(0xc);
          owned(0x390,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (7 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        iVar5 = random.range(0,0x14);
        a.launchParameters[0] = (float)(iVar5 + -10);
        events.sound(0xc);
        owned(0x390,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        selectFrame(6);
        return;
      case 0x2ef:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 10;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
          state.actionStage = 0;
          state.emissionIndex = 0;
        }
        if (((double(0.0f) < double(a.motion.y)) && (0x13 < i16(a.animation.cursor.index))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          selectFrame(0x18);
          a.motion.y = 0.0f;
          return;
        }
        if ((5 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x14)) {
          local_1e4 = (u32)(double(0.0f) < (double(other.motion.x) -
                                   double(a.motion.x)) * (float)(int)(i8)u8(a.motion.facing));
          a.motion.opponentPush =
               (float)(int)i16(state.actionTicks) * 0.1 *
               (float)((int)(i8)u8(a.motion.facing) * (int)i8(other.motion.facing))
               * (float)(int)(local_1e4 * 2 + -1);
          state.actionTicks = i16(state.actionTicks) + -2;
          state.emissionIndex = i16(state.emissionIndex) + -1;
          if (i16(state.actionTicks) < 0x28) {
            state.actionTicks = 0x28;
          }
          if (i16(state.emissionIndex) < 1) {
            if (u8(a.motion.facing) == (u8)0x1) {
              uVar9 = 1;
              uVar6 = (u32)(u8)u8(a.motion.facing);
              iVar5 = random.range(0,0x118);
              owned(0x38b,double(a.combat.screenBounds[0]) - double(100.0f),(float)iVar5,uVar6,uVar9);
            }
            if (u8(a.motion.facing) == (u8)0xff) {
              uVar9 = 1;
              uVar6 = (u32)(u8)u8(a.motion.facing);
              iVar5 = random.range(0,0x118);
              owned(0x38b,double(a.combat.screenBounds[1]) + double(100.0f),(float)iVar5,uVar6,uVar9);
            }
            state.emissionIndex = state.difficulty * -8 + 0x23;
          }
          iVar5 = random.range(0,100);
          if (iVar5 < 0x32) {
            uVar10 = 1;
            uVar6 = (u32)(u8)u8(a.motion.facing);
            uVar9 = 0;
            iVar5 = random.range(0,0x280);
            owned(0x38c,(float)(iVar5 * (i8)u8(a.motion.facing)) + double(a.motion.x),uVar9,uVar6,uVar10);
          }
          else {
            uVar10 = 1;
            iVar7 = -(int)(i8)u8(a.motion.facing);
            uVar9 = 0;
            iVar5 = random.range(0,0x280);
            owned(0x38c,double(a.motion.x) - (float)(iVar5 * (i8)u8(a.motion.facing)),uVar9,iVar7,uVar10);
          }
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          a.commandTimers[2] = state.difficulty * -0x1e + 0xb4;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 100;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(9);
          iVar5 = random.range(0,4);
          a.motion.velocityX = (float)(iVar5 + -2);
          a.motion.velocityY = 1.5f;
          a.motion.accelerationY = 0.20000000298023224f;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if (((i16(a.animation.cursor.index) == 0x14) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionStage) < 10)) {
          state.actionStage = i16(state.actionStage) + 1;
          selectFrame(0xd);
        }
        if (i16(a.animation.cursor.index) != 0x18) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x16);
        return;
      case 0x31b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.clear();
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          fade(0);
          a.begin_spell(mode,replayMode,core);
          events.system_sound(0x17);
          owned(0x3e5,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 9) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (7 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(7);
        return;
      case 0x31c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.clear();
        }
        sVar4 = a.animation.advance();
        if ((sVar4 == i16(a.animation.cursor.frameCount)) &&
           (a.select_action(0), a.phaseVariants[a.combat.spellPhase] == (u8)0x1)) {
          owned(0x393,-200.0f,-200.0f,1,1);
        }
        if ((i16(a.animation.cursor.index) == 0xc) && (i16(a.animation.cursor.elapsed) == 0)) {
          fade(0);
          a.begin_spell(mode,replayMode,core);
          events.system_sound(0x17);
        }
        if (i16(a.animation.cursor.index) != 0xf) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (7 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(0xd);
        a.transitionReady = (u8)0x1;
        return;
      }
    }
switchD_004f718a_default:
    sVar4 = a.animation.advance();
    if (sVar4 != i16(a.animation.cursor.frameCount)) {
      return;
    }
    a.select_action(0);
    return;
  }
  if (iVar5 == 0x32) {
    if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
      a.motion.velocityX = -8.0f;
      a.motion.velocityY = 0.0f;
      a.response = 0;
    }
    if ((double(a.motion.velocityX) < double(0.0f)) &&
       (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
       double(0.0f) < double(a.motion.velocityX))) {
      a.motion.velocityX = 0.0f;
    }
    cVar2 = a.motion.horizontal_limit(other.motion);
    if ((cVar2 != '\0') && (u8(a.allowPush) != (u8)0x0)) {
      a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
    }
    sVar4 = a.animation.advance();
    if (sVar4 != i16(a.animation.cursor.frameCount)) {
      return;
    }
    a.select_action(0);
    return;
  }
  switch(iVar5) {
  case 0:
    if (u8(a.carryMomentum) == (u8)0x0) {
      a.motion.clear();
    }
    else {
      if ((double(0.0f) < double(a.motion.velocityX)) &&
         (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
         double(a.motion.velocityX) < double(0.0f))) {
        a.motion.clear();
        a.carryMomentum = (u8)0x0;
      }
      if ((double(a.motion.velocityX) < double(0.0f)) &&
         (a.motion.velocityX = double(a.motion.velocityX) + double(0.5f),
         double(0.0f) < double(a.motion.velocityX))) {
        a.motion.clear();
        a.carryMomentum = (u8)0x0;
      }
    }
    goto LAB_00501dd7;
  case 1:
    if (u8(a.carryMomentum) == (u8)0x0) {
      a.motion.clear();
    }
    else {
      if ((double(0.0f) < double(a.motion.velocityX)) &&
         (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
         double(a.motion.velocityX) < double(0.0f))) {
        a.motion.clear();
        a.carryMomentum = (u8)0x0;
      }
      if ((double(a.motion.velocityX) < double(0.0f)) &&
         (a.motion.velocityX = double(a.motion.velocityX) + double(0.5f),
         double(0.0f) < double(a.motion.velocityX))) {
        a.motion.clear();
        a.carryMomentum = (u8)0x0;
      }
    }
    sVar4 = a.animation.advance();
    if (sVar4 == i16(a.animation.cursor.frameCount)) {
      a.select_action(2);
    }
    break;
  case 2:
    if (u8(a.carryMomentum) == (u8)0x0) {
      a.motion.clear();
    }
    else {
      if ((double(0.0f) < double(a.motion.velocityX)) &&
         (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
         double(a.motion.velocityX) < double(0.0f))) {
        a.motion.clear();
        a.carryMomentum = (u8)0x0;
      }
      if ((double(a.motion.velocityX) < double(0.0f)) &&
         (a.motion.velocityX = double(a.motion.velocityX) + double(0.5f),
         double(0.0f) < double(a.motion.velocityX))) {
        a.motion.clear();
        a.carryMomentum = (u8)0x0;
      }
    }
    goto LAB_00501dd7;
  case 3:
    if (u8(a.carryMomentum) == (u8)0x0) {
      a.motion.clear();
    }
    else {
      if ((double(0.0f) < double(a.motion.velocityX)) &&
         (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
         double(a.motion.velocityX) < double(0.0f))) {
        a.motion.clear();
        a.carryMomentum = (u8)0x0;
      }
      if ((double(a.motion.velocityX) < double(0.0f)) &&
         (a.motion.velocityX = double(a.motion.velocityX) + double(0.5f),
         double(0.0f) < double(a.motion.velocityX))) {
        a.motion.clear();
        a.carryMomentum = (u8)0x0;
      }
    }
    sVar4 = a.animation.advance();
    if (sVar4 == i16(a.animation.cursor.frameCount)) {
      a.select_action(0);
    }
    break;
  case 4:
    a.motion.velocityX = 4.0f;
    goto LAB_00501dd7;
  case 5:
    a.motion.velocityX = -4.0f;
LAB_00501dd7:
    a.animation.advance();
    break;
  case 6:
    if (((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) &&
       ((u8(a.command) == (u8)0x3 || ('\0' < (i8)u8(a.carryMomentum))))) {
      a.select_action(0xcc);
    }
    else {
      if ((i16(a.animation.cursor.index) == 0) && (u8(a.carryMomentum) == (u8)0x0)) {
        a.motion.clear();
      }
      if ((double(a.motion.y) <= double(0.0f)) ||
         (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
         double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) ==
         (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
        if ((i16(a.animation.cursor.index) == 4) && (double(a.motion.velocityY) < double(4.0f))) {
          selectFrame(5);
        }
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.duration) <= i16(a.animation.cursor.elapsed)) {
          a.animation.cursor.elapsed = 0;
          a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
          if (i16(a.animation.cursor.index) == 2) {
            a.motion.velocityX = 0.0f;
            a.motion.velocityY = 13.0f;
            a.motion.accelerationY = 0.4000000059604645f;
            if ('\0' < (i8)u8(a.carryMomentum)) {
              a.motion.velocityX = 2.0f;
              a.motion.velocityY = 10.0f;
              a.motion.accelerationY = 0.25f;
            }
          }
          if (i16(a.animation.cursor.frameCount) <= i16(a.animation.cursor.index)) {
            a.animation.cursor.index = i16(a.animation.cursor.index) + -2;
          }
          a.animation.set(a.animation.cursor.action,a.animation.cursor.index,a.animation.cursor.elapsed);
        }
      }
      else {
        a.select_action(10);
        a.motion.y = 0.0f;
        a.motion.clear();
      }
    }
    break;
  case 7:
    if (((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) &&
       ((u8(a.command) == (u8)0x5 || ('\0' < (i8)u8(a.carryMomentum))))) {
      a.select_action(0xcd);
    }
    else {
      if ((i16(a.animation.cursor.index) == 0) && (u8(a.carryMomentum) == (u8)0x0)) {
        a.motion.clear();
      }
      if ((double(a.motion.y) <= double(0.0f)) ||
         (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
         double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) ==
         (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
        if ((i16(a.animation.cursor.index) == 4) && (double(a.motion.velocityY) < double(4.0f))) {
          selectFrame(5);
        }
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.duration) <= i16(a.animation.cursor.elapsed)) {
          a.animation.cursor.elapsed = 0;
          a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
          if (i16(a.animation.cursor.index) == 2) {
            a.motion.velocityX = 3.0f;
            a.motion.velocityY = 13.0f;
            a.motion.accelerationY = 0.4000000059604645f;
            if ('\0' < (i8)u8(a.carryMomentum)) {
              a.motion.velocityX = 4.5f;
              a.motion.velocityY = 10.0f;
              a.motion.accelerationY = 0.25f;
              a.carryMomentum = (u8)0x0;
            }
          }
          if (i16(a.animation.cursor.frameCount) <= i16(a.animation.cursor.index)) {
            a.animation.cursor.index = i16(a.animation.cursor.index) + -2;
          }
          a.animation.set(a.animation.cursor.action,a.animation.cursor.index,a.animation.cursor.elapsed);
        }
      }
      else {
        a.select_action(10);
        a.motion.y = 0.0f;
        a.motion.clear();
      }
    }
    break;
  case 8:
    if (((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) &&
       ((u8(a.command) == (u8)0x4 || ('\0' < (i8)u8(a.carryMomentum))))) {
      a.select_action(0xce);
    }
    else {
      if ((i16(a.animation.cursor.index) == 0) && (u8(a.carryMomentum) == (u8)0x0)) {
        a.motion.clear();
      }
      if ((double(a.motion.y) <= double(0.0f)) ||
         (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
         double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) ==
         (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.duration) <= i16(a.animation.cursor.elapsed)) {
          a.animation.cursor.elapsed = 0;
          a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
          if (i16(a.animation.cursor.index) == 2) {
            a.motion.velocityX = -4.0f;
            a.carryMomentum = (u8)0x0;
            a.motion.velocityY = 13.0f;
            a.motion.accelerationY = 0.4000000059604645f;
          }
          if (i16(a.animation.cursor.frameCount) <= i16(a.animation.cursor.index)) {
            a.animation.cursor.index = i16(a.animation.cursor.index) + -2;
          }
          a.animation.set(a.animation.cursor.action,a.animation.cursor.index,a.animation.cursor.elapsed);
        }
      }
      else {
        a.select_action(10);
        a.motion.y = 0.0f;
        a.motion.clear();
      }
    }
    break;
  case 9:
    if (double(a.motion.accelerationY) == double(0.0f)) {
      a.motion.accelerationY = 0.6000000238418579f;
    }
    a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
    if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) ==
        (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
      sVar4 = a.animation.advance();
      if (sVar4 == i16(a.animation.cursor.frameCount)) {
        selectFrame(0);
      }
    }
    else {
      a.select_action(10);
      a.motion.y = 0.0f;
      a.motion.clear();
    }
    break;
  case 10:
    if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
      events.system_sound(0x1e);
    }
    a.motion.clear();
    sVar4 = a.animation.advance();
    if (sVar4 == i16(a.animation.cursor.frameCount)) {
      if (a.controls.current.vertical == 0) {
        a.select_action(0);
      }
      else {
        a.select_action(1);
      }
    }
    break;
  default:
    goto switchD_004f718a_default;
  }
  return;

}
}
