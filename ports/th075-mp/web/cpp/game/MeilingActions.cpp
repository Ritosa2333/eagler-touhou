#include "MeilingState.hpp"
#include <cmath>
namespace th075 {
void advance_meiling_action(FighterActor& a,FighterActor& other,MeilingState& state,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){

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
          if (double(a.motion.velocityY) < double(2.0f)) {
            a.motion.velocityY = 2.0f;
          }
          a.motion.accelerationY = 0.800000011920929f;
          if (double(12.0f) < double(a.motion.velocityY)) {
            a.motion.velocityY = 12.0f;
          }
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(3);
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
        selectFrame(5);
        return;
      case 0x47:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -double(a.impulseX);
          a.motion.velocityY = double(a.impulseY);
          a.motion.accelerationY = 0.800000011920929f;
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(3);
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
        if ((i16(a.animation.cursor.index) == 2) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(3);
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
        if ((i16(a.animation.cursor.index) == 2) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(3);
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
        a.begin_pose(-7.0f,75.0f,0.0f);
        return;
      case 0x5d:
        if (((i16(a.animation.cursor.index) < 6) && (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(6);
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
          a.motion.velocityY = 4.5f;
          a.motion.velocityX = 20.0f;
          a.motion.accelerationY = 0.5f;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(5);
        return;
      case 0x5e:
        if (((i16(a.animation.cursor.index) < 6) && (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(6);
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
          a.motion.velocityY = 4.5f;
          a.motion.velocityX = -20.0f;
          a.motion.accelerationY = 0.5f;
        }
        if (i16(a.animation.cursor.index) != 6) {
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
      case 0x61:
        if ((0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) &&
           (((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
            ((a.controls.current.buttons[2] != 0 || (a.controls.current.buttons[3] != 0)))))) {
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
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = (float)(int)i16(state.impactX) * double(0.005f);
          a.motion.velocityY = (float)-(int)i16(state.impactY) * double(0.005f);
          a.motion.accelerationY = 2.0f;
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(4);
        return;
      case 0x62:
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
             ((a.phaseVariants[a.combat.spellPhase] == (u8)0x5 &&
              (a.phaseKinds[a.combat.spellPhase] == (u8)0x2)))) {
            a.select_action(0x303);
            return;
          }
          if (((1 < i16(a.animation.cursor.index)) && ('\0' < (i8)u8(a.combat.spellPhase))) &&
             ((i16(other.animation.cursor.action) < 0x96 &&
              (a.phaseKinds[a.combat.spellPhase] == (u8)0x2)))) {
            a.select_action(0x31c);
            return;
          }
          if ((((0 < i16(a.animation.cursor.index)) && ('\0' < (i8)u8(a.combat.spellPhase))) &&
              (i16(other.animation.cursor.action) < 0x96)) &&
             (a.phaseKinds[a.combat.spellPhase] == (u8)0x1)) {
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
        if (i16(a.animation.cursor.index) != 1) {
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
        if ((i16(a.animation.cursor.index) == 2) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(3);
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
        selectFrame(6);
        return;
      case 200:
        a.carryMomentum = (u8)0x1;
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          state.impactX = 0;
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
        if ((0 < i16(state.impactX)) &&
           ((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 1)) {
          a.select_action(0xcf);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          if ((0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) &&
             (i16(state.impactX) < 3)) {
            selectFrame(2);
            state.impactX = i16(state.impactX) + 1;
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
        a.motion.velocityX = 13.0f;
        shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        shared(0x7e,double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        events.system_sound(0x1f);
        return;
      case 0xc9:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if (((double(0.0f) < double(a.motion.y)) && (2 < i16(a.animation.cursor.index))) &&
           ((i16(a.animation.cursor.index) < 5 &&
            (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
            double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f)))))) {
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
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -12.0f;
          a.motion.velocityY = 7.0f;
          a.motion.accelerationY = 0.699999988079071f;
          shared(0x7d,double(a.motion.x),double(a.motion.y) + double(80.0f),-(int)(i8)u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(80.0f),-(int)(i8)u8(a.motion.facing),1);
          events.system_sound(0x1f);
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
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 11.0f;
          a.motion.velocityY = 5.0f;
          a.motion.accelerationY = 0.44999998807907104f;
          shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
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
        selectFrame(10);
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
          selectFrame(10);
        }
        if (i16(a.animation.cursor.index) != 1) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = -11.0f;
        a.motion.velocityY = 5.0f;
        a.motion.accelerationY = 0.44999998807907104f;
        shared(0x7d,double(a.motion.x),double(a.motion.y) + double(120.0f),-(int)(i8)u8(a.motion.facing),1);
        shared(0x7e,double(a.motion.x),double(a.motion.y) + double(120.0f),-(int)(i8)u8(a.motion.facing),1);
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
        if ((i16(a.animation.cursor.index) == 3) && (double(a.motion.velocityY) < double(4.0f))) {
          selectFrame(4);
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
          a.motion.accelerationY = 0.75f;
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
        if ((i16(a.animation.cursor.index) == 3) && (double(a.motion.velocityY) < double(4.0f))) {
          selectFrame(4);
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
          a.motion.accelerationY = 0.699999988079071f;
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
          a.motion.velocityX = -8.0f;
          a.motion.velocityY = 16.0f;
          a.motion.accelerationY = 0.699999988079071f;
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
        selectFrame(4);
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
          owned(0x3e5,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
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
          owned(0x3e5,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
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
          a.carryMomentum = (u8)0x0;
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = 11.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 0) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        if ((1 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(4.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 1) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1d);
        a.motion.velocityX = 20.0f;
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
        if (((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(25.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if (((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(36.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
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
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) + double(25.0f);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1c);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = double(a.motion.velocityX) + double(36.0f);
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
        if (i16(a.animation.cursor.index) != 1) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1b);
        return;
      case 0x130:
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
          state.impactX = 0;
          a.remainingHits = 1;
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.remainingHits = 1;
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x34e,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(9);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(9);
        return;
      case 0x132:
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
        events.system_sound(0x1c);
        return;
      case 0x133:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
        }
        if ((('\0' < (i8)u8(a.carryMomentum)) || (2 < i16(a.animation.cursor.index))) &&
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
          a.motion.velocityX = 8.0f;
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1c);
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
        events.system_sound(0x1c);
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
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1c);
        return;
      case 0x136:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.carryMomentum = (u8)0x0;
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
        }
        if (((i16(a.animation.cursor.index) < 5) && (double(0.0f) < double(a.motion.velocityX))) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.8f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if (((4 < i16(a.animation.cursor.index)) && (double(0.0f) < double(a.motion.velocityX))) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(1.7f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 12.5f;
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 25.0f;
        events.system_sound(0x1d);
        return;
      case 0x140:
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
        if (i16(a.animation.cursor.index) != 1) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1b);
        return;
      case 0x141:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
        }
        if (((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(25.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if (((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(36.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
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
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) + double(25.0f);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1c);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = double(a.motion.velocityX) + double(36.0f);
        return;
      case 0x142:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
        }
        if ((0 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(1.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 1) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1c);
        a.motion.velocityX = 12.0f;
        return;
      case 0x143:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
        }
        if ((('\0' < (i8)u8(a.carryMomentum)) || (2 < i16(a.animation.cursor.index))) &&
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
          a.motion.velocityX = 8.0f;
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1c);
        return;
      case 0x144:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
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
        events.system_sound(0x1c);
        return;
      case 0x145:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1c);
          a.motion.velocityX = 30.0f;
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 10.0f;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 10.0f;
        }
        if (i16(a.animation.cursor.index) != 9) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 10.0f;
        return;
      case 0x146:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
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
        events.system_sound(0x1c);
        return;
      }
    }
    else if (step < 0x1fc) {
      if (step == 0x1fb) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.remainingHits = 1;
          a.response = 0;
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
        owned(0x33c,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(168.0f),u8(a.motion.facing),1);
        owned(0x33d,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(168.0f),u8(a.motion.facing),1);
        owned(0x33e,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(168.0f),u8(a.motion.facing),1);
        shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        shared(0x81,double(a.motion.x),double(a.motion.y),-(int)(i8)u8(a.motion.facing),1);
        events.shake(5.0f);
        events.sound(2);
        consumeSpirit(200);
        return;
      }
      switch(step) {
      case 0x187:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.carryMomentum = (u8)0x0;
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = 12.0f;
        }
        a.motion.velocityX = double(a.motion.velocityX) - double(0.65f);
        if (double(a.motion.velocityX) < double(0.0f)) {
          a.motion.velocityX = 0.0f;
        }
        if (((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(25.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if (((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(36.0f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.impactX = 0;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) + double(25.0f);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1c);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = double(a.motion.velocityX) + double(36.0f);
        return;
      case 0x188:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.response = 0;
          a.motion.velocityX = 10.0f;
        }
        if ((0 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
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
      case 0x189:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = 12.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
        }
        a.motion.velocityX = double(a.motion.velocityX) - double(0.65f);
        if (double(a.motion.velocityX) < double(0.0f)) {
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
        events.system_sound(0x1b);
        return;
      case 400:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
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
        owned(800,(float)((i8)u8(a.motion.facing) * 0x5a) + double(a.motion.x),double(a.motion.y) + double(95.0f),u8(a.motion.facing),1);
        events.sound(4);
        consumeSpirit(0xfa);
        return;
      case 0x192:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
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
        owned(0x33f,(float)((i8)u8(a.motion.facing) * 0x78) + double(a.motion.x),double(a.motion.y) + double(0.0f),u8(a.motion.facing),1);
        events.sound(7);
        consumeSpirit(0xfa);
        return;
      case 0x194:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.impactX = 0;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
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
        a.motion.velocityY = 8.0f;
        a.motion.accelerationY = 0.550000011920929f;
        owned(800,(float)((i8)u8(a.motion.facing) * 0x7d) + double(a.motion.x),double(a.motion.y) + double(115.0f),u8(a.motion.facing),1);
        events.sound(4);
        consumeSpirit(0xfa);
        return;
      case 0x195:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.impactX = 0;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -3.0f;
          a.motion.velocityY = 8.0f;
          a.motion.accelerationY = 0.550000011920929f;
          a.launchParameters[0] = -30.0f;
          a.launchParameters[1] = 10.0f;
          owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x),double(a.motion.y) + double(75.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = -40.0f;
          a.launchParameters[1] = 10.0f;
          owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x),double(a.motion.y) + double(75.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = -50.0f;
          a.launchParameters[1] = 10.0f;
          owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x),double(a.motion.y) + double(75.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = -60.0f;
          a.launchParameters[1] = 10.0f;
          owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x),double(a.motion.y) + double(75.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = -70.0f;
          a.launchParameters[1] = 10.0f;
          owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x),double(a.motion.y) + double(75.0f),u8(a.motion.facing),1);
          events.sound(0);
          consumeSpirit(100);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (1 < i16(state.impactX)) {
          return;
        }
        if (a.controls.current.buttons[2] == 0) {
          return;
        }
        a.launchParameters[0] = -30.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x),double(a.motion.y) + double(75.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -40.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x),double(a.motion.y) + double(75.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -50.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x),double(a.motion.y) + double(75.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -60.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x),double(a.motion.y) + double(75.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = -70.0f;
        a.launchParameters[1] = 10.0f;
        owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x43) + double(a.motion.x),double(a.motion.y) + double(75.0f),u8(a.motion.facing),1);
        events.sound(0);
        consumeSpirit(0x19);
        state.impactX = i16(state.impactX) + 1;
        selectFrame(4);
        return;
      case 0x196:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
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
        consumeSpirit(0x96);
        for (spark = 0; spark < 7; spark = spark + 1) {
          a.launchParameters[1] = 13.0f;
          step = spark * -0xc + 0x24;
          a.launchParameters[0] = (float)step;
          owned(spark + 0x32b,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          events.sound(5);
        }
        return;
      case 0x197:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
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
        consumeSpirit(0x96);
        for (volley = 0; volley < 7; volley = volley + 1) {
          a.launchParameters[1] = 13.0f;
          step = volley * -5 + 0xf;
          a.launchParameters[0] = (float)step;
          owned(volley + 0x32b,double(a.motion.x) + double(0.0f),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          events.sound(5);
        }
        return;
      case 0x198:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
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
        owned(0x323,(float)((i8)u8(a.motion.facing) * 0x86) + double(a.motion.x),double(a.motion.y) + double(82.0f),u8(a.motion.facing),1);
        consumeSpirit(0xfa);
        owned(0x324,(float)((i8)u8(a.motion.facing) * 0x86) + double(a.motion.x),double(a.motion.y) + double(82.0f),u8(a.motion.facing),1);
        events.sound(8);
        return;
      case 0x19a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * 0.1;
          a.motion.velocityY = double(a.motion.velocityY) * 0.1;
          a.response = 1;
          state.impactX = 0;
        }
        if (6 < i16(a.animation.cursor.index)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        }
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.velocityY = 0.0f;
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
        consumeSpirit(0x96);
        for (emission = 0; emission < 7; emission = emission + 1) {
          a.launchParameters[1] = 13.0f;
          step = emission * -0xc + 0x24;
          a.launchParameters[0] = (float)step;
          owned(emission + 0x32b,(float)((i8)u8(a.motion.facing) * 10) + double(a.motion.x),double(a.motion.y) + double(130.0f),u8(a.motion.facing),1);
          events.sound(5);
        }
        return;
      case 0x19b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * 0.1;
          a.motion.velocityY = double(a.motion.velocityY) * 0.1;
          a.response = 1;
          state.impactX = 0;
        }
        if (6 < i16(a.animation.cursor.index)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        }
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.velocityY = 0.0f;
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
        consumeSpirit(0x96);
        for (local_1c = 0; local_1c < 7; local_1c = local_1c + 1) {
          a.launchParameters[1] = 13.0f;
          step = local_1c * -5 + 0xf;
          a.launchParameters[0] = (float)step;
          owned(local_1c + 0x32b,(float)((i8)u8(a.motion.facing) * 10) + double(a.motion.x),double(a.motion.y) + double(130.0f),u8(a.motion.facing),1);
          events.sound(5);
        }
        return;
      case 0x19c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
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
        owned(0x339,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        events.sound(6);
        consumeSpirit(0xfa);
        return;
      case 0x19e:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
        }
        if (((i16(a.animation.cursor.index) < 10) && (1 < i16(a.animation.cursor.index))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(10);
          a.motion.clear();
          a.motion.y = 0.0f;
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 4.0f;
          a.motion.velocityY = 8.0f;
          a.motion.accelerationY = 0.4000000059604645f;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x33b,(float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x),double(a.motion.y) + double(40.0f),u8(a.motion.facing),1);
          events.sound(6);
          consumeSpirit(0xfa);
        }
        if (i16(a.animation.cursor.index) != 10) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(8);
        return;
      case 0x1a0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * 0.1;
          a.motion.velocityY = double(a.motion.velocityY) * 0.1;
          a.response = 1;
          state.impactX = 0;
        }
        if (4 < i16(a.animation.cursor.index)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        }
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.velocityY = 0.0f;
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
        owned(0x33a,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        events.sound(6);
        consumeSpirit(0xfa);
        return;
      case 0x1a1:
        if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
          a.select_action(10);
          a.motion.y = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.velocityY = 0.0f;
          return;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * 0.1;
          a.motion.velocityY = double(a.motion.velocityY) * 0.1;
          a.response = 1;
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
        a.launchParameters[0] = 30.0f;
        a.launchParameters[1] = 8.0f;
        owned(0x32c,(float)((i8)u8(a.motion.facing) * 0xc) + double(a.motion.x),double(a.motion.y) + double(120.0f),u8(a.motion.facing),1);
        events.sound(1);
        consumeSpirit(200);
        return;
      case 500:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 0xc) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if (((7 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
           (step = random.range(0,100), step < 0x50)) {
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,0x46);
          position = (float)step + position + double(85.0f);
          step = random.range(0,0x4b);
          effectY = (float)((step + 0x5a) * (int)(i8)u8(a.motion.facing)) + double(a.motion.x);
          step = random.range(0,7);
          owned(step + 0x332,effectY,position,direction,layer);
          owned(0x352,(float)((i8)u8(a.motion.facing) * 0x82) + double(a.motion.x),double(a.motion.y) + double(120.0f),u8(a.motion.facing),1);
        }
        if (((double(0.0f) < double(a.motion.y)) && (i16(a.animation.cursor.index) < 0xb)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          a.motion.y = 0.0f;
          selectFrame(0xb);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 16.0f;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 54.0f;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 20.0f;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 11.0f;
          a.motion.velocityY = 3.0f;
          a.motion.accelerationY = 0.30000001192092896f;
          owned(0x34f,(float)((i8)u8(a.motion.facing) * 0x6e) + double(a.motion.x),double(a.motion.y) + double(120.0f),u8(a.motion.facing),1);
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(0);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(9);
        return;
      case 0x1f5:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 0xc) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if (((7 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
           (step = random.range(0,100), step < 0x50)) {
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,0x46);
          position = (float)step + position + double(85.0f);
          step = random.range(0,0x4b);
          effectY = (float)((step + 0x5a) * (int)(i8)u8(a.motion.facing)) + double(a.motion.x);
          step = random.range(0,7);
          owned(step + 0x332,effectY,position,direction,layer);
          owned(0x352,(float)((i8)u8(a.motion.facing) * 0x82) + double(a.motion.x),double(a.motion.y) + double(120.0f),u8(a.motion.facing),1);
        }
        if (((double(0.0f) < double(a.motion.y)) && (i16(a.animation.cursor.index) < 0xb)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          a.motion.y = 0.0f;
          selectFrame(0xb);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 16.0f;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 54.0f;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 20.0f;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 13.0f;
          a.motion.velocityY = 3.0f;
          a.motion.accelerationY = 0.20000000298023224f;
          owned(0x34f,(float)((i8)u8(a.motion.facing) * 0x6e) + double(a.motion.x),double(a.motion.y) + double(120.0f),u8(a.motion.facing),1);
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(0);
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
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.remainingHits = 1;
          a.response = 0;
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
        owned(0x33c,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(168.0f),u8(a.motion.facing),1);
        owned(0x33d,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(168.0f),u8(a.motion.facing),1);
        owned(0x33e,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(168.0f),u8(a.motion.facing),1);
        shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        shared(0x81,double(a.motion.x),double(a.motion.y),-(int)(i8)u8(a.motion.facing),1);
        events.shake(5.0f);
        events.sound(2);
        consumeSpirit(200);
        return;
      }
    }
    else if (step < 0x2bd) {
      if (step == 700) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.response = 0;
          a.remainingHits = 99;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x326,double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x326,double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
        }
        if (((10 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x12)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.5f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(0x12);
          a.motion.clear();
          a.motion.y = 0.0f;
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x73,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x12),double(a.motion.y) + double(50.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          consumeSpirit(500);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 3.0f;
          events.sound(10);
        }
        if (((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 5)) {
          state.impactX = i16(state.impactX) + 1;
          selectFrame(7);
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
      switch(step) {
      case 0x1fc:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if (((double(0.0f) < double(a.motion.y)) && (i16(a.animation.cursor.index) < 0xc)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          a.motion.y = 0.0f;
          selectFrame(0xc);
          return;
        }
        if (((2 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xc)) &&
           (step = random.range(0,100), step < 0x3c)) {
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,200);
          effectY = double(float(double(position)+200.0))-step;
          position = double(a.motion.x);
          step = random.range(0,200);
          position = double(float(double(position)+100.0))-step;
          step = random.range(0,7);
          owned(step + 0x332,position,effectY,direction,layer);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 3.0f;
          a.motion.velocityY = 13.0f;
          a.motion.accelerationY = 0.8999999761581421f;
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x34d,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(1);
        }
        if (i16(a.animation.cursor.index) != 0xc) {
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
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          a.motion.velocityX = 0.0f;
        }
        if (((double(0.0f) < double(a.motion.y)) && (i16(a.animation.cursor.index) < 0x10)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          a.motion.y = 0.0f;
          selectFrame(0x10);
          return;
        }
        if (((5 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x10)) &&
           (step = random.range(0,100), step < 0x50)) {
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,200);
          effectY = double(float(double(position)+200.0))-step;
          position = double(a.motion.x);
          step = random.range(0,200);
          position = double(float(double(position)+100.0))-step;
          step = random.range(0,7);
          owned(step + 0x332,position,effectY,direction,layer);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 16.0f;
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 54.0f;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 44.0f;
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 8.5f;
          a.motion.velocityY = 15.0f;
          a.motion.accelerationY = 0.949999988079071f;
        }
        if ((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x34d,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 0xc) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(1);
        }
        if (i16(a.animation.cursor.index) != 0x10) {
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
          state.impactX = 0;
        }
        if ((i16(a.combat.spiritBreak) == 0) && (a.response == 1)) {
          a.combat.spirit = 1000;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 2) {
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
        if (i16(a.animation.cursor.index) != 2) {
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
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        shared(0x37,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        events.system_sound(0x21);
        other.knockdownTime = 0;
        return;
      case 600:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x323,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          consumeSpirit(0);
        }
        if (i16(a.animation.cursor.index) != 8) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.launchParameters[0] = 1.0f;
        if (3 < i16(state.impactX)) {
          a.launchParameters[0] = 2.0f;
        }
        if (6 < i16(state.impactX)) {
          a.launchParameters[0] = 3.0f;
        }
        if (10 < i16(state.impactX)) {
          a.launchParameters[0] = 5.0f;
        }
        owned(0x324,(float)((i8)u8(a.motion.facing) * 0x78) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
        return;
      case 0x259:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
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
        a.launchParameters[0] = 0.0f;
        a.launchParameters[1] = double(a.motion.y) + double(96.0f);
        owned(0x322,(float)((i8)u8(a.motion.facing) * 0x72) + double(a.motion.x),double(a.motion.y) + double(96.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 1.0f;
        owned(0x322,(float)((i8)u8(a.motion.facing) * 0x72) + double(a.motion.x),double(a.motion.y) + double(96.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 2.0f;
        owned(0x322,(float)((i8)u8(a.motion.facing) * 0x72) + double(a.motion.x),double(a.motion.y) + double(96.0f),u8(a.motion.facing),1);
        a.launchParameters[0] = 3.0f;
        owned(0x322,(float)((i8)u8(a.motion.facing) * 0x72) + double(a.motion.x),double(a.motion.y) + double(96.0f),u8(a.motion.facing),1);
        consumeSpirit(300);
        events.sound(4);
        return;
      case 0x25a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
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
        events.shake(5.0f);
        owned(0x343,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),0);
        shared(3,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        shared(0x81,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y),-(int)(i8)u8(a.motion.facing),1);
        shared(0x81,(float)((i8)u8(a.motion.facing) * 0x28) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        consumeSpirit(200);
        events.sound(3);
        return;
      case 0x25b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.impactX = 0;
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
        events.shake(10.0f);
        owned(0x344,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),0);
        shared(3,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        shared(0x7f,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y),-(int)(i8)u8(a.motion.facing),1);
        shared(0x7f,(float)((i8)u8(a.motion.facing) * 0x28) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        consumeSpirit(200);
        events.sound(3);
        return;
      }
    }
    else {
      switch(step) {
      case 0x2bd:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.response = 0;
          a.remainingHits = 99;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x348,double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x348,double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
        }
        if (((6 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xb)) &&
           (step = random.range(0,100), step < 0x4b)) {
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,200);
          effectY = double(float(double(position)+100.0))-step;
          position = double(a.motion.x);
          step = random.range(0,0x191);
          position = double(float(double(position)+200.0))-step;
          step = random.range(0,7);
          owned(step + 0x332,position,effectY,direction,layer);
        }
        if (((10 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x12)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.5f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(0x12);
          a.motion.clear();
          a.motion.y = 0.0f;
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x74,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x12),double(a.motion.y) + double(50.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          consumeSpirit(500);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 2.5f;
          events.sound(0xb);
        }
        if (((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.impactX) < 0xe)) {
          state.impactX = i16(state.impactX) + 1;
          selectFrame(7);
        }
        if (i16(a.animation.cursor.index) != 0x12) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x10);
        return;
      case 0x2be:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x73,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          ownedVisual(0x327,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1,0,1);
          events.sound(0xc);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          ownedVisual(0x328,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1,0,1);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          ownedVisual(0x329,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1,0,1);
        }
        if (i16(a.animation.cursor.index) != 0xf) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0xd);
        consumeSpirit(500);
        shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x2bf:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x74,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
        }
        if (((0xb < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x1b)) &&
           (a.controls.current.buttons[3] == 0)) {
          selectFrame(0x1b);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          ownedVisual(0x349,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1,0,1);
          events.sound(0xe);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          ownedVisual(0x34a,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1,0,1);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          ownedVisual(0x34b,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1,0,1);
        }
        if (i16(a.animation.cursor.index) != 0x1d) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0xf);
        consumeSpirit(500);
        shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x2c0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.motion.clear();
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x73,(float)((i8)u8(a.motion.facing) * 0x14) + double(a.motion.x),double(a.motion.y) + double(120.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          a.remainingHits = 1;
          consumeSpirit(500);
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          shared(0x81,double(a.motion.x),double(a.motion.y),-(int)(i8)u8(a.motion.facing),1);
          if (a.response == 1) {
            events.shake(10.0f);
            events.sound(0x10);
            events.sound(0x11);
            owned(0x345,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(168.0f),u8(a.motion.facing),1);
            owned(0x346,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(168.0f),u8(a.motion.facing),1);
            owned(0x347,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(168.0f),u8(a.motion.facing),1);
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x2c1:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.response = 0;
          a.remainingHits = 1;
          a.motion.clear();
        }
        if (double(a.motion.velocityX) <= double(0.0f)) {
          a.motion.velocityX = 0.0f;
        }
        else {
          a.motion.velocityX = double(a.motion.velocityX) - double(0.75f);
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
        events.shake(10.0f);
        events.sound(0x10);
        events.sound(0x11);
        owned(0x345,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(168.0f),u8(a.motion.facing),1);
        owned(0x346,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(168.0f),u8(a.motion.facing),1);
        owned(0x347,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(168.0f),u8(a.motion.facing),1);
        return;
      case 0x2c2:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.motion.clear();
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x74,(float)((i8)u8(a.motion.facing) * 0x14) + double(a.motion.x),double(a.motion.y) + double(120.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          a.remainingHits = 1;
          consumeSpirit(500);
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 0xe) && (a.response == 1)) {
          events.sound(0x12);
          a.select_action(0x2c3);
          owned(0x350,(float)((i8)u8(a.motion.facing) * 0x5e) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 0xd) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        shared(0x81,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y),-(int)(i8)u8(a.motion.facing),1);
        shared(0x81,(float)((i8)u8(a.motion.facing) * 0x28) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x2c3:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          state.impactY = 0;
          a.response = 0;
          a.remainingHits = 2;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 7) && (a.response == 1)) {
          events.sound(0x13);
          owned(0x351,(float)((i8)u8(a.motion.facing) * 0x2d) + double(a.motion.x),double(a.motion.y) + double(120.0f),u8(a.motion.facing),1);
        }
        if (double(a.motion.velocityX) <= double(0.0f)) {
          a.motion.velocityX = 0.0f;
        }
        else {
          a.motion.velocityX = double(a.motion.velocityX) - double(0.75f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 0x10) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.shake(10.0f);
          a.response = 0;
          events.sound(0x14);
          events.sound(0x15);
          owned(0x345,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(168.0f),u8(a.motion.facing),1);
          owned(0x346,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(168.0f),u8(a.motion.facing),1);
          owned(0x347,(float)((i8)u8(a.motion.facing) * 0x4c) + double(a.motion.x),double(a.motion.y) + double(168.0f),u8(a.motion.facing),1);
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          shared(0x81,double(a.motion.x),double(a.motion.y),-(int)(i8)u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 12.0f;
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
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          fade(0);
          a.begin_spell(mode,replayMode,core);
          events.system_sound(0x17);
          owned(0x3e5,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (7 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(4);
        return;
      case 0x31c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.impactX = 0;
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if ((advancedFrame == i16(a.animation.cursor.frameCount)) &&
           (a.select_action(0), a.phaseVariants[a.combat.spellPhase] == (u8)0x4)) {
          state.chargeFrames = 0;
          a.commandTimers[2] = 0;
          a.launchParameters[1] = 0.0f;
          owned(0x3a3,double(a.motion.x),double(a.motion.y) + double(100.0f),1,1)
          ;
          if (state.difficulty == '\0') {
            for (local_20 = 0; local_20 < 6; local_20 = local_20 + 1) {
              a.launchParameters[0] = (float)(local_20 * 0x3c);
              owned(0x397,double(a.motion.x),double(a.motion.y) + double(100.0f),1,1);
            }
          }
          if (state.difficulty == '\x01') {
            for (local_24 = 0; local_24 < 6; local_24 = local_24 + 1) {
              a.launchParameters[0] = (float)(local_24 * 0x3c);
              owned(0x397,double(a.motion.x),double(a.motion.y) + double(100.0f),1,1);
            }
          }
          if (state.difficulty == '\x02') {
            for (local_28 = 0; local_28 < 8; local_28 = local_28 + 1) {
              a.launchParameters[0] = (float)(local_28 * 0x2d);
              owned(0x397,double(a.motion.x),double(a.motion.y) + double(100.0f),1,1);
            }
          }
          if (state.difficulty == '\x03') {
            for (local_2c = 0; local_2c < 8; local_2c = local_2c + 1) {
              a.launchParameters[0] = (float)(local_2c * 0x2d);
              owned(0x397,double(a.motion.x),double(a.motion.y) + double(100.0f),1,1);
            }
          }
        }
        if ((i16(a.animation.cursor.index) == 0x14) && (i16(a.animation.cursor.elapsed) == 0)) {
          fade(0);
          a.begin_spell(mode,replayMode,core);
          events.system_sound(0x17);
        }
        if (i16(a.animation.cursor.index) != 0x18) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (7 < i16(state.impactX)) {
          return;
        }
        state.impactX = i16(state.impactX) + 1;
        selectFrame(0x16);
        a.transitionReady = (u8)0x1;
        return;
      }
    }
switchD_005d741a_default:
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
    goto LAB_005e1cc6;
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
    goto LAB_005e1cc6;
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
    goto LAB_005e1cc6;
  case 5:
    a.motion.velocityX = -4.5f;
LAB_005e1cc6:
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
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.duration) <= i16(a.animation.cursor.elapsed)) {
          a.animation.cursor.elapsed = 0;
          a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
          if (i16(a.animation.cursor.index) == 2) {
            a.motion.velocityX = 0.0f;
            a.motion.velocityY = 14.0f;
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
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.duration) <= i16(a.animation.cursor.elapsed)) {
          a.animation.cursor.elapsed = 0;
          a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
          if (i16(a.animation.cursor.index) == 2) {
            a.motion.velocityX = 6.0f;
            a.motion.velocityY = 14.0f;
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
            a.motion.velocityY = 14.0f;
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
        a.select_action(2);
      }
    }
    break;
  default:
    goto switchD_005d741a_default;
  }
  return;

}
}
