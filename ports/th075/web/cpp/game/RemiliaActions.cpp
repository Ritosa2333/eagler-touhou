#include "RemiliaState.hpp"
#include <cmath>
namespace th075 {
void advance_remilia_action(FighterActor& a,FighterActor& other,RemiliaState& state,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){

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
  int local_44;
  int local_40;
  int local_3c;
  int local_38;
  int local_34;
  int local_30;
  int local_2c;
  int local_28;
  int local_24;
  int local_20;
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
        if ((i16(a.animation.cursor.index) == 1) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(2);
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
        if ((i16(a.animation.cursor.index) == 1) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(2);
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
        if ((i16(a.animation.cursor.index) == 1) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(2);
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
        if ((i16(a.animation.cursor.index) == 1) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(2);
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
        if (1 < i16(a.animation.cursor.index)) {
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
          selectFrame(2);
          return;
        }
        if (i16(a.animation.cursor.index) != 2) {
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
        if (((i16(a.animation.cursor.index) < 8) && (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(8);
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
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 8.0f;
          a.motion.velocityX = 15.5f;
          a.motion.accelerationY = 0.75f;
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(6);
        return;
      case 0x5e:
        if (((i16(a.animation.cursor.index) < 8) && (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(8);
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
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 8.0f;
          a.motion.velocityX = -15.5f;
          a.motion.accelerationY = 0.75f;
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(6);
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
        if ((i16(a.animation.cursor.index) == 3) &&
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
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = (float)(int)i16(state.impactX) * double(0.005f);
        a.motion.velocityY = (float)-(int)i16(state.impactY) * double(0.005f);
        a.motion.accelerationY = 2.0f;
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
        if (((3 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 5)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(5);
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
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = (float)(int)i16(state.impactX) * double(0.005f);
        a.motion.velocityY = (float)-(int)i16(state.impactY) * double(0.005f);
        a.motion.accelerationY = 2.0f;
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
        if (i16(a.animation.cursor.index) != 2) {
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
        if ((i16(a.animation.cursor.index) == 1) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(2);
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
          state.guardEffectSent = 0;
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
          state.guardEffectSent = 0;
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
          state.guardEffectSent = 0;
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
          state.guardEffectSent = 0;
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
          state.guardEffectSent = 0;
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
          state.guardEffectSent = 0;
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
          state.guardEffectSent = 0;
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
        selectFrame(6);
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
        selectFrame(6);
        return;
      case 200:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          state.impactX = 0;
        }
        if (((double(0.0f) < double(a.motion.y)) && (i16(a.animation.cursor.index) < 7)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(2.5f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(7);
          return;
        }
        if ((i16(a.animation.cursor.index) == 7) && (a.controls.current.vertical < 0)) {
          if (0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) {
            a.select_action(0xcd);
            return;
          }
          a.select_action(0xcc);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          return;
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 15.0f;
        a.motion.velocityY = 25.0f;
        a.motion.accelerationY = 2.5f;
        shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        shared(0x7e,double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        shared(0x81,double(a.motion.x),0.0f,u8(a.motion.facing),0);
        events.system_sound(0x1f);
        return;
      case 0xc9:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          state.impactX = 0;
        }
        if (((double(0.0f) < double(a.motion.y)) && (i16(a.animation.cursor.index) < 6)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(6.0f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(6);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          return;
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = -25.0f;
        a.motion.velocityY = 40.0f;
        a.motion.accelerationY = 6.0f;
        shared(0x7d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x50),double(a.motion.y) + double(80.0f),-(int)(i8)u8(a.motion.facing),1);
        shared(0x7e,double(a.motion.x),double(a.motion.y) + double(80.0f),-(int)(i8)u8(a.motion.facing),1);
        shared(0x81,double(a.motion.x),0.0f,-(int)(i8)u8(a.motion.facing),0);
        events.system_sound(0x1f);
        return;
      case 0xca:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
        }
        if (((double(0.0f) < double(a.motion.y)) && (5 < i16(a.animation.cursor.index))) &&
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
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 30.0f;
          a.motion.velocityY = -4.0f;
          a.motion.accelerationY = 0.6000000238418579f;
          shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          events.system_sound(0x1f);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 10.0f;
        a.motion.velocityY = -4.0f;
        return;
      case 0xcb:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
        }
        if (((double(0.0f) < double(a.motion.y)) && (4 < i16(a.animation.cursor.index))) &&
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
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -30.0f;
          a.motion.velocityY = 2.0f;
          a.motion.accelerationY = 0.6000000238418579f;
          shared(0x7d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x50),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          events.system_sound(0x1f);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = -10.0f;
        a.motion.velocityY = 2.0f;
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
        if ((i16(a.animation.cursor.index) == 4) && (double(a.motion.velocityY) < double(4.0f))) {
          selectFrame(5);
        }
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.elapsed) < i16(a.animation.cursor.duration)) {
          return;
        }
        a.animation.cursor.elapsed = 0;
        a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
        if (i16(a.animation.cursor.index) == 2) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 21.0f;
          a.motion.accelerationY = 0.8500000238418579f;
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
        if ((i16(a.animation.cursor.index) == 4) && (double(a.motion.velocityY) < double(4.0f))) {
          selectFrame(5);
        }
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.elapsed) < i16(a.animation.cursor.duration)) {
          return;
        }
        a.animation.cursor.elapsed = 0;
        a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
        if (i16(a.animation.cursor.index) == 2) {
          a.motion.velocityX = 11.0f;
          a.motion.velocityY = 16.0f;
          a.motion.accelerationY = 0.8500000238418579f;
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
          a.motion.velocityX = -11.0f;
          a.motion.velocityY = 16.0f;
          a.motion.accelerationY = 0.8500000238418579f;
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
          owned(0x3e6,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          events.system_sound(0x2c);
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
          a.motion.velocityX = 10.0f;
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
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(2.0f);
        owned(0x325,(float)((i8)u8(a.motion.facing) * 0xaa) + double(a.motion.x),double(a.motion.y) - double(6.0f),u8(a.motion.facing),0);
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
        if (i16(a.animation.cursor.index) != 4) {
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
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1d);
        return;
      case 0x12f:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
        }
        if ((('\0' < (i8)u8(a.carryMomentum)) && (i16(a.animation.cursor.index) < 4)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        if ((3 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.9f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
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
        a.motion.velocityX = 25.0f;
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
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1d);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x339,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
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
          events.system_sound(0x1c);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1d);
        return;
      case 0x133:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.remainingHits = 1;
          a.response = 0;
        }
        if (double(0.0f) < double(a.motion.velocityX)) {
          a.motion.velocityX = double(a.motion.velocityX) - double(1.5f);
        }
        if (double(a.motion.velocityX) < double(0.0f)) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1c);
          owned(0x341,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 15.0f;
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1c);
        a.remainingHits = 1;
        a.response = 0;
        owned(0x342,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
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
        if (i16(a.animation.cursor.index) != 1) {
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
        if (((i16(a.animation.cursor.index) == 10) || (i16(a.animation.cursor.index) == 0xb)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.impactX = 0;
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 20.0f;
        }
        if (i16(a.animation.cursor.index) != 10) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x33f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
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
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x33e,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
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
        iVar5 = random.range(0,100);
        if (((iVar5 < 0x28) && (3 < i16(a.animation.cursor.index))) && (i16(a.animation.cursor.index) < 8)
           ) {
          uVar9 = 1;
          uVar6 = (u32)(u8)u8(a.motion.facing);
          fVar1 = double(a.motion.y);
          iVar5 = random.range(0,0x8c);
          fVar8 = double(iVar5)+float(double(fVar1)+40.0);
          fVar1 = double(a.motion.x);
          iVar5 = random.range(0,0x3c);
          owned(0x330,double(float(double(fVar1)+30.0))-iVar5,fVar8,uVar6,uVar9);
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
          selectFrame(0xc);
          a.motion.y = 0.0f;
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(4);
          a.motion.velocityX = 8.0f;
          a.motion.velocityY = 14.0f;
          a.motion.accelerationY = 0.800000011920929f;
        }
        if (i16(a.animation.cursor.index) != 0x12) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x10);
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
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x188:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = 15.0f;
        }
        if ((0 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0x189:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.carryMomentum = (u8)0x0;
          a.remainingHits = 1;
          a.response = 0;
          a.motion.accelerationY = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 15.0f;
        }
        if (double(0.0f) < double(a.motion.velocityX)) {
          a.motion.velocityX = double(a.motion.velocityX) - double(0.05f);
        }
        if ((((double(0.0f) < double(a.motion.y)) && (6 < i16(a.animation.cursor.index))) &&
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
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 9.0f;
          a.motion.accelerationY = 0.8999999761581421f;
        }
        if (i16(a.animation.cursor.index) != 9) {
          return;
        }
        if (double(a.motion.y) <= double(0.0f)) {
          return;
        }
        selectFrame(i16(a.animation.cursor.index) + -2);
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
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
        }
        if (a.controls.current.buttons[2] == 0) {
          state.impactX = 1;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0);
          a.launchParameters[0] = 0.0f;
          consumeSpirit(200);
          for (volley = 0; volley < 8; volley = volley + 1) {
            owned(0x324,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(106.0f),u8(a.motion.facing),1);
          }
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
          state.impactX = 0;
        }
        if (a.controls.current.buttons[2] == 0) {
          state.impactX = 1;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0);
          a.launchParameters[0] = 20.0f;
          consumeSpirit(200);
          for (emission = 0; emission < 8; emission = emission + 1) {
            owned(0x324,(float)((int)(i8)u8(a.motion.facing) << 6) + double(a.motion.x),double(a.motion.y) + double(161.0f),u8(a.motion.facing),1);
          }
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
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0);
          a.launchParameters[0] = 45.0f;
          consumeSpirit(200);
          for (local_1c = 0; local_1c < 8; local_1c = local_1c + 1) {
            owned(0x324,(float)((i8)u8(a.motion.facing) * 0x2c) + double(a.motion.x),double(a.motion.y) + double(91.0f),u8(a.motion.facing),1);
          }
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0x194:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 0.0f;
          a.motion.y = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0);
          a.motion.velocityX = -4.0f;
          a.motion.velocityY = 9.0f;
          a.motion.accelerationY = 0.6499999761581421f;
          a.launchParameters[0] = 0.0f;
          consumeSpirit(200);
          for (local_20 = 0; local_20 < 8; local_20 = local_20 + 1) {
            owned(0x324,(float)((i8)u8(a.motion.facing) * 0x23) + double(a.motion.x),double(a.motion.y) + double(111.0f),u8(a.motion.facing),1);
          }
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x195:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 0.0f;
          a.motion.y = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0);
          a.motion.velocityX = -4.0f;
          a.motion.velocityY = 9.0f;
          a.motion.accelerationY = 0.6499999761581421f;
          a.launchParameters[0] = -30.0f;
          consumeSpirit(200);
          for (local_24 = 0; local_24 < 8; local_24 = local_24 + 1) {
            owned(0x324,(float)((i8)u8(a.motion.facing) * 0x1b) + double(a.motion.x),double(a.motion.y) + double(89.0f),u8(a.motion.facing),1);
          }
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x196:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
          a.launchParameters[0] = 0.0f;
          owned(0x330,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y) + double(112.0f),u8(a.motion.facing),1);
          consumeSpirit(100);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (a.controls.current.buttons[0] == 0) {
          return;
        }
        if (5 < i16(state.impactX)) {
          return;
        }
        if (i16(a.combat.spirit) < 1) {
          return;
        }
        events.sound(2);
        a.launchParameters[0] = 0.0f;
        owned(0x330,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y) + double(112.0f),u8(a.motion.facing),1);
        consumeSpirit(10);
        selectFrame(6);
        state.impactX = i16(state.impactX) + 1;
        return;
      case 0x197:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
          a.launchParameters[0] = 30.0f;
          owned(0x330,(float)((i8)u8(a.motion.facing) * 0x44) + double(a.motion.x),double(a.motion.y) + double(159.0f),u8(a.motion.facing),1);
          consumeSpirit(100);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (a.controls.current.buttons[0] == 0) {
          return;
        }
        if (5 < i16(state.impactX)) {
          return;
        }
        if (i16(a.combat.spirit) < 1) {
          return;
        }
        events.sound(2);
        a.launchParameters[0] = 20.0f;
        owned(0x330,(float)((i8)u8(a.motion.facing) * 0x44) + double(a.motion.x),double(a.motion.y) + double(159.0f),u8(a.motion.facing),1);
        consumeSpirit(10);
        selectFrame(6);
        state.impactX = i16(state.impactX) + 1;
        return;
      case 0x198:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(1);
          a.launchParameters[0] = 60.0f;
          consumeSpirit(0xfa);
          owned(0x347,(float)((i8)u8(a.motion.facing) * 0x1c) + double(a.motion.x),double(a.motion.y) + double(87.0f),u8(a.motion.facing),1);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0x19a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 1;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
        }
        if ((5 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(10);
          return;
        }
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 0.0f;
          a.motion.y = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
          a.launchParameters[0] = 0.0f;
          owned(0x330,(float)((i8)u8(a.motion.facing) * 0x23) + double(a.motion.x),double(a.motion.y) + double(111.0f),u8(a.motion.facing),1);
          consumeSpirit(100);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (a.controls.current.buttons[0] == 0) {
          return;
        }
        if (5 < i16(state.impactX)) {
          return;
        }
        if (i16(a.combat.spirit) < 1) {
          return;
        }
        events.sound(2);
        a.launchParameters[0] = 0.0f;
        owned(0x330,(float)((i8)u8(a.motion.facing) * 0x23) + double(a.motion.x),double(a.motion.y) + double(111.0f),u8(a.motion.facing),1);
        consumeSpirit(0x14);
        selectFrame(4);
        state.impactX = i16(state.impactX) + 1;
        return;
      case 0x19b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 1;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
        }
        if ((5 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(10);
          return;
        }
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 0.0f;
          a.motion.y = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
          a.launchParameters[0] = -20.0f;
          owned(0x330,(float)((i8)u8(a.motion.facing) * 0x1b) + double(a.motion.x),double(a.motion.y) + double(89.0f),u8(a.motion.facing),1);
          consumeSpirit(100);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (a.controls.current.buttons[0] == 0) {
          return;
        }
        if (5 < i16(state.impactX)) {
          return;
        }
        if (i16(a.combat.spirit) < 1) {
          return;
        }
        events.sound(2);
        a.launchParameters[0] = -20.0f;
        owned(0x330,(float)((i8)u8(a.motion.facing) * 0x1b) + double(a.motion.x),double(a.motion.y) + double(89.0f),u8(a.motion.facing),1);
        consumeSpirit(0x14);
        selectFrame(4);
        state.impactX = i16(state.impactX) + 1;
        return;
      case 0x19c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
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
        events.sound(3);
        a.launchParameters[0] = 0.0f;
        owned(0x335,(float)((i8)u8(a.motion.facing) * 0x28) + double(a.motion.x),double(a.motion.y) + double(102.0f),u8(a.motion.facing),1);
        consumeSpirit(200);
        return;
      case 0x19e:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(10);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          selectFrame(10);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -6.0f;
          a.motion.velocityY = 17.5f;
          a.motion.accelerationY = 0.800000011920929f;
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(3);
        a.launchParameters[0] = -20.0f;
        owned(0x335,(float)((i8)u8(a.motion.facing) * 0x1b) + double(a.motion.x),double(a.motion.y) + double(89.0f),u8(a.motion.facing),1);
        consumeSpirit(200);
        return;
      case 0x1a0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 0.0f;
          a.motion.y = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(3);
          a.motion.velocityX = -8.0f;
          a.motion.velocityY = 10.0f;
          a.motion.accelerationY = 0.6499999761581421f;
          a.launchParameters[0] = 0.0f;
          consumeSpirit(200);
          owned(0x335,(float)((i8)u8(a.motion.facing) * 0x2d) + double(a.motion.x),double(a.motion.y) + double(115.0f),u8(a.motion.facing),1);
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
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 0.0f;
          a.motion.y = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(3);
          a.motion.velocityX = -8.0f;
          a.motion.velocityY = 10.0f;
          a.motion.accelerationY = 0.6499999761581421f;
          a.launchParameters[0] = -20.0f;
          consumeSpirit(200);
          owned(0x335,(float)((i8)u8(a.motion.facing) * 0x1b) + double(a.motion.x),double(a.motion.y) + double(89.0f),u8(a.motion.facing),1);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
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
        if (i16(a.animation.cursor.index) != 4) {
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
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        iVar5 = random.range(0,100);
        if (((iVar5 < 0x28) && (3 < i16(a.animation.cursor.index))) && (i16(a.animation.cursor.index) < 8)
           ) {
          uVar9 = 1;
          uVar6 = (u32)(u8)u8(a.motion.facing);
          fVar1 = double(a.motion.y);
          iVar5 = random.range(0,0x8c);
          fVar8 = double(iVar5)+float(double(fVar1)+40.0);
          fVar1 = double(a.motion.x);
          iVar5 = random.range(0,0x3c);
          owned(0x330,double(float(double(fVar1)+30.0))-iVar5,fVar8,uVar6,uVar9);
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          selectFrame(0xc);
          a.motion.y = 0.0f;
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(4);
          a.motion.velocityX = 18.0f;
          a.motion.velocityY = 16.0f;
          a.motion.accelerationY = 0.800000011920929f;
        }
        if (i16(a.animation.cursor.index) != 0xc) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(10);
        return;
      case 0x1f6:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if (((i16(a.animation.cursor.index) < 0xb) && (double(0.0f) < double(a.motion.y))) &&
           (2 < i16(a.animation.cursor.index))) {
          a.motion.velocityX = -30.0f;
          a.motion.velocityY = double(a.motion.velocityY) - double(0.6f);
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.motion.clear();
            selectFrame(0xf);
            a.motion.y = 0.0f;
            return;
          }
        }
        if (((10 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xe)) &&
           (cVar2 = a.motion.horizontal_limit(other.motion), cVar2 == '\0')) {
          if (u8(a.motion.facing) == (u8)0x1) {
            a.motion.x = double(a.combat.screenBounds[0]) + double(5.0f);
          }
          if (u8(a.motion.facing) == (u8)0xff) {
            a.motion.x = double(a.combat.screenBounds[1]) - double(5.0f);
          }
        }
        if ((((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
            (double(0.0f) < double(a.motion.y))) &&
           (((cVar2 = a.motion.horizontal_limit(other.motion), cVar2 != '\0' &&
             (((double(a.motion.x) < double(640.0f) && (u8(a.motion.facing) == (u8)0x1)) ||
              ((double(640.0f) < double(a.motion.x) && (u8(a.motion.facing) == (u8)0xff)))))) ||
            (((double(a.motion.x) <= double(a.combat.screenBounds[0]) + double(5.0f) &&
              (u8(a.motion.facing) == (u8)0x1)) ||
             ((double(a.combat.screenBounds[1]) - double(5.0f) < double(a.motion.x) !=
               (double(a.combat.screenBounds[1]) - double(5.0f) == double(a.motion.x)) &&
              (u8(a.motion.facing) == (u8)0xff)))))))) {
          selectFrame(0xb);
          a.motion.clear();
          return;
        }
        if ((i16(a.animation.cursor.index) == 0xe) || (i16(a.animation.cursor.index) == 0xf)) {
          if ((a.response != 0) &&
             ((a.response != 3 && (a.response != 1)))) {
            a.select_action(0x1fa);
            return;
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.motion.velocityY = 0.0f;
            a.motion.velocityX = double(a.motion.velocityX) * double(0.3f);
            selectFrame(0x10);
            a.motion.y = 0.0f;
            return;
          }
        }
        if ((0xf < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(1.5f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(5);
          shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          a.motion.velocityX = -20.0f;
          a.motion.velocityY = 20.0f;
          if (double(0.0f) < double(a.motion.y)) {
            a.motion.velocityY = 8.0f;
          }
          a.motion.accelerationY = 0.800000011920929f;
        }
        if ((i16(a.animation.cursor.index) == 0xe) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(6);
          a.motion.velocityX = 40.0f;
          a.motion.velocityY = -20.0f;
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x336,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (((i16(a.animation.cursor.index) == 0x12) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) == 0)) {
          selectFrame(0x13);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(9);
        return;
      case 0x1f7:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if (((i16(a.animation.cursor.index) < 0xb) && (double(0.0f) < double(a.motion.y))) &&
           (2 < i16(a.animation.cursor.index))) {
          a.motion.velocityX = -30.0f;
          a.motion.velocityY = double(a.motion.velocityY) - double(0.6f);
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.motion.clear();
            selectFrame(0xf);
            a.motion.y = 0.0f;
            return;
          }
        }
        if (((10 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xe)) &&
           (cVar2 = a.motion.horizontal_limit(other.motion), cVar2 == '\0')) {
          if (u8(a.motion.facing) == (u8)0x1) {
            a.motion.x = double(a.combat.screenBounds[0]) + double(5.0f);
          }
          if (u8(a.motion.facing) == (u8)0xff) {
            a.motion.x = double(a.combat.screenBounds[1]) - double(5.0f);
          }
        }
        if ((((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
            (double(0.0f) < double(a.motion.y))) &&
           (((cVar2 = a.motion.horizontal_limit(other.motion), cVar2 != '\0' &&
             (((double(a.motion.x) < double(640.0f) && (u8(a.motion.facing) == (u8)0x1)) ||
              ((double(640.0f) < double(a.motion.x) && (u8(a.motion.facing) == (u8)0xff)))))) ||
            (((double(a.motion.x) <= double(a.combat.screenBounds[0]) + double(5.0f) &&
              (u8(a.motion.facing) == (u8)0x1)) ||
             ((double(a.combat.screenBounds[1]) - double(5.0f) < double(a.motion.x) !=
               (double(a.combat.screenBounds[1]) - double(5.0f) == double(a.motion.x)) &&
              (u8(a.motion.facing) == (u8)0xff)))))))) {
          selectFrame(0xb);
          a.motion.clear();
          return;
        }
        if ((i16(a.animation.cursor.index) == 0xe) || (i16(a.animation.cursor.index) == 0xf)) {
          if ((a.response != 0) &&
             ((a.response != 3 && (a.response != 1)))) {
            a.select_action(0x1fa);
            return;
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.motion.velocityY = 0.0f;
            a.motion.velocityX = double(a.motion.velocityX) * double(0.3f);
            selectFrame(0x10);
            a.motion.y = 0.0f;
            return;
          }
        }
        if ((0xf < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(1.5f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(5);
          shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          a.motion.velocityX = -20.0f;
          a.motion.velocityY = 20.0f;
          if (double(0.0f) < double(a.motion.y)) {
            a.motion.velocityY = 8.0f;
          }
          a.motion.accelerationY = 0.800000011920929f;
        }
        if ((i16(a.animation.cursor.index) == 0xe) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(6);
          a.motion.velocityX = 30.0f;
          a.motion.velocityY = -30.0f;
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x337,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (((i16(a.animation.cursor.index) == 0x12) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) == 0)) {
          selectFrame(0x13);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(9);
        return;
      case 0x1f8:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if (((i16(a.animation.cursor.index) < 0xb) && (double(0.0f) < double(a.motion.y))) &&
           (2 < i16(a.animation.cursor.index))) {
          a.motion.velocityX = -30.0f;
          a.motion.velocityY = double(a.motion.velocityY) - double(0.6f);
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.motion.clear();
            selectFrame(0xf);
            a.motion.y = 0.0f;
            return;
          }
        }
        if (((10 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xe)) &&
           (cVar2 = a.motion.horizontal_limit(other.motion), cVar2 == '\0')) {
          if (u8(a.motion.facing) == (u8)0x1) {
            a.motion.x = double(a.combat.screenBounds[0]) + double(5.0f);
          }
          if (u8(a.motion.facing) == (u8)0xff) {
            a.motion.x = double(a.combat.screenBounds[1]) - double(5.0f);
          }
        }
        if ((((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
            (double(0.0f) < double(a.motion.y))) &&
           (((cVar2 = a.motion.horizontal_limit(other.motion), cVar2 != '\0' &&
             (((double(a.motion.x) < double(640.0f) && (u8(a.motion.facing) == (u8)0x1)) ||
              ((double(640.0f) < double(a.motion.x) && (u8(a.motion.facing) == (u8)0xff)))))) ||
            (((double(a.motion.x) <= double(a.combat.screenBounds[0]) + double(5.0f) &&
              (u8(a.motion.facing) == (u8)0x1)) ||
             ((double(a.combat.screenBounds[1]) - double(5.0f) < double(a.motion.x) !=
               (double(a.combat.screenBounds[1]) - double(5.0f) == double(a.motion.x)) &&
              (u8(a.motion.facing) == (u8)0xff)))))))) {
          selectFrame(0xb);
          a.motion.clear();
          return;
        }
        if ((i16(a.animation.cursor.index) == 0xe) || (i16(a.animation.cursor.index) == 0xf)) {
          if ((a.response != 0) &&
             ((a.response != 3 && (a.response != 1)))) {
            a.begin_pose(0.0f,0.0f,0.0f);
            a.select_action(0x1fa);
            return;
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.motion.velocityY = 0.0f;
            a.motion.velocityX = double(a.motion.velocityX) * double(0.3f);
            selectFrame(0x10);
            a.begin_pose(0.0f,0.0f,0.0f);
            a.motion.y = 0.0f;
            return;
          }
        }
        if ((0xf < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(1.5f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(5);
          shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          a.motion.velocityX = -20.0f;
          a.motion.velocityY = 20.0f;
          if (double(0.0f) < double(a.motion.y)) {
            a.motion.velocityY = 8.0f;
          }
          a.motion.accelerationY = 0.800000011920929f;
        }
        if ((i16(a.animation.cursor.index) == 0xe) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(6);
          a.motion.velocityX = 30.0f;
          a.motion.velocityY = -30.0f;
          (aimRadians = atan(double(other.motion.y) - double(a.motion.y),(double(other.motion.x) - double(a.motion.x)) *
                       (float)(int)(i8)u8(a.motion.facing)));
          uVar3 = i16(i32(aimRadians/double(0.017453289f)));
          state.aimAngle = uVar3;
          if (-0xf < i16(state.aimAngle)) {
            state.aimAngle = 0xfff1;
          }
          a.set_polar_velocity((float)-(int)i16(state.aimAngle),28.0f);
          a.begin_pose(0.0f,80.0f,0.0f);
          position = (double)(aimRadians = atan(double(a.motion.velocityY),double(a.motion.velocityX)));
          a.pose.rotation[2] = (float)-(position / (double)double(0.017453289f) + (double)double(15.0f));
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = (float)(int)i16(state.aimAngle);
          owned(0x338,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (((i16(a.animation.cursor.index) == 0x12) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) == 0)) {
          selectFrame(0x13);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(9);
        return;
      case 0x1fa:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -5.0f;
          a.motion.velocityY = (float)(i16(state.impactX) == 1) * double(5.0f) + double(15.0f);
          a.motion.accelerationY = -0.800000011920929f;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if (((double(0.0f) < double(a.motion.y)) && (i16(a.animation.cursor.index) < 8)) &&
           (a.motion.velocityY = double(a.motion.velocityY) + double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          selectFrame(8);
          a.motion.y = 0.0f;
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(6);
        return;
      case 0x1fb:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
        }
        if (((i16(a.animation.cursor.index) < 8) && (double(0.0f) < double(a.motion.y))) &&
           (2 < i16(a.animation.cursor.index))) {
          a.motion.velocityY = double(a.motion.velocityY) - double(1.0f);
        }
        if (i16(a.animation.cursor.index) == 10) {
          if (((a.response != 0) && (a.response != 3)) &&
             (a.response != 1)) {
            a.select_action(0x1fa);
            return;
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.motion.velocityY = 0.0f;
            a.motion.velocityX = double(a.motion.velocityX) * double(0.3f);
            selectFrame(0xb);
            a.motion.y = 0.0f;
            return;
          }
        }
        if ((10 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(1.5f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x3c,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          a.motion.velocityY = 22.0f;
          a.motion.accelerationY = 0.699999988079071f;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.impactX) < 1) {
            selectFrame(8);
            state.impactX = i16(state.impactX) + 1;
            return;
          }
          a.motion.velocityX = 20.0f;
          a.motion.velocityY = -20.0f;
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(9);
        return;
      case 0x1fe:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
        }
        if (((i16(a.animation.cursor.index) < 0x12) && (double(0.0f) < double(a.motion.y))) &&
           (7 < i16(a.animation.cursor.index))) {
          a.motion.velocityX = double(a.motion.velocityX) - double(0.35f);
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if (double(a.motion.velocityX) < double(3.0f)) {
            a.motion.velocityX = 3.0f;
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            selectFrame(0x12);
            a.motion.y = 0.0f;
            a.motion.clear();
            return;
          }
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x3c,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(4);
          a.motion.velocityY = 6.0f;
          a.motion.velocityX = 4.5f;
          a.motion.accelerationY = 0.6499999761581421f;
          owned(0x33a,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 0)) {
          selectFrame(4);
          state.impactX = i16(state.impactX) + 1;
          return;
        }
        if (i16(a.animation.cursor.index) != 0x12) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x10);
        return;
      case 0x1ff:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
        }
        if (((i16(a.animation.cursor.index) < 0x16) && (double(0.0f) < double(a.motion.y))) &&
           (0xc < i16(a.animation.cursor.index))) {
          a.motion.velocityX = double(a.motion.velocityX) - double(0.35f);
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if (double(a.motion.velocityX) < double(3.0f)) {
            a.motion.velocityX = 3.0f;
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            selectFrame(0x16);
            a.motion.y = 0.0f;
            a.motion.clear();
            return;
          }
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(4);
          shared(0x3c,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          a.motion.velocityY = 8.0f;
          a.motion.velocityX = 12.0f;
          a.motion.accelerationY = 0.5f;
          owned(0x33a,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (((i16(a.animation.cursor.index) == 0xc) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 0)) {
          selectFrame(8);
          state.impactX = i16(state.impactX) + 1;
          return;
        }
        if (i16(a.animation.cursor.index) != 0x16) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x14);
        return;
      case 0x200:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
        }
        if ((7 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.9f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 7) && (a.response == 2)) {
          selectFrame(8);
          a.motion.velocityX = 10.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(7);
          shared(0x3c,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 50.0f;
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 10.0f;
        return;
      case 0x201:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
        }
        if ((7 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.9f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 7) && (a.response == 2)) {
          selectFrame(8);
          a.motion.velocityX = 10.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(7);
          shared(0x3c,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 50.0f;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 10.0f;
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (i16(state.impactX) != 0) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(2);
        a.response = 0;
        a.remainingHits = 1;
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
        if (i16(a.animation.cursor.index) != 4) {
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
        if (i16(a.animation.cursor.index) != 4) {
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
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(100);
          a.launchParameters[0] = -110.0f;
          a.launchParameters[1] = 5.0f;
          owned(0x322,(float)((i8)u8(a.motion.facing) * 0x28) + double(a.motion.x),double(a.motion.y) + double(140.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 110.0f;
          a.launchParameters[1] = 5.0f;
          owned(0x322,(float)((i8)u8(a.motion.facing) * 0x28) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = -70.0f;
          a.launchParameters[1] = 4.0f;
          owned(0x322,(float)((i8)u8(a.motion.facing) * 0x4b) + double(a.motion.x),double(a.motion.y) + double(130.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 70.0f;
          a.launchParameters[1] = 4.0f;
          owned(0x322,(float)((i8)u8(a.motion.facing) * 0x4b) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
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
        consumeSpirit(0xfa);
        a.launchParameters[0] = -10.0f;
        owned(0x321,(float)((i8)u8(a.motion.facing) * 0x28) + double(a.motion.x),double(a.motion.y) + double(150.0f),u8(a.motion.facing),1);
        owned(0x322,(float)((i8)u8(a.motion.facing) * 0x28) + double(a.motion.x),double(a.motion.y) + double(150.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 10.0f;
        owned(0x321,(float)((i8)u8(a.motion.facing) * 0x28) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1);
        owned(0x322,(float)((i8)u8(a.motion.facing) * 0x28) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -6.0f;
        owned(0x321,(float)((i8)u8(a.motion.facing) * 0x4b) + double(a.motion.x),double(a.motion.y) + double(130.0f),u8(a.motion.facing),1);
        owned(0x322,(float)((i8)u8(a.motion.facing) * 0x4b) + double(a.motion.x),double(a.motion.y) + double(130.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 6.0f;
        owned(0x321,(float)((i8)u8(a.motion.facing) * 0x4b) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        owned(0x322,(float)((i8)u8(a.motion.facing) * 0x4b) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        return;
      case 0x25a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          a.response = 1;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x32b,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x25b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          a.response = 1;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x32d,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1);
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
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x332,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(159.0f),u8(a.motion.facing),1);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x25d:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x333,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(159.0f),u8(a.motion.facing),1);
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 700:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 1;
          a.motion.clear();
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x73,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        if (((3 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.45f),
           double(a.motion.velocityY) < double(0.0f))) {
          a.motion.velocityY = 0.0f;
        }
        if (((10 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xe)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(0xe);
          return;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.shake(2.0f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 15.0f;
          a.motion.accelerationY = 0.8500000238418579f;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x326,double(a.motion.x),0.0f,u8(a.motion.facing),1);
          owned(0x33b,double(a.motion.x),0.0f,u8(a.motion.facing),1);
          consumeSpirit(500);
          events.sound(8);
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 7)) {
          selectFrame(5);
          state.impactX = i16(state.impactX) + 1;
        }
        if (((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 9)) {
          selectFrame(8);
          state.impactX = i16(state.impactX) + 1;
        }
        if (i16(a.animation.cursor.index) != 0xe) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xc);
        return;
      case 0x2bf:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 1;
          a.motion.clear();
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          hitFreeze(0x28);
          events.system_sound(0x17);
          events.sound(9);
          shared(0x73,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x14),double(a.motion.y) + double(180.0f),u8(a.motion.facing),1);
          ownedVisual(0x334,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 0xc) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x41) + double(a.motion.x),double(a.motion.y) + double(140.0f),u8(a.motion.facing),1);
        owned(0x33c,(float)((i8)u8(a.motion.facing) * 0x41) + double(a.motion.x),double(a.motion.y) + double(140.0f),u8(a.motion.facing),1);
        consumeSpirit(500);
        events.sound(10);
        return;
      case 0x2c0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 99;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
        }
        if (((i16(a.animation.cursor.index) < 0x19) && (double(0.0f) < double(a.motion.y))) &&
           (0xf < i16(a.animation.cursor.index))) {
          a.motion.velocityX = double(a.motion.velocityX) - double(0.8f);
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if (double(a.motion.velocityX) < double(3.0f)) {
            a.motion.velocityX = 3.0f;
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            selectFrame(0x19);
            a.motion.y = 0.0f;
            a.motion.clear();
            return;
          }
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x73,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x2e),double(a.motion.y) + double(75.0f),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x3c,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(0xb);
          a.motion.velocityY = 9.0f;
          a.motion.velocityX = 20.0f;
          consumeSpirit(500);
          a.motion.accelerationY = 0.800000011920929f;
        }
        if ((6 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xf)) {
          owned(0x329,(float)((i8)u8(a.motion.facing) * 0x3e) + double(a.motion.x),double(a.motion.y) + double(83.0f),u8(a.motion.facing),1);
        }
        if (((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 3)) {
          selectFrame(0xb);
          state.impactX = i16(state.impactX) + 1;
          return;
        }
        if (i16(a.animation.cursor.index) != 0x19) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x17);
        return;
      case 0x2c1:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 1;
          a.motion.clear();
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x74,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
        }
        if (((3 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.35f),
           double(a.motion.velocityY) < double(0.0f))) {
          a.motion.velocityY = 0.0f;
        }
        if (((10 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xe)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(0xe);
          return;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.shake(5.0f);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 15.0f;
          a.motion.accelerationY = 0.8500000238418579f;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x32a,(float)((i8)u8(a.motion.facing) * 0x28) + double(a.motion.x),10.0f,u8(a.motion.facing),1);
          owned(0x340,double(a.motion.x),0.0f,u8(a.motion.facing),1);
          consumeSpirit(500);
          events.sound(0xc);
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 0xc)) {
          selectFrame(5);
          state.impactX = i16(state.impactX) + 1;
        }
        if (((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 0xe)) {
          selectFrame(8);
          state.impactX = i16(state.impactX) + 1;
        }
        if (i16(a.animation.cursor.index) != 0xe) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xc);
        return;
      case 0x2c4:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 1;
          a.motion.clear();
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          hitFreeze(0x28);
          events.system_sound(0x17);
          events.sound(0xd);
          shared(0x74,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x14),double(a.motion.y) + double(180.0f),u8(a.motion.facing),1);
          ownedVisual(0x334,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 0xc) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        consumeSpirit(0x2ee);
        owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x41) + double(a.motion.x),double(a.motion.y) + double(140.0f),u8(a.motion.facing),1);
        owned(0x33d,(float)((i8)u8(a.motion.facing) * 0x41) + double(a.motion.x),double(a.motion.y) + double(140.0f),u8(a.motion.facing),1);
        events.sound(0xe);
        return;
      case 0x2c5:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 99;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
        }
        if (((i16(a.animation.cursor.index) < 0x19) && (double(0.0f) < double(a.motion.y))) &&
           (0xf < i16(a.animation.cursor.index))) {
          a.motion.velocityX = double(a.motion.velocityX) - double(0.8f);
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if (double(a.motion.velocityX) < double(3.0f)) {
            a.motion.velocityX = 3.0f;
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            selectFrame(0x19);
            a.motion.y = 0.0f;
            a.motion.clear();
            return;
          }
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x74,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x2e),double(a.motion.y) + double(75.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x3c,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          owned(0x344,(float)((i8)u8(a.motion.facing) * 0x37) + double(a.motion.x),double(a.motion.y) + double(86.0f),u8(a.motion.facing),1);
          consumeSpirit(500);
          events.sound(0xf);
          a.motion.velocityY = 10.5f;
          a.motion.velocityX = 25.0f;
          a.motion.accelerationY = 0.800000011920929f;
        }
        if ((6 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xf)) {
          owned(0x32e,(float)((i8)u8(a.motion.facing) * 0x3e) + double(a.motion.x),double(a.motion.y) + double(83.0f),u8(a.motion.facing),1);
        }
        if (((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 5)) {
          selectFrame(0xb);
          state.impactX = i16(state.impactX) + 1;
          return;
        }
        if (i16(a.animation.cursor.index) != 0x19) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x17);
        return;
      case 0x2d0:
        trail(3,4,0xff0000);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          state.impactX = 0;
        }
        if (((double(0.0f) < double(a.motion.y)) && (i16(a.animation.cursor.index) < 7)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(6.0f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(7);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          return;
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        iVar5 = random.range(0,0xf);
        a.motion.velocityX = (float)iVar5 + double(25.0f);
        a.motion.velocityY = 40.0f;
        a.motion.accelerationY = 6.0f;
        shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        shared(0x7e,double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        events.system_sound(0x1f);
        return;
      case 0x2d1:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          state.impactX = 0;
        }
        if (((double(0.0f) < double(a.motion.y)) && (i16(a.animation.cursor.index) < 6)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(6.0f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(6);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          return;
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        iVar5 = random.range(0,0xf);
        a.motion.velocityX = -double(25.0f) - (float)iVar5;
        a.motion.velocityY = 40.0f;
        a.motion.accelerationY = 6.0f;
        shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        shared(0x7e,double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        events.system_sound(0x1f);
        return;
      case 0x2db:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
        }
        if (((i16(a.animation.cursor.index) < 10) && (double(0.0f) < double(a.motion.y))) &&
           (2 < i16(a.animation.cursor.index))) {
          a.motion.velocityX = -30.0f;
          a.motion.velocityY = double(a.motion.velocityY) - double(0.6f);
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.motion.clear();
            a.select_action(10);
            a.motion.y = 0.0f;
            return;
          }
        }
        if (((9 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xd)) &&
           (cVar2 = a.motion.horizontal_limit(other.motion), cVar2 == '\0')) {
          if (u8(a.motion.facing) == (u8)0x1) {
            a.motion.x = double(a.combat.screenBounds[0]) + double(5.0f);
          }
          if (u8(a.motion.facing) == (u8)0xff) {
            a.motion.x = double(a.combat.screenBounds[1]) - double(5.0f);
          }
        }
        if ((((7 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 10)) &&
            (double(0.0f) < double(a.motion.y))) &&
           (((cVar2 = a.motion.horizontal_limit(other.motion), cVar2 != '\0' &&
             (((double(a.motion.x) < double(640.0f) && (u8(a.motion.facing) == (u8)0x1)) ||
              ((double(640.0f) < double(a.motion.x) && (u8(a.motion.facing) == (u8)0xff)))))) ||
            (((double(a.motion.x) <= double(a.combat.screenBounds[0]) + double(5.0f) &&
              (u8(a.motion.facing) == (u8)0x1)) ||
             ((double(a.combat.screenBounds[1]) - double(5.0f) < double(a.motion.x) !=
               (double(a.combat.screenBounds[1]) - double(5.0f) == double(a.motion.x)) &&
              (u8(a.motion.facing) == (u8)0xff)))))))) {
          selectFrame(10);
          a.motion.clear();
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 != i16(a.animation.cursor.frameCount)) {
          if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
            position = (double)std::abs(float(double(a.motion.x) -
                                          double(other.motion.x)));
            if (position < (double)double(320.0f)) {
              iVar5 = random.range(0,100);
              if (iVar5 < 0x32) {
                a.motion.facing = (u8)0x1;
              }
              else {
                a.motion.facing = (u8)0xff;
              }
            }
            events.sound(5);
            shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
            shared(0x7e,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
            a.motion.velocityX = -20.0f;
            a.motion.velocityY = 20.0f;
            a.motion.accelerationY = 0.800000011920929f;
          }
          if (i16(a.animation.cursor.index) != 10) {
            return;
          }
          if (i16(a.animation.cursor.elapsed) != 0) {
            return;
          }
          selectFrame(8);
          return;
        }
        if ((int)i16(state.impactX) < 3 - state.difficulty) {
          state.impactX = i16(state.impactX) + 1;
          selectFrame(0xc);
          return;
        }
        state.impactX = 0;
        if (state.difficulty == '\x03') {
          a.select_action(0x2df);
          return;
        }
        if (double(240.0f) < double(a.motion.y)) {
          a.select_action(0x2dc);
          return;
        }
        a.select_action(0x2dc);
        return;
      case 0x2dc:
        trail(3,4,0xff0000);
        if (((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          state.impactX = 1;
          events.sound(0xb);
          a.motion.velocityY = -10.0f;
          a.motion.velocityX = 30.0f;
          a.motion.accelerationY = 0.800000011920929f;
        }
        if (((i16(a.animation.cursor.index) < 4) && (double(0.0f) < double(a.motion.y))) &&
           (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(4);
          a.motion.y = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          return;
        }
        if ((i16(a.animation.cursor.index) < 4) &&
           (owned(0x333,(float)((i8)u8(a.motion.facing) * 0x3e) + double(a.motion.x),double(a.motion.y) + double(83.0f),u8(a.motion.facing),1), '\x01' < state.difficulty))
        {
          owned(0x387,(float)((i8)u8(a.motion.facing) * 0x3e) + double(a.motion.x),double(a.motion.y) + double(83.0f),u8(a.motion.facing),1);
        }
        if (((3 < i16(a.animation.cursor.index)) && (double(0.0f) < double(a.motion.velocityX))) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(1.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(100,0xb4);
          a.commandTimers[2] = sVar4 + state.difficulty * -0x1e;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0);
        return;
      case 0x2dd:
        trail(3,4,0xff0000);
        if (((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          events.sound(0xb);
          a.motion.velocityY = 10.0f;
          a.motion.velocityX = 30.0f;
          a.motion.accelerationY = 0.44999998807907104f;
          state.impactX = 1;
        }
        if (((i16(a.animation.cursor.index) < 9) && (double(0.0f) < double(a.motion.y))) &&
           (3 < i16(a.animation.cursor.index))) {
          a.motion.velocityX = double(a.motion.velocityX) - double(1.5f);
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if (double(a.motion.velocityX) < double(2.0f)) {
            a.motion.velocityX = 2.0f;
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            selectFrame(9);
            a.motion.y = 0.0f;
            a.motion.clear();
            return;
          }
        }
        if (i16(a.animation.cursor.index) < 4) {
          owned(0x329,(float)((i8)u8(a.motion.facing) * 0x3e) + double(a.motion.x),double(a.motion.y) + double(83.0f),u8(a.motion.facing),1);
          owned(0x387,(float)((i8)u8(a.motion.facing) * 0x3e) + double(a.motion.x),double(a.motion.y) + double(83.0f),u8(a.motion.facing),1);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(100,0xb4);
          a.commandTimers[2] = sVar4 + state.difficulty * -0x1e;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if (((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 4)) {
          selectFrame(0);
          state.impactX = i16(state.impactX) + 1;
          return;
        }
        if (i16(a.animation.cursor.index) != 9) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(7);
        return;
      case 0x2de:
        trail(3,4,0xff0000);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 99;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
        }
        if (((i16(a.animation.cursor.index) < 0x19) && (double(0.0f) < double(a.motion.y))) &&
           (0xe < i16(a.animation.cursor.index))) {
          a.motion.velocityX = double(a.motion.velocityX) - double(0.8f);
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if (double(a.motion.velocityX) < double(3.0f)) {
            a.motion.velocityX = 3.0f;
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            selectFrame(0x19);
            a.motion.y = 0.0f;
            a.motion.clear();
            return;
          }
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(100,0xb4);
          a.commandTimers[2] = sVar4 + state.difficulty * -0x1e;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x3c,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(0xb);
          a.motion.velocityY = 9.0f;
          a.motion.velocityX = 20.0f;
          a.motion.accelerationY = 0.800000011920929f;
        }
        if ((6 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x10)) {
          owned(0x329,(float)((i8)u8(a.motion.facing) * 0x3e) + double(a.motion.x),double(a.motion.y) + double(83.0f),u8(a.motion.facing),1);
        }
        if (((i16(a.animation.cursor.index) == 0x10) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 3)) {
          selectFrame(0xc);
          state.impactX = i16(state.impactX) + 1;
          return;
        }
        if (i16(a.animation.cursor.index) != 0x19) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x17);
        return;
      case 0x2df:
        trail(3,4,0xff0000);
        if (((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          state.impactX = 1;
          events.sound(0xb);
          a.motion.velocityY = -10.0f;
          a.motion.velocityX = 30.0f;
          a.motion.accelerationY = 0.800000011920929f;
        }
        if (((i16(a.animation.cursor.index) < 4) && (double(0.0f) < double(a.motion.y))) &&
           (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(4);
          a.motion.y = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          return;
        }
        if ((i16(a.animation.cursor.index) < 4) &&
           (owned(0x333,(float)((i8)u8(a.motion.facing) * 0x3e) + double(a.motion.x),double(a.motion.y) + double(83.0f),u8(a.motion.facing),1), '\x01' < state.difficulty))
        {
          owned(0x387,(float)((i8)u8(a.motion.facing) * 0x3e) + double(a.motion.x),double(a.motion.y) + double(83.0f),u8(a.motion.facing),1);
        }
        if (((3 < i16(a.animation.cursor.index)) && (double(0.0f) < double(a.motion.velocityX))) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(1.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(100,0xb4);
          a.commandTimers[2] = sVar4 + state.difficulty * -0x1e;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0);
        return;
      case 0x2e5:
        trail(3,4,0xff0000);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.response = 1;
          a.motion.clear();
          ownedVisual(0x38e,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(0x3c,0x50);
          a.commandTimers[2] = sVar4 + state.difficulty * -0x14;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(9);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (double(other.motion.y) == double(0.0f)) {
            state.impactX = 0;
          }
          else {
            state.impactX = 0x14;
          }
          iVar5 = random.range(0,100);
          if (iVar5 < 0x14) {
            iVar5 = random.range(0,100);
            if (iVar5 < 0x32) {
              state.impactX = 0;
            }
            else {
              state.impactX = 0x14;
            }
          }
          a.launchParameters[0] = (float)(int)i16(state.impactX);
          owned(0x386,(float)((i8)u8(a.motion.facing) * 0x41) + double(a.motion.x),double(a.motion.y) + double(140.0f),u8(a.motion.facing),1);
        }
        if (((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) &&
           ((int)i16(state.impactY) < (3 - state.difficulty) * 2)) {
          state.impactY = i16(state.impactY) + 1;
          selectFrame(9);
        }
        if (i16(a.animation.cursor.index) != 0xc) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.launchParameters[0] = (float)(int)i16(state.impactX);
        if (i16(state.impactX) == 0) {
          owned(900,(float)((i8)u8(a.motion.facing) * 0x41) + double(a.motion.x),double(a.motion.y) + double(140.0f),u8(a.motion.facing),1);
        }
        if (i16(state.impactX) == 0x14) {
          owned(0x388,(float)((i8)u8(a.motion.facing) * 0x41) + double(a.motion.x),double(a.motion.y) + double(140.0f),u8(a.motion.facing),1);
        }
        events.sound(10);
        return;
      case 0x2ef:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          state.aimAngle = 0;
          a.response = 1;
          a.motion.clear();
        }
        if (((3 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.95f),
           double(a.motion.velocityY) < double(0.0f))) {
          a.motion.velocityY = 0.0f;
        }
        if ((4 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 8)) {
          state.impactY = i16(state.impactY) + 1;
          state.aimAngle = i16(state.aimAngle) + 1;
          if (state.difficulty * -0x14 + 100 < (int)i16(state.impactY)) {
            state.impactY = 0;
            events.sound(0x10);
            owned(0x389,double(a.motion.x),double(a.motion.y) + double(250.0f),u8(a.motion.facing),1);
            owned(0x38a,double(a.motion.x),double(a.motion.y) + double(250.0f),u8(a.motion.facing),1);
          }
        }
        if (((10 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xe)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(0xe);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(0x3c,100);
          a.commandTimers[2] = sVar4 + state.difficulty * -0x14;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 16.5f;
          a.motion.accelerationY = 0.15000000596046448f;
          events.sound(0x15);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x38b,double(a.motion.x),double(a.motion.y) + double(250.0f),u8(a.motion.facing),1);
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 0x19)) {
          selectFrame(5);
          state.impactX = i16(state.impactX) + 1;
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (0x18 < i16(state.impactX)) {
          return;
        }
        selectFrame(8);
        state.impactX = i16(state.impactX) + 1;
        return;
      case 0x2f0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          state.aimAngle = 0;
          a.response = 1;
          a.motion.clear();
        }
        if (((3 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.95f),
           double(a.motion.velocityY) < double(0.0f))) {
          a.motion.velocityY = 0.0f;
        }
        if (((10 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xe)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(0xe);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(0x3c,100);
          a.commandTimers[2] = sVar4 + state.difficulty * -0x14;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 13.5f;
          a.motion.accelerationY = 0.15000000596046448f;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x38c,double(a.motion.x),0.0f,u8(a.motion.facing),1);
          owned(0x38f,double(a.motion.x),0.0f,u8(a.motion.facing),1);
          events.sound(0x10);
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 6)) {
          selectFrame(5);
          state.impactX = i16(state.impactX) + 1;
          if ((int)i16(state.impactX) == 6 - state.difficulty) {
            owned(0x38d,double(a.motion.x) + double(100.0f),30.0f,u8(a.motion.facing),0);
            owned(0x38d,double(a.motion.x) - double(100.0f),30.0f,u8(a.motion.facing),0);
            owned(0x38d,double(a.motion.x) + double(180.0f),0.0f,u8(a.motion.facing),1);
            owned(0x38d,double(a.motion.x) - double(180.0f),0.0f,u8(a.motion.facing),1);
            owned(0x38d,double(a.motion.x) + double(70.0f),-30.0f,u8(a.motion.facing),1);
            owned(0x38d,double(a.motion.x) - double(70.0f),-30.0f,u8(a.motion.facing),1);
            if ('\x01' < state.difficulty) {
              owned(0x38d,double(a.motion.x) + double(220.0f),50.0f,u8(a.motion.facing),0);
              owned(0x38d,double(a.motion.x) - double(220.0f),50.0f,u8(a.motion.facing),0);
              owned(0x38d,double(a.motion.x) + double(280.0f),0.0f,u8(a.motion.facing),1);
              owned(0x38d,double(a.motion.x) - double(280.0f),0.0f,u8(a.motion.facing),1);
              owned(0x38d,double(a.motion.x) + double(120.0f),-50.0f,u8(a.motion.facing),1);
              owned(0x38d,double(a.motion.x) - double(120.0f),-50.0f,u8(a.motion.facing),1);
            }
            events.shake(20.0f);
            events.sound(0x14);
          }
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (4 < i16(state.impactX)) {
          return;
        }
        selectFrame(8);
        state.impactX = i16(state.impactX) + 1;
        return;
      case 0x2f1:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          state.aimAngle = 0;
          a.response = 1;
          a.motion.clear();
        }
        if (((3 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.95f),
           double(a.motion.velocityY) < double(0.0f))) {
          a.motion.velocityY = 0.0f;
        }
        if ((((4 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 8)) &&
            (i16(state.impactX) < 3)) &&
           (state.impactY = i16(state.impactY) + 1,
           4 < i16(state.impactY))) {
          shared(0x40,double(a.motion.x),double(a.motion.y) + double(250.0f),1,1);
          state.impactY = 0;
        }
        if (((4 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 8)) &&
           ((3 < i16(state.impactX) &&
            ((state.impactY = i16(state.impactY) + 1,
             2 < i16(state.impactY) &&
             ((int)i16(state.aimAngle) < state.difficulty * 5 + 0xf)))))) {
          state.impactY = 0;
          state.aimAngle = i16(state.aimAngle) + 1;
          iVar5 = random.range(0,100);
          if (iVar5 < 0x33) {
            owned(0x390,double(a.motion.x),double(a.motion.y) + double(250.0f),0xffffffff,1);
          }
          else {
            owned(0x390,double(a.motion.x),double(a.motion.y) + double(250.0f),1,1);
          }
        }
        if (((10 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xe)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(0xe);
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(0x3c,0x78);
          a.commandTimers[2] = sVar4 + state.difficulty * -0x14;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 16.5f;
          a.motion.accelerationY = 0.15000000596046448f;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x38b,double(a.motion.x),double(a.motion.y) + double(250.0f),u8(a.motion.facing),1);
          events.sound(0x15);
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 10)) {
          selectFrame(5);
          state.impactX = i16(state.impactX) + 1;
          if (i16(state.impactX) == 4) {
            events.sound(0x13);
          }
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (4 < i16(state.impactX)) {
          return;
        }
        selectFrame(8);
        state.impactX = i16(state.impactX) + 1;
        return;
      case 0x2f8:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          state.aimAngle = 0;
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 2) &&
           (state.impactY = i16(state.impactY) + 1,
           4 < i16(state.impactY))) {
          shared(0x40,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 3),double(a.motion.y) + double(104.0f),1,1);
          state.impactY = 0;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0x12);
          iVar5 = random.range(0,0x28);
          a.launchParameters[0] = (float)(iVar5 + -0x14);
          owned(0x391,(float)((i8)u8(a.motion.facing) * 0x24) + double(a.motion.x),double(a.motion.y) + double(101.0f),u8(a.motion.facing),1);
          for (local_28 = 0; local_28 < state.difficulty * 2 + 2; local_28 = local_28 + 1) {
            owned(0x392,(float)((i8)u8(a.motion.facing) * 0x24) + double(a.motion.x),double(a.motion.y) + double(101.0f),u8(a.motion.facing),1);
          }
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(0x3c,100);
          a.commandTimers[2] = sVar4 + state.difficulty * -0x14;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0x15);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          if ((int)i16(state.impactX) < 3 - state.difficulty) {
            state.impactX = i16(state.impactX) + 1;
            selectFrame(2);
            return;
          }
          state.impactX = 0;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          if ((int)i16(state.impactX) < 3 - state.difficulty) {
            state.impactX = i16(state.impactX) + 1;
            selectFrame(3);
            return;
          }
          state.impactX = 0;
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           ((int)i16(state.impactX) < state.difficulty + 6)) {
          selectFrame(6);
          state.impactX = i16(state.impactX) + 1;
        }
        if (i16(a.animation.cursor.index) != 10) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (10 - state.difficulty <= (int)i16(state.impactX)) {
          return;
        }
        selectFrame(8);
        state.impactX = i16(state.impactX) + 1;
        return;
      case 0x2f9:
        trail(3,4,0xff0000);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          state.aimAngle = 0;
          a.response = 1;
          a.motion.clear();
        }
        if ((1 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x14)) {
          if ((0xf < i16(a.animation.cursor.index)) || (i16(a.animation.cursor.index) < 10)) {
            a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            selectFrame(0x14);
            a.motion.y = 0.0f;
            a.motion.clear();
            return;
          }
        }
        if ((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0x12);
          a.launchParameters[0] =
               (float)((int)i16(state.impactY) + i16(state.impactX) * 10);
          owned(0x391,(float)((i8)u8(a.motion.facing) * 0x1e) + double(a.motion.x),double(a.motion.y) + double(87.0f),u8(a.motion.facing),1);
          for (local_2c = 0; local_2c < state.difficulty * 2 + 4; local_2c = local_2c + 1) {
            owned(0x392,(float)((i8)u8(a.motion.facing) * 0x1e) + double(a.motion.x),double(a.motion.y) + double(87.0f),u8(a.motion.facing),1);
          }
        }
        if ((i16(a.animation.cursor.index) == 0xe) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0x12);
          a.launchParameters[0] =
               (float)((int)i16(state.impactY) + i16(state.impactX) * -10);
          owned(0x391,(float)((i8)u8(a.motion.facing) * 0x1e) + double(a.motion.x),double(a.motion.y) + double(87.0f),u8(a.motion.facing),1);
          for (local_30 = 0; local_30 < state.difficulty * 2 + 4; local_30 = local_30 + 1) {
            owned(0x392,(float)((i8)u8(a.motion.facing) * 0x1e) + double(a.motion.x),double(a.motion.y) + double(87.0f),u8(a.motion.facing),1);
          }
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          sVar4 = random.range(0x3c,100);
          a.commandTimers[2] = sVar4 + state.difficulty * -0x14;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 12.5f;
          a.motion.velocityX = -8.0f;
          a.motion.accelerationY = 0.4000000059604645f;
        }
        if ((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.2f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.2f);
        }
        if ((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) {
          (aimRadians = atan(double(other.motion.y) - double(a.motion.y),(double(other.motion.x) - double(a.motion.x)) *
                       (float)(int)(i8)u8(a.motion.facing)));
          uVar3 = i16(i32(aimRadians/double(0.017453289f)));
          state.impactY = uVar3;
        }
        if ((i16(a.animation.cursor.index) == 0xd) && (i16(a.animation.cursor.elapsed) == 0)) {
          if ((int)i16(state.impactX) < state.difficulty * 2 + 1) {
            selectFrame(0xb);
            state.impactX = i16(state.impactX) + 1;
          }
          else {
            state.impactX = 0;
          }
        }
        if ((i16(a.animation.cursor.index) == 0xe) && (i16(a.animation.cursor.elapsed) == 0)) {
          (aimRadians = atan(double(other.motion.y) - double(a.motion.y),(double(other.motion.x) - double(a.motion.x)) *
                       (float)(int)(i8)u8(a.motion.facing)));
          uVar3 = i16(i32(aimRadians/double(0.017453289f)));
          state.impactY = uVar3;
        }
        if (((i16(a.animation.cursor.index) == 0x10) && (i16(a.animation.cursor.elapsed) == 0)) &&
           ((int)i16(state.impactX) < state.difficulty * 2 + 1)) {
          selectFrame(0xe);
          state.impactX = i16(state.impactX) + 1;
        }
        if (i16(a.animation.cursor.index) != 0x14) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x12);
        return;
      case 0x2fa:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
        }
        if (((i16(a.animation.cursor.index) < 10) && (double(0.0f) < double(a.motion.y))) &&
           (2 < i16(a.animation.cursor.index))) {
          a.motion.velocityX = -30.0f;
          a.motion.velocityY = double(a.motion.velocityY) - double(0.6f);
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.motion.clear();
            a.select_action(10);
            a.motion.y = 0.0f;
            return;
          }
        }
        if (((9 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xd)) &&
           (cVar2 = a.motion.horizontal_limit(other.motion), cVar2 == '\0')) {
          if (u8(a.motion.facing) == (u8)0x1) {
            a.motion.x = double(a.combat.screenBounds[0]) + double(5.0f);
          }
          if (u8(a.motion.facing) == (u8)0xff) {
            a.motion.x = double(a.combat.screenBounds[1]) - double(5.0f);
          }
        }
        if ((((7 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 10)) &&
            (double(0.0f) < double(a.motion.y))) &&
           (((cVar2 = a.motion.horizontal_limit(other.motion), cVar2 != '\0' &&
             (((double(a.motion.x) < double(640.0f) && (u8(a.motion.facing) == (u8)0x1)) ||
              ((double(640.0f) < double(a.motion.x) && (u8(a.motion.facing) == (u8)0xff)))))) ||
            (((double(a.motion.x) <= double(a.combat.screenBounds[0]) + double(5.0f) &&
              (u8(a.motion.facing) == (u8)0x1)) ||
             ((double(a.combat.screenBounds[1]) - double(5.0f) < double(a.motion.x) !=
               (double(a.combat.screenBounds[1]) - double(5.0f) == double(a.motion.x)) &&
              (u8(a.motion.facing) == (u8)0xff)))))))) {
          selectFrame(10);
          a.motion.clear();
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          if ((int)i16(state.impactX) < 3 - state.difficulty) {
            state.impactX = i16(state.impactX) + 1;
            selectFrame(0xc);
            return;
          }
          a.select_action(0x2fb);
          return;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          position = (double)std::abs(float(double(a.motion.x) -
                                        double(other.motion.x)));
          if (position < (double)double(320.0f)) {
            iVar5 = random.range(0,100);
            if (iVar5 < 0x32) {
              a.motion.facing = (u8)0x1;
            }
            else {
              a.motion.facing = (u8)0xff;
            }
          }
          events.sound(5);
          shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          a.motion.velocityX = -20.0f;
          a.motion.velocityY = 17.5f;
          iVar5 = random.range(0,100);
          if (iVar5 < 0x32) {
            a.motion.velocityY = 15.0f;
          }
          a.motion.accelerationY = 0.800000011920929f;
        }
        if (i16(a.animation.cursor.index) != 10) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(8);
        return;
      case 0x2fb:
        trail(3,4,0xff0000);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          state.aimAngle = 0;
          a.motion.velocityX = 15.0f;
          a.motion.velocityY = 10.0f;
          a.motion.accelerationY = 0.30000001192092896f;
        }
        if ((i16(a.animation.cursor.index) < 0x12) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(0x12);
          a.motion.y = 0.0f;
          a.motion.clear();
          return;
        }
        if ((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0x12);
          position = (double)(aimRadians = atan(double(other.motion.y) -
                                        double(a.motion.y),(double(other.motion.x) -
                                        double(a.motion.x)) *
                                        (float)(int)(i8)u8(a.motion.facing)));
          a.launchParameters[0] = (float)(position / (double)double(0.017453289f));
          owned(0x391,(float)((i8)u8(a.motion.facing) * 0x1e) + double(a.motion.x),double(a.motion.y) + double(87.0f),u8(a.motion.facing),1);
          for (local_34 = 0; local_34 < state.difficulty * 2 + 2; local_34 = local_34 + 1) {
            owned(0x392,(float)((i8)u8(a.motion.facing) * 0x24) + double(a.motion.x),double(a.motion.y) + double(101.0f),u8(a.motion.facing),1);
          }
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (((i16(a.animation.cursor.index) == 0xd) && (i16(a.animation.cursor.elapsed) == 0)) &&
           ((int)i16(state.impactX) < state.difficulty + 3)) {
          selectFrame(0xb);
          state.impactX = i16(state.impactX) + 1;
        }
        if (i16(a.animation.cursor.index) != 0x12) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x10);
        return;
      case 0x302:
        trail(3,4,0xff0000);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          state.aimAngle = 0;
          a.response = 1;
          a.motion.clear();
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(0xb);
          a.motion.y = 0.0f;
          a.motion.clear();
          return;
        }
        sVar4 = a.animation.advance();
        if (sVar4 == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 13.5f;
          iVar5 = random.range(0,8);
          a.motion.velocityX = (float)(iVar5 + -4);
          a.motion.accelerationY = 0.25f;
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0x12);
          a.commandTimers[2] = 0x78;
          uVar3 = random.range(0,0x167);
          state.aimAngle = uVar3;
          if (state.difficulty == '\0') {
            for (local_38 = 0; local_38 < 6; local_38 = local_38 + 1) {
              a.launchParameters[0] =
                   (float)(local_38 * 0x3c + (int)i16(state.aimAngle));
              owned(0x393,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
            }
          }
          if (state.difficulty == '\x01') {
            for (local_3c = 0; local_3c < 6; local_3c = local_3c + 1) {
              a.launchParameters[0] =
                   (float)(local_3c * 0x3c + (int)i16(state.aimAngle));
              owned(0x393,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
            }
          }
          if (state.difficulty == '\x02') {
            for (local_40 = 0; local_40 < 8; local_40 = local_40 + 1) {
              a.launchParameters[0] =
                   (float)(local_40 * 0x2d + (int)i16(state.aimAngle));
              owned(0x393,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
            }
          }
          if (state.difficulty == '\x03') {
            for (local_44 = 0; local_44 < 8; local_44 = local_44 + 1) {
              a.launchParameters[0] =
                   (float)(local_44 * 0x2d + (int)i16(state.aimAngle));
              owned(0x393,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
            }
          }
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(9);
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
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (7 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(9);
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
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          fade(0);
          a.begin_spell(mode,replayMode,core);
          events.system_sound(0x17);
        }
        if (i16(a.animation.cursor.index) != 0x16) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (7 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(0x14);
        a.transitionReady = (u8)0x1;
        return;
      }
    }
switchD_0054326a_default:
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
    goto LAB_005515ba;
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
    goto LAB_005515ba;
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
    a.motion.velocityX = 6.5f;
    goto LAB_005515ba;
  case 5:
    a.motion.velocityX = -6.5f;
LAB_005515ba:
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
        if ((i16(a.animation.cursor.index) == 3) && (double(a.motion.velocityY) < double(4.0f))) {
          selectFrame(4);
        }
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.duration) <= i16(a.animation.cursor.elapsed)) {
          a.animation.cursor.elapsed = 0;
          a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
          if (i16(a.animation.cursor.index) == 3) {
            a.motion.velocityX = 0.0f;
            a.motion.velocityY = 19.0f;
            a.motion.accelerationY = 0.949999988079071f;
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
          if (i16(a.animation.cursor.index) == 3) {
            a.motion.velocityX = 7.0f;
            a.motion.velocityY = 18.0f;
            a.motion.accelerationY = 0.949999988079071f;
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
      if ((i16(a.animation.cursor.index) == 3) && (double(a.motion.velocityY) < double(4.0f))) {
        selectFrame(4);
      }
      if ((double(a.motion.y) <= double(0.0f)) ||
         (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
         double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) ==
         (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.duration) <= i16(a.animation.cursor.elapsed)) {
          a.animation.cursor.elapsed = 0;
          a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
          if (i16(a.animation.cursor.index) == 3) {
            a.motion.velocityX = -7.0f;
            a.carryMomentum = (u8)0x0;
            a.motion.velocityY = 18.0f;
            a.motion.accelerationY = 0.949999988079071f;
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
        selectFrame(1);
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
        a.select_action(3);
      }
      else {
        a.select_action(2);
      }
    }
    break;
  default:
    goto switchD_0054326a_default;
  }
  return;

}
}
