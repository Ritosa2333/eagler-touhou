#include "SakuyaState.hpp"
#include <cmath>
namespace th075 {
void advance_sakuya_action(FighterActor& a,FighterActor& other,SakuyaState& state,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){

    double aimRadians=0;const auto atan=[](float y,float x){return std::atan2(double(y),double(x));};
    const auto selectFrame=[&](i16 frame){a.animation.set(a.animation.cursor.action,frame,0);};
    const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters);};
    const auto shared=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Shared,id,x,y,facing,layer,a.launchParameters);};
    const auto ownedVisual=[&](i16 id,float x,float y,i8 facing,u8 layer,u8 visual,u8 order){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters,order,visual?&a.effectVisual:nullptr);};
    const auto consumeSpirit=[&](i16 amount){if(a.combat.spiritBreak<1){a.combat.spirit=i16(u16(a.combat.spirit)-u16(amount));if(a.combat.spirit<1&&a.combat.spiritBreak<1){events.system_sound(43);a.combat.spirit=0;a.combat.spiritBreak=1000;}}else a.combat.spiritBreak=1000;};
    const auto trail=[&](i32 frames,i8 interval,u32 mask){a.combat.noticeTimer=frames;a.trails.interval=interval;a.trails.colorMask=mask;};
    const auto hitFreeze=[&](i16 frames){if(frames>=0)a.motion.hitFreeze=frames;};
    const auto fade=[&](u8 kind){a.combat.fade=240;a.fadeKind=kind;};

  i8 boundary;
  u16 impactWord;
  i16 advancedFrame;
  int step;
  u32 direction;
  float effectY;
  float position;
  u32 layer;
  u32 uVar9;
  u32 uVar10;
  u32 uVar11;
  int local_64;
  int local_60;
  int local_5c;
  int local_58;
  int local_54;
  int local_50;
  int local_4c;
  int local_48;
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
  step = (int)i16(a.animation.cursor.action);
  if (0x32 < step) {
    if (step < 0x8d) {
      if (step == 0x8c) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
      switch(step) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(0x61);
          events.shake(2.0f);
          events.system_sound(0x16);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(5);
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
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(0x61);
          events.shake(2.0f);
          events.system_sound(0x16);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(5);
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
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(0x61);
          events.shake(2.0f);
          events.system_sound(0x16);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(5);
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
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(0x61);
          events.shake(2.0f);
          events.system_sound(0x16);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') &&
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
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(0x61);
          events.shake(2.0f);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(5);
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
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(0x61);
          a.pose.rotation[2] = 0.0f;
          events.shake(2.0f);
          (a.pose.active=false);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
            impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
            state.actionTicks = impactWord;
            impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
            state.actionStage = impactWord;
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          selectFrame(3);
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
        if (((i16(a.animation.cursor.index) < 0xc) && (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(0xc);
          return;
        }
        advancedFrame = a.animation.advance();
        if ((advancedFrame == i16(a.animation.cursor.frameCount)) &&
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
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 8.0f;
          a.motion.velocityX = 12.5f;
          a.motion.accelerationY = 0.699999988079071f;
        }
        if (i16(a.animation.cursor.index) != 0xc) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(10);
        return;
      case 0x5e:
        if (((i16(a.animation.cursor.index) < 9) && (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(9);
          return;
        }
        advancedFrame = a.animation.advance();
        if ((advancedFrame == i16(a.animation.cursor.frameCount)) &&
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
          a.motion.velocityX = -12.5f;
          a.motion.accelerationY = 0.699999988079071f;
        }
        if (i16(a.animation.cursor.index) != 9) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(8);
        return;
      case 0x5f:
        advancedFrame = a.animation.advance();
        if ((advancedFrame == i16(a.animation.cursor.frameCount)) &&
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
        if (((2 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 6)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
           (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(6);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          for (spark = 0; spark < 10; spark = spark + 1) {
            owned(0x3e4,double(a.motion.x),double(a.motion.y),1,1);
          }
          if (0x19 < i16(a.knockdownTime)) {
            a.knockdownTime = 0;
            owned(0x3e3,double(a.motion.x),double(a.motion.y),1,1);
          }
        }
        if (((2 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 6)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(6);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(2);
        return;
      }
    }
    else if (step < 0x12d) {
      if (step == 300) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1b);
        return;
      }
      switch(step) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
        if ((i16(a.animation.cursor.index) == 1) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(2);
          return;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          a.motion.clear();
          a.motion.y = 0.0f;
          a.select_action(0x61);
          events.shake(2.0f);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(5);
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        if ((double(11.0f) < double(a.motion.velocityX)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.75f),
           double(a.motion.velocityX) < double(11.0f))) {
          a.motion.velocityX = 11.0f;
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          if ((0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) &&
             (i16(state.actionTicks) < 2)) {
            selectFrame(4);
            state.actionTicks = i16(state.actionTicks) + 1;
            return;
          }
          a.select_action(0xcf);
          return;
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 14.5f;
        shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        shared(0x7e,double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        events.system_sound(0x1f);
        return;
      case 0xc9:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -10.0f;
        }
        if ((((double(0.0f) < double(a.motion.y)) && (3 < i16(a.animation.cursor.index))) &&
            (i16(a.animation.cursor.index) < 6)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
           (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f)))) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.y = 0.0f;
          a.animation.cursor.elapsed = 0;
          selectFrame(6);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -50.0f;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 4.0f;
          a.motion.accelerationY = 0.6000000238418579f;
          a.motion.velocityX = -30.0f;
          shared(0x7d,double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          events.system_sound(0x1f);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(5);
        return;
      case 0xca:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          state.actionTicks = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 11.0f;
          a.motion.velocityY = 5.0f;
          a.motion.accelerationY = 0.6000000238418579f;
          shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(70.0f),u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(70.0f),u8(a.motion.facing),1);
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
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = -11.0f;
        a.motion.velocityY = 5.0f;
        a.motion.accelerationY = 0.6000000238418579f;
        shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x44) + double(a.motion.x),double(a.motion.y) + double(70.0f),-(int)(i8)u8(a.motion.facing),1);
        shared(0x7e,(float)((i8)u8(a.motion.facing) * 0x44) + double(a.motion.x),double(a.motion.y) + double(70.0f),-(int)(i8)u8(a.motion.facing),1);
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
          shared(0x3c,double(a.motion.x),double(a.motion.y),1,1);
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 20.0f;
          a.motion.accelerationY = 0.699999988079071f;
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
          shared(0x3c,double(a.motion.x),double(a.motion.y),1,1);
          a.motion.velocityX = 10.0f;
          a.motion.velocityY = 15.0f;
          a.motion.accelerationY = 0.699999988079071f;
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
        if (i16(a.animation.cursor.index) == 3) {
          shared(0x3c,double(a.motion.x),double(a.motion.y),1,1);
          a.motion.velocityX = -10.0f;
          a.motion.velocityY = 15.0f;
          a.motion.accelerationY = 0.699999988079071f;
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
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
    else if (step < 0x187) {
      if (step == 0x186) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1c);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        ownedVisual(0x324,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
        return;
      }
      switch(step) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.actionTicks = 0;
        }
        if (i16(a.animation.cursor.index) != 4) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(2);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1d);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        ownedVisual(0x333,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(2);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1c);
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1c);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        ownedVisual(0x334,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 5) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.actionTicks = 0;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1c);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        ownedVisual(0x324,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
        return;
      case 0x134:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.remainingHits = 1;
          a.response = 0;
        }
        if (double(0.0f) < double(a.motion.velocityX)) {
          a.motion.velocityX = double(a.motion.velocityX) - double(0.5f);
        }
        if ((double(a.motion.velocityX) < double(2.0f)) && (double(0.0f) < double(a.motion.y))) {
          a.motion.velocityX = 2.0f;
        }
        if (((double(0.0f) < double(a.motion.y)) && (i16(a.animation.cursor.index) < 0xe)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
           (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f)))) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.y = 0.0f;
          selectFrame(0xe);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 12.5f;
          a.motion.velocityY = 8.5f;
          a.motion.accelerationY = 0.6499999761581421f;
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          ownedVisual(0x336,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1d);
        }
        if (i16(a.animation.cursor.index) != 0xe) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(i16(a.animation.cursor.index) + -2);
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
        if (((i16(a.animation.cursor.index) == 3) || (i16(a.animation.cursor.index) == 4)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(1.0f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(5);
          shared(0x80,(float)((i8)u8(a.motion.facing) * 0x96) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          return;
        }
        if ((4 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.4f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 15.0f;
          a.motion.velocityY = 4.5f;
          a.carryMomentum = (u8)0x0;
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 4.0f;
        return;
      case 0x136:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 2;
          a.response = 0;
        }
        if ((('\0' < (i8)u8(a.carryMomentum)) && (double(a.motion.y) == double(0.0f))) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        if (((double(0.0f) < double(a.motion.y)) &&
            (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
            double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) &&
           (i16(a.animation.cursor.index) < 0xc)) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(0xc);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 4.0f;
          a.motion.velocityY = 9.5f;
          a.motion.accelerationY = 1.0f;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          ownedVisual(0x337,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
        }
        if (i16(a.animation.cursor.index) != 9) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.response = 0;
        return;
      }
    }
    else if (step < 0x255) {
      if (step == 0x254) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.selectedSpell = (u8)((i8)u8(a.selectedSpell) + -1);
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 6) {
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
      switch(step) {
      case 0x187:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 2;
          a.response = 0;
          a.motion.velocityX = 10.0f;
        }
        if ((i16(a.animation.cursor.index) < 2) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(0xe);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 15.0f;
          a.motion.velocityY = 8.5f;
          a.motion.accelerationY = 0.6000000238418579f;
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1c);
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 0xe) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xc);
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
        if ((double(a.motion.y) == double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
           (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f)))) {
          a.motion.clear();
          a.motion.y = 0.0f;
          selectFrame(0xc);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1c);
        }
        a.motion.velocityX = 10.0f;
        a.motion.velocityY = 12.5f;
        a.motion.accelerationY = 1.0f;
        return;
      case 0x189:
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(2);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1c);
        return;
      case 400:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if (((5 < i16(a.animation.cursor.index)) && (i16(state.actionTicks) < 1)) &&
           (i16(state.actionStage) < 5)) {
          events.sound(0);
          consumeSpirit(0x28);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + -8);
          a.launchParameters[1] = 7.0f;
          owned(800,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + -4);
          a.launchParameters[1] = 7.0f;
          owned(800,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + -0xc);
          a.launchParameters[1] = 7.0f;
          owned(800,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          state.actionTicks = 2;
          state.actionStage = i16(state.actionStage) + 1;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x191:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if (((4 < i16(a.animation.cursor.index)) && (i16(state.actionTicks) < 1)) &&
           (i16(state.actionStage) < 5)) {
          events.sound(0);
          consumeSpirit(0x28);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + 0x1e);
          a.launchParameters[1] = 7.0f;
          owned(800,(float)((i8)u8(a.motion.facing) * 0x61) + double(a.motion.x),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + 0x22);
          a.launchParameters[1] = 7.0f;
          owned(800,(float)((i8)u8(a.motion.facing) * 0x61) + double(a.motion.x),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + 0x1a);
          a.launchParameters[1] = 7.0f;
          owned(800,(float)((i8)u8(a.motion.facing) * 0x61) + double(a.motion.x),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
          state.actionTicks = 2;
          state.actionStage = i16(state.actionStage) + 1;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x192:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if (((4 < i16(a.animation.cursor.index)) && (i16(state.actionTicks) < 1)) &&
           (i16(state.actionStage) < 5)) {
          events.sound(0);
          consumeSpirit(0x28);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + 0x2d);
          a.launchParameters[1] = 7.0f;
          owned(800,(float)((i16(state.actionStage) * -10 + 0x2f) *
                                  (int)(i8)u8(a.motion.facing)) + double(a.motion.x),double(a.motion.y) + double(60.0f) +
                       (float)(i16(state.actionStage) * 0x14),u8(a.motion.facing),1);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + 0x31);
          a.launchParameters[1] = 7.0f;
          owned(800,(float)((i16(state.actionStage) * -10 + 0x2f) *
                                  (int)(i8)u8(a.motion.facing)) + double(a.motion.x),double(a.motion.y) + double(60.0f) +
                       (float)(i16(state.actionStage) * 0x14),u8(a.motion.facing),1);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + 0x29);
          a.launchParameters[1] = 7.0f;
          owned(800,(float)((i16(state.actionStage) * -10 + 0x2f) *
                                  (int)(i8)u8(a.motion.facing)) + double(a.motion.x),double(a.motion.y) + double(60.0f) +
                       (float)(i16(state.actionStage) * 0x14),u8(a.motion.facing),1);
          state.actionTicks = 2;
          state.actionStage = i16(state.actionStage) + 1;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0x194:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if (((4 < i16(a.animation.cursor.index)) && (i16(state.actionTicks) < 1)) &&
           (i16(state.actionStage) < 5)) {
          events.sound(0);
          consumeSpirit(0x28);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + -8);
          a.launchParameters[1] = 7.0f;
          owned(800,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + -4);
          a.launchParameters[1] = 7.0f;
          owned(800,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + -0xc);
          a.launchParameters[1] = 7.0f;
          owned(800,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          state.actionTicks = 2;
          state.actionStage = i16(state.actionStage) + 1;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x335,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = -6.0f;
        a.motion.velocityY = 9.0f;
        a.motion.accelerationY = 0.699999988079071f;
        return;
      case 0x195:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if (((4 < i16(a.animation.cursor.index)) && (i16(state.actionTicks) < 1)) &&
           (i16(state.actionStage) < 5)) {
          events.sound(0);
          consumeSpirit(0x28);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + -0x2d);
          a.launchParameters[1] = 7.0f;
          owned(800,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + -0x29);
          a.launchParameters[1] = 7.0f;
          owned(800,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + -0x31);
          a.launchParameters[1] = 7.0f;
          owned(800,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          state.actionTicks = 2;
          state.actionStage = i16(state.actionStage) + 1;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x335,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = -6.0f;
        a.motion.velocityY = 9.0f;
        a.motion.accelerationY = 0.699999988079071f;
        return;
      case 0x196:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0);
        a.launchParameters[0] = 0.0f;
        a.launchParameters[1] = 20.0f;
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x27) + double(a.motion.x),double(a.motion.y) + double(146.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x2c) + double(a.motion.x),double(a.motion.y) + double(131.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x36) + double(a.motion.x),double(a.motion.y) + double(101.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x3b) + double(a.motion.x),double(a.motion.y) + double(86.0f),u8(a.motion.facing),1);
        consumeSpirit(0x96);
        return;
      case 0x197:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0);
        a.launchParameters[0] = 25.0f;
        a.launchParameters[1] = 20.0f;
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x55) + double(a.motion.x),double(a.motion.y) + double(173.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x5b) + double(a.motion.x),double(a.motion.y) + double(163.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x61) + double(a.motion.x),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x67) + double(a.motion.x),double(a.motion.y) + double(143.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x6d) + double(a.motion.x),double(a.motion.y) + double(133.0f),u8(a.motion.facing),1);
        consumeSpirit(0x96);
        return;
      case 0x198:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(2);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0);
        a.launchParameters[0] = 0.0f;
        a.launchParameters[1] = 20.0f;
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x5c) + double(a.motion.x),double(a.motion.y) + double(75.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x62) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x68) + double(a.motion.x),double(a.motion.y) + double(45.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x6e) + double(a.motion.x),double(a.motion.y) + double(30.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x74) + double(a.motion.x),double(a.motion.y) + double(15.0f),u8(a.motion.facing),1);
        consumeSpirit(0x96);
        return;
      case 0x199:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          a.response = 1;
        }
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          a.select_action(10);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0);
        a.launchParameters[0] = 0.0f;
        a.launchParameters[1] = 20.0f;
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(136.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x51) + double(a.motion.x),double(a.motion.y) + double(122.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x56) + double(a.motion.x),double(a.motion.y) + double(107.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x5b) + double(a.motion.x),double(a.motion.y) + double(92.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x60) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        consumeSpirit(0x96);
        return;
      case 0x19a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          a.response = 1;
        }
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          a.select_action(10);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0);
        a.launchParameters[0] = -20.0f;
        a.launchParameters[1] = 20.0f;
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x62) + double(a.motion.x),double(a.motion.y) + double(77.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x5c) + double(a.motion.x),double(a.motion.y) + double(67.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x56) + double(a.motion.x),double(a.motion.y) + double(57.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(47.0f),u8(a.motion.facing),1);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x4a) + double(a.motion.x),double(a.motion.y) + double(37.0f),u8(a.motion.facing),1);
        consumeSpirit(0x96);
        return;
      case 0x19b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if (((5 < i16(a.animation.cursor.index)) && (i16(state.actionTicks) < 1)) &&
           (i16(state.actionStage) < 5)) {
          events.sound(0);
          consumeSpirit(0x28);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + -0x1e);
          a.launchParameters[1] = 15.0f;
          owned(0x326,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          state.actionTicks = 2;
          state.actionStage = i16(state.actionStage) + 1;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x19c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if (((4 < i16(a.animation.cursor.index)) && (i16(state.actionTicks) < 1)) &&
           (i16(state.actionStage) < 5)) {
          events.sound(0);
          consumeSpirit(0x28);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + 0x1e);
          a.launchParameters[1] = 15.0f;
          owned(0x326,(float)((i8)u8(a.motion.facing) * 0x61) + double(a.motion.x),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
          state.actionTicks = 2;
          state.actionStage = i16(state.actionStage) + 1;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x19d:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if (((5 < i16(a.animation.cursor.index)) && (i16(state.actionTicks) < 1)) &&
           (i16(state.actionStage) < 5)) {
          events.sound(0);
          consumeSpirit(0x28);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 4 + 0x37);
          a.launchParameters[1] = 15.0f;
          owned(0x326,(float)((i16(state.actionStage) * -10 + 0x2f) *
                                    (int)(i8)u8(a.motion.facing)) + double(a.motion.x),double(a.motion.y) + double(60.0f) +
                       (float)(i16(state.actionStage) * 0x14),u8(a.motion.facing),1);
          state.actionTicks = 2;
          state.actionStage = i16(state.actionStage) + 1;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0x19e:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.actionTicks = 0;
          state.actionStage = 0;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
        }
        if (((4 < i16(a.animation.cursor.index)) && (i16(state.actionTicks) < 1)) &&
           (i16(state.actionStage) < 5)) {
          events.sound(0);
          consumeSpirit(0x28);
          a.launchParameters[0] = (float)(i16(state.actionStage) * -4);
          a.launchParameters[1] = 15.0f;
          owned(0x326,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          state.actionTicks = 2;
          state.actionStage = i16(state.actionStage) + 1;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          a.select_action(10);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x335,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x19f:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.actionTicks = 0;
          state.actionStage = 0;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
        }
        if (((4 < i16(a.animation.cursor.index)) && (i16(state.actionTicks) < 1)) &&
           (i16(state.actionStage) < 5)) {
          events.sound(0);
          consumeSpirit(0x28);
          a.launchParameters[0] = (float)(i16(state.actionStage) * -4 + -0x2d);
          a.launchParameters[1] = 15.0f;
          owned(0x326,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          state.actionTicks = 2;
          state.actionStage = i16(state.actionStage) + 1;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          a.select_action(10);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x335,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x1a0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0);
        if (u8(a.combat.spellPhase) == (u8)0x2) {
          for (volley = 0; volley < 5; volley = volley + 1) {
            a.launchParameters[0] = 0.0f;
            a.launchParameters[1] = 10.0f;
            uVar10 = 1;
            uVar9 = 0;
            layer = 1;
            direction = (u32)(u8)u8(a.motion.facing);
            position = double(a.motion.y);
            step = random.range(0,0x14);
            position = (float)step + position + double(20.0f) + (float)(volley * 0x28);
            step = random.range(0,0x50);
            ownedVisual(0x344,(float)((step + 0x1e) * (int)(i8)u8(a.motion.facing)) +
                               double(a.motion.x),position,direction,layer,uVar9,uVar10);
          }
          return;
        }
        for (emission = 0; emission < 8; emission = emission + 1) {
          a.launchParameters[0] = 0.0f;
          a.launchParameters[1] = 10.0f;
          uVar10 = 1;
          uVar9 = 0;
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,0x14);
          position = (float)step + position + double(20.0f) + (float)(emission * 0x1e);
          step = random.range(0,100);
          ownedVisual(0x344,(float)((step + 0x14) * (int)(i8)u8(a.motion.facing)) +
                             double(a.motion.x),position,direction,layer,uVar9,uVar10);
        }
        return;
      case 0x1a1:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0);
        if (u8(a.combat.spellPhase) == (u8)0x2) {
          for (local_1c = 0; local_1c < 5; local_1c = local_1c + 1) {
            a.launchParameters[0] = 30.0f;
            a.launchParameters[1] = 10.0f;
            uVar10 = 1;
            uVar9 = 0;
            layer = 1;
            direction = (u32)(u8)u8(a.motion.facing);
            position = double(a.motion.y);
            step = random.range(0,0x14);
            position = (float)step + position + double(20.0f) + (float)(local_1c * 0x28);
            step = random.range(0,0x50);
            ownedVisual(0x344,(float)((local_1c * -10 + 0x1e + step) * (int)(i8)u8(a.motion.facing)) +
                               double(a.motion.x),position,direction,layer,uVar9,uVar10);
          }
          return;
        }
        for (local_20 = 0; local_20 < 8; local_20 = local_20 + 1) {
          a.launchParameters[0] = 30.0f;
          a.launchParameters[1] = 10.0f;
          uVar10 = 1;
          uVar9 = 0;
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,0x14);
          position = (float)step + position + double(20.0f) + (float)(local_20 * 0x1e);
          step = random.range(0,100);
          ownedVisual(0x344,(float)((local_20 * -10 + 0x14 + step) * (int)(i8)u8(a.motion.facing)) +
                             double(a.motion.x),position,direction,layer,uVar9,uVar10);
        }
        return;
      case 0x1a2:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(2);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0);
        if (u8(a.combat.spellPhase) == (u8)0x2) {
          a.launchParameters[0] = 65.0f;
          a.launchParameters[1] = 10.0f;
          ownedVisual(0x344,(float)((i8)u8(a.motion.facing) * 0x2f) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1,0,1);
          a.launchParameters[0] = 55.0f;
          a.launchParameters[1] = 10.0f;
          ownedVisual(0x344,(float)((i8)u8(a.motion.facing) * 0x2f) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1,0,1);
          a.launchParameters[0] = 45.0f;
          a.launchParameters[1] = 10.0f;
          ownedVisual(0x344,(float)((i8)u8(a.motion.facing) * 0x2f) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1,0,1);
          a.launchParameters[0] = 35.0f;
          a.launchParameters[1] = 10.0f;
          ownedVisual(0x344,(float)((i8)u8(a.motion.facing) * 0x2f) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1,0,1);
          a.launchParameters[0] = 25.0f;
          a.launchParameters[1] = 10.0f;
          ownedVisual(0x344,(float)((i8)u8(a.motion.facing) * 0x2f) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1,0,1);
          return;
        }
        a.launchParameters[0] = 65.0f;
        a.launchParameters[1] = 10.0f;
        ownedVisual(0x344,(float)((i8)u8(a.motion.facing) * 0x2f) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1,0,1);
        a.launchParameters[0] = 55.0f;
        a.launchParameters[1] = 10.0f;
        ownedVisual(0x344,(float)((i8)u8(a.motion.facing) * 0x2f) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1,0,1);
        a.launchParameters[0] = 45.0f;
        a.launchParameters[1] = 10.0f;
        ownedVisual(0x344,(float)((i8)u8(a.motion.facing) * 0x2f) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1,0,1);
        a.launchParameters[0] = 35.0f;
        a.launchParameters[1] = 10.0f;
        ownedVisual(0x344,(float)((i8)u8(a.motion.facing) * 0x2f) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1,0,1);
        a.launchParameters[0] = 25.0f;
        a.launchParameters[1] = 10.0f;
        ownedVisual(0x344,(float)((i8)u8(a.motion.facing) * 0x2f) + double(a.motion.x),double(a.motion.y) + double(60.0f),u8(a.motion.facing),1,0,1);
        return;
      case 0x1a3:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0);
        if (u8(a.combat.spellPhase) == (u8)0x2) {
          for (local_24 = 0; local_24 < 5; local_24 = local_24 + 1) {
            a.launchParameters[0] = 0.0f;
            a.launchParameters[1] = 10.0f;
            uVar10 = 1;
            uVar9 = 0;
            layer = 1;
            direction = (u32)(u8)u8(a.motion.facing);
            position = double(a.motion.y);
            step = random.range(0,0x14);
            position = double(step)+float(double(position)+40.0) + (float)(local_24 * 0x1e);
            step = random.range(0,0x50);
            ownedVisual(0x344,(float)((step + 0x1e) * (int)(i8)u8(a.motion.facing)) +
                               double(a.motion.x),position,direction,layer,uVar9,uVar10);
          }
          return;
        }
        for (local_28 = 0; local_28 < 8; local_28 = local_28 + 1) {
          a.launchParameters[0] = 0.0f;
          a.launchParameters[1] = 10.0f;
          uVar10 = 1;
          uVar9 = 0;
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,0x14);
          position = double(step)+float(double(position)+40.0) + (float)(local_28 * 0x14);
          step = random.range(0,100);
          ownedVisual(0x344,(float)((step + 0x14) * (int)(i8)u8(a.motion.facing)) +
                             double(a.motion.x),position,direction,layer,uVar9,uVar10);
        }
        return;
      case 0x1a4:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0);
        if (u8(a.combat.spellPhase) == (u8)0x2) {
          for (local_2c = 0; local_2c < 5; local_2c = local_2c + 1) {
            a.launchParameters[0] = -30.0f;
            a.launchParameters[1] = 10.0f;
            uVar10 = 1;
            uVar9 = 0;
            layer = 1;
            direction = (u32)(u8)u8(a.motion.facing);
            position = double(a.motion.y);
            step = random.range(0,0x14);
            position = (float)step + position + double(20.0f) + (float)(local_2c * 0x28);
            step = random.range(0,0x50);
            ownedVisual(0x344,(float)((local_2c * 10 + 0x1e + step) * (int)(i8)u8(a.motion.facing)) +
                               double(a.motion.x),position,direction,layer,uVar9,uVar10);
          }
          return;
        }
        for (local_30 = 0; local_30 < 8; local_30 = local_30 + 1) {
          a.launchParameters[0] = -30.0f;
          a.launchParameters[1] = 10.0f;
          uVar10 = 1;
          uVar9 = 0;
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,0x14);
          position = (float)step + position + double(20.0f) + (float)(local_30 * 0x1e);
          step = random.range(0,100);
          ownedVisual(0x344,(float)((local_30 * 10 + 0x14 + step) * (int)(i8)u8(a.motion.facing)) +
                             double(a.motion.x),position,direction,layer,uVar9,uVar10);
        }
        return;
      case 500:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 99;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
          state.actionStage = 0;
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
        if ((a.response != 0) &&
           (state.actionStage = i16(state.actionStage) + 1,
           6 < i16(state.actionStage))) {
          state.actionStage = 0;
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 2.0f;
          a.motion.velocityY = 6.5f;
          a.motion.accelerationY = 0.4000000059604645f;
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(3);
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = 15.0f;
          a.launchParameters[1] = -20.0f;
          a.launchParameters[2] = -60.0f;
          ownedVisual(0x32a,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
          ownedVisual(0x329,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
          a.launchParameters[0] = 10.0f;
          a.launchParameters[1] = -135.0f;
          a.launchParameters[2] = -30.0f;
          ownedVisual(0x32a,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
          ownedVisual(0x329,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
        }
        if (((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) < 1)) {
          selectFrame(8);
          state.actionTicks = i16(state.actionTicks) + 1;
        }
        if (i16(a.animation.cursor.index) != 0xc) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xb);
        return;
      case 0x1f5:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 99;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
          state.actionStage = 0;
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
        if ((a.response != 0) &&
           (state.actionStage = i16(state.actionStage) + 1,
           6 < i16(state.actionStage))) {
          state.actionStage = 0;
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 8.0f;
          a.motion.velocityY = 13.5f;
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          a.motion.accelerationY = 0.5f;
          events.sound(3);
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = 15.0f;
          a.launchParameters[1] = -20.0f;
          a.launchParameters[2] = -60.0f;
          ownedVisual(0x32a,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
          ownedVisual(0x329,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
          a.launchParameters[0] = 10.0f;
          a.launchParameters[1] = -135.0f;
          a.launchParameters[2] = -30.0f;
          ownedVisual(0x32a,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
          ownedVisual(0x329,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
        }
        if (((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) < 3)) {
          selectFrame(8);
          state.actionTicks = i16(state.actionTicks) + 1;
        }
        if (i16(a.animation.cursor.index) != 0xc) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xb);
        return;
      case 0x1f6:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          a.remainingHits = 99;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if (((3 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 6)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          selectFrame(6);
          a.motion.y = 0.0f;
          return;
        }
        if ((a.response != 0) &&
           (state.actionStage = i16(state.actionStage) + 1,
           6 < i16(state.actionStage))) {
          state.actionStage = 0;
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = 15.0f;
          a.launchParameters[1] = -20.0f;
          a.launchParameters[2] = -60.0f;
          ownedVisual(0x32a,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
          ownedVisual(0x329,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
          a.launchParameters[0] = 10.0f;
          a.launchParameters[1] = -135.0f;
          a.launchParameters[2] = -30.0f;
          ownedVisual(0x32a,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
          ownedVisual(0x329,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
          events.sound(3);
        }
        if (((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) < 2)) {
          selectFrame(2);
          state.actionTicks = i16(state.actionTicks) + 1;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(5);
        return;
      case 0x1f7:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * 0.1;
          a.motion.velocityY = double(a.motion.velocityY) * 0.1;
          a.remainingHits = 99;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if (((3 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 6)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          selectFrame(6);
          a.motion.y = 0.0f;
          return;
        }
        if ((a.response != 0) &&
           (state.actionStage = i16(state.actionStage) + 1,
           6 < i16(state.actionStage))) {
          state.actionStage = 0;
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = 15.0f;
          a.launchParameters[1] = -20.0f;
          a.launchParameters[2] = -60.0f;
          ownedVisual(0x32a,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
          ownedVisual(0x329,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
          a.launchParameters[0] = 10.0f;
          a.launchParameters[1] = -135.0f;
          a.launchParameters[2] = -30.0f;
          ownedVisual(0x32a,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
          ownedVisual(0x329,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1,0,1);
          events.sound(3);
        }
        if (((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) < 4)) {
          selectFrame(2);
          state.actionTicks = i16(state.actionTicks) + 1;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(5);
        return;
      case 0x1f8:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if ((i16(a.animation.cursor.index) == 6) || (i16(a.animation.cursor.index) == 7)) {
          if ((u8)u8(a.pose.channel(24)) < 0x1f) {
            a.pose.set_channel(24,u8((u8)0x0));
            selectFrame(8);
            a.motion.velocityX = 60.0f;
            a.motion.velocityY = 10.0f;
            a.motion.accelerationY = 0.4000000059604645f;
            return;
          }
          a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + -0x1e)));
        }
        if ((i16(a.animation.cursor.index) == 9) || (i16(a.animation.cursor.index) == 10)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if ((u8)u8(a.pose.channel(24)) < 0xeb) {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + '\x14')));
          }
          else {
            a.pose.set_channel(24,u8((u8)0xff));
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.pose.set_channel(24,u8((u8)0xff));
            a.motion.clear();
            selectFrame(0xb);
            a.motion.y = 0.0f;
            return;
          }
        }
        if ((i16(a.animation.cursor.index) < 8) && (1 < i16(a.animation.cursor.index))) {
          uVar11 = 1;
          uVar10 = 0;
          uVar9 = 1;
          layer = 1;
          position = double(a.motion.y);
          step = random.range(0,0x5a);
          position = (float)step + (position - double(15.0f));
          step = random.range(0,0x78);
          ownedVisual(0x33e,((float)step + double(a.motion.x)) - double(60.0f),position,layer,uVar9,uVar10,uVar11);
        }
        if (((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          state.actionTicks = i16(state.actionTicks) + 1;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
          ownedVisual(0x338,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x339,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33a,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33b,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33c,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          a.begin_pose(0.0f,0.0f,0.0f);
          a.pose.set_channel(24,u8((u8)0xff));
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionStage) < 4)) {
          selectFrame(6);
          state.actionStage = i16(state.actionStage) + 1;
          return;
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(9);
        return;
      case 0x1f9:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if ((i16(a.animation.cursor.index) == 6) || (i16(a.animation.cursor.index) == 7)) {
          if ((u8)u8(a.pose.channel(24)) < 0x1f) {
            a.pose.set_channel(24,u8((u8)0x0));
            selectFrame(8);
            a.motion.velocityX = -60.0f;
            a.motion.velocityY = 10.0f;
            a.motion.accelerationY = 0.4000000059604645f;
            return;
          }
          a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + -0x1e)));
        }
        if ((i16(a.animation.cursor.index) == 9) || (i16(a.animation.cursor.index) == 10)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if ((u8)u8(a.pose.channel(24)) < 0xeb) {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + '\x14')));
          }
          else {
            a.pose.set_channel(24,u8((u8)0xff));
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.pose.set_channel(24,u8((u8)0xff));
            a.motion.clear();
            selectFrame(0xb);
            a.motion.y = 0.0f;
            return;
          }
        }
        if ((i16(a.animation.cursor.index) < 8) && (1 < i16(a.animation.cursor.index))) {
          uVar11 = 1;
          uVar10 = 0;
          uVar9 = 1;
          layer = 1;
          position = double(a.motion.y);
          step = random.range(0,0x5a);
          position = (float)step + (position - double(15.0f));
          step = random.range(0,0x78);
          ownedVisual(0x33e,((float)step + double(a.motion.x)) - double(60.0f),position,layer,uVar9,uVar10,uVar11);
        }
        if (((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          state.actionTicks = i16(state.actionTicks) + 1;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
          ownedVisual(0x338,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x339,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33a,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33b,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33c,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          a.begin_pose(0.0f,0.0f,0.0f);
          a.pose.set_channel(24,u8((u8)0xff));
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionStage) < 4)) {
          selectFrame(6);
          state.actionStage = i16(state.actionStage) + 1;
          return;
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
          a.motion.clear();
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if ((i16(a.animation.cursor.index) == 6) || (i16(a.animation.cursor.index) == 7)) {
          if ((u8)u8(a.pose.channel(24)) < 0x1f) {
            a.pose.set_channel(24,u8((u8)0x0));
            selectFrame(8);
            a.motion.velocityX = 120.0f;
            a.motion.velocityY = 10.0f;
            a.motion.accelerationY = 0.4000000059604645f;
            return;
          }
          a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + -0x1e)));
        }
        if ((i16(a.animation.cursor.index) == 9) || (i16(a.animation.cursor.index) == 10)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if ((u8)u8(a.pose.channel(24)) < 0xeb) {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + '\x14')));
          }
          else {
            a.pose.set_channel(24,u8((u8)0xff));
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.pose.set_channel(24,u8((u8)0xff));
            a.motion.clear();
            selectFrame(0xb);
            a.motion.y = 0.0f;
            return;
          }
        }
        if ((i16(a.animation.cursor.index) < 8) && (1 < i16(a.animation.cursor.index))) {
          uVar11 = 1;
          uVar10 = 0;
          uVar9 = 1;
          layer = 1;
          position = double(a.motion.y);
          step = random.range(0,0x5a);
          position = (float)step + (position - double(15.0f));
          step = random.range(0,0x78);
          ownedVisual(0x33e,((float)step + double(a.motion.x)) - double(60.0f),position,layer,uVar9,uVar10,uVar11);
        }
        if (((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          state.actionTicks = i16(state.actionTicks) + 1;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
          ownedVisual(0x338,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x339,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33a,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33b,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33c,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          a.begin_pose(0.0f,0.0f,0.0f);
          a.pose.set_channel(24,u8((u8)0xff));
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionStage) < 4)) {
          selectFrame(6);
          state.actionStage = i16(state.actionStage) + 1;
          return;
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(9);
        return;
      case 0x1fb:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.response = 1;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if ((i16(a.animation.cursor.index) == 6) || (i16(a.animation.cursor.index) == 7)) {
          if ((u8)u8(a.pose.channel(24)) < 0x15) {
            a.pose.set_channel(24,u8((u8)0x0));
          }
          else {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + -0x14)));
          }
        }
        if ((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xd)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if ((u8)u8(a.pose.channel(24)) < 0xeb) {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + '\x14')));
          }
          else {
            a.pose.set_channel(24,u8((u8)0xff));
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.pose.set_channel(24,u8((u8)0xff));
            a.motion.clear();
            selectFrame(0xd);
            a.motion.y = 0.0f;
            return;
          }
        }
        if ((i16(a.animation.cursor.index) < 8) && (1 < i16(a.animation.cursor.index))) {
          uVar11 = 1;
          uVar10 = 0;
          uVar9 = 1;
          layer = 1;
          position = double(a.motion.y);
          step = random.range(0,0x5a);
          position = (float)step + (position - double(15.0f));
          step = random.range(0,0x78);
          ownedVisual(0x33e,((float)step + double(a.motion.x)) - double(60.0f),position,layer,uVar9,uVar10,uVar11);
        }
        if (((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          state.actionTicks = i16(state.actionTicks) + 1;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
          ownedVisual(0x338,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x339,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33a,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33b,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33c,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          consumeSpirit(200);
          a.begin_pose(0.0f,0.0f,0.0f);
          a.pose.set_channel(24,u8((u8)0xff));
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionStage) < 4)) {
          selectFrame(6);
          state.actionStage = i16(state.actionStage) + 1;
          return;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 60.0f;
          a.motion.velocityY = 3.0f;
          a.motion.accelerationY = 0.5f;
        }
        if (i16(a.animation.cursor.index) != 0xd) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xb);
        return;
      case 0x1fc:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.response = 1;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if ((i16(a.animation.cursor.index) == 6) || (i16(a.animation.cursor.index) == 7)) {
          if ((u8)u8(a.pose.channel(24)) < 0x15) {
            a.pose.set_channel(24,u8((u8)0x0));
          }
          else {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + -0x14)));
          }
        }
        if ((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xd)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if ((u8)u8(a.pose.channel(24)) < 0xeb) {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + '\x14')));
          }
          else {
            a.pose.set_channel(24,u8((u8)0xff));
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.pose.set_channel(24,u8((u8)0xff));
            a.motion.clear();
            selectFrame(0xd);
            a.motion.y = 0.0f;
            return;
          }
        }
        if ((i16(a.animation.cursor.index) < 8) && (1 < i16(a.animation.cursor.index))) {
          uVar11 = 1;
          uVar10 = 0;
          uVar9 = 1;
          layer = 1;
          position = double(a.motion.y);
          step = random.range(0,0x5a);
          position = (float)step + (position - double(15.0f));
          step = random.range(0,0x78);
          ownedVisual(0x33e,((float)step + double(a.motion.x)) - double(60.0f),position,layer,uVar9,uVar10,uVar11);
        }
        if (((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          state.actionTicks = i16(state.actionTicks) + 1;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
          ownedVisual(0x338,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x339,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33a,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33b,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33c,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          consumeSpirit(200);
          a.begin_pose(0.0f,0.0f,0.0f);
          a.pose.set_channel(24,u8((u8)0xff));
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionStage) < 4)) {
          selectFrame(6);
          state.actionStage = i16(state.actionStage) + 1;
          return;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -60.0f;
          a.motion.velocityY = 3.0f;
          a.motion.accelerationY = 0.5f;
        }
        if (i16(a.animation.cursor.index) != 0xd) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xb);
        return;
      case 0x1fd:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.response = 1;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if ((i16(a.animation.cursor.index) == 6) || (i16(a.animation.cursor.index) == 7)) {
          if ((u8)u8(a.pose.channel(24)) < 0x15) {
            a.pose.set_channel(24,u8((u8)0x0));
          }
          else {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + -0x14)));
          }
        }
        if ((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xd)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if ((u8)u8(a.pose.channel(24)) < 0xeb) {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + '\x14')));
          }
          else {
            a.pose.set_channel(24,u8((u8)0xff));
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.pose.set_channel(24,u8((u8)0xff));
            a.motion.clear();
            selectFrame(0xd);
            a.motion.y = 0.0f;
            return;
          }
        }
        if ((i16(a.animation.cursor.index) < 8) && (1 < i16(a.animation.cursor.index))) {
          uVar11 = 1;
          uVar10 = 0;
          uVar9 = 1;
          layer = 1;
          position = double(a.motion.y);
          step = random.range(0,0x5a);
          position = (float)step + (position - double(15.0f));
          step = random.range(0,0x78);
          ownedVisual(0x33e,((float)step + double(a.motion.x)) - double(60.0f),position,layer,uVar9,uVar10,uVar11);
        }
        if (((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          state.actionTicks = i16(state.actionTicks) + 1;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
          ownedVisual(0x338,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x339,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33a,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33b,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33c,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          consumeSpirit(200);
          a.begin_pose(0.0f,0.0f,0.0f);
          a.pose.set_channel(24,u8((u8)0xff));
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionStage) < 4)) {
          selectFrame(6);
          state.actionStage = i16(state.actionStage) + 1;
          return;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 120.0f;
          a.motion.velocityY = 3.0f;
          a.motion.accelerationY = 0.5f;
        }
        if (i16(a.animation.cursor.index) != 0xd) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xb);
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 6) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 6) {
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
    }
    else {
      switch(step) {
      case 0x255:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if (4 < i16(a.animation.cursor.index)) {
          if ((i16(state.actionStage) < 3) && (i16(state.actionTicks) < 1)) {
            events.sound(0);
            a.launchParameters[0] = (float)(i16(state.actionStage) * 3 + 100);
            a.launchParameters[1] = 10.0f;
            owned(0x325,(float)((i8)u8(a.motion.facing) * 0x61) + double(a.motion.x),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(i16(state.actionStage) * 3 + 0x5f);
            a.launchParameters[1] = 10.0f;
            owned(0x325,(float)((i8)u8(a.motion.facing) * 0x61) + double(a.motion.x),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(i16(state.actionStage) * 3 + 0x5a);
            a.launchParameters[1] = 10.0f;
            owned(0x325,(float)((i8)u8(a.motion.facing) * 0x61) + double(a.motion.x),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(i16(state.actionStage) * 3 + 0x55);
            a.launchParameters[1] = 10.0f;
            owned(0x325,(float)((i8)u8(a.motion.facing) * 0x61) + double(a.motion.x),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(i16(state.actionStage) * 3 + 0x50);
            a.launchParameters[1] = 10.0f;
            owned(0x325,(float)((i8)u8(a.motion.facing) * 0x61) + double(a.motion.x),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
            consumeSpirit(100);
            state.actionStage = i16(state.actionStage) + 1;
            state.actionTicks = 3;
          }
          state.actionTicks = i16(state.actionTicks) + -1;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x259:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if (4 < i16(a.animation.cursor.index)) {
          if ((i16(state.actionStage) < 3) && (i16(state.actionTicks) < 1)) {
            events.sound(0);
            a.launchParameters[0] = (float)(i16(state.actionStage) * 3 + 0x91);
            a.launchParameters[1] = 10.0f;
            owned(0x325,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 100),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(i16(state.actionStage) * 3 + 0x8c);
            a.launchParameters[1] = 10.0f;
            owned(0x325,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 100),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(i16(state.actionStage) * 3 + 0x87);
            a.launchParameters[1] = 10.0f;
            owned(0x325,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 100),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(i16(state.actionStage) * 3 + 0x82);
            a.launchParameters[1] = 10.0f;
            owned(0x325,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 100),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(i16(state.actionStage) * 3 + 0x7d);
            a.launchParameters[1] = 10.0f;
            owned(0x325,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 100),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
            consumeSpirit(100);
            state.actionStage = i16(state.actionStage) + 1;
            state.actionTicks = 3;
          }
          state.actionTicks = i16(state.actionTicks) + -1;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x25a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0);
        a.launchParameters[0] = 10.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x325,(float)((i8)u8(a.motion.facing) * 0x61) + double(a.motion.x),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 5.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x325,(float)((i8)u8(a.motion.facing) * 0x61) + double(a.motion.x),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 0.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x325,(float)((i8)u8(a.motion.facing) * 0x61) + double(a.motion.x),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 5.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x325,(float)((i8)u8(a.motion.facing) * 0x61) + double(a.motion.x),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 10.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x325,(float)((i8)u8(a.motion.facing) * 0x61) + double(a.motion.x),double(a.motion.y) + double(153.0f),u8(a.motion.facing),1);
        consumeSpirit(0xfa);
        return;
      case 0x25b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        if (((5 < i16(a.animation.cursor.index)) && (i16(state.actionTicks) < 1)) &&
           (i16(state.actionStage) < 1)) {
          events.sound(0);
          consumeSpirit(0xfa);
          a.launchParameters[0] = 0.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 10.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 20.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 30.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 40.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 50.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 60.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 70.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 80.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 90.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 100.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 110.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 120.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 130.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 140.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 150.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 160.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 170.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 180.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 190.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 200.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 210.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 220.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 230.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 240.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 250.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 260.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 270.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 280.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 290.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 300.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 310.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 320.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 330.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 340.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 350.0f;
          a.launchParameters[1] = 7.0f;
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
          state.actionTicks = 2;
          state.actionStage = i16(state.actionStage) + 1;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x25c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0);
        consumeSpirit(0xfa);
        a.launchParameters[0] = -28.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -21.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -14.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -7.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 0.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 7.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 14.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 21.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 28.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
        return;
      case 0x25d:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.actionTicks = 0;
          state.actionStage = 0;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
        }
        if (4 < i16(a.animation.cursor.index)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = -3.0f;
        a.motion.velocityY = 9.0f;
        a.motion.accelerationY = 0.699999988079071f;
        events.sound(0);
        consumeSpirit(0xfa);
        a.launchParameters[0] = 0.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 10.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 20.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 30.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 40.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 50.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 60.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 70.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 80.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 90.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 100.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 110.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 120.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 130.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 140.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 150.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 160.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 170.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 180.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 190.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 200.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 210.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 220.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 230.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 240.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 250.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 260.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 270.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 280.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 290.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 300.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 310.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 320.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 330.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 340.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 350.0f;
        a.launchParameters[1] = 7.0f;
        owned(0x327,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        return;
      case 0x25e:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.actionTicks = 0;
          state.actionStage = 0;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
        }
        if (4 < i16(a.animation.cursor.index)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = -3.0f;
        a.motion.velocityY = 9.0f;
        a.motion.accelerationY = 0.699999988079071f;
        events.sound(0);
        consumeSpirit(0xfa);
        a.launchParameters[0] = -28.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -21.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -14.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -7.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 0.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 7.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 14.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 21.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 28.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x342,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        return;
      case 0x25f:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (0 < i16(state.actionTicks)) {
          return;
        }
        if (0 < i16(state.actionStage)) {
          return;
        }
        events.sound(0);
        consumeSpirit(0x96);
        for (local_34 = 0; local_34 < 10; local_34 = local_34 + 1) {
          a.launchParameters[0] = (float)(local_34 * -6 + 0x1e);
          a.launchParameters[1] = 15.0f;
          owned(0x346,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
        }
        a.motion.velocityX = (float)(int)i16(state.actionTicks) * double(0.01f);
        a.motion.velocityY = (float)(int)i16(state.actionStage) * double(0.01f);
        state.actionTicks = 2;
        state.actionStage = i16(state.actionStage) + 1;
        return;
      case 0x260:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (0 < i16(state.actionTicks)) {
          return;
        }
        if (0 < i16(state.actionStage)) {
          return;
        }
        events.sound(0);
        consumeSpirit(0x96);
        for (local_38 = 0; local_38 < 10; local_38 = local_38 + 1) {
          a.launchParameters[0] = (float)(local_38 * -6 + 0x1e);
          a.launchParameters[1] = 15.0f;
          owned(0x347,(float)((i8)u8(a.motion.facing) * 0x31) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
        }
        a.motion.velocityX = (float)(int)i16(state.actionTicks) * double(0.01f);
        a.motion.velocityY = (float)(int)i16(state.actionStage) * double(0.01f);
        state.actionTicks = 2;
        state.actionStage = i16(state.actionStage) + 1;
        return;
      case 0x261:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.actionTicks = 0;
          state.actionStage = 0;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
        }
        if (4 < i16(a.animation.cursor.index)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = -3.0f;
        a.motion.velocityY = 9.0f;
        a.motion.accelerationY = 0.699999988079071f;
        events.sound(0);
        consumeSpirit(0x96);
        for (local_3c = 0; local_3c < 10; local_3c = local_3c + 1) {
          a.launchParameters[0] = (float)(local_3c * -6 + 0x1e);
          a.launchParameters[1] = 15.0f;
          owned(0x346,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        }
        return;
      case 0x262:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.actionTicks = 0;
          state.actionStage = 0;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
        }
        if (4 < i16(a.animation.cursor.index)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = -3.0f;
        a.motion.velocityY = 9.0f;
        a.motion.accelerationY = 0.699999988079071f;
        events.sound(0);
        consumeSpirit(0x96);
        for (local_40 = 0; local_40 < 10; local_40 = local_40 + 1) {
          a.launchParameters[0] = (float)(local_40 * -6 + 0x1e);
          a.launchParameters[1] = 15.0f;
          owned(0x347,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        }
        return;
      case 700:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
          state.emissionDelay = 0;
          state.volleyLoops = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x73,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x12),double(a.motion.y) + double(92.0f),u8(a.motion.facing),1);
        }
        if (0 < i16(state.emissionDelay)) {
          state.emissionDelay = i16(state.emissionDelay) + -1;
        }
        if ((((4 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
            (i16(state.emissionDelay) == 0)) && (i16(state.actionStage) < 0x24)) {
          consumeSpirit(10);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 5);
          owned(0x323,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 4),double(a.motion.y) + double(136.0f),u8(a.motion.facing),1);
          step = i16(state.actionStage) * 5 + 0xb4;
          a.launchParameters[0] = (float)step;
          owned(0x323,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 4),double(a.motion.y) + double(136.0f),u8(a.motion.facing),1);
          state.actionStage = i16(state.actionStage) + 1;
          state.emissionDelay = i16(state.emissionDelay) + 1;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(5);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (4 < i16(state.volleyLoops)) {
          return;
        }
        selectFrame(9);
        state.volleyLoops = i16(state.volleyLoops) + 1;
        return;
      case 0x2bd:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
          state.emissionDelay = 0;
          state.volleyLoops = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x74,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x12),double(a.motion.y) + double(92.0f),u8(a.motion.facing),1);
        }
        if (0 < i16(state.emissionDelay)) {
          state.emissionDelay = i16(state.emissionDelay) + -1;
        }
        if ((((4 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
            (i16(state.emissionDelay) == 0)) && (i16(state.actionStage) < 0x48)) {
          state.knivesActive = 1;
          consumeSpirit(10);
          a.launchParameters[0] = (float)(i16(state.actionStage) * 5);
          owned(0x32b,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 4),double(a.motion.y) + double(136.0f),u8(a.motion.facing),1);
          step = i16(state.actionStage) * 5 + 0xb4;
          a.launchParameters[0] = (float)step;
          owned(0x32b,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 4),double(a.motion.y) + double(136.0f),u8(a.motion.facing),1);
          state.actionStage = i16(state.actionStage) + 1;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(6);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (4 < i16(state.volleyLoops)) {
          return;
        }
        selectFrame(9);
        state.volleyLoops = i16(state.volleyLoops) + 1;
        return;
      case 0x2be:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 99;
        }
        if ((((double(0.0f) < double(a.motion.y)) && (0x17 < i16(a.animation.cursor.index))) &&
            (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
            double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) &&
           (i16(a.animation.cursor.index) < 0x22)) {
          selectFrame(0x22);
          a.motion.clear();
          a.motion.y = 0.0f;
          return;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x73,(float)((i8)u8(a.motion.facing) * 0x1e) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 0xd) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(7);
          a.motion.velocityX = 0.5f;
          a.motion.velocityY = 1.0f;
          a.motion.accelerationY = 0.5f;
          a.combat.recoveryTimers[0] = 10;
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0xc);
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y) + double(130.0f);
          effectY = (float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x);
          step = random.range(0,100);
          owned((200 < step) + 0x321,effectY,position,direction,layer);
          owned(0x33f,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y) + double(130.0f),u8(a.motion.facing),1);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x11) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0xc);
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y) + double(130.0f);
          effectY = (float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x);
          step = random.range(0,100);
          owned((200 < step) + 0x321,effectY,position,direction,layer);
          owned(0x33f,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y) + double(130.0f),u8(a.motion.facing),1);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x13) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0xc);
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y) + double(130.0f);
          effectY = (float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x);
          step = random.range(0,100);
          owned((200 < step) + 0x321,effectY,position,direction,layer);
          owned(0x33f,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y) + double(130.0f),u8(a.motion.facing),1);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x15) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0xc);
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y) + double(130.0f);
          effectY = (float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x);
          step = random.range(0,100);
          owned((200 < step) + 0x321,effectY,position,direction,layer);
          owned(0x33f,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y) + double(130.0f),u8(a.motion.facing),1);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x17) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0xc);
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y) + double(130.0f);
          effectY = (float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x);
          step = random.range(0,100);
          owned((200 < step) + 0x321,effectY,position,direction,layer);
          owned(0x33f,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y) + double(130.0f),u8(a.motion.facing),1);
          events.sound(7);
          if (i16(state.actionTicks) < 6) {
            state.actionTicks = i16(state.actionTicks) + 1;
            selectFrame(0xf);
          }
        }
        if (i16(a.animation.cursor.index) != 0x22) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x20);
        return;
      case 0x2bf:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 99;
        }
        if ((((double(0.0f) < double(a.motion.y)) && (0x17 < i16(a.animation.cursor.index))) &&
            (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
            double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) &&
           (i16(a.animation.cursor.index) < 0x22)) {
          selectFrame(0x22);
          a.motion.clear();
          a.motion.y = 0.0f;
          return;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x74,(float)((i8)u8(a.motion.facing) * 0x1e) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 0xd) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(7);
          a.motion.velocityX = 0.5f;
          a.motion.velocityY = 0.8999999761581421f;
          a.motion.accelerationY = 0.5f;
          a.combat.recoveryTimers[0] = 0x14;
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y) + double(130.0f);
          effectY = (float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x);
          step = random.range(0,100);
          owned((200 < step) + 0x322,effectY,position,direction,layer);
          owned(0x340,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y) + double(130.0f),u8(a.motion.facing),1);
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,400);
          owned(0x341,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x280),(position + double(300.0f)) - (float)step,direction,layer);
          consumeSpirit(0xc);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x11) && (i16(a.animation.cursor.elapsed) == 0)) {
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y) + double(130.0f);
          effectY = (float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x);
          step = random.range(0,100);
          owned((200 < step) + 0x322,effectY,position,direction,layer);
          owned(0x340,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y) + double(130.0f),u8(a.motion.facing),1);
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,400);
          owned(0x341,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x280),(position + double(300.0f)) - (float)step,direction,layer);
          consumeSpirit(0xc);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x13) && (i16(a.animation.cursor.elapsed) == 0)) {
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y) + double(130.0f);
          effectY = (float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x);
          step = random.range(0,100);
          owned((200 < step) + 0x322,effectY,position,direction,layer);
          owned(0x340,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y) + double(130.0f),u8(a.motion.facing),1);
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,400);
          owned(0x341,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x280),(position + double(300.0f)) - (float)step,direction,layer);
          consumeSpirit(0xc);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x15) && (i16(a.animation.cursor.elapsed) == 0)) {
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y) + double(130.0f);
          effectY = (float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x);
          step = random.range(0,100);
          owned((200 < step) + 0x322,effectY,position,direction,layer);
          owned(0x340,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y) + double(130.0f),u8(a.motion.facing),1);
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,400);
          owned(0x341,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x280),(position + double(300.0f)) - (float)step,direction,layer);
          consumeSpirit(0xc);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x17) && (i16(a.animation.cursor.elapsed) == 0)) {
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y) + double(130.0f);
          effectY = (float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x);
          step = random.range(0,100);
          owned((200 < step) + 0x322,effectY,position,direction,layer);
          owned(0x340,(float)((i8)u8(a.motion.facing) * 0x3c) + double(a.motion.x),double(a.motion.y) + double(130.0f),u8(a.motion.facing),1);
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,400);
          owned(0x341,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x280),(position + double(300.0f)) - (float)step,direction,layer);
          consumeSpirit(0xc);
          events.sound(7);
          if (i16(state.actionTicks) < 10) {
            state.actionTicks = i16(state.actionTicks) + 1;
            selectFrame(0xf);
          }
        }
        if (i16(a.animation.cursor.index) != 0x22) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x20);
        return;
      case 0x2c0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          state.actionStage = 0;
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 99;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          ownedVisual(0x332,double(a.motion.x),double(a.motion.y) + double(100.0f),1,1,0,1);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.actionStage) < 1) {
            state.actionStage = i16(state.actionStage) + 1;
            selectFrame(3);
            return;
          }
          hitFreeze(100);
          events.system_sound(0x17);
          events.sound(9);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x73,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x12),double(a.motion.y) + double(92.0f),u8(a.motion.facing),1);
          ownedVisual(0x330,640.0f,320.0f,1,2,0,1);
          state.timeReserve = a.combat.spirit;
          state.timeActive = 1;
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (3 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(9);
        return;
      case 0x2c1:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          state.actionStage = 0;
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 99;
        }
        if (4 < i16(a.animation.cursor.index)) {
          state.clockReserve = 0xb4;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          ownedVisual(0x32f,double(a.motion.x),double(a.motion.y) + double(100.0f),1,1,0,1);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.actionStage) < 4) {
            state.actionStage = i16(state.actionStage) + 1;
            selectFrame(3);
            return;
          }
          hitFreeze(0x28);
          events.sound(10);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x73,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x12),double(a.motion.y) + double(92.0f),u8(a.motion.facing),1);
          ownedVisual(0x32e,640.0f,320.0f,1,2,0,1);
          state.clockReserve = a.combat.spirit;
          state.clockPulse = 0;
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (3 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(9);
        return;
      case 0x2c2:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          state.actionStage = 0;
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 99;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.timeActive = 1;
          ownedVisual(0x332,double(a.motion.x),double(a.motion.y) + double(100.0f),1,1,0,1);
          return;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.actionStage) < 1) {
            state.actionStage = i16(state.actionStage) + 1;
            selectFrame(3);
            return;
          }
          hitFreeze(100);
          events.system_sound(0x17);
          events.sound(9);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x74,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x12),double(a.motion.y) + double(92.0f),u8(a.motion.facing),1);
          ownedVisual(0x330,640.0f,320.0f,1,2,0,1);
          state.timeReserve = a.combat.spirit;
          state.timeActive = 1;
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (3 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(9);
        return;
      case 0x2c3:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          state.actionStage = 0;
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 99;
        }
        if (5 < i16(a.animation.cursor.index)) {
          state.clockReserve = 300;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          ownedVisual(0x32f,double(a.motion.x),double(a.motion.y) + double(100.0f),1,1,0,1);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.actionStage) < 4) {
            state.actionStage = i16(state.actionStage) + 1;
            selectFrame(3);
            return;
          }
          hitFreeze(0x28);
          events.system_sound(0x17);
          events.sound(10);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x74,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x12),double(a.motion.y) + double(92.0f),u8(a.motion.facing),1);
          ownedVisual(0x32e,640.0f,320.0f,1,2,0,1);
          state.clockReserve = a.combat.spirit;
          state.clockPulse = 0;
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (3 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(9);
        return;
      case 0x2db:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
          state.actionStage = 0;
          state.emissionDelay = 0;
          state.volleyLoops = 0;
          a.motion.accelerationY = 0.20000000298023224f;
        }
        if ((i16(a.animation.cursor.index) == 6) || (i16(a.animation.cursor.index) == 7)) {
          if ((u8)u8(a.pose.channel(24)) < 0x1f) {
            a.pose.set_channel(24,u8((u8)0x0));
            selectFrame(8);
            return;
          }
          a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + -0x1e)));
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          step = random.range(0,100);
          a.motion.velocityX = (float)(step + -0x32);
          step = random.range(0,0x1e);
          a.motion.velocityY = (float)(step + -0xf);
          if (double(a.motion.x) <= double(other.motion.x)) {
            a.motion.facing = (u8)0x1;
          }
          else {
            a.motion.facing = (u8)0xff;
          }
        }
        if (i16(a.animation.cursor.index) == 9) {
          if ((u8)u8(a.pose.channel(24)) < 0xe1) {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + '\x1e')));
          }
          else {
            a.pose.set_channel(24,u8((u8)0xff));
          }
        }
        if (((i16(a.animation.cursor.index) == 0xf) || (i16(a.animation.cursor.index) == 0x10)) &&
           (i16(state.actionTicks) < 2)) {
          if ((u8)u8(a.pose.channel(24)) < 0x29) {
            a.pose.set_channel(24,u8((u8)0x0));
            if (i16(state.actionTicks) < 3) {
              selectFrame(8);
              return;
            }
          }
          else {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + -0x28)));
          }
        }
        if ((i16(a.animation.cursor.index) == 0x12) || (i16(a.animation.cursor.index) == 0x13)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if ((u8)u8(a.pose.channel(24)) < 0xeb) {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + '\x14')));
          }
          else {
            a.pose.set_channel(24,u8((u8)0xff));
            selectFrame(0x14);
          }
        }
        if (((i16(a.animation.cursor.index) == 0x14) || (i16(a.animation.cursor.index) == 0x15)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.pose.set_channel(24,u8((u8)0xff));
          a.motion.clear();
          selectFrame(0x16);
          a.motion.y = 0.0f;
          return;
        }
        if ((i16(a.animation.cursor.index) < 8) && (1 < i16(a.animation.cursor.index))) {
          uVar11 = 1;
          uVar10 = 0;
          uVar9 = 1;
          layer = 1;
          position = double(a.motion.y);
          step = random.range(0,0x5a);
          position = (float)step + (position - double(15.0f));
          step = random.range(0,0x78);
          ownedVisual(0x33e,((float)step + double(a.motion.x)) - double(60.0f),position,layer,uVar9,uVar10,uVar11);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
          ownedVisual(0x338,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x339,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33a,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33b,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33c,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          a.begin_pose(0.0f,0.0f,0.0f);
          a.pose.set_channel(24,u8((u8)0xff));
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionStage) < 4)) {
          selectFrame(6);
          state.actionStage = i16(state.actionStage) + 1;
          return;
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = i16(state.actionTicks) + 1;
          state.volleyLoops = 0;
          for (local_44 = 0; local_44 < 0x168; local_44 = local_44 + 10) {
            a.launchParameters[0] = (float)local_44;
            a.launchParameters[1] = 5.0f;
            owned(0x385,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 3),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
          }
          for (local_44 = 0; local_44 < 0x168; local_44 = local_44 + 10) {
            a.launchParameters[0] = (float)local_44;
            a.launchParameters[1] = 10.0f;
            owned(0x385,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 3),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
          }
        }
        if (((i16(a.animation.cursor.index) == 0x11) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.volleyLoops) < 6)) {
          selectFrame(0xf);
          state.volleyLoops = i16(state.volleyLoops) + 1;
        }
        if ((i16(a.animation.cursor.index) == 0x14) && (i16(a.animation.cursor.elapsed) == 0)) {
          selectFrame(0x12);
        }
        if (i16(a.animation.cursor.index) != 0x16) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x14);
        return;
      case 0x2e5:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
          state.emissionDelay = 0;
          state.volleyLoops = 0;
          state.burstCount = state.difficulty + 2;
        }
        if (((4 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
           (0 < i16(state.burstCount))) {
          state.volleyLoops = i16(state.volleyLoops) + -1;
          if (3 - state.difficulty < (int)i16(state.volleyLoops)) {
            a.launchParameters[0] = (float)(i16(state.actionStage) * 5);
            owned(0x387,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 4),double(a.motion.y) + double(136.0f),u8(a.motion.facing),1);
            step = i16(state.actionStage) * 5 + 0xb4;
            a.launchParameters[0] = (float)step;
            owned(0x387,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 4),double(a.motion.y) + double(136.0f),u8(a.motion.facing),1);
            state.actionStage = i16(state.actionStage) + 1;
          }
          if ((i16(state.volleyLoops) < 1) && (i16(state.volleyLoops) < -0x1e)) {
            state.volleyLoops = 10;
            state.burstCount = i16(state.burstCount) + -1;
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          advancedFrame = random.range(0x3c,0x78);
          a.commandTimers[2] = advancedFrame + state.difficulty * -0x14;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.volleyLoops = 10;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(5);
        }
        if (((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (0 < i16(state.burstCount))) {
          selectFrame(9);
        }
        if (i16(a.animation.cursor.index) != 0xd) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        state.emissionDelay = i16(state.emissionDelay) + 1;
        if (0xc < i16(state.emissionDelay)) {
          return;
        }
        selectFrame(0xb);
        return;
      case 0x2e6:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
          state.actionStage = 0;
          state.emissionDelay = 0;
          state.volleyLoops = 0x1e;
          state.burstCount = 1;
          a.motion.accelerationY = 0.15000000596046448f;
        }
        if ((i16(a.animation.cursor.index) == 6) || (i16(a.animation.cursor.index) == 7)) {
          if ((u8)u8(a.pose.channel(24)) < 0x1f) {
            a.pose.set_channel(24,u8((u8)0x0));
            selectFrame(8);
            return;
          }
          a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + -0x1e)));
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          step = random.range(0,100);
          a.motion.velocityX = (float)(step + -0x32);
          step = random.range(0,5);
          a.motion.velocityY = (float)(step + 0x14);
          if (double(a.motion.x) <= double(other.motion.x)) {
            a.motion.facing = (u8)0x1;
          }
          else {
            a.motion.facing = (u8)0xff;
          }
        }
        if (i16(a.animation.cursor.index) == 9) {
          if ((u8)u8(a.pose.channel(24)) < 0xe1) {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + '\x1e')));
          }
          else {
            a.pose.set_channel(24,u8((u8)0xff));
          }
        }
        if ((((0xd < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x13)) &&
            (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.pose.set_channel(24,u8((u8)0xff));
          a.motion.clear();
          selectFrame(0x13);
          a.motion.y = 0.0f;
          return;
        }
        if ((i16(a.animation.cursor.index) < 8) && (1 < i16(a.animation.cursor.index))) {
          uVar11 = 1;
          uVar10 = 0;
          uVar9 = 1;
          layer = 1;
          position = double(a.motion.y);
          step = random.range(0,0x5a);
          position = (float)step + (position - double(15.0f));
          step = random.range(0,0x78);
          ownedVisual(0x33e,((float)step + double(a.motion.x)) - double(60.0f),position,layer,uVar9,uVar10,uVar11);
        }
        if (((10 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x11)) &&
           (i16(state.actionStage) == 0)) {
          state.actionStage = i16(state.actionStage) + 1;
          if (state.difficulty == '\0') {
            for (local_48 = 0; local_48 < 0xc; local_48 = local_48 + 1) {
              a.launchParameters[0] = (float)(local_48 * 0x1e);
              owned(0x387,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 4),double(a.motion.y) + double(136.0f),u8(a.motion.facing),1);
            }
          }
          if (state.difficulty == '\x01') {
            for (local_4c = 0; local_4c < 0x14; local_4c = local_4c + 1) {
              a.launchParameters[0] = (float)(local_4c * 0x12);
              owned(0x387,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 4),double(a.motion.y) + double(136.0f),u8(a.motion.facing),1);
            }
          }
          if (state.difficulty == '\x02') {
            for (local_50 = 0; local_50 < 0x1e; local_50 = local_50 + 1) {
              a.launchParameters[0] = (float)(local_50 * 0xc);
              owned(0x387,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 4),double(a.motion.y) + double(136.0f),u8(a.motion.facing),1);
            }
          }
          if (state.difficulty == '\x03') {
            for (local_54 = 0; local_54 < 0x28; local_54 = local_54 + 1) {
              a.launchParameters[0] = (float)(local_54 * 9);
              owned(0x387,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 4),double(a.motion.y) + double(136.0f),u8(a.motion.facing),1);
            }
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          advancedFrame = random.range(0x3c,0x78);
          a.commandTimers[2] = advancedFrame + state.difficulty * -0x14;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
          ownedVisual(0x338,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x339,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33a,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33b,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33c,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          a.begin_pose(0.0f,0.0f,0.0f);
          a.pose.set_channel(24,u8((u8)0xff));
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) < 4)) {
          selectFrame(6);
          state.actionTicks = i16(state.actionTicks) + 1;
          return;
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(5);
        }
        if (((i16(a.animation.cursor.index) == 0x11) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) < 10)) {
          selectFrame(0xf);
          state.actionTicks = i16(state.actionTicks) + 1;
        }
        if (i16(a.animation.cursor.index) != 0x13) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x11);
        return;
      case 0x2ef:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          state.actionStage = 0;
          state.emissionDelay = 0;
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 99;
        }
        if ((double(0.0f) < double(a.motion.y)) && (i16(a.animation.cursor.index) < 0x16)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(0.01f);
        }
        if ((((double(0.0f) < double(a.motion.y)) && (0x17 < i16(a.animation.cursor.index))) &&
            (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
            double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) &&
           (i16(a.animation.cursor.index) < 0x22)) {
          selectFrame(0x22);
          a.motion.clear();
          a.motion.y = 0.0f;
          return;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          impactWord = random.range(0x3c,0x78);
          a.commandTimers[2] = impactWord;
        }
        if ((i16(a.animation.cursor.index) == 0xd) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(7);
          step = random.range(0,6);
          a.motion.velocityX = (float)(step + -3);
          a.motion.velocityY = 1.5f;
          a.motion.accelerationY = 0.5f;
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          step = random.range(0,100);
          a.launchParameters[0] = (float)(step + -0x46);
          step = random.range(0,10);
          a.launchParameters[1] = (float)(step + 5);
          a.set_polar_velocity(double(a.launchParameters[0]),100.0f);
          owned(0x38f,(double(a.motion.velocityX) + double(60.0f)) * (float)(int)(i8)u8(a.motion.facing) +
                             double(a.motion.x),(double(a.motion.y) + double(130.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
          a.motion.velocityX = (float)(int)i16(state.actionTicks) * double(0.01f);
          a.motion.velocityY = (float)(int)i16(state.actionStage) * double(0.01f);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x11) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          step = random.range(0,100);
          a.launchParameters[0] = (float)(step + -0x32);
          step = random.range(0,10);
          a.launchParameters[1] = (float)(step + 5);
          a.set_polar_velocity(double(a.launchParameters[0]),100.0f);
          owned(0x38f,(double(a.motion.velocityX) + double(60.0f)) * (float)(int)(i8)u8(a.motion.facing) +
                             double(a.motion.x),(double(a.motion.y) + double(130.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
          a.motion.velocityX = (float)(int)i16(state.actionTicks) * double(0.01f);
          a.motion.velocityY = (float)(int)i16(state.actionStage) * double(0.01f);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x13) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          step = random.range(0,100);
          a.launchParameters[0] = (float)(step + -0x32);
          step = random.range(0,10);
          a.launchParameters[1] = (float)(step + 5);
          a.set_polar_velocity(double(a.launchParameters[0]),100.0f);
          owned(0x38f,(double(a.motion.velocityX) + double(60.0f)) * (float)(int)(i8)u8(a.motion.facing) +
                             double(a.motion.x),(double(a.motion.y) + double(130.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
          a.motion.velocityX = (float)(int)i16(state.actionTicks) * double(0.01f);
          a.motion.velocityY = (float)(int)i16(state.actionStage) * double(0.01f);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x15) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          step = random.range(0,100);
          a.launchParameters[0] = (float)(step + -0x32);
          step = random.range(0,10);
          a.launchParameters[1] = (float)(step + 5);
          a.set_polar_velocity(double(a.launchParameters[0]),100.0f);
          owned(0x38f,(double(a.motion.velocityX) + double(60.0f)) * (float)(int)(i8)u8(a.motion.facing) +
                             double(a.motion.x),(double(a.motion.y) + double(130.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
          a.motion.velocityX = (float)(int)i16(state.actionTicks) * double(0.01f);
          a.motion.velocityY = (float)(int)i16(state.actionStage) * double(0.01f);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x17) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          step = random.range(0,100);
          a.launchParameters[0] = (float)(step + -0x32);
          step = random.range(0,10);
          a.launchParameters[1] = (float)(step + 5);
          a.set_polar_velocity(double(a.launchParameters[0]),100.0f);
          owned(0x38f,(double(a.motion.velocityX) + double(60.0f)) * (float)(int)(i8)u8(a.motion.facing) +
                             double(a.motion.x),(double(a.motion.y) + double(130.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
          a.motion.velocityX = (float)(int)i16(state.actionTicks) * double(0.01f);
          a.motion.velocityY = (float)(int)i16(state.actionStage) * double(0.01f);
          events.sound(7);
          if ((int)i16(state.emissionDelay) < state.difficulty * 2 + 8) {
            state.emissionDelay = i16(state.emissionDelay) + 1;
            selectFrame(0xf);
          }
        }
        if (i16(a.animation.cursor.index) != 0x22) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x20);
        return;
      case 0x2f0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          state.actionStage = 0;
          state.emissionDelay = 0;
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 99;
        }
        if ((double(0.0f) < double(a.motion.y)) && (i16(a.animation.cursor.index) < 0x16)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(0.02f);
        }
        if ((((double(0.0f) < double(a.motion.y)) && (0x17 < i16(a.animation.cursor.index))) &&
            (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
            double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) &&
           (i16(a.animation.cursor.index) < 0x22)) {
          selectFrame(0x22);
          a.motion.clear();
          a.motion.y = 0.0f;
          return;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          impactWord = random.range(0x3c,0x78);
          a.commandTimers[2] = impactWord;
        }
        if ((i16(a.animation.cursor.index) == 0xd) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(7);
          step = random.range(0,6);
          a.motion.velocityX = (float)(step + -3);
          a.motion.velocityY = 3.5f;
          a.motion.accelerationY = 0.5f;
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          step = random.range(0,100);
          a.launchParameters[0] = (float)(step + -0x46);
          step = random.range(0,10);
          a.launchParameters[1] = (float)(step + 5);
          a.set_polar_velocity(double(a.launchParameters[0]),100.0f);
          owned(0x38f,(double(a.motion.velocityX) + double(60.0f)) * (float)(int)(i8)u8(a.motion.facing) +
                             double(a.motion.x),(double(a.motion.y) + double(130.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
          a.motion.velocityX = (float)(int)i16(state.actionTicks) * double(0.01f);
          a.motion.velocityY = (float)(int)i16(state.actionStage) * double(0.01f);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x11) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          step = random.range(0,100);
          a.launchParameters[0] = (float)(step + -0x46);
          step = random.range(0,10);
          a.launchParameters[1] = (float)(step + 5);
          a.set_polar_velocity(double(a.launchParameters[0]),100.0f);
          owned(0x38f,(double(a.motion.velocityX) + double(60.0f)) * (float)(int)(i8)u8(a.motion.facing) +
                             double(a.motion.x),(double(a.motion.y) + double(130.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
          a.motion.velocityX = (float)(int)i16(state.actionTicks) * double(0.01f);
          a.motion.velocityY = (float)(int)i16(state.actionStage) * double(0.01f);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x13) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          step = random.range(0,100);
          a.launchParameters[0] = (float)(step + -0x46);
          step = random.range(0,10);
          a.launchParameters[1] = (float)(step + 5);
          a.set_polar_velocity(double(a.launchParameters[0]),100.0f);
          owned(0x38f,(double(a.motion.velocityX) + double(60.0f)) * (float)(int)(i8)u8(a.motion.facing) +
                             double(a.motion.x),(double(a.motion.y) + double(130.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
          a.motion.velocityX = (float)(int)i16(state.actionTicks) * double(0.01f);
          a.motion.velocityY = (float)(int)i16(state.actionStage) * double(0.01f);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x15) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          step = random.range(0,100);
          a.launchParameters[0] = (float)(step + -0x46);
          step = random.range(0,10);
          a.launchParameters[1] = (float)(step + 5);
          a.set_polar_velocity(double(a.launchParameters[0]),100.0f);
          owned(0x38f,(double(a.motion.velocityX) + double(60.0f)) * (float)(int)(i8)u8(a.motion.facing) +
                             double(a.motion.x),(double(a.motion.y) + double(130.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
          a.motion.velocityX = (float)(int)i16(state.actionTicks) * double(0.01f);
          a.motion.velocityY = (float)(int)i16(state.actionStage) * double(0.01f);
          events.sound(7);
        }
        if ((i16(a.animation.cursor.index) == 0x17) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionTicks = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.actionStage = impactWord;
          step = random.range(0,100);
          a.launchParameters[0] = (float)(step + -0x46);
          step = random.range(0,10);
          a.launchParameters[1] = (float)(step + 5);
          a.set_polar_velocity(double(a.launchParameters[0]),100.0f);
          owned(0x38f,(double(a.motion.velocityX) + double(60.0f)) * (float)(int)(i8)u8(a.motion.facing) +
                             double(a.motion.x),(double(a.motion.y) + double(130.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
          a.motion.velocityX = (float)(int)i16(state.actionTicks) * double(0.01f);
          a.motion.velocityY = (float)(int)i16(state.actionStage) * double(0.01f);
          events.sound(7);
          if ((int)i16(state.emissionDelay) < state.difficulty * 2 + 8) {
            state.emissionDelay = i16(state.emissionDelay) + 1;
            selectFrame(0xf);
          }
        }
        if (i16(a.animation.cursor.index) != 0x22) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x20);
        return;
      case 0x2f9:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          state.actionStage = 0;
          impactWord = random.range(0,3);
          state.emissionDelay = impactWord;
          impactWord = random.range(0,100);
          state.volleyLoops = impactWord;
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 99;
        }
        if ((i16(a.animation.cursor.elapsed) == 0) &&
           ((i16(a.animation.cursor.index) == 9 || (i16(a.animation.cursor.index) == 10)))) {
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          step = random.range(0,200);
          effectY = (float)(int)(step + (u32)(0x32 < i16(state.volleyLoops)) * 200);
          position = double(a.motion.x);
          step = random.range(0,0x1e0);
          owned(0x38b,double(float(double(position)+240.0))-step,effectY,direction,layer);
          if ('\0' < state.difficulty) {
            layer = 1;
            direction = (u32)(u8)u8(a.motion.facing);
            step = random.range(0,0xfa);
            effectY = (float)(int)(step + (u32)(0x32 < i16(state.volleyLoops)) * 0xfa);
            position = double(a.motion.x);
            step = random.range(0,0x230);
            owned(0x38b,double(float(double(position)+280.0))-step,effectY,direction,layer);
          }
          if (((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) &&
             ('\x01' < state.difficulty)) {
            uVar10 = 1;
            uVar9 = 0;
            layer = 1;
            direction = (u32)(u8)u8(a.motion.facing);
            step = random.range(0,200);
            effectY = (float)(int)(step + (u32)(0x32 < i16(state.volleyLoops)) * 200);
            position = double(a.motion.x);
            step = random.range(0,0x1e0);
            ownedVisual(0x38a,double(float(double(position)+240.0))-step,effectY,direction,layer,uVar9,uVar10);
          }
          if (((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) &&
             ('\x02' < state.difficulty)) {
            uVar10 = 1;
            uVar9 = 0;
            layer = 1;
            direction = (u32)(u8)u8(a.motion.facing);
            step = random.range(0,0xfa);
            effectY = (float)(int)(step + (u32)(0x32 < i16(state.volleyLoops)) * 0xfa);
            position = double(a.motion.x);
            step = random.range(0,0x230);
            ownedVisual(0x38a,double(float(double(position)+280.0))-step,effectY,direction,layer,uVar9,uVar10);
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          a.commandTimers[2] = state.difficulty * -0x14 + 0xb4;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.actionStage) < 3) {
            state.actionStage = i16(state.actionStage) + 1;
            selectFrame(3);
            return;
          }
          events.sound(9);
          hitFreeze(0x4b);
          events.system_sound(0x17);
          ownedVisual(0x330,640.0f,320.0f,1,2,0,1);
          state.timeReserve = 1;
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.flags = 0x82;
        }
        if ((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.actionTicks) < 10) {
            state.actionTicks = i16(state.actionTicks) + 1;
            selectFrame(9);
          }
          else {
            state.timeReserve = 0;
            other.pose.flags = 0;
            state.actionTicks = 0;
          }
        }
        if (i16(a.animation.cursor.index) != 0xd) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (9 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(0xb);
        return;
      case 0x2fa:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
          state.emissionDelay = 0;
          state.volleyLoops = 0;
          state.burstCount = state.difficulty + 2;
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(0xf);
          a.motion.clear();
          a.motion.y = 0.0f;
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        if ((3 < i16(a.animation.cursor.index)) &&
           ((i16(a.animation.cursor.index) < 0xb && (8 < i16(state.actionTicks))))) {
          state.actionTicks = 0;
          a.launchParameters[0] = -45.0f;
          owned(0x38e,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          events.sound(0xc);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          advancedFrame = random.range(0x3c,0x78);
          a.commandTimers[2] = advancedFrame + state.difficulty * -0x14;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 16.0f;
          a.motion.accelerationY = 0.5f;
          step = random.range(0,6);
          a.motion.velocityX = (float)(step + 0xc);
          if (((u8(a.motion.facing) == (u8)0x1) && (double(1180.0f) < double(a.motion.x))) ||
             ((u8(a.motion.facing) == (u8)0xff && (double(a.motion.x) < double(100.0f))))) {
            a.motion.velocityX = -double(a.motion.velocityX);
          }
        }
        if (((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionStage) < 1)) {
          state.actionStage = i16(state.actionStage) + 1;
          selectFrame(4);
        }
        if (i16(a.animation.cursor.index) != 0xf) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xd);
        return;
      case 0x2fb:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.response = 1;
          a.carryMomentum = (u8)0x0;
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if ((i16(a.animation.cursor.index) == 6) || (i16(a.animation.cursor.index) == 7)) {
          if ((u8)u8(a.pose.channel(24)) < 0x15) {
            a.pose.set_channel(24,u8((u8)0x0));
          }
          else {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + -0x14)));
          }
        }
        if ((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xd)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if ((u8)u8(a.pose.channel(24)) < 0xeb) {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + '\x14')));
          }
          else {
            a.pose.set_channel(24,u8((u8)0xff));
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.pose.set_channel(24,u8((u8)0xff));
            a.motion.clear();
            selectFrame(0xd);
            a.motion.y = 0.0f;
            return;
          }
        }
        if ((i16(a.animation.cursor.index) < 8) && (1 < i16(a.animation.cursor.index))) {
          uVar11 = 1;
          uVar10 = 0;
          uVar9 = 1;
          layer = 1;
          position = double(a.motion.y);
          step = random.range(0,0x5a);
          position = (float)step + (position - double(15.0f));
          step = random.range(0,0x78);
          ownedVisual(0x33e,((float)step + double(a.motion.x)) - double(60.0f),position,layer,uVar9,uVar10,uVar11);
        }
        if (((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          state.actionTicks = i16(state.actionTicks) + 1;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x2f9);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
          ownedVisual(0x338,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x339,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33a,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33b,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33c,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          ownedVisual(0x33d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 7),double(a.motion.y) + double(131.0f),1,1,0,1);
          consumeSpirit(200);
          a.begin_pose(0.0f,0.0f,0.0f);
          a.pose.set_channel(24,u8((u8)0xff));
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionStage) < 4)) {
          selectFrame(6);
          state.actionStage = i16(state.actionStage) + 1;
          return;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          step = random.range(0,0x1e0);
          a.motion.velocityX = (float)(step + -0xf0);
          if (double(a.motion.x) < double(320.0f)) {
            step = random.range(0,0xb4);
            a.motion.velocityX = (float)((step + 0x5a) * (int)(i8)u8(a.motion.facing));
          }
          if (double(960.0f) < double(a.motion.x)) {
            step = random.range(0,0xb4);
            a.motion.velocityX = (float)(-(step + 0x5a) * (int)(i8)u8(a.motion.facing));
          }
          a.motion.velocityY = 3.0f;
          a.motion.accelerationY = 0.5f;
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (double(a.motion.x) <= double(other.motion.x)) {
            a.motion.facing = (u8)0x1;
          }
          else {
            a.motion.facing = (u8)0xff;
          }
        }
        if (i16(a.animation.cursor.index) != 0xd) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xb);
        return;
      case 0x302:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
          state.emissionDelay = 0;
          state.burstCount = state.difficulty + 2;
        }
        if (((i16(a.animation.cursor.index) < 0xd) && (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) < double(1.0f))) {
          a.motion.velocityY = 1.0f;
        }
        if (((0xc < i16(a.animation.cursor.index)) && (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(0xf);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          advancedFrame = random.range(0,0x3c);
          a.commandTimers[2] = advancedFrame + 0xb4 + state.difficulty * -0x14;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 13.5f;
          a.motion.accelerationY = 0.5f;
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(5);
        if (state.difficulty == '\0') {
          for (local_58 = 0; local_58 < 10; local_58 = local_58 + 1) {
            a.launchParameters[0] = (float)(local_58 * 0x24);
            a.launchParameters[1] = 4.0f;
            owned(0x389,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 4),double(a.motion.y) + double(136.0f),u8(a.motion.facing),1);
          }
        }
        if (state.difficulty == '\x01') {
          for (local_5c = 0; local_5c < 0xc; local_5c = local_5c + 1) {
            a.launchParameters[0] = (float)(local_5c * 0x1e);
            a.launchParameters[1] = 4.0f;
            owned(0x389,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 4),double(a.motion.y) + double(136.0f),u8(a.motion.facing),1);
          }
        }
        if (state.difficulty == '\x02') {
          for (local_60 = 0; local_60 < 0xf; local_60 = local_60 + 1) {
            a.launchParameters[0] = (float)(local_60 * 0x18);
            a.launchParameters[1] = 4.0f;
            owned(0x389,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 4),double(a.motion.y) + double(136.0f),u8(a.motion.facing),1);
          }
        }
        if (state.difficulty != '\x03') {
          return;
        }
        for (local_64 = 0; local_64 < 0x14; local_64 = local_64 + 1) {
          a.launchParameters[0] = (float)(local_64 * 0x12);
          a.launchParameters[1] = 4.0f;
          owned(0x389,double(a.motion.x) - (float)((int)(i8)u8(a.motion.facing) << 4),double(a.motion.y) + double(136.0f),u8(a.motion.facing),1);
        }
        return;
      case 0x303:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
          state.emissionDelay = 0;
          step = random.range(0,100);
          if (step < 0x33) {
            state.volleyLoops = 0xfffb;
          }
          else {
            state.volleyLoops = 5;
          }
        }
        if (((i16(a.animation.cursor.index) < 9) && (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) < double(1.0f))) {
          a.motion.velocityY = 1.0f;
        }
        if (((8 < i16(a.animation.cursor.index)) && (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(0xb);
        }
        if (((i16(a.animation.cursor.index) == 7) || (i16(a.animation.cursor.index) == 8)) &&
           ((i16(a.animation.cursor.elapsed) == 0 &&
            (state.actionTicks = i16(state.actionTicks) + 1,
            (int)i16(state.actionTicks) < state.difficulty * 5 + 10)))) {
          events.sound(0);
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.actionStage = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.burstCount = impactWord;
          a.launchParameters[0] = (float)(int)i16(state.emissionDelay);
          a.launchParameters[1] = 10.0f;
          a.set_polar_velocity((float)(int)i16(state.emissionDelay),100.0f);
          owned(0x388,(float)(int)(i8)u8(a.motion.facing) * double(a.motion.velocityX) +
                             double(a.motion.x),(double(a.motion.y) + double(140.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
          a.motion.velocityX = (float)(int)i16(state.actionStage) * double(0.01f);
          a.motion.velocityY = (float)(int)i16(state.burstCount) * double(0.01f);
          state.emissionDelay = i16(state.emissionDelay) + i16(state.volleyLoops);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          advancedFrame = random.range(0x3c,0x78);
          a.commandTimers[2] = advancedFrame + state.difficulty * -0x14;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 6.5f;
          a.motion.accelerationY = 0.25f;
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          aimRadians = atan(-(((double(other.motion.y) + double(100.0f)) -
                         double(a.motion.y)) + double(140.0f)),(double(other.motion.x) - double(a.motion.x)) *
                       (float)(int)(i8)u8(a.motion.facing));
          impactWord = i16(i32(aimRadians/double(0.017453289f)));
          state.emissionDelay = impactWord;
        }
        if (i16(a.animation.cursor.index) != 9) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (0x18 < i16(state.actionTicks)) {
          return;
        }
        selectFrame(7);
        return;
      case 0x31b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          fade(0);
          a.begin_spell(mode,replayMode,core);
          events.system_sound(0x17);
          owned(0x3e5,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (7 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(9);
        return;
      case 0x31c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 0x10) && (i16(a.animation.cursor.elapsed) == 0)) {
          fade(0);
          a.begin_spell(mode,replayMode,core);
          events.system_sound(0x17);
        }
        if (i16(a.animation.cursor.index) != 0x15) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (7 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(0x13);
        a.transitionReady = (u8)0x1;
        return;
      }
    }
switchD_004ac09b_default:
    advancedFrame = a.animation.advance();
    if (advancedFrame != i16(a.animation.cursor.frameCount)) {
      return;
    }
    a.select_action(0);
    return;
  }
  if (step == 0x32) {
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
    boundary = a.motion.horizontal_limit(other.motion);
    if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
      a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
    }
    advancedFrame = a.animation.advance();
    if (advancedFrame != i16(a.animation.cursor.frameCount)) {
      return;
    }
    a.select_action(0);
    return;
  }
  switch(step) {
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
    goto LAB_004c039f;
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
    advancedFrame = a.animation.advance();
    if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
    goto LAB_004c039f;
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
    advancedFrame = a.animation.advance();
    if (advancedFrame == i16(a.animation.cursor.frameCount)) {
      a.select_action(0);
    }
    break;
  case 4:
    a.motion.velocityX = 6.0f;
    goto LAB_004c039f;
  case 5:
    a.motion.velocityX = -6.0f;
LAB_004c039f:
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
            a.motion.velocityY = 15.0f;
            a.motion.accelerationY = 0.699999988079071f;
            a.carryMomentum = (u8)0x0;
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
            a.carryMomentum = (u8)0x0;
            a.motion.velocityX = 5.0f;
            a.motion.velocityY = 15.0f;
            a.motion.accelerationY = 0.699999988079071f;
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
            a.carryMomentum = (u8)0x0;
            a.motion.velocityX = -4.0f;
            a.motion.velocityY = 15.0f;
            a.motion.accelerationY = 0.699999988079071f;
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
      advancedFrame = a.animation.advance();
      if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
    advancedFrame = a.animation.advance();
    if (advancedFrame == i16(a.animation.cursor.frameCount)) {
      if (a.controls.current.vertical == 0) {
        a.select_action(0);
      }
      else {
        a.select_action(1);
      }
    }
    break;
  default:
    goto switchD_004ac09b_default;
  }
  return;

}
}
