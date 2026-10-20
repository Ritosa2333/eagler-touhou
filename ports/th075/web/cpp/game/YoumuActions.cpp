#include "YoumuState.hpp"
#include <cmath>
namespace th075 {
void advance_youmu_action(FighterActor& a,FighterActor& other,YoumuState& state,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){

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
  double position;
  float fVar8;
  u32 uVar9;
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
          state.impactX = 0;
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
        if (4 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
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
          state.impactX = 0;
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
          state.impactX = 0;
          return;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (0 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
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
          state.impactX = 0;
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
          state.impactX = 0;
          return;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (0 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
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
          state.impactX = 0;
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
          state.impactX = 0;
          return;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (0 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
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
          state.impactX = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = uVar3;
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
          state.impactX = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = uVar3;
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
          state.impactX = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = uVar3;
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
          state.impactX = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = uVar3;
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
          state.impactX = 0;
          return;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          events.system_sound(0x16);
          uVar3 = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.impactX = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = uVar3;
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
        if (i16(state.impactX) == 0) {
          state.impactX = i16(state.impactX) + 1;
          a.begin_pose(0.0f,95.0f,0.0f);
        }
        a.pose.rotation[2] = double(a.pose.rotation[2]) + double(30.0f);
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          events.system_sound(0x16);
          uVar3 = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.impactX = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = uVar3;
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
          state.impactX = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = uVar3;
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
        if (i16(a.animation.cursor.index) == 3) {
          a.pose.rotation[2] = double(a.pose.rotation[2]) + double(40.0f);
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
              (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
            uVar3 = i16(i32(double(a.motion.velocityX)*double(100.0f)));
            state.impactX = uVar3;
            uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
            state.impactY = uVar3;
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
          selectFrame(3);
          return;
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = (float)(int)i16(state.impactX) * double(0.0025f);
        a.motion.velocityY = (float)-(int)i16(state.impactY) * double(0.01f);
        a.motion.accelerationY = 0.699999988079071f;
        a.begin_pose(0.0f,95.0f,0.0f);
        return;
      case 0x5d:
        if (((i16(a.animation.cursor.index) < 0x13) && (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(0x13);
          return;
        }
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
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 8.0f;
          a.motion.velocityX = 13.5f;
          a.motion.accelerationY = 0.6000000238418579f;
        }
        if (i16(a.animation.cursor.index) != 0x13) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x12);
        return;
      case 0x5e:
        if (((i16(a.animation.cursor.index) < 0x12) && (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(0x12);
          return;
        }
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
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 8.0f;
          a.motion.velocityX = -13.5f;
          a.motion.accelerationY = 0.6000000238418579f;
        }
        if (i16(a.animation.cursor.index) != 0x12) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x10);
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
           ((((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
             (a.controls.current.buttons[2] != 0)) || (a.controls.current.buttons[3] != 0)))) {
          state.impactX = 1;
        }
        if (((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 0) &&
           (((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
            ((a.controls.current.buttons[2] != 0 || (a.controls.current.buttons[3] != 0)))))) {
          state.impactX = 2;
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
        if (((i16(a.animation.cursor.index) == 3) || (i16(a.animation.cursor.index) == 2)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(4);
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
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = (float)(int)i16(state.impactX) * double(0.005f);
          a.motion.velocityY = (float)-(int)i16(state.impactY) * double(0.005f);
          a.motion.accelerationY = 2.0f;
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(3);
        return;
      case 0x61:
        if ((0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) &&
           (((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
            ((a.controls.current.buttons[2] != 0 || (a.controls.current.buttons[3] != 0)))))) {
          state.impactX = 1;
        }
        if (((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 0) &&
           ((((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
             (a.controls.current.buttons[2] != 0)) || (a.controls.current.buttons[3] != 0)))) {
          state.impactX = 2;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          for (spark = 0; spark < 10; spark = spark + 1) {
            owned(0x3e4,double(a.motion.x),double(a.motion.y),1,1);
          }
          if (0x19 < i16(a.knockdownTime)) {
            a.knockdownTime = 0;
            owned(0x3e3,double(a.motion.x),double(a.motion.y),1,1);
          }
        }
        if (((i16(a.animation.cursor.index) == 3) || (i16(a.animation.cursor.index) == 2)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(4);
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
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = (float)(int)i16(state.impactX) * double(0.005f);
          a.motion.velocityY = (float)-(int)i16(state.impactY) * double(0.005f);
          a.motion.accelerationY = 2.0f;
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(3);
        return;
      case 0x62:
        if ((0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) &&
           ((((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
             (a.controls.current.buttons[2] != 0)) || (a.controls.current.buttons[3] != 0)))) {
          state.impactX = 1;
        }
        if (((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 0) &&
           (((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
            ((a.controls.current.buttons[2] != 0 || (a.controls.current.buttons[3] != 0)))))) {
          state.impactX = 2;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0x5f);
        if (i16(state.impactX) == 1) {
          a.select_action(0x5d);
        }
        if (i16(state.impactX) != 2) {
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
        if (i16(a.animation.cursor.index) != 1) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1b);
        return;
      }
      switch(iVar5) {
      case 0x8f:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -12.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
          state.impactX = 0;
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
        if (6 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(1);
        return;
      case 0x90:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -12.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
          state.impactX = 0;
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
        if (6 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(1);
        return;
      case 0x91:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -10.0f;
          a.motion.velocityY = 14.0f;
          a.motion.accelerationY = 0.5f;
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
          state.impactX = uVar3;
          uVar3 = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = uVar3;
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
          state.impactX = 0;
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
          state.impactX = 0;
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
          state.impactX = 0;
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
          state.impactX = 0;
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
          state.impactX = 0;
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
          state.impactX = 0;
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
          state.impactX = 0;
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
          state.impactX = 0;
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
          state.impactX = 0;
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
          state.impactX = 0xb4;
          shared(0x38,(float)((i8)u8(a.motion.facing) * 0x1e) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        a.motion.opponentPush = (float)-(int)i16(state.impactX) * 0.1;
        if (0 < i16(state.impactX)) {
          state.impactX = i16(state.impactX) + -10;
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
          state.impactX = 0xb4;
          shared(0x38,(float)((i8)u8(a.motion.facing) * 0x1e) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        a.motion.opponentPush = (float)-(int)i16(state.impactX) * 0.1;
        if (0 < i16(state.impactX)) {
          state.impactX = i16(state.impactX) + -10;
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
        trail(3,4,0xff);
        a.carryMomentum = (u8)0x1;
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          state.impactX = 0;
        }
        if ((double(15.0f) < double(a.motion.velocityX)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
           double(a.motion.velocityX) < double(9.0f))) {
          a.motion.velocityX = 9.0f;
        }
        if (a.controls.current.vertical < 0) {
          if (0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) {
            a.select_action(0xcd);
            return;
          }
          a.select_action(0xcc);
          return;
        }
        if (((0 < i16(state.impactX)) || (5 < i16(a.animation.cursor.index))) &&
           ((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 1)) {
          a.select_action(0xcf);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          if ((0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) &&
             (i16(state.impactX) < 1)) {
            selectFrame(2);
            state.impactX = i16(state.impactX) + 1;
            return;
          }
          a.select_action(0xcf);
          return;
        }
        if (((i16(a.animation.cursor.elapsed) == 0) &&
            ((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 1)) &&
           (0 < i16(state.impactX))) {
          a.select_action(0xcf);
          return;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 11.5f;
          shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0xf);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0x10);
        return;
      case 0xc9:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -15.0f;
          shared(0x7d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x50),double(a.motion.y) + double(80.0f),-(int)(i8)u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(80.0f),-(int)(i8)u8(a.motion.facing),1);
          events.system_sound(0x1f);
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(1.0f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0xca:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
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
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 10.0f;
        a.motion.velocityY = 5.0f;
        a.motion.accelerationY = 0.6000000238418579f;
        shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        shared(0x7e,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        events.system_sound(0x1f);
        return;
      case 0xcb:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
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
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -10.0f;
          a.motion.velocityY = 5.0f;
          a.motion.accelerationY = 0.6000000238418579f;
          shared(0x7d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x50),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          events.system_sound(0x1f);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (0 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(1);
        return;
      case 0xcc:
        if ((double(a.motion.y) == double(0.0f)) || (double(0.0f) < double(a.motion.velocityY))) {
          trail(3,4,0xff);
          a.combat.recoveryTimers[2] = 2;
        }
        if ((i16(a.animation.cursor.index) == 0) && (u8(a.carryMomentum) == (u8)0x0)) {
          a.motion.clear();
        }
        if ((2 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.clear();
          return;
        }
        if ((i16(a.animation.cursor.index) == 4) && (double(a.motion.velocityY) < double(4.0f))) {
          selectFrame(5);
        }
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.elapsed) < i16(a.animation.cursor.duration)) {
          return;
        }
        a.animation.cursor.elapsed = 0;
        a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
        if (i16(a.animation.cursor.index) == 3) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 23.0f;
          a.motion.accelerationY = 0.75f;
          shared(0x3c,double(a.motion.x),double(a.motion.y),1,1);
          a.carryMomentum = (u8)0x0;
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
        if ((2 < i16(a.animation.cursor.index)) &&
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
        if (i16(a.animation.cursor.index) == 3) {
          a.motion.velocityX = 11.0f;
          a.motion.velocityY = 17.5f;
          a.motion.accelerationY = 0.800000011920929f;
          shared(0x3c,double(a.motion.x),double(a.motion.y),1,1);
          a.carryMomentum = (u8)0x0;
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
        if ((2 < i16(a.animation.cursor.index)) &&
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
        if (i16(a.animation.cursor.index) == 3) {
          a.motion.velocityX = -11.0f;
          a.motion.velocityY = 17.5f;
          a.motion.accelerationY = 0.800000011920929f;
          shared(0x3c,double(a.motion.x),double(a.motion.y),1,1);
          a.carryMomentum = (u8)0x0;
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
        a.combat.spirit = i16(a.combat.spirit) + -6;
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
        a.combat.spirit = i16(a.combat.spirit) + -6;
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
          a.motion.velocityX = 12.5f;
        }
        if ((0 < i16(a.animation.cursor.index)) &&
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
        events.system_sound(0x1d);
        owned(0x341,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
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
          state.impactX = 0;
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1c);
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
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1c);
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
        if (i16(a.animation.cursor.index) != 1) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1b);
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
        events.system_sound(0x1c);
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
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1d);
        return;
      case 0x133:
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
          state.impactX = 0;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1d);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.x =
             (float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x);
        return;
      case 0x134:
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
          state.impactX = 0;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x334,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1d);
        return;
      case 0x135:
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
          state.impactX = 0;
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x339,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1d);
        return;
      case 0x136:
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
        if (((2 < i16(a.animation.cursor.index)) && (double(0.0f) < double(a.motion.velocityX))) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(1.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
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
        owned(0x33a,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        events.system_sound(0x1d);
        return;
      }
    }
    else if (iVar5 < 0x1f5) {
      if (iVar5 == 500) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((double(0.0f) < double(a.motion.velocityX)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.3f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          selectFrame(0xd);
          a.motion.y = 0.0f;
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x338,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(1);
          a.motion.velocityX = 8.0f;
          a.motion.velocityY = 9.5f;
          a.motion.accelerationY = 0.6499999761581421f;
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 0xd) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xb);
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
          a.motion.velocityX = 12.5f;
        }
        if ((0 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.8f),
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
      case 0x188:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = 12.5f;
        }
        if ((0 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.8f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(2);
        }
        if (i16(a.animation.cursor.index) != 2) {
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
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = 12.5f;
          a.motion.accelerationY = 0.0f;
        }
        if ((double(a.motion.y) == double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(1.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        if (((double(0.0f) < double(a.motion.y)) && (i16(a.animation.cursor.index) < 7)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.5f);
          selectFrame(7);
          return;
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
        a.motion.velocityX = 15.0f;
        a.motion.velocityY = 8.5f;
        a.motion.accelerationY = 0.8500000238418579f;
        events.system_sound(0x1d);
        return;
      case 0x18a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = 15.0f;
          state.impactX = 0;
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
          state.impactX = 0;
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
           (i16(state.impactX) < 2)) {
          state.impactX = i16(state.impactX) + 1;
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
          a.remainingHits = 1;
          a.response = 0;
          a.motion.clear();
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = 1.0f;
          owned(800,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          consumeSpirit(0xfa);
          events.sound(0);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.response = 1;
        return;
      case 0x191:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x321,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(124.0f),u8(a.motion.facing),1);
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
          a.remainingHits = 1;
          a.response = 0;
          a.motion.clear();
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(2);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = 3.0f;
          owned(800,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          consumeSpirit(0xfa);
          events.sound(0);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.response = 1;
        return;
      case 0x194:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.velocityY = 0.0f;
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = 2.0f;
          owned(800,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          consumeSpirit(0xfa);
          events.sound(0);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.response = 1;
        return;
      case 0x196:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.motion.clear();
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
        events.sound(3);
        owned(0x353,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        consumeSpirit(200);
        return;
      case 0x198:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.motion.clear();
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
        events.sound(3);
        owned(0x351,(float)((i8)u8(a.motion.facing) * 0x5a) + double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
        consumeSpirit(200);
        return;
      case 0x19a:
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.velocityY = 0.0f;
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x352,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
          consumeSpirit(200);
          events.sound(3);
          a.motion.velocityX = -5.0f;
          a.motion.velocityY = 12.0f;
          a.motion.accelerationY = 0.6499999761581421f;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.response = 1;
        return;
      case 0x19c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.motion.clear();
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
        a.response = 1;
        return;
      case 0x19e:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.motion.clear();
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
        a.response = 1;
        return;
      case 0x1a0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.2f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.2f);
        }
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.y = 0.0f;
        }
        if (3 < i16(a.animation.cursor.index)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.response = 1;
        return;
      }
    }
    else if (iVar5 < 0x253) {
      if (iVar5 == 0x252) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.selectedSpell = (u8)((i8)u8(a.selectedSpell) + -1);
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
        }
        if ((i16(a.combat.spiritBreak) == 0) && (a.response == 1)) {
          a.combat.spirit = 1000;
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
        shared(0x37,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        events.system_sound(0x21);
        other.knockdownTime = 0;
        return;
      }
      switch(iVar5) {
      case 0x1f5:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((double(0.0f) < double(a.motion.velocityX)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.3f),
           double(a.motion.velocityX) < double(2.0f))) {
          a.motion.velocityX = 2.0f;
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          selectFrame(0x18);
          a.motion.y = 0.0f;
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 9.5f;
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x33b,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(1);
        }
        if ((i16(a.animation.cursor.index) == 0xd) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x338,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(1);
        }
        if ((i16(a.animation.cursor.index) == 0xe) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 10.0f;
          a.motion.velocityY = 15.0f;
          a.motion.accelerationY = 0.6499999761581421f;
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 0x18) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x16);
        return;
      case 0x1f6:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((double(0.0f) < double(a.motion.velocityX)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if (((7 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
           (u8(a.command) == (u8)0x14)) {
          a.select_action(0x1f7);
          a.motion.clear();
          a.reset_command_history();
          return;
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
        a.motion.velocityX = 14.0f;
        shared(0x80,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        owned(0x336,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        events.sound(2);
        return;
      case 0x1f7:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((double(0.0f) < double(a.motion.velocityX)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if (((3 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 7)) &&
           (u8(a.command) == (u8)0x14)) {
          a.select_action(0x1f8);
          a.motion.clear();
          a.reset_command_history();
          return;
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
        owned(0x337,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        events.sound(3);
        return;
      case 0x1f8:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if (((double(0.0f) < double(a.motion.velocityX)) && (double(a.motion.y) == double(0.0f))) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          selectFrame(0xf);
          a.motion.y = 0.0f;
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 8.0f;
          a.motion.velocityY = 12.0f;
          a.motion.accelerationY = 0.800000011920929f;
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x335,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(4);
        }
        if (i16(a.animation.cursor.index) != 0xf) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xe);
        return;
      case 0x1f9:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (a.response == 3)) {
          shared(0x32,(float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x),double(a.motion.y) + double(76.0f),u8(a.motion.facing),1);
          events.system_sound(0x14);
          other.combat.freeze = 0x1e;
          a.select_action(0x1fb);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x1fa:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (a.response == 3)) {
          shared(0x32,(float)((i8)u8(a.motion.facing) * 0x24) + double(a.motion.x),double(a.motion.y) + double(41.0f),u8(a.motion.facing),1);
          events.system_sound(0x14);
          other.combat.freeze = 0x1e;
          a.select_action(0x1fc);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x1fb:
        if ((double(0.0f) < double(a.motion.velocityX)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.75f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 15.0f;
        return;
      case 0x1fc:
        if ((double(0.0f) < double(a.motion.velocityX)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.75f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 15.0f;
        return;
      case 0x1fd:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 0;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (a.response == 1)) {
          shared(0x50,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(41.0f),u8(a.motion.facing),1);
          other.select_action(0x66);
          other.motion.clear();
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x);
          a.select_action(0x1ff);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if ((double(other.motion.y) == double(0.0f)) &&
           ((i16(other.animation.cursor.action) < 0x32 ||
            (0x95 < i16(other.animation.cursor.action))))) {
          a.remainingHits = 1;
          return;
        }
        a.remainingHits = 0;
        return;
      case 0x1fe:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 0;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (a.response == 1)) {
          shared(0x50,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(41.0f),u8(a.motion.facing),1);
          other.motion.clear();
          other.select_action(0x66);
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x);
          a.select_action(0x200);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if ((double(other.motion.y) == double(0.0f)) &&
           ((i16(other.animation.cursor.action) < 0x32 ||
            (0x95 < i16(other.animation.cursor.action))))) {
          a.remainingHits = 1;
          return;
        }
        a.remainingHits = 0;
        return;
      case 0x1ff:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 0;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.select_action(100);
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x4a) + double(a.motion.x);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.select_action(100);
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x28) + double(a.motion.x);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.select_action(100);
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x24) + double(a.motion.x);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.select_action(100);
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x21) + double(a.motion.x);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.select_action(100);
          other.motion.x =
               (float)((int)(i8)u8(a.motion.facing) << 5) + double(a.motion.x);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        other.select_action(0x35);
        return;
      case 0x200:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 0;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          return;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.select_action(100);
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x4a) + double(a.motion.x);
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.select_action(100);
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x2e) + double(a.motion.x);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.select_action(100);
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x17) + double(a.motion.x);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.select_action(100);
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0xd) + double(a.motion.x);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.select_action(100);
          other.motion.facing = u8(a.motion.facing);
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * -6) + double(a.motion.x);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.select_action(100);
          other.motion.x =
               double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x1a);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        other.select_action(0x35);
        return;
      case 0x201:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((double(0.0f) < double(a.motion.velocityX)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.75f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x336,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(2);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 30.0f;
        return;
      }
    }
    else {
      switch(iVar5) {
      case 0x253:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.selectedSpell = (u8)((i8)u8(a.selectedSpell) + -1);
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
        }
        if ((i16(a.combat.spiritBreak) == 0) && (a.response == 1)) {
          a.combat.spirit = 1000;
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
          state.impactX = 0;
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
          state.impactX = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x37,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          events.system_sound(0x21);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (0 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(6);
        return;
      case 0x256:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
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
        events.system_sound(0x21);
        return;
      case 599:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x37,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          events.system_sound(0x21);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (0 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(6);
        return;
      case 600:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 1;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x340,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        consumeSpirit(0xfa);
        events.sound(5);
        return;
      case 0x259:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 1;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x34d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        consumeSpirit(500);
        events.sound(5);
        return;
      case 700:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 0;
          a.remainingHits = 1;
          a.motion.clear();
        }
        if ((8 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(1.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if (i16(a.animation.cursor.index) == 7) {
          a.pose.scale[0] = double(a.pose.scale[0]) + double(0.2f);
          a.pose.scale[1] = double(a.pose.scale[1]) - double(0.19f);
        }
        if (i16(a.animation.cursor.index) == 9) {
          a.pose.scale[0] = double(a.pose.scale[0]) - double(0.2f);
          a.pose.scale[1] = double(a.pose.scale[1]) + double(0.19f);
        }
        if (((i16(state.impactX) == 0) && (a.response != 0)) &&
           (state.impactX = i16(state.impactX) + 1,
           a.response == 1)) {
          events.sound(0xd);
          ownedVisual(0x346,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1,0,1);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          events.system_sound(0x17);
          shared(0x73,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 6),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(10);
          a.begin_pose(-200.0f,60.0f,0.0f);
          a.pose.scale[0] = 1.0f;
          a.pose.scale[1] = 1.0f;
          a.pose.set_channel(24,u8((u8)0xff));
          a.pose.set_channel(16,u8((u8)0xff));
          a.pose.set_channel(8,u8((u8)0xff));
          a.pose.set_channel(0,u8((u8)0xff));
        }
        if ((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.pose.scale[0] = 1.0f;
          a.pose.scale[1] = 1.0f;
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.begin_pose(100.0f,60.0f,0.0f);
        a.pose.scale[0] = 3.0f;
        a.pose.scale[1] = 0.05000000074505806f;
        a.motion.velocityX = 20.0f;
        consumeSpirit(500);
        shared(0x80,(float)((i8)u8(a.motion.facing) * 0x96) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x2bd:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 0;
          a.remainingHits = 1;
          a.motion.clear();
        }
        if ((8 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(1.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if (i16(a.animation.cursor.index) == 7) {
          a.pose.scale[0] = double(a.pose.scale[0]) + double(0.2f);
          a.pose.scale[1] = double(a.pose.scale[1]) - double(0.19f);
        }
        if (i16(a.animation.cursor.index) == 9) {
          a.pose.scale[0] = double(a.pose.scale[0]) - double(0.2f);
          a.pose.scale[1] = double(a.pose.scale[1]) + double(0.19f);
        }
        if (((i16(state.impactX) == 0) && (a.response != 0)) &&
           (state.impactX = i16(state.impactX) + 1,
           a.response == 1)) {
          ownedVisual(0x346,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1,0,1);
          events.sound(0xd);
          a.select_action(0x2c2);
          a.motion.clear();
          a.motion.x = double(other.motion.x);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x74,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 6),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0xb);
          a.begin_pose(-200.0f,60.0f,0.0f);
          a.pose.scale[0] = 1.0f;
          a.pose.scale[1] = 1.0f;
          a.pose.set_channel(24,u8((u8)0xff));
          a.pose.set_channel(16,u8((u8)0xff));
          a.pose.set_channel(8,u8((u8)0xff));
          a.pose.set_channel(0,u8((u8)0xff));
        }
        if ((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.pose.scale[0] = 1.0f;
          a.pose.scale[1] = 1.0f;
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.begin_pose(100.0f,60.0f,0.0f);
        a.pose.scale[0] = 3.0f;
        a.pose.scale[1] = 0.05000000074505806f;
        a.motion.velocityX = 30.0f;
        consumeSpirit(500);
        shared(0x80,(float)((i8)u8(a.motion.facing) * 0x96) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x2be:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 0;
          a.remainingHits = 6;
          a.motion.clear();
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x73,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 6),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
          uVar3 = i16(i32(a.motion.x));
          state.impactX = uVar3;
          uVar3 = i16(i32(a.motion.y));
          state.impactY = uVar3;
          ownedVisual(0x33c,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
        }
        if ((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(500);
          events.sound(6);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(5.0f);
        shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x2bf:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 0;
          a.remainingHits = 99;
          a.remainingHits = 1;
          a.motion.clear();
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x74,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 6),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
          uVar3 = i16(i32(a.motion.x));
          state.impactX = uVar3;
          uVar3 = i16(i32(a.motion.y));
          state.impactY = uVar3;
          ownedVisual(0x33d,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
        }
        if ((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(500);
          events.sound(7);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(10.0f);
        shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x2c0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 0;
          a.remainingHits = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          events.sound(8);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x73,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        if (10 < i16(a.animation.cursor.index)) {
          state.phantomTime = 0x1a4;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        ownedVisual(0x345,double(a.motion.x),double(a.motion.y) + double(100.0f),1,1,0,1);
        return;
      case 0x2c1:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 0;
          a.remainingHits = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          events.sound(9);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x74,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        state.phantomTime = 0x1a4;
        a.select_action(0);
        ownedVisual(0x345,double(a.motion.x),double(a.motion.y) + double(100.0f),1,1,0,1);
        return;
      case 0x2c2:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          state.emissionIndex = 0;
          a.response = 0;
          a.remainingHits = 1;
          a.motion.clear();
        }
        if ((9 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(1.5f),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
           (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          a.select_action(0x2c3);
          return;
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(state.impactY) == 0)) {
          sVar4 = i16(state.impactY);
          state.impactY = sVar4 + 1;
          owned(0x34c,double(a.motion.x),double(other.motion.y) + double(100.0f),u8(a.motion.facing),1);
          events.sound(0xe);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          selectFrame(0x10);
        }
        if ((i16(a.animation.cursor.index) < 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x34b,double(other.motion.x),double(other.motion.y) + double(100.0f),u8(a.motion.facing),1);
          if (i16(state.impactX) == 0) {
            a.launchParameters[0] = 0.0f;
            owned(0x34a,double(other.motion.x) -
                               (float)((i8)u8(a.motion.facing) * 100),double(other.motion.y) + double(200.0f),u8(a.motion.facing),1);
            owned(0x329,double(other.motion.x),double(other.motion.y) + double(250.0f),-(int)(i8)u8(a.motion.facing),1);
          }
          if (i16(state.impactX) == 1) {
            a.launchParameters[0] = 0.0f;
            owned(0x34a,(float)((i8)u8(a.motion.facing) * 100) +
                               double(other.motion.x),double(other.motion.y),-(int)(i8)u8(a.motion.facing),1);
            owned(0x329,double(other.motion.x),double(other.motion.y) - double(50.0f),-(int)(i8)u8(a.motion.facing),1);
          }
          if (i16(state.impactX) == 2) {
            a.launchParameters[0] = 90.0f;
            owned(0x34a,(float)((i8)u8(a.motion.facing) * 100) +
                               double(other.motion.x),double(other.motion.y) + double(200.0f),u8(a.motion.facing),1);
            owned(0x329,(float)((i8)u8(a.motion.facing) * 0x96) +
                               double(other.motion.x),double(other.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          }
          if (i16(state.impactX) == 3) {
            a.launchParameters[0] = -90.0f;
            owned(0x34a,double(other.motion.x) -
                               (float)((i8)u8(a.motion.facing) * 100),double(other.motion.y),u8(a.motion.facing),1);
            owned(0x329,double(other.motion.x) -
                               (float)((i8)u8(a.motion.facing) * 0x96),double(other.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          }
          if (i16(state.impactX) == 4) {
            a.launchParameters[0] = 45.0f;
            owned(0x34a,double(other.motion.x) + double(0.0f),double(other.motion.y) + double(220.0f),u8(a.motion.facing),1);
            owned(0x329,(float)((i8)u8(a.motion.facing) * 0x7d) +
                               double(other.motion.x),double(other.motion.y) + double(225.0f),-(int)(i8)u8(a.motion.facing),1);
          }
          if (i16(state.impactX) == 5) {
            a.launchParameters[0] = -45.0f;
            owned(0x34a,double(other.motion.x) + double(0.0f),double(other.motion.y) - double(20.0f),-(int)(i8)u8(a.motion.facing),1);
            owned(0x329,double(other.motion.x) -
                               (float)((i8)u8(a.motion.facing) * 0x7d),double(other.motion.y) - double(25.0f),u8(a.motion.facing),1);
          }
          if (i16(state.impactX) == 6) {
            a.launchParameters[0] = 45.0f;
            owned(0x34a,(float)((i8)u8(a.motion.facing) * 0x78) +
                               double(other.motion.x),double(other.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
            owned(0x329,(float)((i8)u8(a.motion.facing) * 0x7d) +
                               double(other.motion.x),double(other.motion.y) - double(25.0f),u8(a.motion.facing),1);
          }
          if (i16(state.impactX) == 7) {
            a.launchParameters[0] = -45.0f;
            owned(0x34a,double(other.motion.x) -
                               (float)((i8)u8(a.motion.facing) * 0x78),double(other.motion.y) + double(100.0f),u8(a.motion.facing),1);
            owned(0x329,double(other.motion.x) -
                               (float)((i8)u8(a.motion.facing) * 0x7d),double(other.motion.y) + double(225.0f),-(int)(i8)u8(a.motion.facing),1);
          }
          state.impactX = i16(state.impactX) + 1;
          if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
             (i16(state.emissionIndex) == 0)) {
            state.impactX = 1;
            state.emissionIndex = i16(state.emissionIndex) + 1;
            selectFrame(1);
            a.launchParameters[0] = 0.0f;
            owned(0x34a,double(other.motion.x) -
                               (float)((i8)u8(a.motion.facing) * 100),double(other.motion.y) + double(200.0f),u8(a.motion.facing),1);
            owned(0x329,double(other.motion.x),double(other.motion.y) + double(250.0f),-(int)(i8)u8(a.motion.facing),1);
          }
        }
        if (i16(a.animation.cursor.index) != 9) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.begin_pose(0.0f,0.0f,0.0f);
        a.motion.x = double(other.motion.x);
        a.motion.y = 0.0f;
        a.motion.velocityX = 0.0f;
        a.motion.velocityY = 50.0f;
        return;
      case 0x2c3:
        a.motion.clear();
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        if (a.controls.current.vertical == 0) {
          a.select_action(0);
          return;
        }
        a.select_action(1);
        return;
      case 0x2db:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 0;
          a.remainingHits = 1;
          state.impactX = 0;
          state.impactY = 0;
          state.emissionIndex = 0;
          a.motion.clear();
        }
        if (((1 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 10)) &&
           (position = (double)std::abs(float(double(other.motion.x) -
                                          double(a.motion.x))), position < (double)double(320.0f))) {
          selectFrame(10);
          return;
        }
        if ((0xf < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.25f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          uVar3 = random.range(0x1e,0x3c);
          a.commandTimers[2] = uVar3;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 12.5f;
        }
        if (((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 1)) {
          state.impactX = i16(state.impactX) + 1;
          selectFrame(2);
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x80,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          owned(0x336,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(2);
        }
        if (i16(a.animation.cursor.index) != 0x10) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 17.5f;
        return;
      case 0x2dc:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          state.impactX = 0;
          state.impactY = 0;
          state.emissionIndex = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 0;
          a.remainingHits = 1;
          a.motion.velocityX = 10.0f;
          a.motion.velocityY = 0.0f;
          owned(0x33b,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(1);
        }
        if ((i16(a.animation.cursor.index) == 0xd) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x338,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(1);
        }
        if ((double(0.0f) < double(a.motion.velocityX)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if (((double(0.0f) < double(a.motion.y)) && (i16(a.animation.cursor.index) < 0x18)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.65f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(0x18);
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.y = 0.0f;
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          uVar3 = random.range(0,0x1e);
          a.commandTimers[2] = uVar3;
        }
        if (((i16(a.animation.cursor.index) == 0xd) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 1)) {
          state.impactX = i16(state.impactX) + 1;
          selectFrame(2);
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 12.5f;
          a.motion.velocityY = 15.0f;
          a.response = 0;
          a.remainingHits = 1;
        }
        if (i16(a.animation.cursor.index) != 0x18) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x16);
        return;
      case 0x2dd:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          state.impactX = 0;
          state.impactY = 0;
          state.emissionIndex = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 0;
          a.remainingHits = 1;
          a.motion.velocityX = 10.0f;
          a.motion.velocityY = 0.0f;
        }
        if (((double(0.0f) < double(a.motion.y)) && (i16(a.animation.cursor.index) < 0x1a)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.8f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(0x1a);
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.y = 0.0f;
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          uVar3 = random.range(0,0x3c);
          a.commandTimers[2] = uVar3;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          iVar5 = random.range(0,6);
          a.motion.velocityX = (float)iVar5 + double(10.5f);
          a.motion.velocityY = 16.0f;
          a.response = 0;
          a.remainingHits = 1;
        }
        if ((i16(a.animation.cursor.index) == 0x12) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x335,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(4);
        }
        if (i16(a.animation.cursor.index) != 0x1a) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x18);
        return;
      case 0x2e4:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 0;
          a.remainingHits = 1;
          state.impactX = 0;
          state.impactY = 0;
          state.emissionIndex = 0;
          a.motion.clear();
        }
        if (((1 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 10)) &&
           (position = (double)std::abs(float(double(other.motion.x) -
                                          double(a.motion.x))), position < (double)double(400.0f))) {
          iVar5 = random.range(0,4);
          a.select_action(iVar5 + 0x2e4);
          return;
        }
        if ((9 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.25f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          uVar3 = random.range(0x1e,0x3c);
          a.commandTimers[2] = uVar3;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 12.5f;
        }
        if (i16(a.animation.cursor.index) != 10) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (1 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(2);
        return;
      case 0x2e5:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 0;
          a.remainingHits = 1;
        }
        a.motion.velocityX = double(a.motion.velocityX) - double(0.5f);
        if (double(a.motion.velocityX) < double(0.0f)) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          if (i16(a.animation.cursor.index) != 2) {
            return;
          }
          if (i16(a.animation.cursor.elapsed) != 0) {
            return;
          }
          a.motion.velocityX = 15.0f;
          events.sound(7);
          owned(0x38e,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          return;
        }
        uVar3 = random.range(0,100);
        state.impactX = uVar3;
        if (a.response == 0) {
          a.select_action(0);
          uVar3 = random.range(0x3c,0x78);
          a.commandTimers[2] = uVar3;
          return;
        }
        if (i16(state.impactX) < 0x1e) {
          a.select_action(0x2e6);
          return;
        }
        if (i16(state.impactX) < 0x3c) {
          a.select_action(0x2e7);
          return;
        }
        a.select_action(0x2e8);
        return;
      case 0x2e6:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 0;
          a.remainingHits = 1;
        }
        a.motion.velocityX = double(a.motion.velocityX) - double(0.5f);
        if (double(a.motion.velocityX) < double(0.0f)) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          if (i16(a.animation.cursor.index) != 3) {
            return;
          }
          if (i16(a.animation.cursor.elapsed) != 0) {
            return;
          }
          a.motion.velocityX = 15.0f;
          events.sound(7);
          owned(0x38e,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          return;
        }
        uVar3 = random.range(0,100);
        state.impactX = uVar3;
        if (a.response == 0) {
          a.select_action(0);
          uVar3 = random.range(0x3c,0x78);
          a.commandTimers[2] = uVar3;
          return;
        }
        if (i16(state.impactX) < 0x1e) {
          a.select_action(0x2e5);
          return;
        }
        if (i16(state.impactX) < 0x3c) {
          a.select_action(0x2e7);
          return;
        }
        a.select_action(0x2e8);
        return;
      case 0x2e7:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 0;
          a.remainingHits = 99;
          a.motion.clear();
        }
        if (((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) < 0x1e)) &&
           (state.impactY = i16(state.impactY) + 1,
           4 < i16(state.impactY))) {
          shared(0x3e,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x41),double(a.motion.y) + double(90.0f),1,1);
          state.impactY = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(0,0x3c);
          a.commandTimers[2] = (sVar4 + 0x3c) - (i16)state.difficulty;
          return;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x17);
          uVar3 = i16(i32(a.motion.x));
          state.impactX = uVar3;
          uVar3 = i16(i32(a.motion.y));
          state.impactY = uVar3;
          ownedVisual(0x33d,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          ownedVisual(0x38b,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
          events.sound(7);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(10.0f);
        shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x2e8:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 0;
          a.remainingHits = 99;
          a.motion.clear();
        }
        if (((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) < 0x1e)) &&
           (state.impactY = i16(state.impactY) + 1,
           4 < i16(state.impactY))) {
          shared(0x3e,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x41),double(a.motion.y) + double(90.0f),1,1);
          state.impactY = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(0,0x3c);
          a.commandTimers[2] = (sVar4 + 0x3c) - (i16)state.difficulty;
          return;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x17);
          uVar3 = i16(i32(a.motion.x));
          state.impactX = uVar3;
          uVar3 = i16(i32(a.motion.y));
          state.impactY = uVar3;
          ownedVisual(0x33d,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          ownedVisual(0x38b,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
          events.sound(7);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(10.0f);
        shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x2e9:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 0;
          a.remainingHits = 99;
          a.motion.clear();
        }
        if (((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) < 0x1e)) &&
           (state.impactY = i16(state.impactY) + 1,
           4 < i16(state.impactY))) {
          shared(0x3e,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x41),double(a.motion.y) + double(90.0f),1,1);
          state.impactY = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(0,0x3c);
          a.commandTimers[2] = (sVar4 + 0x3c) - (i16)state.difficulty;
          return;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x17);
          uVar3 = i16(i32(a.motion.x));
          state.impactX = uVar3;
          uVar3 = i16(i32(a.motion.y));
          state.impactY = uVar3;
          ownedVisual(0x33d,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          ownedVisual(0x38c,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
          events.sound(7);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(10.0f);
        shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x2ea:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 0;
          a.remainingHits = 99;
          a.motion.clear();
        }
        if (((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) < 0x1e)) &&
           (state.impactY = i16(state.impactY) + 1,
           4 < i16(state.impactY))) {
          shared(0x3e,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x41),double(a.motion.y) + double(90.0f),1,1);
          state.impactY = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(0,0x3c);
          a.commandTimers[2] = (sVar4 + 0x3c) - (i16)state.difficulty;
          return;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x17);
          uVar3 = i16(i32(a.motion.x));
          state.impactX = uVar3;
          uVar3 = i16(i32(a.motion.y));
          state.impactY = uVar3;
          ownedVisual(0x33d,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          ownedVisual(0x38c,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
          events.sound(7);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(10.0f);
        shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x2f0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.response = 0;
          a.remainingHits = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) < 6) &&
           (state.impactY = i16(state.impactY) + 1,
           4 < i16(state.impactY))) {
          shared(0x3e,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x28),double(a.motion.y) + double(35.0f),1,1);
          state.impactY = 0;
        }
        if ((10 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(1.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if (i16(a.animation.cursor.index) == 7) {
          a.pose.scale[0] = double(a.pose.scale[0]) + double(0.2f);
          a.pose.scale[1] = double(a.pose.scale[1]) - double(0.19f);
        }
        if (i16(a.animation.cursor.index) == 9) {
          a.pose.scale[0] = double(a.pose.scale[0]) - double(0.2f);
          a.pose.scale[1] = double(a.pose.scale[1]) + double(0.19f);
          a.motion.velocityX = 2.0f;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0xd);
          owned(0x38d,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(10);
          a.begin_pose(-200.0f,60.0f,0.0f);
          a.pose.scale[0] = 1.0f;
          a.pose.scale[1] = 1.0f;
          a.pose.set_channel(24,u8((u8)0xff));
          a.pose.set_channel(16,u8((u8)0xff));
          a.pose.set_channel(8,u8((u8)0xff));
          a.pose.set_channel(0,u8((u8)0xff));
        }
        if ((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.pose.scale[0] = 1.0f;
          a.pose.scale[1] = 1.0f;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.commandTimers[2] = state.difficulty * 0x1e + 0xf0;
          a.begin_pose(100.0f,60.0f,0.0f);
          a.pose.scale[0] = 3.0f;
          a.pose.scale[1] = 0.05000000074505806f;
          a.motion.velocityX = 20.0f;
          shared(0x80,(float)((i8)u8(a.motion.facing) * 0x96) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 15.0f;
        return;
      case 0x2f8:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          state.emissionIndex = 0;
          a.response = 0;
          a.remainingHits = 1;
          a.motion.clear();
        }
        if (((i16(a.animation.cursor.index) < 6) && (i16(a.animation.cursor.elapsed) < 10)) &&
           (state.impactY = i16(state.impactY) + 1,
           4 < i16(state.impactY))) {
          shared(0x3e,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x28),double(a.motion.y) + double(35.0f),1,1);
          state.impactY = 0;
        }
        if ((0xd < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(1.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if (i16(a.animation.cursor.index) == 7) {
          a.pose.scale[0] = double(a.pose.scale[0]) + double(0.2f);
          a.pose.scale[1] = double(a.pose.scale[1]) - double(0.19f);
        }
        if (i16(a.animation.cursor.index) == 9) {
          a.pose.scale[0] = double(a.pose.scale[0]) - double(0.2f);
          a.pose.scale[1] = double(a.pose.scale[1]) + double(0.19f);
          a.motion.velocityX = 2.0f;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0xd);
          owned(0x393,double(a.motion.x),0.0f,u8(a.motion.facing),1);
        }
        if ((6 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xd)) {
          uVar9 = 1;
          uVar6 = (u32)(u8)u8(a.motion.facing);
          fVar8 = double(a.motion.y) + double(80.0f);
          fVar1 = double(a.motion.x);
          iVar5 = random.range(0,200);
          owned(0x392,double(float(double(fVar1)+100.0))-iVar5,fVar8,uVar6,uVar9);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(10);
          a.begin_pose(-200.0f,60.0f,0.0f);
          a.pose.scale[0] = 1.0f;
          a.pose.scale[1] = 1.0f;
          a.pose.set_channel(24,u8((u8)0xff));
          a.pose.set_channel(16,u8((u8)0xff));
          a.pose.set_channel(8,u8((u8)0xff));
          a.pose.set_channel(0,u8((u8)0xff));
        }
        if ((i16(a.animation.cursor.index) == 0xd) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.pose.scale[0] = 1.0f;
          a.pose.scale[1] = 1.0f;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.begin_pose(100.0f,60.0f,0.0f);
          a.pose.scale[0] = 3.0f;
          a.pose.scale[1] = 0.05000000074505806f;
          iVar5 = random.range(0,0x3c);
          a.motion.velocityX = (float)iVar5 + double(25.0f);
          shared(0x80,(float)((i8)u8(a.motion.facing) * 0x96) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if ((((int)i16(a.animation.cursor.index) == 0xd - state.difficulty) &&
            (i16(a.animation.cursor.elapsed) == 0)) &&
           ((int)i16(state.emissionIndex) < state.difficulty + 1)) {
          state.emissionIndex = i16(state.emissionIndex) + 1;
          selectFrame(7);
          events.sound(10);
          a.begin_pose(-200.0f,60.0f,0.0f);
          a.pose.scale[0] = 1.0f;
          a.pose.scale[1] = 1.0f;
          a.pose.set_channel(24,u8((u8)0xff));
          a.pose.set_channel(16,u8((u8)0xff));
          a.pose.set_channel(8,u8((u8)0xff));
          a.pose.set_channel(0,u8((u8)0xff));
          a.motion.facing = (u8)-(i8)u8(a.motion.facing);
        }
        if (i16(a.animation.cursor.index) != 0xe) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 15.0f;
        return;
      case 0x31b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 1;
          a.motion.clear();
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          fade(0);
          a.begin_spell(mode,replayMode,core);
          events.system_sound(0x17);
          owned(0x3e5,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (7 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(6);
        return;
      case 0x31c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 1;
          a.motion.clear();
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
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
        if (7 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(0xd);
        a.transitionReady = (u8)0x1;
        return;
      }
    }
switchD_0052150a_default:
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
    goto LAB_0052cb47;
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
    goto LAB_0052cb47;
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
    a.motion.velocityX = 3.0f;
    goto LAB_0052cb47;
  case 5:
    a.motion.velocityX = -3.0f;
LAB_0052cb47:
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
      if ((i16(a.animation.cursor.index) < 3) ||
         (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
         double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) ==
         (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
        if ((i16(a.animation.cursor.index) == 4) && (double(a.motion.velocityY) < double(4.0f))) {
          selectFrame(5);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          selectFrame(0xb);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 16.0f;
          a.motion.accelerationY = 0.800000011920929f;
          a.carryMomentum = (u8)0x0;
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
      if ((i16(a.animation.cursor.index) < 3) ||
         (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
         double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) ==
         (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 5.5f;
          a.motion.velocityY = 16.0f;
          a.motion.accelerationY = 0.800000011920929f;
          a.carryMomentum = (u8)0x0;
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
      if ((i16(a.animation.cursor.index) < 3) ||
         (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
         double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) ==
         (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          selectFrame(0xd);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -5.5f;
          a.motion.velocityY = 16.0f;
          a.motion.accelerationY = 0.800000011920929f;
          a.carryMomentum = (u8)0x0;
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
    goto switchD_0052150a_default;
  }
  return;

}
}
