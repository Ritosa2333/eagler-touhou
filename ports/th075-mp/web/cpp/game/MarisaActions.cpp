#include "MarisaState.hpp"
namespace th075 {
void advance_marisa_action(FighterActor& a,FighterActor& other,MarisaState& state,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){

    const auto selectFrame=[&](i16 frame){a.animation.set(a.animation.cursor.action,frame,0);};
    const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters);};
    const auto shared=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Shared,id,x,y,facing,layer,a.launchParameters);};
    const auto ownedVisual=[&](i16 id,float x,float y,i8 facing,u8 layer,u8 visual,u8 order){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters,order,visual?&a.effectVisual:nullptr);};
    const auto consumeSpirit=[&](i16 amount){if(a.combat.spiritBreak<1){a.combat.spirit=i16(u16(a.combat.spirit)-u16(amount));if(a.combat.spirit<1&&a.combat.spiritBreak<1){events.system_sound(43);a.combat.spirit=0;a.combat.spiritBreak=1000;}}else a.combat.spiritBreak=1000;};
    const auto trail=[&](i32 frames,i8 interval,u32 mask){a.combat.noticeTimer=frames;a.trails.interval=interval;a.trails.colorMask=mask;};
    const auto hitFreeze=[&](i16 frames){if(frames>=0)a.motion.hitFreeze=frames;};
    const auto fade=[&](u8 kind){a.combat.fade=240;a.fadeKind=kind;};

  float position;
  i8 boundary;
  u16 impactWord;
  i16 advancedFrame;
  int step;
  u32 direction;
  
  float effectY;
  u32 layer;
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
        if ((i16(a.animation.cursor.index) == 3) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(4);
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
        if (((i16(a.animation.cursor.index) < 0xb) && (double(0.0f) < double(a.motion.y))) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(0xb);
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
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 8.0f;
          a.motion.velocityX = 13.5f;
          a.motion.accelerationY = 0.6000000238418579f;
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(9);
        return;
      case 0x5e:
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
          a.motion.velocityX = -13.5f;
          a.motion.accelerationY = 0.6000000238418579f;
        }
        if (i16(a.animation.cursor.index) != 0xc) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(10);
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
           (((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
            ((a.controls.current.buttons[2] != 0 || (a.controls.current.buttons[3] != 0)))))) {
          state.actionTicks = 1;
        }
        if (((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 0) &&
           ((((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
             (a.controls.current.buttons[2] != 0)) || (a.controls.current.buttons[3] != 0)))) {
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
        if ((i16(a.animation.cursor.index) == 3) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(4);
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
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        a.motion.opponentPush = (float)-(int)i16(state.actionTicks) * double(0.1f);
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
        a.motion.opponentPush = (float)-(int)i16(state.actionTicks) * double(0.1f);
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
        if ((double(10.5f) < double(a.motion.velocityX)) &&
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
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 15.0f;
        shared(0x7d,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        shared(0x7e,double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        events.system_sound(0x1f);
        return;
      case 0xc9:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -11.0f;
          a.motion.velocityY = 5.0f;
          a.motion.accelerationY = 0.6000000238418579f;
          shared(0x7d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x50),double(a.motion.y) + double(80.0f),-(int)(i8)u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(80.0f),-(int)(i8)u8(a.motion.facing),1);
          events.system_sound(0x1f);
        }
        if ((((double(0.0f) < double(a.motion.y)) && (1 < i16(a.animation.cursor.index))) &&
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
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 10.5f;
          a.motion.velocityY = 5.0f;
          a.motion.accelerationY = 0.6000000238418579f;
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
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -10.5f;
          a.motion.velocityY = 5.0f;
          a.motion.accelerationY = 0.6000000238418579f;
          shared(0x7d,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x50),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          shared(0x7e,double(a.motion.x),double(a.motion.y) + double(100.0f),-(int)(i8)u8(a.motion.facing),1);
          events.system_sound(0x1f);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (0 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
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
        if (i16(a.animation.cursor.index) == 4) {
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
        if ((i16(a.animation.cursor.index) == 4) && (double(a.motion.velocityY) < double(4.0f))) {
          selectFrame(5);
        }
        a.animation.cursor.elapsed = i16(a.animation.cursor.elapsed) + 1;
        if (i16(a.animation.cursor.elapsed) < i16(a.animation.cursor.duration)) {
          return;
        }
        a.animation.cursor.elapsed = 0;
        a.animation.cursor.index = i16(a.animation.cursor.index) + 1;
        if (i16(a.animation.cursor.index) == 4) {
          a.motion.velocityX = 10.0f;
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
        if (i16(a.animation.cursor.index) == 4) {
          a.motion.velocityX = -10.0f;
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
          owned(0x3e6,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          events.system_sound(0x2c);
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
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          a.motion.velocityX = 12.5f;
        }
        if ((0 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1d);
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(2);
        }
        if (i16(a.animation.cursor.index) != 3) {
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
          return;
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x323,(float)((i8)u8(a.motion.facing) * 0x26) + double(a.motion.x),double(a.motion.y) + double(3.0f),u8(a.motion.facing),1);
        owned(0x324,(float)((i8)u8(a.motion.facing) * 0x26) + double(a.motion.x),double(a.motion.y) + double(3.0f),u8(a.motion.facing),1);
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
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.remainingHits = 1;
          a.response = 0;
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
          events.sound(0x13);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 15.0f;
          a.motion.velocityY = 7.0f;
          a.motion.accelerationY = 0.8999999761581421f;
          events.sound(0x12);
        }
        if (i16(a.animation.cursor.index) != 9) {
          return;
        }
        if (double(a.motion.y) <= double(0.0f)) {
          return;
        }
        selectFrame(i16(a.animation.cursor.index) + -2);
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
          state.actionTicks = 0;
        }
        if (i16(a.animation.cursor.index) != 5) {
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 12.0f;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1d);
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
    }
    else if (step < 0x1f5) {
      if (step == 500) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        step = random.range(0,100);
        if (((step < 0x28) && (3 < i16(a.animation.cursor.index))) && (i16(a.animation.cursor.index) < 8)
           ) {
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,0x8c);
          effectY = double(step)+float(double(position)+40.0);
          position = double(a.motion.x);
          step = random.range(0,0x3c);
          owned(0x330,double(float(double(position)+30.0))-step,effectY,direction,layer);
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 8.0f;
          a.motion.velocityY = 14.0f;
          a.motion.accelerationY = 0.800000011920929f;
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 0xc) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(10);
        return;
      }
      switch(step) {
      case 0x187:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.carryMomentum = (u8)0x0;
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = 12.5f;
        }
        if ((0 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
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
        events.system_sound(0x1d);
        return;
      case 0x188:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          a.motion.velocityX = 12.5f;
        }
        if ((0 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.5f),
           double(a.motion.velocityX) < double(0.0f))) {
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
        events.system_sound(0x1d);
        return;
      case 0x189:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.carryMomentum = (u8)0x0;
          a.remainingHits = 1;
          a.response = 0;
          a.motion.accelerationY = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 12.5f;
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 13.5f;
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
          state.actionTicks = 0;
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
          state.actionTicks = 0;
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
        owned(800,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        consumeSpirit(0x96);
        events.sound(6);
        return;
      case 0x191:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x321,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(124.0f),u8(a.motion.facing),1);
        }
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
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x321,(float)((i8)u8(a.motion.facing) * 99) + double(a.motion.x),double(a.motion.y) + double(66.0f),u8(a.motion.facing),1);
          consumeSpirit(0x96);
          events.sound(6);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(2);
        return;
      case 0x194:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.1f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.1f);
          a.response = 1;
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
        owned(0x322,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(116.0f),u8(a.motion.facing),1);
        consumeSpirit(0x96);
        events.sound(6);
        return;
      case 0x196:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = (float)((int)i16(state.actionTicks) << 3);
          owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x60) + double(a.motion.x),double(a.motion.y) + double(104.0f),u8(a.motion.facing),1);
          events.sound(1);
          consumeSpirit(100);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (a.controls.current.buttons[2] == 0) {
          return;
        }
        if (4 < i16(state.actionTicks)) {
          return;
        }
        if (i16(a.combat.spirit) < 1) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        a.launchParameters[0] = (float)((int)i16(state.actionTicks) << 3);
        owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x60) + double(a.motion.x),double(a.motion.y) + double(104.0f),u8(a.motion.facing),1);
        events.sound(1);
        consumeSpirit(0x14);
        selectFrame(6);
        return;
      case 0x197:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          step = i16(state.actionTicks) * 8 + 0x1e;
          a.launchParameters[0] = (float)step;
          owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x5a) + double(a.motion.x),double(a.motion.y) + double(124.0f),u8(a.motion.facing),1);
          events.sound(1);
          consumeSpirit(100);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (a.controls.current.buttons[2] == 0) {
          return;
        }
        if (4 < i16(state.actionTicks)) {
          return;
        }
        if (i16(a.combat.spirit) < 1) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        step = i16(state.actionTicks) * 8 + 0x1e;
        a.launchParameters[0] = (float)step;
        owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x5a) + double(a.motion.x),double(a.motion.y) + double(124.0f),u8(a.motion.facing),1);
        events.sound(1);
        consumeSpirit(0x14);
        selectFrame(6);
        return;
      case 0x198:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
          consumeSpirit(100);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = (float)((int)i16(state.actionTicks) << 2);
          owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x5c) + double(a.motion.x),double(a.motion.y) + double(70.0f),u8(a.motion.facing),1);
          events.sound(1);
          consumeSpirit(0x14);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (a.controls.current.buttons[2] == 0) {
          return;
        }
        if (4 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(6);
        return;
      case 0x199:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          step = i16(state.actionTicks) * -8 + 0x2d;
          a.launchParameters[0] = (float)step;
          owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x51) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
          events.sound(1);
          consumeSpirit(100);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (a.controls.current.buttons[2] == 0) {
          return;
        }
        if (4 < i16(state.actionTicks)) {
          return;
        }
        if (i16(a.combat.spirit) < 1) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        step = i16(state.actionTicks) * -8 + 0x2d;
        a.launchParameters[0] = (float)step;
        owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x51) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        events.sound(1);
        consumeSpirit(0x14);
        selectFrame(6);
        return;
      case 0x19a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.actionTicks = 0;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -3.0f;
          a.motion.velocityY = 8.5f;
          a.motion.accelerationY = 0.5f;
          a.launchParameters[0] = (float)(i16(state.actionTicks) * -8);
          owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(114.0f),u8(a.motion.facing),1);
          events.sound(1);
          consumeSpirit(100);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (a.controls.current.buttons[2] == 0) {
          return;
        }
        if (4 < i16(state.actionTicks)) {
          return;
        }
        if (i16(a.combat.spirit) < 1) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        a.launchParameters[0] = (float)(i16(state.actionTicks) * -8);
        owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(114.0f),u8(a.motion.facing),1);
        events.sound(1);
        consumeSpirit(0x14);
        selectFrame(6);
        return;
      case 0x19b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.actionTicks = 0;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -3.0f;
          a.motion.velocityY = 8.5f;
          a.motion.accelerationY = 0.5f;
          step = i16(state.actionTicks) * 8 + -0x2d;
          a.launchParameters[0] = (float)step;
          owned(0x32b,(float)((int)(i8)u8(a.motion.facing) << 6) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
          events.sound(1);
          consumeSpirit(100);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (a.controls.current.buttons[2] == 0) {
          return;
        }
        if (4 < i16(state.actionTicks)) {
          return;
        }
        if (i16(a.combat.spirit) < 1) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        step = i16(state.actionTicks) * 8 + -0x2d;
        a.launchParameters[0] = (float)step;
        owned(0x32b,(float)((int)(i8)u8(a.motion.facing) << 6) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        events.sound(1);
        consumeSpirit(0x14);
        selectFrame(6);
        return;
      case 0x19c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
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
        a.launchParameters[0] = 0.0f;
        owned(0x32c,(float)((i8)u8(a.motion.facing) * 0x60) + double(a.motion.x),double(a.motion.y) + double(104.0f),u8(a.motion.facing),1);
        events.sound(1);
        consumeSpirit(200);
        return;
      case 0x19d:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
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
        a.launchParameters[0] = 20.0f;
        owned(0x32c,(float)((i8)u8(a.motion.facing) * 0x5a) + double(a.motion.x),double(a.motion.y) + double(124.0f),u8(a.motion.facing),1);
        events.sound(1);
        consumeSpirit(200);
        return;
      case 0x19e:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = 0.0f;
          owned(0x32c,(float)((i8)u8(a.motion.facing) * 0x5c) + double(a.motion.x),double(a.motion.y) + double(70.0f),u8(a.motion.facing),1);
          events.sound(1);
          consumeSpirit(200);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (a.controls.current.buttons[2] == 0) {
          return;
        }
        if (4 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(6);
        return;
      case 0x19f:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
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
        a.launchParameters[0] = 55.0f;
        owned(0x32c,(float)((i8)u8(a.motion.facing) * 0x51) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        events.sound(1);
        consumeSpirit(200);
        return;
      case 0x1a0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.launchParameters[0] = 0.0f;
        owned(0x32c,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(114.0f),u8(a.motion.facing),1);
        events.sound(1);
        consumeSpirit(200);
        return;
      case 0x1a1:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.launchParameters[0] = -20.0f;
        owned(0x32c,(float)((int)(i8)u8(a.motion.facing) << 6) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        events.sound(1);
        consumeSpirit(200);
        return;
      case 0x1a2:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0xfa);
          a.launchParameters[0] = 5.0f;
          owned(0x332,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(159.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 6.0f;
          owned(0x332,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(159.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 7.0f;
          owned(0x332,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(159.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 8.0f;
          owned(0x332,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(159.0f),u8(a.motion.facing),1);
          events.sound(5);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x1a3:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0xfa);
          a.launchParameters[0] = 10.0f;
          owned(0x332,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(159.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 11.0f;
          owned(0x332,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(159.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 12.0f;
          owned(0x332,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(159.0f),u8(a.motion.facing),1);
          a.launchParameters[0] = 13.0f;
          owned(0x332,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(159.0f),u8(a.motion.facing),1);
          events.sound(5);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      }
    }
    else if (step < 0x2c3) {
      if (step == 0x2c2) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 0;
          a.remainingHits = 6;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x74,double(a.motion.x) - double(0.0f),double(a.motion.y) + double(70.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x343,double(a.motion.x) - double(0.0f),double(a.motion.y),u8(a.motion.facing),1);
        consumeSpirit(500);
        state.chargeCount = 6;
        state.chargeRemaining = 6;
        state.spellCounters[0] = 0x1a4;
        events.sound(0xd);
        return;
      }
      switch(step) {
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
        step = random.range(0,100);
        if (((step < 0x28) && (3 < i16(a.animation.cursor.index))) && (i16(a.animation.cursor.index) < 8)
           ) {
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,0x8c);
          effectY = double(step)+float(double(position)+40.0);
          position = double(a.motion.x);
          step = random.range(0,0x3c);
          owned(0x330,double(float(double(position)+30.0))-step,effectY,direction,layer);
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 18.0f;
          a.motion.velocityY = 16.0f;
          a.motion.accelerationY = 0.800000011920929f;
          shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 0xc) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(10);
        return;
      case 0x208:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 1;
          state.actionTicks = 0;
        }
        if ((6 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.7f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 12.5f;
        }
        if (((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) < 0)) {
          state.actionTicks = i16(state.actionTicks) + 1;
          selectFrame(4);
        }
        if (i16(a.animation.cursor.index) != 9) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(3);
        return;
      case 0x20a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 1;
          state.actionTicks = 0;
        }
        if (((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xf)) &&
           ((a.response != 0 ||
            ((boundary = a.motion.horizontal_limit(other.motion), boundary != '\0' &&
             (((double(640.0f) < double(a.motion.x) && (u8(a.motion.facing) == (u8)0x1)) ||
              ((double(a.motion.x) < double(640.0f) && (u8(a.motion.facing) == (u8)0xff))))))))))
        {
          selectFrame(0xf);
          a.motion.velocityX = -3.0f;
          a.motion.velocityY = 14.0f;
          a.motion.accelerationY = 0.8999999761581421f;
          return;
        }
        if (((0xe < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x1b)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          selectFrame(0x1b);
          a.motion.y = 0.0f;
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 14.5f;
          events.sound(10);
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          selectFrame(9);
          state.actionTicks = i16(state.actionTicks) + 1;
          if (0 < i16(state.actionTicks)) {
            a.select_action(0x20e);
            return;
          }
        }
        if (i16(a.animation.cursor.index) != 0x1b) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x19);
        return;
      case 0x20b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 1;
          state.actionTicks = 0;
        }
        if (((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xf)) &&
           ((a.response != 0 ||
            ((boundary = a.motion.horizontal_limit(other.motion), boundary != '\0' &&
             (((double(640.0f) < double(a.motion.x) && (u8(a.motion.facing) == (u8)0x1)) ||
              ((double(a.motion.x) < double(640.0f) && (u8(a.motion.facing) == (u8)0xff))))))))))
        {
          selectFrame(0xf);
          a.motion.velocityX = -3.0f;
          a.motion.velocityY = 15.0f;
          a.motion.accelerationY = 0.8999999761581421f;
          return;
        }
        if (((0xe < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x1b)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          selectFrame(0x1b);
          a.motion.y = 0.0f;
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 18.5f;
          events.sound(10);
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          selectFrame(9);
          state.actionTicks = i16(state.actionTicks) + 1;
          if (1 < i16(state.actionTicks)) {
            a.select_action(0x20e);
            return;
          }
        }
        if (i16(a.animation.cursor.index) != 0x1b) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x19);
        return;
      case 0x20c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 1;
          state.actionTicks = 0;
        }
        if (((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xf)) &&
           ((a.response != 0 ||
            ((boundary = a.motion.horizontal_limit(other.motion), boundary != '\0' &&
             (((double(640.0f) < double(a.motion.x) && (u8(a.motion.facing) == (u8)0x1)) ||
              ((double(a.motion.x) < double(640.0f) && (u8(a.motion.facing) == (u8)0xff))))))))))
        {
          selectFrame(0xf);
          a.motion.velocityX = -3.0f;
          a.motion.velocityY = 14.0f;
          a.motion.accelerationY = 0.8999999761581421f;
          return;
        }
        if (((0xe < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x1b)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          selectFrame(0x1b);
          a.motion.y = 0.0f;
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 14.5f;
          events.sound(10);
          shared(0x80,(float)((i8)u8(a.motion.facing) * 0x96) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          selectFrame(9);
          state.actionTicks = i16(state.actionTicks) + 1;
          if (0 < i16(state.actionTicks)) {
            a.select_action(0x20e);
            return;
          }
        }
        if (i16(a.animation.cursor.index) != 0x1b) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x19);
        return;
      case 0x20d:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 1;
          state.actionTicks = 0;
        }
        if (((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xf)) &&
           ((a.response != 0 ||
            ((boundary = a.motion.horizontal_limit(other.motion), boundary != '\0' &&
             (((double(640.0f) < double(a.motion.x) && (u8(a.motion.facing) == (u8)0x1)) ||
              ((double(a.motion.x) < double(640.0f) && (u8(a.motion.facing) == (u8)0xff))))))))))
        {
          selectFrame(0xf);
          a.motion.velocityX = -3.0f;
          a.motion.velocityY = 15.0f;
          a.motion.accelerationY = 0.8999999761581421f;
          return;
        }
        if (((0xe < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x1b)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          selectFrame(0x1b);
          a.motion.y = 0.0f;
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 18.5f;
          events.sound(10);
          shared(0x80,(float)((i8)u8(a.motion.facing) * 0x96) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          selectFrame(9);
          state.actionTicks = i16(state.actionTicks) + 1;
          if (1 < i16(state.actionTicks)) {
            a.select_action(0x20e);
            return;
          }
        }
        if (i16(a.animation.cursor.index) != 0x1b) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x19);
        return;
      case 0x20e:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (double(a.motion.y) == double(0.0f)) {
            a.response = 1;
          }
          a.motion.clear();
          a.motion.velocityX = 10.0f;
          a.motion.velocityY = 7.0f;
          a.motion.accelerationY = 0.5f;
        }
        if ((i16(a.animation.cursor.index) < 10) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          selectFrame(10);
          a.motion.y = 0.0f;
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 10) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(8);
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
          state.actionTicks = 0;
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
        events.system_sound(0x21);
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
      case 600:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = 0.0f;
          owned(0x32c,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          events.sound(4);
          consumeSpirit(200);
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
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = 30.0f;
          owned(0x32c,(float)((i8)u8(a.motion.facing) * 0x5a) + double(a.motion.x),double(a.motion.y) + double(124.0f),u8(a.motion.facing),1);
          events.sound(4);
          consumeSpirit(200);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x25a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          a.response = 1;
          state.actionTicks = 0;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = 0.0f;
          owned(0x32c,(float)((i8)u8(a.motion.facing) * 0x4a) + double(a.motion.x),double(a.motion.y) + double(115.0f),u8(a.motion.facing),1);
          events.sound(4);
          consumeSpirit(200);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x25b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          a.response = 1;
          state.actionTicks = 0;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = -30.0f;
          owned(0x32c,(float)((int)(i8)u8(a.motion.facing) << 6) + double(a.motion.x),double(a.motion.y) + double(92.0f),u8(a.motion.facing),1);
          events.sound(4);
          consumeSpirit(200);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
          events.sound(5);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
          events.sound(5);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x270:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0xfa);
          a.launchParameters[0] = 20.0f;
          a.launchParameters[1] = 150.0f;
          owned(0x332,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(159.0f),u8(a.motion.facing),1);
          events.sound(5);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x271:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0xfa);
          a.launchParameters[0] = 60.0f;
          a.launchParameters[1] = 150.0f;
          owned(0x332,(float)((i8)u8(a.motion.facing) * 0x46) + double(a.motion.x),double(a.motion.y) + double(159.0f),u8(a.motion.facing),1);
          events.sound(5);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 700:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 10)) {
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x73,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x12),double(a.motion.y) + double(92.0f),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x334,(float)((int)(i8)u8(a.motion.facing) << 5) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
          owned(0x329,(float)((int)(i8)u8(a.motion.facing) << 6) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
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
        a.select_action(0x2bd);
        owned(0x34f,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x50),double(a.motion.y) + double(130.0f),u8(a.motion.facing),0);
        events.sound(0);
        return;
      case 0x2bd:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x32a,(float)((int)(i8)u8(a.motion.facing) << 6) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        }
        consumeSpirit(10);
        if (i16(a.combat.spirit) < 1) {
          state.actionTicks = i16(state.actionTicks) + 1;
        }
        if (0x14 < i16(state.actionTicks)) {
          state.actionTicks = 0;
          a.select_action(700);
          selectFrame(6);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(0);
        return;
      case 0x2bf:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 0;
          a.remainingHits = 6;
          a.motion.clear();
          state.actionStage = 0;
          state.volleyIndex = 0;
        }
        if ((a.response == 1) && (i16(state.actionStage) == 0)) {
          state.actionStage = i16(state.actionStage) + 1;
          events.sound(0xb);
        }
        boundary = a.motion.horizontal_limit(other.motion);
        if (((((boundary != '\0') && (a.response != 1)) &&
             (((u8(a.motion.facing) == (u8)0x1 && (double(640.0f) < double(a.motion.x))) ||
              ((u8(a.motion.facing) == (u8)0xff && (double(a.motion.x) < double(640.0f))))))) ||
            ((i16(a.remainingHits) == 0 && (a.response == 2)))) &&
           (i16(a.animation.cursor.index) < 0x17)) {
          selectFrame(0x17);
          a.motion.velocityX = -2.5f;
          a.motion.velocityY = 15.0f;
          a.motion.accelerationY = 0.5f;
        }
        if ((0xe < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x17)) {
          owned(0x34c,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x5a),double(a.motion.y) + double(95.0f),-(int)(i8)u8(a.motion.facing),1);
        }
        if ((((double(0.0f) < double(a.motion.y)) && (0x16 < i16(a.animation.cursor.index))) &&
            (i16(a.animation.cursor.index) < 0x23)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          a.motion.y = 0.0f;
          selectFrame(0x23);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          events.system_sound(0x17);
          shared(0x73,double(a.motion.x) - double(0.0f),double(a.motion.y) + double(70.0f),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 0x14;
          a.motion.velocityX = 25.0f;
          events.sound(10);
          consumeSpirit(500);
          owned(0x33b,double(a.motion.x) - double(0.0f),double(a.motion.y),u8(a.motion.facing),1);
          owned(0x34e,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x6e),double(a.motion.y) + double(95.0f),u8(a.motion.facing),1);
          if (double(a.motion.y) == double(0.0f)) {
            shared(0x7f,double(a.motion.x),0.0f,u8(a.motion.facing),1);
          }
        }
        if (((i16(a.animation.cursor.elapsed) == 0) && (0xe < i16(a.animation.cursor.index))) &&
           (i16(a.animation.cursor.index) < 0x17)) {
          if (double(a.motion.y) == double(0.0f)) {
            shared(0x80,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 200),double(a.motion.y),u8(a.motion.facing),1);
          }
          if (a.response == 0) {
            a.response = 0;
            a.motion.velocityX = 25.0f;
          }
          else {
            a.response = 0;
            a.motion.velocityX = 10.0f;
          }
        }
        if ((i16(a.animation.cursor.index) == 0x17) && (i16(a.animation.cursor.elapsed) == 0)) {
          selectFrame(0xf);
        }
        if (i16(a.animation.cursor.index) != 0x23) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x21);
        return;
      case 0x2c0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 0;
          a.remainingHits = 6;
          a.motion.clear();
          state.actionTicks = 0;
          state.actionStage = 0;
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          a.motion.y = 0.0f;
          selectFrame(7);
          return;
        }
        if ((i16(a.animation.cursor.index) == 3) &&
           (state.actionTicks = i16(state.actionTicks) + 1,
           0x3c < i16(state.actionTicks))) {
          state.actionTicks = 0;
          selectFrame(4);
          events.sound(0xc);
          consumeSpirit(500);
          owned(0x339,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 200),0.0f,u8(a.motion.facing),1);
          owned(0x33a,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 200),0.0f,u8(a.motion.facing),1);
          return;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(state.actionStage) == 1)) {
          selectFrame(5);
          a.motion.clear();
          a.motion.accelerationY = 0.5f;
          a.motion.y = 1600.0f;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x74,double(a.motion.x) - double(0.0f),double(a.motion.y) + double(70.0f),u8(a.motion.facing),1);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          ownedVisual(0x338,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1,0,1);
          a.motion.velocityX = -30.0f;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          selectFrame(3);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          selectFrame(4);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(5);
        return;
      case 0x2c1:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 0;
          a.remainingHits = 6;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
          shared(0x73,double(a.motion.x) - double(0.0f),double(a.motion.y) + double(70.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x342,double(a.motion.x) - double(0.0f),double(a.motion.y),u8(a.motion.facing),1);
        consumeSpirit(500);
        state.chargeCount = 4;
        state.chargeRemaining = 4;
        state.spellCounters[0] = 0x1a4;
        events.sound(0xd);
        return;
      }
    }
    else {
      switch(step) {
      case 0x2c3:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x73,double(a.motion.x) - double(0.0f),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0xe);
        consumeSpirit(500);
        return;
      case 0x2c4:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x73,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x14),double(a.motion.y) + double(85.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0xf);
        consumeSpirit(500);
        state.spellCounters[0] = 300;
        return;
      case 0x2c5:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
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
        events.sound(0x10);
        return;
      case 0x2c6:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.1f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.1f);
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
        events.sound(0x10);
        return;
      case 0x2c8:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x345,(float)((int)(i8)u8(a.motion.facing) << 5) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
          owned(0x346,(float)((int)(i8)u8(a.motion.facing) << 6) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x74,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x12),double(a.motion.y) + double(92.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.select_action(0x2c9);
        owned(0x34f,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x50),double(a.motion.y) + double(130.0f),u8(a.motion.facing),0);
        events.sound(0x11);
        return;
      case 0x2c9:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x347,(float)((int)(i8)u8(a.motion.facing) << 6) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        }
        events.shake(5.0f);
        if ((double(3.0f) <= double(a.combat.spellExtent)) || (0x13 < i16(a.combat.spirit))) {
          a.combat.spellExtent = double(a.combat.spellExtent) - double(3.0f);
          consumeSpirit(0x14);
        }
        else {
          a.combat.spellExtent = 0.0f;
          state.actionTicks = i16(state.actionTicks) + 1;
        }
        if (0x32 < i16(state.actionTicks)) {
          state.actionTicks = 0;
          a.select_action(700);
          selectFrame(6);
          if (i16(other.combat.health) < 1) {
            return;
          }
          a.combat.spellTimer = 1;
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(0);
        return;
      case 0x2ca:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x74,double(a.motion.x) - double(0.0f),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0xe);
        consumeSpirit(500);
        return;
      case 0x2cb:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x74,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x14),double(a.motion.y) + double(85.0f),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(6);
        consumeSpirit(500);
        state.spellCounters[0] = 300;
        return;
      case 0x2cc:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
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
        events.sound(6);
        return;
      case 0x2cd:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 1;
          a.motion.velocityX = double(a.motion.velocityX) * double(0.1f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.1f);
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
        events.sound(6);
        return;
      case 0x2ce:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x2cf:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.actionTicks = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x2d0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.1f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.1f);
          a.response = 1;
          state.actionTicks = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x2d1:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.1f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.1f);
          a.response = 1;
          state.actionTicks = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x2db:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          a.response = 0;
          a.remainingHits = 1;
          a.motion.clear();
          state.actionStage = 0;
          state.volleyIndex = 0;
        }
        boundary = a.motion.horizontal_limit(other.motion);
        if ((boundary != '\0') &&
           ((((double(640.0f) < double(a.motion.x) && (u8(a.motion.facing) == (u8)0x1)) ||
             ((double(a.motion.x) < double(640.0f) && (u8(a.motion.facing) == (u8)0xff)))) &&
            (i16(a.animation.cursor.index) < 0x11)))) {
          selectFrame(0x11);
          a.motion.velocityX = -4.0f;
          a.motion.velocityY = 17.0f;
          a.motion.accelerationY = 0.4000000059604645f;
        }
        if (((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x11)) &&
           (state.actionStage = i16(state.actionStage) + -1,
           i16(state.actionStage) < 1)) {
          owned(0x385,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x8c),double(a.motion.y) + double(110.0f),u8(a.motion.facing),1);
          state.actionStage = -(state.difficulty + -3);
        }
        if ((double(0.0f) < double(a.motion.y)) && (0x10 < i16(a.animation.cursor.index))) {
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          a.motion.velocityX = double(a.motion.velocityX) + double(0.5f);
          if (-double(5.0f) < double(a.motion.velocityX)) {
            a.motion.velocityX = -5.0f;
          }
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.motion.clear();
            a.motion.y = 0.0f;
            selectFrame(0x1a);
            return;
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          advancedFrame = random.range(0,0x3c);
          a.commandTimers[2] = advancedFrame + 0xb4 + state.difficulty * -0x1e;
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 25.0f;
          events.sound(10);
          owned(0x33b,double(a.motion.x) - double(0.0f),double(a.motion.y),u8(a.motion.facing),1);
          position = double(a.motion.y);
          if (position == double(0.0f)) {
            shared(0x7f,double(a.motion.x),0.0f,u8(a.motion.facing),1);
          }
        }
        if (((i16(a.animation.cursor.index) == 0x11) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.actionTicks) < 3)) {
          state.actionTicks = i16(state.actionTicks) + 1;
          selectFrame(9);
        }
        if (i16(a.animation.cursor.index) != 0x1a) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x18);
        return;
      case 0x2e5:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.actionTicks = 0;
          state.actionStage = 0;
          a.motion.clear();
        }
        if (((5 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 8)) &&
           (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = random.range(0,4);
          state.actionStage = impactWord;
          step = random.range(0,0x167);
          a.launchParameters[0] = (float)step;
          for (volley = 0; volley < state.difficulty + 1; volley = volley + 1) {
            events.sound(0x14);
            a.launchParameters[2] = 3.0f;
            a.launchParameters[1] = (float)(volley * 7);
            owned(i16(state.actionStage) + 0x38e,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x19),double(a.motion.y) + double(165.0f),u8(a.motion.facing),1);
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          advancedFrame = random.range(0x3c,0x78);
          a.commandTimers[2] = advancedFrame + state.difficulty * -0x14;
        }
        if (((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) &&
           ((int)i16(state.actionTicks) < state.difficulty * 3 + 10)) {
          state.actionTicks = i16(state.actionTicks) + 1;
          selectFrame(6);
        }
        if (i16(a.animation.cursor.index) != 10) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (5 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(8);
        return;
      case 0x2e6:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.actionTicks = 0;
          state.actionStage = 0;
          a.motion.clear();
        }
        if (((0x11 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x16)) &&
           (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = random.range(0,4);
          state.actionStage = impactWord;
          step = random.range(0,0x167);
          a.launchParameters[0] = (float)step;
          for (emission = 0; emission < state.difficulty + 1; emission = emission + 1) {
            events.sound(0x14);
            a.launchParameters[1] = (float)(emission * 5);
            a.launchParameters[2] = 3.0f;
            owned(i16(state.actionStage) + 0x38e,(float)((i8)u8(a.motion.facing) * 0x41) + double(a.motion.x),double(a.motion.y) + double(85.0f),u8(a.motion.facing),1);
          }
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(0x1c);
          a.motion.y = 0.0f;
          a.motion.clear();
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          advancedFrame = random.range(0x3c,0xb4);
          a.commandTimers[2] = advancedFrame + state.difficulty * -0x14;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          step = random.range(0,8);
          a.motion.velocityX = (float)step + double(4.0f);
          step = random.range(0,5);
          a.motion.velocityY = (float)step + double(8.0f);
          a.motion.accelerationY = 0.20000000298023224f;
        }
        if (((i16(a.animation.cursor.index) == 0x16) && (i16(a.animation.cursor.elapsed) == 0)) &&
           ((int)i16(state.actionTicks) < state.difficulty * 3 + 10)) {
          state.actionTicks = i16(state.actionTicks) + 1;
          selectFrame(0x15);
        }
        if (i16(a.animation.cursor.index) != 0x1c) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x1a);
        return;
      case 0x2e7:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.actionTicks = 0;
          state.actionStage = 0;
          a.motion.clear();
        }
        if ((double(0.0f) < double(a.motion.y)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(0xe);
          a.motion.y = 0.0f;
          a.motion.clear();
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(3);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          step = random.range(0,8);
          a.motion.velocityX = (float)step + double(4.0f);
          step = random.range(0,5);
          a.motion.velocityY = (float)step + double(8.0f);
          a.motion.accelerationY = 0.20000000298023224f;
        }
        if (i16(a.animation.cursor.index) != 0xe) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xc);
        return;
      case 0x2ef:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          state.actionStage = 0;
          a.response = 1;
          a.motion.clear();
        }
        if (((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) < 0x2d)) &&
           (state.actionStage = i16(state.actionStage) + 1,
           4 < i16(state.actionStage))) {
          shared(0x3e,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x17),double(a.motion.y) + double(90.0f),1,1);
          state.actionStage = 0;
        }
        if ((5 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xc)) {
          events.shake(5.0f);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x329,(float)((int)(i8)u8(a.motion.facing) << 6) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          advancedFrame = random.range(0x78,0xb4);
          a.commandTimers[2] = advancedFrame + state.difficulty * -0x14;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x399,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x50),double(a.motion.y) + double(130.0f),u8(a.motion.facing),0);
          owned(0x329,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x50),double(a.motion.y) + double(130.0f),u8(a.motion.facing),0);
          owned(0x32a,(float)((int)(i8)u8(a.motion.facing) << 6) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
          owned(0x353,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x52),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1);
          events.sound(0);
        }
        if (i16(a.animation.cursor.index) != 0xc) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (state.difficulty * 2 + 4 <= (int)i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(6);
        owned(0x32a,(float)((int)(i8)u8(a.motion.facing) << 6) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        return;
      case 0x2f0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.actionTicks = 0;
          state.actionStage = 0;
          a.response = 1;
          a.motion.clear();
        }
        if (((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) < 0x2d)) &&
           (state.actionStage = i16(state.actionStage) + 1,
           4 < i16(state.actionStage))) {
          shared(0x3e,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x17),double(a.motion.y) + double(90.0f),1,1);
          state.actionStage = 0;
        }
        if ((5 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xc)) {
          events.shake(10.0f);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x329,(float)((int)(i8)u8(a.motion.facing) << 6) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          advancedFrame = random.range(0x78,0xb4);
          a.commandTimers[2] = advancedFrame + state.difficulty * -0x14;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x39a,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x50),double(a.motion.y) + double(130.0f),u8(a.motion.facing),0);
          owned(0x329,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x50),double(a.motion.y) + double(130.0f),u8(a.motion.facing),0);
          owned(0x32a,(float)((int)(i8)u8(a.motion.facing) << 6) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
          owned(0x353,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x52),double(a.motion.y) + double(132.0f),u8(a.motion.facing),1);
          events.sound(0);
        }
        if (i16(a.animation.cursor.index) != 0xc) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (state.difficulty * 2 + 4 <= (int)i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(6);
        owned(0x32a,(float)((int)(i8)u8(a.motion.facing) << 6) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        return;
      case 0x2f2:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          state.actionTicks = 0;
          state.actionStage = 0;
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
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          selectFrame(0xc);
          a.commandTimers[2] = 0;
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        step = random.range(0,8);
        a.motion.velocityX = (float)step + double(8.0f);
        step = random.range(0,5);
        a.motion.velocityY = (float)step + double(10.0f);
        a.motion.accelerationY = 0.5f;
        return;
      case 0x2f8:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        step = random.range(0,100);
        if (((step < 0x28) && (3 < i16(a.animation.cursor.index))) && (i16(a.animation.cursor.index) < 8)
           ) {
          layer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          position = double(a.motion.y);
          step = random.range(0,0x8c);
          effectY = double(step)+float(double(position)+40.0);
          position = double(a.motion.x);
          step = random.range(0,0x3c);
          owned(0x330,double(float(double(position)+30.0))-step,effectY,direction,layer);
        }
        if ((i16(a.animation.cursor.index) == 7) && (double(920.0f) < double(a.motion.y))) {
          a.motion.y = 920.0f;
          a.select_action(0x2f9);
          a.motion.clear();
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(2);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 0.0f;
        a.motion.velocityY = 15.0f;
        shared(0x7f,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        return;
      case 0x2f9:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.facing = (u8)0x1;
          state.actionTicks = 0;
          state.actionStage = 0;
          state.volleyIndex = 0;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          if ((i16(state.volleyIndex) == 0) && (state.difficulty < '\x02')) {
            owned(state.difficulty + 0x39f,double(a.motion.x),0.0f,1,1);
          }
          if ((i16(state.volleyIndex) == 4) && ('\x01' < state.difficulty)) {
            owned(state.difficulty + 0x39f,double(other.motion.x),0.0f,1,1);
          }
        }
        a.motion.velocityX = 0.0f;
        if (double(a.motion.x) < double(other.motion.x) - double(50.0f)) {
          a.motion.velocityX = (float)(int)state.difficulty * double(0.5f) + double(1.0f);
        }
        if (double(other.motion.x) + double(50.0f) < double(a.motion.x)) {
          a.motion.velocityX = -double(1.0f) - (float)(int)state.difficulty * double(0.5f);
        }
        if ((i16(a.animation.cursor.index) == 1) && (double(920.0f) < double(a.motion.y))) {
          a.motion.y = 920.0f;
          a.select_action(0x2f9);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          if ((0 < i16(state.actionStage)) && (state.difficulty < '\x02')) {
            a.select_action(0x2fa);
            return;
          }
          if ((1 < i16(state.actionStage)) && ('\x01' < state.difficulty)) {
            a.select_action(0x2fa);
            return;
          }
          selectFrame(1);
          state.volleyIndex = i16(state.volleyIndex) + 1;
          return;
        }
        if (i16(a.animation.cursor.index) != 1) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(state.difficulty + 0x39f,double(a.motion.x),0.0f,1,1);
        return;
      case 0x2fa:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.facing = (u8)0x1;
          state.actionTicks = 0;
          state.actionStage = 0;
          a.motion.velocityX = 0.0f;
        }
        state.actionTicks = i16(state.actionTicks) + -1;
        if ((i16(a.animation.cursor.index) < 2) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.55f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(2);
          return;
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
        selectFrame(0);
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
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          fade(0);
          a.begin_spell(mode,replayMode,core);
          events.system_sound(0x17);
          owned(0x3e5,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
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
        selectFrame(6);
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
        if (i16(a.animation.cursor.index) != 0x14) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (7 < i16(state.actionTicks)) {
          return;
        }
        state.actionTicks = i16(state.actionTicks) + 1;
        selectFrame(0x12);
        a.transitionReady = (u8)0x1;
        return;
      }
    }
idleAction:
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
    goto LAB_00492b73;
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
    goto LAB_00492b73;
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
    goto LAB_00492b73;
  case 5:
    a.motion.velocityX = -6.0f;
LAB_00492b73:
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
          if (i16(a.animation.cursor.index) == 4) {
            a.motion.velocityX = 0.0f;
            a.motion.velocityY = 16.0f;
            a.motion.accelerationY = 0.75f;
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
          if (i16(a.animation.cursor.index) == 4) {
            a.motion.velocityX = 5.0f;
            a.motion.velocityY = 16.0f;
            a.motion.accelerationY = 0.75f;
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
          if (i16(a.animation.cursor.index) == 4) {
            a.motion.velocityX = -4.0f;
            a.carryMomentum = (u8)0x0;
            a.motion.velocityY = 16.0f;
            a.motion.accelerationY = 0.75f;
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
        a.select_action(3);
      }
      else {
        a.select_action(2);
      }
    }
    break;
  default:
    goto idleAction;
  }
  return;

}
}
