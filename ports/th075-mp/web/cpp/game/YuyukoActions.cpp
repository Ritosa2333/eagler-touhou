#include "YuyukoState.hpp"
#include <cmath>
namespace th075 {
void advance_yuyuko_action(FighterActor& a,FighterActor& other,YuyukoState& state,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){

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
  int local_88;
  int local_84;
  int local_80;
  int local_7c;
  int local_78;
  int local_74;
  int local_70;
  int local_6c;
  int local_68;
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
          state.impactX = 0;
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
        if (4 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') && (u8(a.allowPush) != (u8)0x0)) {
          a.motion.opponentPush = double(a.motion.velocityX) * double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.impactX = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = impactWord;
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
        selectFrame(6);
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
          state.impactX = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = impactWord;
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
        selectFrame(6);
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
          state.impactX = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = impactWord;
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
        selectFrame(6);
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
          state.impactX = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = impactWord;
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
          state.impactX = 0;
          return;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          events.system_sound(0x16);
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.impactX = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = impactWord;
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
        selectFrame(6);
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
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.impactX = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = impactWord;
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
          state.impactX = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = impactWord;
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
            state.impactX = impactWord;
            impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
            state.impactY = impactWord;
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
        a.motion.velocityX = (float)(int)i16(state.impactX) * double(0.0025f);
        a.motion.velocityY = (float)-(int)i16(state.impactY) * double(0.01f);
        a.motion.accelerationY = 0.699999988079071f;
        a.begin_pose(0.0f,95.0f,0.0f);
        return;
      case 0x5d:
        if (((i16(a.animation.cursor.index) < 7) && (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(7);
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
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 8.0f;
          a.motion.velocityX = 13.5f;
          a.motion.accelerationY = 0.6000000238418579f;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(5);
        return;
      case 0x5e:
        if (((i16(a.animation.cursor.index) < 7) && (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(7);
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
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 8.0f;
          a.motion.velocityX = -11.5f;
          a.motion.accelerationY = 0.5f;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(5);
        return;
      case 0x5f:
        advancedFrame = a.animation.advance();
        if ((advancedFrame == i16(a.animation.cursor.frameCount)) &&
           (a.select_action(3), 0 < a.controls.current.vertical)) {
          a.select_action(2);
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
        if ((((i16(a.animation.cursor.index) == 2) || (i16(a.animation.cursor.index) == 3)) ||
            (i16(a.animation.cursor.index) == 4)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(5);
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
        if ((((i16(a.animation.cursor.index) == 2) || (i16(a.animation.cursor.index) == 3)) ||
            (i16(a.animation.cursor.index) == 4)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(5);
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
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
            if (a.phaseVariants[a.combat.spellPhase] != (u8)0x4) {
              return;
            }
            a.select_action(0x302);
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
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x34c,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
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
      switch(step) {
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
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.impactX = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = impactWord;
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
        selectFrame(6);
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
        selectFrame(7);
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
        selectFrame(7);
        return;
      case 200:
        a.carryMomentum = (u8)0x1;
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          state.impactX = 0;
        }
        if ((double(5.0f) < double(a.motion.velocityX)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
           double(a.motion.velocityX) < double(6.0f))) {
          a.motion.velocityX = 6.0f;
        }
        if (a.controls.current.vertical < 0) {
          if (0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) {
            a.select_action(0xcd);
            return;
          }
          a.select_action(0xcc);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          if ((0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) &&
             (i16(state.impactX) < 5)) {
            selectFrame(2);
            state.impactX = i16(state.impactX) + 1;
            return;
          }
          a.select_action(0xcf);
          return;
        }
        if (i16(a.animation.cursor.index) != 1) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 15.0f;
        return;
      case 0xc9:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -10.0f;
          a.motion.velocityY = 2.0f;
          a.motion.accelerationY = 0.20000000298023224f;
        }
        if ((((double(0.0f) < double(a.motion.y)) && (0 < i16(a.animation.cursor.index))) &&
            (i16(a.animation.cursor.index) < 5)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
           (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f)))) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.y = 0.0f;
          a.animation.cursor.elapsed = 0;
          selectFrame(5);
          return;
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
        selectFrame(i16(a.animation.cursor.index) + -2);
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
          return;
        }
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 7.5f;
        a.motion.velocityY = 3.5f;
        a.motion.accelerationY = 0.44999998807907104f;
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -7.5f;
          a.motion.velocityY = 3.5f;
          a.motion.accelerationY = 0.44999998807907104f;
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (0 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(3);
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
          a.motion.velocityY = 18.5f;
          a.motion.accelerationY = 0.550000011920929f;
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
          a.motion.velocityX = 10.0f;
          a.motion.velocityY = 13.5f;
          a.motion.accelerationY = 0.5f;
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
          a.motion.velocityX = -10.0f;
          a.motion.velocityY = 13.5f;
          a.motion.accelerationY = 0.5f;
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
      }
    }
    else if (step < 0x187) {
      if (step == 0x186) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 3;
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
        if ((i16(a.animation.cursor.elapsed) == 0) && (i16(a.animation.cursor.index) < 7)) {
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x334,(float)((i8)u8(a.motion.facing) * 0x92) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
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
        if (((3 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xf)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          a.motion.y = 0.0f;
          selectFrame(0xf);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.impactX = 0;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 5.0f;
          a.motion.velocityY = 10.0f;
          a.motion.accelerationY = 0.800000011920929f;
          events.sound(0xc);
          owned(0x32f,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 0xf) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xe);
        return;
      case 0x12e:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 2;
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
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1b);
        }
        if (i16(a.animation.cursor.index) != 1) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x34d,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
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
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1d);
        }
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x34f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
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
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x327,(float)((i8)u8(a.motion.facing) * 3) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        events.sound(0xc);
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
          a.remainingHits = 4;
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.elapsed) == 0) && (i16(a.animation.cursor.index) < 9)) {
          a.response = 0;
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
        owned(0x34e,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x133:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.remainingHits = 1;
          a.response = 0;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          events.sound(0xc);
          owned(0x32e,(float)((i8)u8(a.motion.facing) * 0x14) + double(a.motion.x),125.0f,u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 20.0f;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 20.0f;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 20.0f;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 20.0f;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 20.0f;
        return;
      case 0x134:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
        }
        if ((('\0' < (i8)u8(a.carryMomentum)) && (i16(a.animation.cursor.index) < 2)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.3f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -15.0f;
        }
        if (((double(a.motion.velocityX) < double(0.0f)) && (1 < i16(a.animation.cursor.index))) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.8f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.impactX = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -50.0f;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1d);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 0.0f;
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.impactX = 0;
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x349,(float)((i8)u8(a.motion.facing) * 0x14) + double(a.motion.x),double(a.motion.y) + double(40.0f),u8(a.motion.facing),1);
        events.sound(0xc);
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
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x335,(float)((i8)u8(a.motion.facing) * 0x14) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          events.sound(0xc);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 27.0f;
        return;
      }
    }
    else if (step < 0x1f5) {
      if (step == 500) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
          state.aimAngle = 0;
        }
        if (((double(0.0f) < double(a.motion.velocityX)) && (5 < i16(a.animation.cursor.index))) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.55f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if ((5 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 10)) {
          state.aimAngle = i16(state.aimAngle) + -1;
          advancedFrame = i16(state.aimAngle);
          if (advancedFrame < 1) {
            state.aimAngle = 3;
            owned(0x343,(float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x),double(a.motion.y) + double(124.0f),u8(a.motion.facing),1);
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          a.motion.velocityX = 8.0f;
          events.sound(0xb);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0);
        }
        if (i16(a.animation.cursor.index) != 10) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (-1 < i16(state.impactX)) {
          return;
        }
        selectFrame(6);
        state.impactX = i16(state.impactX) + 1;
        return;
      }
      switch(step) {
      case 0x187:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = 10.0f;
        }
        if (((0 < i16(a.animation.cursor.index)) && (double(a.motion.y) == double(0.0f))) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        if (((3 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xf)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          a.motion.y = 0.0f;
          selectFrame(0xf);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.impactX = 0;
        }
        if ((i16(a.animation.cursor.elapsed) == 0) && (i16(a.animation.cursor.index) < 10)) {
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 8.0f;
          a.motion.velocityY = 10.0f;
          a.motion.accelerationY = 0.800000011920929f;
          owned(0x32f,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          events.sound(0xc);
        }
        if (i16(a.animation.cursor.index) != 0xf) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xe);
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
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x335,(float)((i8)u8(a.motion.facing) * 0x14) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        events.sound(0xc);
        return;
      case 0x189:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.carryMomentum = (u8)0x0;
          a.remainingHits = 1;
          a.response = 0;
          a.motion.accelerationY = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 10.0f;
        }
        if (double(0.0f) < double(a.motion.velocityX)) {
          a.motion.velocityX = double(a.motion.velocityX) - double(0.3f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(2);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1d);
        }
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x34f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
          state.impactY = 0;
        }
        state.impactX = i16(state.impactX) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) < 8) {
          return;
        }
        if (0 < i16(state.impactX)) {
          return;
        }
        if (2 < i16(state.impactY)) {
          return;
        }
        consumeSpirit(100);
        events.sound(0);
        state.impactX = 0xf;
        state.impactY = i16(state.impactY) + 1;
        for (volley = 0; volley < 5; volley = volley + 1) {
          a.launchParameters[1] = 1.0f;
          step = volley * -0x14 + 0x28;
          a.launchParameters[0] = (float)step;
          owned(800,(float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x),double(a.motion.y) + double(140.0f),u8(a.motion.facing),1);
        }
        return;
      case 0x191:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
          state.impactY = 0;
        }
        state.impactX = i16(state.impactX) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        consumeSpirit(0xfa);
        events.sound(0);
        for (emission = 0; emission < 8; emission = emission + 1) {
          step = emission * 2 + 2;
          a.launchParameters[1] = (float)step;
          owned(0x33e,(float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x),double(a.motion.y) + double(140.0f),u8(a.motion.facing),1);
        }
        return;
      case 0x192:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.response = 1;
          state.impactX = 0;
          state.impactY = 0;
        }
        if (((3 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xf)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          a.motion.y = 0.0f;
          selectFrame(0xf);
          return;
        }
        if (((3 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
           (state.impactY = i16(state.impactY) + -1,
           i16(state.impactY) < 1)) {
          a.launchParameters[1] = 1.0f;
          a.launchParameters[0] = 0.0f;
          events.sound(0);
          consumeSpirit(0x28);
          owned(800,(float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          state.impactY = 4;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.impactX = 0;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 10.0f;
          a.motion.accelerationY = 0.30000001192092896f;
        }
        if (((i16(a.animation.cursor.index) == 0xc) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 1)) {
          state.impactX = i16(state.impactX) + 1;
          selectFrame(5);
        }
        if (i16(a.animation.cursor.index) != 0xf) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xe);
        return;
      case 0x194:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          state.impactX = 0;
          state.impactY = 0;
        }
        if (0x10 < i16(a.animation.cursor.index)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        }
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 0.0f;
        }
        state.impactX = i16(state.impactX) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) < 8) {
          return;
        }
        if (0 < i16(state.impactX)) {
          return;
        }
        if (2 < i16(state.impactY)) {
          return;
        }
        events.sound(0);
        state.impactX = 0xf;
        state.impactY = i16(state.impactY) + 1;
        consumeSpirit(100);
        for (local_1c = 0; local_1c < 5; local_1c = local_1c + 1) {
          a.launchParameters[1] = 1.0f;
          step = local_1c * -0x14 + 0x28;
          a.launchParameters[0] = (float)step;
          owned(0x322,(float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x),double(a.motion.y) + double(140.0f),u8(a.motion.facing),1);
        }
        return;
      case 0x196:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
          state.impactY = 0;
        }
        state.impactX = i16(state.impactX) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(10);
        }
        if (i16(a.animation.cursor.index) < 6) {
          return;
        }
        if (0 < i16(state.impactX)) {
          return;
        }
        if (4 < i16(state.impactY)) {
          return;
        }
        a.launchParameters[0] = (float)(i16(state.impactY) * -10 + 10);
        a.launchParameters[1] = 10.0f;
        owned(0x342,(float)((i8)u8(a.motion.facing) * 0x69) + double(a.motion.x),double(a.motion.y) + double(145.0f),u8(a.motion.facing),1);
        consumeSpirit(0x1e);
        state.impactX = 5;
        state.impactY = i16(state.impactY) + 1;
        return;
      case 0x197:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
          state.impactY = 0;
        }
        state.impactX = i16(state.impactX) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(10);
        }
        if (i16(a.animation.cursor.index) < 6) {
          return;
        }
        if (0 < i16(state.impactX)) {
          return;
        }
        if (4 < i16(state.impactY)) {
          return;
        }
        a.launchParameters[0] = (float)(i16(state.impactY) * 10 + 10);
        a.launchParameters[1] = 10.0f;
        owned(0x342,(float)((i8)u8(a.motion.facing) * 0x41) + double(a.motion.x),double(a.motion.y) + double(170.0f),u8(a.motion.facing),1);
        consumeSpirit(0x1e);
        state.impactX = 5;
        state.impactY = i16(state.impactY) + 1;
        return;
      case 0x198:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
          state.impactY = 0;
        }
        state.impactX = i16(state.impactX) + -1;
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(10);
        }
        if (i16(a.animation.cursor.index) < 6) {
          return;
        }
        if (0 < i16(state.impactX)) {
          return;
        }
        if (4 < i16(state.impactY)) {
          return;
        }
        a.launchParameters[0] = (float)(i16(state.impactY) * 10 + 0x1e);
        a.launchParameters[1] = 10.0f;
        owned(0x342,(float)((i8)u8(a.motion.facing) * 0x41) + double(a.motion.x),double(a.motion.y) + double(170.0f),u8(a.motion.facing),1);
        consumeSpirit(0x1e);
        state.impactX = 5;
        state.impactY = i16(state.impactY) + 1;
        return;
      case 0x19a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.2f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.2f);
          state.impactX = 0;
          state.impactY = 0;
          state.aimAngle = 0;
          state.emissionDelay = 0;
        }
        if ((7 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 0.0f;
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
        events.sound(10);
        impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
        state.impactX = impactWord;
        impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
        state.impactY = impactWord;
        consumeSpirit(0x96);
        for (local_20 = 0; local_20 < 7; local_20 = local_20 + 1) {
          a.launchParameters[0] = (float)(local_20 * -10 + 0x1e);
          a.launchParameters[1] = 10.0f;
          a.set_polar_velocity(double(a.launchParameters[0]),80.0f);
          owned(0x342,(double(a.motion.velocityX) + double(5.0f)) * (float)(int)(i8)u8(a.motion.facing) +
                             double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
        }
        a.motion.velocityX = (float)(int)i16(state.impactX) * double(0.01f);
        a.motion.velocityY = (float)(int)i16(state.impactY) * double(0.01f);
        return;
      case 0x19c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
          state.impactY = 0;
        }
        state.impactX = i16(state.impactX) + -1;
        if (((7 < i16(a.animation.cursor.index)) && (i16(state.impactX) < 1)) &&
           (i16(state.impactY) < 5)) {
          owned(0x347,(float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x),double(a.motion.y) + double(140.0f),u8(a.motion.facing),1);
          state.impactY = i16(state.impactY) + 1;
          state.impactX = 10;
          consumeSpirit(0x1e);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(10);
        return;
      case 0x19e:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
        }
        if ((('\0' < (i8)u8(a.carryMomentum)) && (i16(a.animation.cursor.index) < 2)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.3f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -15.0f;
        }
        if (((double(a.motion.velocityX) < double(0.0f)) && (1 < i16(a.animation.cursor.index))) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.8f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.impactX = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -50.0f;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0);
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.impactX = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.impactY = impactWord;
          consumeSpirit(300);
          for (local_24 = 0; local_24 < 5; local_24 = local_24 + 1) {
            a.launchParameters[0] = (float)(local_24 * 0xf + 10);
            a.launchParameters[1] = 2.0f;
            a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
            owned(800,(double(a.motion.velocityX) + -double(60.0f)) * (float)(int)(i8)u8(a.motion.facing)
                             + double(a.motion.x),double(a.motion.y) - double(a.motion.velocityY),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_24 * 0xf + 10);
            a.launchParameters[1] = 6.0f;
            a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
            owned(0x321,(double(a.motion.velocityX) + -double(60.0f)) *
                               (float)(int)(i8)u8(a.motion.facing) + double(a.motion.x),double(a.motion.y) - double(a.motion.velocityY),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_24 * 0xf + 10);
            a.launchParameters[1] = 10.0f;
            a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
            owned(0x322,(double(a.motion.velocityX) + -double(60.0f)) *
                               (float)(int)(i8)u8(a.motion.facing) + double(a.motion.x),double(a.motion.y) - double(a.motion.velocityY),u8(a.motion.facing),1);
          }
          a.motion.velocityX = (float)(int)i16(state.impactX) * double(0.01f);
          a.motion.velocityY = (float)(int)i16(state.impactY) * double(0.01f);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 0.0f;
        return;
      case 0x1a0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          state.impactX = 0;
          state.impactY = 0;
        }
        if (0x10 < i16(a.animation.cursor.index)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        }
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 0.0f;
        }
        state.impactX = i16(state.impactX) + -1;
        if (((7 < i16(a.animation.cursor.index)) && (i16(state.impactX) < 1)) &&
           (i16(state.impactY) < 5)) {
          owned(0x347,(float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x),double(a.motion.y) + double(140.0f),u8(a.motion.facing),1);
          state.impactY = i16(state.impactY) + 1;
          state.impactX = 10;
          consumeSpirit(0x1e);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(10);
        return;
      }
    }
    else if (step < 0x2bd) {
      if (step == 700) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x73,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(500);
          events.sound(4);
          for (local_40 = 0; local_40 < 0xc; local_40 = local_40 + 1) {
            a.launchParameters[0] = (float)(local_40 * 0x1e);
            a.launchParameters[1] = 8.0f;
            a.launchParameters[2] = 30.0f;
            owned(0x323,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_40 * -0x1e);
            a.launchParameters[1] = 8.0f;
            a.launchParameters[2] = -30.0f;
            owned(0x323,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_40 * 0x1e);
            a.launchParameters[1] = 7.0f;
            a.launchParameters[2] = 35.0f;
            owned(0x323,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_40 * -0x1e);
            a.launchParameters[1] = 7.0f;
            a.launchParameters[2] = -35.0f;
            owned(0x323,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
          }
        }
        if (((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 2)) {
          state.impactX = i16(state.impactX) + 1;
          selectFrame(8);
        }
        if (i16(a.animation.cursor.index) != 0xc) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (1 < i16(state.impactY)) {
          return;
        }
        state.impactY = i16(state.impactY) + 1;
        selectFrame(10);
        return;
      }
      switch(step) {
      case 0x1f5:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.impactX = 0;
          state.impactY = 0;
          state.aimAngle = 0;
        }
        if (((double(0.0f) < double(a.motion.velocityX)) && (5 < i16(a.animation.cursor.index))) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.45f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if ((5 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 10)) {
          state.aimAngle = i16(state.aimAngle) + -1;
          advancedFrame = i16(state.aimAngle);
          if (advancedFrame < 1) {
            state.aimAngle = 3;
            owned(0x343,(float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x),double(a.motion.y) + double(124.0f),u8(a.motion.facing),1);
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          a.motion.velocityX = 12.0f;
          events.sound(0xb);
        }
        if (((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactY) < 2)) {
          state.impactY = i16(state.impactY) + 1;
          selectFrame(2);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0);
        }
        if (i16(a.animation.cursor.index) != 10) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (0 < i16(state.impactX)) {
          return;
        }
        selectFrame(6);
        state.impactX = i16(state.impactX) + 1;
        return;
      case 0x252:
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
          state.impactX = 0;
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
          events.sound(1);
          consumeSpirit(0xfa);
          for (local_28 = 0; local_28 < 5; local_28 = local_28 + 1) {
            a.launchParameters[0] = (float)(local_28 * 0x14);
            a.launchParameters[1] = 5.0f;
            owned(0x32c,(float)((i8)u8(a.motion.facing) * 0x2b) + double(a.motion.x),double(a.motion.y) + double(170.0f),u8(a.motion.facing),1);
          }
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
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0xfa);
          events.sound(1);
          for (local_2c = 0; local_2c < 5; local_2c = local_2c + 1) {
            a.launchParameters[0] = (float)(local_2c * 0x14 + 0x32);
            a.launchParameters[1] = 5.0f;
            owned(0x330,(float)((i8)u8(a.motion.facing) * 0x2b) + double(a.motion.x),double(a.motion.y) + double(170.0f),u8(a.motion.facing),1);
          }
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
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(1);
          consumeSpirit(0xfa);
          for (local_30 = 0; local_30 < 5; local_30 = local_30 + 1) {
            a.launchParameters[0] = (float)(local_30 * 0x14 + 0x6e);
            a.launchParameters[1] = 5.0f;
            owned(0x331,(float)((i8)u8(a.motion.facing) * 0x2b) + double(a.motion.x),double(a.motion.y) + double(170.0f),u8(a.motion.facing),1);
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x25f:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(200);
          owned(0x336,(float)((i8)u8(a.motion.facing) * 0x69) + double(a.motion.x),double(a.motion.y) + double(145.0f),u8(a.motion.facing),1);
          events.sound(2);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x260:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0xfa);
          owned(0x339,(float)((i8)u8(a.motion.facing) * 0x69) + double(a.motion.x),double(a.motion.y) + double(145.0f),u8(a.motion.facing),1);
          events.sound(2);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x261:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.2f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.2f);
        }
        if (0xf < i16(a.animation.cursor.index)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(200);
          owned(0x336,(float)((i8)u8(a.motion.facing) * 0x55) + double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
          events.sound(2);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x262:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.2f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.2f);
        }
        if (0xf < i16(a.animation.cursor.index)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0xfa);
          owned(0x339,(float)((i8)u8(a.motion.facing) * 0x55) + double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
          events.sound(2);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x263:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 0;
          a.remainingHits = 8;
          a.motion.clear();
          state.impactX = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(200);
          for (local_34 = 0; local_34 < 6; local_34 = local_34 + 1) {
            a.launchParameters[1] =
                 (float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x);
            a.launchParameters[2] = double(a.motion.y) + double(165.0f);
            a.launchParameters[0] = (float)(local_34 * 0x3c);
            owned(0x33f,(float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x),double(a.motion.y) + double(165.0f),u8(a.motion.facing),1);
          }
          events.sound(3);
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.impactX) < 3) {
            state.impactX = i16(state.impactX) + 1;
            selectFrame(7);
            return;
          }
          a.response = 1;
        }
        if (((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 2)) {
          state.impactX = i16(state.impactX) + 1;
          selectFrame(9);
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (8 < i16(a.animation.cursor.index)) {
          return;
        }
        a.response = 0;
        return;
      case 0x264:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 0;
          a.remainingHits = 10;
          a.motion.clear();
          state.impactX = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(200);
          for (local_38 = 0; local_38 < 8; local_38 = local_38 + 1) {
            a.launchParameters[1] =
                 (float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x);
            a.launchParameters[2] = double(a.motion.y) + double(165.0f);
            a.launchParameters[0] = (float)(local_38 * 0x2d);
            owned(0x346,(float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x),double(a.motion.y) + double(165.0f),u8(a.motion.facing),1);
          }
          events.sound(3);
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.impactX) < 5) {
            state.impactX = i16(state.impactX) + 1;
            selectFrame(7);
            return;
          }
          a.response = 1;
        }
        if (((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 4)) {
          state.impactX = i16(state.impactX) + 1;
          selectFrame(9);
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (8 < i16(a.animation.cursor.index)) {
          return;
        }
        a.response = 0;
        return;
      case 0x265:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.remainingHits = 10;
          a.remainingHits = 0;
          a.motion.clear();
          state.impactX = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(200);
          for (local_3c = 0; local_3c < 8; local_3c = local_3c + 1) {
            a.launchParameters[0] = (float)(local_3c * 0x2d);
            a.launchParameters[1] =
                 (float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x);
            a.launchParameters[2] = double(a.motion.y) + double(165.0f);
            owned(0x346,(float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x),double(a.motion.y) + double(165.0f),u8(a.motion.facing),1);
          }
          events.sound(3);
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.impactX) < 1) {
            state.impactX = i16(state.impactX) + 1;
            selectFrame(7);
            return;
          }
          a.response = 1;
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (6 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(9);
        return;
      }
    }
    else {
      switch(step) {
      case 0x2bd:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x73,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          for (local_44 = 0; local_44 < 0xc; local_44 = local_44 + 1) {
            a.launchParameters[0] = (float)(local_44 * 0x1e);
            a.launchParameters[1] = 8.0f;
            a.launchParameters[2] = 30.0f;
            owned(0x323,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_44 * -0x1e);
            a.launchParameters[1] = 8.0f;
            a.launchParameters[2] = -30.0f;
            owned(0x323,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_44 * 0x1e);
            a.launchParameters[1] = 7.0f;
            a.launchParameters[2] = 35.0f;
            owned(0x323,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_44 * -0x1e);
            a.launchParameters[1] = 7.0f;
            a.launchParameters[2] = -35.0f;
            owned(0x323,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
          }
        }
        if (((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 2)) {
          state.impactX = i16(state.impactX) + 1;
          selectFrame(8);
        }
        if (i16(a.animation.cursor.index) != 0xc) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (1 < i16(state.impactY)) {
          return;
        }
        state.impactY = i16(state.impactY) + 1;
        selectFrame(10);
        return;
      case 0x2be:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.response = 1;
          a.motion.clear();
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x73,double(a.motion.x),double(a.motion.y) + double(140.0f),u8(a.motion.facing),1);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(500);
          events.sound(6);
          owned(0x326,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (5 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(9);
        return;
      case 0x2bf:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x73,(float)((i8)u8(a.motion.facing) * 0x2a) + double(a.motion.x),double(a.motion.y) + double(128.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(8);
        consumeSpirit(500);
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x53) + double(a.motion.x),double(a.motion.y) + double(137.0f),u8(a.motion.facing),1);
        return;
      case 0x2c0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x74,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(300);
          events.sound(5);
          for (local_48 = 0; local_48 < 0xc; local_48 = local_48 + 1) {
            a.launchParameters[0] = (float)(local_48 * 0x1e);
            a.launchParameters[1] = 9.0f;
            a.launchParameters[2] = 30.0f;
            owned(0x340,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_48 * -0x1e);
            a.launchParameters[1] = 9.0f;
            a.launchParameters[2] = -30.0f;
            owned(0x340,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_48 * 0x1e);
            a.launchParameters[1] = 8.0f;
            a.launchParameters[2] = 35.0f;
            owned(0x340,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_48 * -0x1e);
            a.launchParameters[1] = 8.0f;
            a.launchParameters[2] = -35.0f;
            owned(0x340,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
          }
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(300);
          events.sound(5);
          for (local_4c = 0; local_4c < 0xc; local_4c = local_4c + 1) {
            a.launchParameters[0] = (float)(local_4c * 0x1e);
            a.launchParameters[1] = 7.0f;
            a.launchParameters[2] = 30.0f;
            owned(0x341,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_4c * -0x1e);
            a.launchParameters[1] = 7.0f;
            a.launchParameters[2] = -30.0f;
            owned(0x341,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_4c * 0x1e);
            a.launchParameters[1] = 6.0f;
            a.launchParameters[2] = 35.0f;
            owned(0x341,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_4c * -0x1e);
            a.launchParameters[1] = 6.0f;
            a.launchParameters[2] = -35.0f;
            owned(0x341,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
          }
        }
        if (((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 2)) {
          state.impactX = i16(state.impactX) + 1;
          selectFrame(8);
        }
        if (i16(a.animation.cursor.index) != 0xc) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (1 < i16(state.impactY)) {
          return;
        }
        state.impactY = i16(state.impactY) + 1;
        selectFrame(10);
        return;
      case 0x2c2:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.response = 1;
          a.motion.clear();
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x74,double(a.motion.x),double(a.motion.y) + double(140.0f),u8(a.motion.facing),1);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(500);
          events.sound(6);
          owned(0x344,double(other.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (5 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(9);
        return;
      case 0x2c3:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x74,(float)((i8)u8(a.motion.facing) * 0x2a) + double(a.motion.x),double(a.motion.y) + double(128.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0xd);
        consumeSpirit(500);
        owned(0x34b,(float)((i8)u8(a.motion.facing) * 0x53) + double(a.motion.x),double(a.motion.y) + double(137.0f),u8(a.motion.facing),1);
        return;
      case 0x2da:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          state.aimAngle = 0;
          a.response = 1;
          a.motion.clear();
        }
        if (((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) < 0x1e)) &&
           (state.aimAngle = i16(state.aimAngle) + 1,
           4 < i16(state.aimAngle))) {
          shared(0x3e,double(a.motion.x),double(a.motion.y) + double(130.0f),1,1);
          state.aimAngle = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          if (double(0.0f) < double(a.motion.y)) {
            a.select_action(9);
          }
          a.commandTimers[2] = state.difficulty * -0x28 + 0xb4;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(4);
          for (local_50 = 0; local_50 < 9; local_50 = local_50 + 1) {
            a.launchParameters[0] = (float)(local_50 * 0x28);
            a.launchParameters[1] = 8.0f;
            a.launchParameters[2] = 1.0f;
            owned(900,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_50 * -0x28 + 0x14);
            a.launchParameters[1] = 8.0f;
            a.launchParameters[2] = -1.0f;
            owned(900,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
          }
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(4);
          for (local_54 = 0; local_54 < 9; local_54 = local_54 + 1) {
            a.launchParameters[0] = (float)(local_54 * 0x28);
            a.launchParameters[1] = 10.0f;
            a.launchParameters[2] = 0.800000011920929f;
            owned(900,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_54 * -0x28 + 0x14);
            a.launchParameters[1] = 10.0f;
            a.launchParameters[2] = -0.800000011920929f;
            owned(900,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
          }
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           ('\x01' < state.difficulty)) {
          events.sound(4);
          for (local_58 = 0; local_58 < 9; local_58 = local_58 + 1) {
            a.launchParameters[0] = (float)(local_58 * 0x28);
            a.launchParameters[1] = 12.0f;
            a.launchParameters[2] = 0.6000000238418579f;
            owned(900,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_58 * -0x28 + 0x14);
            a.launchParameters[1] = 12.0f;
            a.launchParameters[2] = -0.6000000238418579f;
            owned(900,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
          }
        }
        if (((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) &&
           ('\x02' < state.difficulty)) {
          events.sound(4);
          for (local_5c = 0; local_5c < 9; local_5c = local_5c + 1) {
            a.launchParameters[0] = (float)(local_5c * 0x28);
            a.launchParameters[1] = 14.0f;
            a.launchParameters[2] = 0.6000000238418579f;
            owned(900,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(local_5c * -0x28 + 0x14);
            a.launchParameters[1] = 14.0f;
            a.launchParameters[2] = -0.6000000238418579f;
            owned(900,double(a.motion.x),double(a.motion.y) + double(125.0f),u8(a.motion.facing),1);
          }
        }
        if (((i16(a.animation.cursor.index) == 0xc) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 2)) {
          state.impactX = i16(state.impactX) + 1;
          selectFrame(10);
        }
        if (i16(a.animation.cursor.index) != 0xe) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (5 < i16(state.impactY)) {
          return;
        }
        state.impactY = i16(state.impactY) + 1;
        selectFrame(0xc);
        return;
      case 0x2db:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.begin_pose(0.0f,0.0f,0.0f);
        }
        if ((u8)u8(a.pose.channel(24)) < 0x81) {
          a.pose.set_channel(24,u8((u8)0x80));
        }
        else {
          a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + -6)));
        }
        if ((i16(a.animation.cursor.index) == 0) && (u8(a.carryMomentum) == (u8)0x0)) {
          a.motion.clear();
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.begin_pose(0.0f,0.0f,0.0f);
          a.select_action(0x2da);
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
          a.motion.velocityX =
               ((double(other.motion.x) - double(a.motion.x)) *
               (float)(int)(i8)u8(a.motion.facing)) / double(50.0f);
          a.motion.velocityY = 10.0f;
          a.motion.accelerationY = 0.4000000059604645f;
        }
        if (i16(a.animation.cursor.frameCount) <= i16(a.animation.cursor.index)) {
          a.animation.cursor.index = i16(a.animation.cursor.index) + -2;
        }
        a.animation.set(a.animation.cursor.action,a.animation.cursor.index,a.animation.cursor.elapsed);
        return;
      case 0x2e4:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 0;
          a.remainingHits = 8;
          a.motion.clear();
          state.impactX = 0;
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x388,(float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x),double(a.motion.y) + double(165.0f),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x388,(float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x),double(a.motion.y) + double(165.0f),u8(a.motion.facing),1);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          owned(0x397,double(a.motion.x),double(a.motion.y) + double(1000.0f),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(3);
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          if ((int)i16(state.impactX) < state.difficulty * 4 + 5) {
            state.impactX = i16(state.impactX) + 1;
            selectFrame(7);
            return;
          }
          a.response = 1;
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
        state.impactX = i16(state.impactX) + 1;
        selectFrame(9);
        return;
      case 0x2ee:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          state.aimAngle = 0;
          a.response = 1;
          a.motion.clear();
        }
        if (((i16(a.animation.cursor.index) == 9) || (i16(a.animation.cursor.index) == 10)) &&
           (state.impactY = i16(state.impactY) + 1,
           0x14 < i16(state.impactY))) {
          owned(0x386,double(other.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(6);
          state.impactY = 0;
          state.impactX = i16(state.impactX) + 1;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          if (double(0.0f) < double(a.motion.y)) {
            a.select_action(9);
          }
          a.commandTimers[2] = state.difficulty * -0x1e + 0xf0;
        }
        if (((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) &&
           ((int)i16(state.impactX) < state.difficulty * 2 + 4)) {
          selectFrame(9);
        }
        if (i16(a.animation.cursor.index) != 0xd) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (1 < i16(state.aimAngle)) {
          return;
        }
        state.aimAngle = i16(state.aimAngle) + 1;
        selectFrame(0xb);
        return;
      case 0x2ef:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          state.aimAngle = 0;
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          if (double(0.0f) < double(a.motion.y)) {
            a.select_action(9);
          }
          a.commandTimers[2] = state.difficulty * -0x1e + 0xf0;
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x389,double(other.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x2f8:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          state.impactX = 0;
          state.impactY = 0;
        }
        if ((((i16(a.animation.cursor.index) < 3) && (i16(state.impactX) < 3)) &&
            (i16(a.animation.cursor.elapsed) < 0x46)) &&
           (state.impactY = i16(state.impactY) + 1,
           4 < i16(state.impactY))) {
          shared(0x40,double(a.motion.x),double(a.motion.y) + double(130.0f),1,1);
          state.impactY = 0;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.velocityY) < double(0.0f)) {
          a.motion.velocityY = 0.0f;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x2f9);
          state.aimAngle = 0;
          return;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 10.0f;
          a.motion.accelerationY = 0.30000001192092896f;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x38a,double(a.motion.x),double(a.motion.y),1,0);
          events.sound(0xd);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (i16(state.aimAngle) != 0) {
          return;
        }
        selectFrame(6);
        return;
      case 0x2f9:
        a.motion.facing = (u8)0x1;
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0x78;
          state.impactY = 0;
          state.aimAngle = 0;
          state.emissionDelay = 0;
          state.effectLoops = 0x3c;
        }
        if ((((0 < i16(other.combat.health)) &&
             (i16(other.animation.cursor.index) == 2)) &&
            ('\0' < i8(other.combat.spellPhase))) &&
           (i16(other.animation.cursor.action) == 99)) {
          other.select_action(0x5f);
          other.transitionReady = 1;
          return;
        }
        state.impactX = i16(state.impactX) + -1;
        state.emissionDelay = i16(state.emissionDelay) + -1;
        state.effectLoops = i16(state.effectLoops) + -1;
        if (0 < i16(other.combat.health)) {
          if (i16(state.impactX) < 1) {
            state.impactX = 0xb4;
            advancedFrame = random.range(0,0x14);
            state.impactY = advancedFrame + 0x1e;
            step = random.range(0,100);
            if (step < 0x32) {
              if (double(a.motion.y) < double(240.0f)) {
                if (double(480.0f) <= double(a.motion.x)) {
                  state.aimAngle = 0xff51;
                }
                else {
                  state.aimAngle = 0xfffb;
                }
              }
            }
            else if (double(180.0f) < double(a.motion.y)) {
              if (double(a.motion.x) <= double(800.0f)) {
                state.aimAngle = 5;
              }
              else {
                state.aimAngle = 0xaf;
              }
            }
          }
          if (0 < i16(state.impactY)) {
            state.impactY = i16(state.impactY) + -1;
          }
          a.set_polar_velocity((float)(int)i16(state.aimAngle),(float)(int)i16(state.impactY) * double(0.1f));
          if (i16(state.emissionDelay) < 1) {
            events.sound(0xd);
            state.emissionDelay = 0xb4;
            if (state.difficulty == '\0') {
              for (local_60 = 0; local_60 < 6; local_60 = local_60 + 1) {
                a.launchParameters[0] = (float)(local_60 * 0x3c);
                owned(0x38c,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
              }
            }
            if (state.difficulty == '\x01') {
              for (local_64 = 0; local_64 < 9; local_64 = local_64 + 1) {
                a.launchParameters[0] = (float)(local_64 * 0x28);
                owned(0x38c,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
              }
            }
            if (state.difficulty == '\x02') {
              for (local_68 = 0; local_68 < 0xc; local_68 = local_68 + 1) {
                a.launchParameters[0] = (float)(local_68 * 0x1e);
                owned(0x38c,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
              }
            }
            if (state.difficulty == '\x03') {
              for (local_6c = 0; local_6c < 0x12; local_6c = local_6c + 1) {
                a.launchParameters[0] = (float)(local_6c * 0x14);
                owned(0x38c,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
              }
            }
          }
          if (i16(state.effectLoops) < 1) {
            if (i16(state.effectLoops) == 0) {
              step = random.range(0,600);
              a.launchParameters[1] = (float)step;
              step = random.range(0,300);
              a.launchParameters[2] = (float)step;
              events.sound(0);
              for (local_70 = 0; local_70 < 6; local_70 = local_70 + 1) {
                a.launchParameters[0] = (float)(local_70 * 0x3c);
                owned(0x38d,(double(a.motion.x) + double(a.launchParameters[1])) -
                                   double(300.0f),double(a.motion.y) + double(a.launchParameters[2]),u8(a.motion.facing),1);
              }
            }
            if (('\0' < state.difficulty) && (i16(state.effectLoops) == -0x14)) {
              step = random.range(0,600);
              a.launchParameters[1] = (float)step;
              step = random.range(0,300);
              a.launchParameters[2] = (float)step;
              events.sound(0);
              for (local_74 = 0; local_74 < 6; local_74 = local_74 + 1) {
                a.launchParameters[0] = (float)(local_74 * 0x3c);
                owned(0x38d,(double(a.motion.x) + double(a.launchParameters[1])) -
                                   double(300.0f),double(a.motion.y) + double(a.launchParameters[2]),u8(a.motion.facing),1);
              }
            }
            if (('\x01' < state.difficulty) && (i16(state.effectLoops) == -0x28)) {
              step = random.range(0,600);
              a.launchParameters[1] = (float)step;
              step = random.range(0,300);
              a.launchParameters[2] = (float)step;
              events.sound(0);
              for (local_78 = 0; local_78 < 6; local_78 = local_78 + 1) {
                a.launchParameters[0] = (float)(local_78 * 0x3c);
                owned(0x38d,(double(a.motion.x) + double(a.launchParameters[1])) -
                                   double(300.0f),double(a.motion.y) + double(a.launchParameters[2]),u8(a.motion.facing),1);
              }
            }
            if (('\x02' < state.difficulty) && (i16(state.effectLoops) == -0x3c)) {
              step = random.range(0,600);
              a.launchParameters[1] = (float)step;
              step = random.range(0,300);
              a.launchParameters[2] = (float)step;
              events.sound(0);
              for (local_7c = 0; local_7c < 6; local_7c = local_7c + 1) {
                a.launchParameters[0] = (float)(local_7c * 0x3c);
                owned(0x38d,(double(a.motion.x) + double(a.launchParameters[1])) -
                                   double(300.0f),double(a.motion.y) + double(a.launchParameters[2]),u8(a.motion.facing),1);
              }
            }
            if (i16(state.effectLoops) < -0x3b) {
              state.effectLoops = state.difficulty * -10 + 0x78;
            }
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(1);
        return;
      case 0x302:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 1;
          a.motion.clear();
        }
        if (i16(a.animation.cursor.index) == 1) {
          if ((u8)u8(a.pose.channel(24)) < 6) {
            a.pose.set_channel(24,u8((u8)0x0));
          }
          else {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + -5)));
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x303);
          a.transitionReady = (u8)0x1;
          a.motion.x = 640.0f;
          a.motion.y = 200.0f;
          a.motion.facing = (u8)0x1;
          return;
        }
        if (i16(a.animation.cursor.index) != 1) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.begin_pose(0.0f,0.0f,0.0f);
        fade(0);
        a.begin_spell(mode,replayMode,core);
        events.system_sound(0x17);
        return;
      case 0x303:
        a.motion.facing = (u8)0x1;
        a.pose.flags = (u8)0x82;
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0x78;
          state.impactY = 0xffa6;
          state.aimAngle = 0x5a;
          state.emissionDelay = 0;
          state.effectLoops = 0;
          a.motion.clear();
        }
        if ((((0 < i16(other.combat.health)) &&
             (i16(other.animation.cursor.index) == 2)) &&
            ('\0' < i8(other.combat.spellPhase))) &&
           (i16(other.animation.cursor.action) == 99)) {
          other.select_action(0x5f);
          other.transitionReady = 1;
          return;
        }
        if (u8(a.pose.channel(24)) != (u8)0xff) {
          if ((u8)u8(a.pose.channel(24)) < 0xfa) {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + '\x05')));
          }
          else {
            a.pose.set_channel(24,u8((u8)0xff));
          }
        }
        a.motion.facing = (u8)0x1;
        a.motion.x = 640.0f;
        a.motion.y = 200.0f;
        if (0 < i16(other.combat.health)) {
          a.pose.set_channel(24,u8((u8)0xff));
          state.impactX = i16(state.impactX) + -1;
          state.emissionDelay = i16(state.emissionDelay) + 1;
          other.motion.opponentPush = 0.0f;
          if (((300 < i16(state.emissionDelay)) && (i16(a.animation.cursor.index) == 1)) &&
             (i16(a.animation.cursor.elapsed) == 0)) {
            state.impactY = i16(state.impactY) + 10;
            if (i16(state.impactY) == 0xb4) {
              state.impactY = 0xff4c;
            }
            state.aimAngle = i16(state.aimAngle) + -10;
            if (i16(state.aimAngle) == -0xb4) {
              state.aimAngle = 0xb4;
            }
            events.sound(0xe);
            a.launchParameters[1] = (float)(int)i16(state.impactY);
            owned(0x38e,640.0f,340.0f,1,0);
            if ('\0' < state.difficulty) {
              a.launchParameters[1] = (float)(int)i16(state.aimAngle);
              owned(0x391,640.0f,340.0f,1,0);
            }
            if (state.difficulty == '\x02') {
              a.launchParameters[1] = (float)(i16(state.aimAngle) + 0xb4);
              if (double(180.0f) <= double(a.launchParameters[1])) {
                a.launchParameters[1] = double(a.launchParameters[1]) - double(360.0f);
              }
              owned(0x391,640.0f,340.0f,1,0);
              a.launchParameters[1] = (float)(i16(state.impactY) + -0xb4);
              if (double(a.launchParameters[1]) < -double(180.0f) != (double(a.launchParameters[1]) == -double(180.0f))) {
                a.launchParameters[1] = double(a.launchParameters[1]) + double(360.0f);
              }
              owned(0x38e,640.0f,340.0f,1,0);
            }
            if (state.difficulty == '\x03') {
              a.launchParameters[1] = (float)(i16(state.aimAngle) + 0x78);
              if (double(180.0f) <= double(a.launchParameters[1])) {
                a.launchParameters[1] = double(a.launchParameters[1]) - double(360.0f);
              }
              owned(0x391,640.0f,340.0f,1,0);
              a.launchParameters[1] = (float)(i16(state.impactY) + -0x78);
              if (double(a.launchParameters[1]) < -double(180.0f) != (double(a.launchParameters[1]) == -double(180.0f))) {
                a.launchParameters[1] = double(a.launchParameters[1]) + double(360.0f);
              }
              owned(0x38e,640.0f,340.0f,1,0);
              a.launchParameters[1] = (float)(i16(state.aimAngle) + 0xf0);
              if (double(180.0f) <= double(a.launchParameters[1])) {
                a.launchParameters[1] = double(a.launchParameters[1]) - double(360.0f);
              }
              owned(0x391,640.0f,340.0f,1,0);
              a.launchParameters[1] = (float)(i16(state.impactY) + -0xf0);
              if (double(a.launchParameters[1]) < -double(180.0f) != (double(a.launchParameters[1]) == -double(180.0f))) {
                a.launchParameters[1] = double(a.launchParameters[1]) + double(360.0f);
              }
              owned(0x38e,640.0f,340.0f,1,0);
            }
          }
          if (i16(state.impactX) < 1) {
            if (i16(state.impactX) == 0) {
              events.sound(0);
              for (local_80 = 0; local_80 < 0x14; local_80 = local_80 + 1) {
                a.launchParameters[0] = (float)(local_80 * 0x12);
                a.launchParameters[1] = 1.0f;
                a.launchParameters[2] = 1.0f;
                owned(0x394,640.0f,340.0f,1,1);
                if (state.difficulty == '\x03') {
                  a.launchParameters[0] = (float)(local_80 * 0x12 + 9);
                  a.launchParameters[1] = 3.0f;
                  owned(0x394,640.0f,340.0f,1,1);
                }
              }
              for (local_80 = 0; local_80 < 0x14; local_80 = local_80 + 1) {
                a.launchParameters[0] = (float)(local_80 * -0x12);
                a.launchParameters[1] = 1.0f;
                a.launchParameters[2] = -1.0f;
                owned(0x394,640.0f,340.0f,1,1);
                if (state.difficulty == '\x03') {
                  a.launchParameters[0] = (float)(local_80 * 0x12 + 9);
                  a.launchParameters[1] = 3.0f;
                  owned(0x394,640.0f,340.0f,1,1);
                }
              }
            }
            if (i16(state.impactX) == -0x1e) {
              events.sound(0);
              for (local_84 = 0; local_84 < 0x14; local_84 = local_84 + 1) {
                a.launchParameters[0] = (float)(local_84 * 0x12);
                a.launchParameters[1] = 1.0f;
                a.launchParameters[2] = 1.0f;
                owned(0x395,640.0f,340.0f,1,1);
                if (state.difficulty == '\x03') {
                  a.launchParameters[0] = (float)(local_84 * 0x12 + 9);
                  a.launchParameters[1] = 3.0f;
                  owned(0x395,640.0f,340.0f,1,1);
                }
              }
              for (local_84 = 0; local_84 < 0x14; local_84 = local_84 + 1) {
                a.launchParameters[0] = (float)(local_84 * -0x12);
                a.launchParameters[1] = 1.0f;
                a.launchParameters[2] = -1.0f;
                owned(0x395,640.0f,340.0f,1,1);
                if (state.difficulty == '\x03') {
                  a.launchParameters[0] = (float)(local_84 * 0x12 + 9);
                  a.launchParameters[1] = 3.0f;
                  owned(0x395,640.0f,340.0f,1,1);
                }
              }
            }
            if (('\x01' < state.difficulty) && (i16(state.impactX) == -0x3c)) {
              events.sound(0);
              for (local_88 = 0; local_88 < 0x14; local_88 = local_88 + 1) {
                a.launchParameters[0] = (float)(local_88 * 0x12);
                a.launchParameters[1] = 1.0f;
                a.launchParameters[2] = 1.0f;
                owned(0x396,640.0f,340.0f,1,1);
                if (state.difficulty == '\x03') {
                  a.launchParameters[0] = (float)(local_88 * 0x12 + 9);
                  a.launchParameters[1] = 3.0f;
                  owned(0x396,640.0f,340.0f,1,1);
                }
              }
              for (local_88 = 0; local_88 < 0x14; local_88 = local_88 + 1) {
                a.launchParameters[0] = (float)(local_88 * -0x12);
                a.launchParameters[1] = 1.0f;
                a.launchParameters[2] = -1.0f;
                owned(0x396,640.0f,340.0f,1,1);
                if (state.difficulty == '\x03') {
                  a.launchParameters[0] = (float)(local_88 * 0x12 + 9);
                  a.launchParameters[1] = 3.0f;
                  owned(0x396,640.0f,340.0f,1,1);
                }
              }
            }
            if (i16(state.impactX) == -0x3c) {
              state.impactX = state.difficulty * -10 + 0xb4;
            }
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          selectFrame(1);
          return;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(1);
        return;
      case 0x31b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
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
          owned(0x3e5,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 9) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (7 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(7);
        return;
      case 0x31c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          if (a.phaseVariants[a.combat.spellPhase] == (u8)0x1) {
            a.select_action(0x2e4);
            return;
          }
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) {
          fade(0);
          a.begin_spell(mode,replayMode,core);
          events.system_sound(0x17);
        }
        if (i16(a.animation.cursor.index) != 0xe) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (7 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(0xc);
        a.transitionReady = (u8)0x1;
        return;
      }
    }
switchD_00564c55_default:
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
    goto LAB_00573d81;
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
    goto LAB_00573d81;
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
    a.motion.velocityX = 4.0f;
    goto LAB_00573d81;
  case 5:
    a.motion.velocityX = -4.0f;
LAB_00573d81:
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
        if ((i16(a.animation.cursor.index) == 2) && (double(a.motion.velocityY) < double(4.0f))) {
          selectFrame(3);
        }
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.duration) <= i16(a.animation.cursor.elapsed)) {
          a.animation.cursor.elapsed = 0;
          a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
          if (i16(a.animation.cursor.index) == 2) {
            a.motion.velocityX = 0.0f;
            a.motion.velocityY = 12.0f;
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
        if ((i16(a.animation.cursor.index) == 2) && (double(a.motion.velocityY) < double(4.0f))) {
          selectFrame(3);
        }
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.duration) <= i16(a.animation.cursor.elapsed)) {
          a.animation.cursor.elapsed = 0;
          a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
          if (i16(a.animation.cursor.index) == 2) {
            a.motion.velocityX = 4.0f;
            a.motion.velocityY = 12.0f;
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
            a.motion.velocityY = 12.0f;
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
    goto switchD_00564c55_default;
  }
  return;

}
}
