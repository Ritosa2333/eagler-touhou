#include "SuikaState.hpp"
#include <cmath>
namespace th075 {
void advance_suika_action(FighterActor& a,FighterActor& other,SuikaState& state,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){

    auto& hudAlpha=events.hud_alpha();auto& completedStages=events.completed_stages();auto& route=events.route();
    const auto biasCamera=[&](u8 reverse){a.bias_camera(other,reverse!=0,events.camera());};
    const auto scriptedDamage=[&](i16 damage,i16 proration,i16 hits){other.combat.action=other.animation.cursor.action;DamageTarget target{other.combat.storyOpponent,other.collisionHealth,other.combat.spellPhase,other.phaseMaximum};a.damage.apply_scripted(damage,proration,hits,other.combat,target,mode==GameMode::Story);};
    const auto ownedWithDraw=[&](i16 id,float x,float y,i8 facing,u8 layer,u8 alternate,u8 shadow,u8 visual){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters,0,visual?&a.effectVisual:nullptr,alternate,shadow);};
    double aimRadians=0;const auto atan=[](float y,float x){return std::atan2(double(y),double(x));};
    const auto selectFrame=[&](i16 frame){a.animation.set(a.animation.cursor.action,frame,0);};
    const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters);};
    const auto shared=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Shared,id,x,y,facing,layer,a.launchParameters);};
    const auto ownedVisual=[&](i16 id,float x,float y,i8 facing,u8 layer,u8 visual,u8 order){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,a.launchParameters,order,visual?&a.effectVisual:nullptr);};
    const auto consumeSpirit=[&](FighterActor& receiver,i16 amount){auto& a=receiver;if(a.combat.spiritBreak<1){a.combat.spirit=i16(u16(a.combat.spirit)-u16(amount));if(a.combat.spirit<1&&a.combat.spiritBreak<1){events.system_sound(43);a.combat.spirit=0;a.combat.spiritBreak=1000;}}else a.combat.spiritBreak=1000;};
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
  float particleCoordinateOrLayer;
  u32 uVar10;
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
          state.introWait = 0;
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
        if (4 < i16(state.introWait)) {
          return;
        }
        state.introWait = i16(state.introWait) + 1;
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
          state.introWait = 0;
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
          state.introWait = 0;
          return;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (0 < i16(state.introWait)) {
          return;
        }
        state.introWait = i16(state.introWait) + 1;
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
          state.introWait = 0;
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
          state.introWait = 0;
          return;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (0 < i16(state.introWait)) {
          return;
        }
        state.introWait = i16(state.introWait) + 1;
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
          state.introWait = 0;
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
          state.introWait = 0;
          return;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (0 < i16(state.introWait)) {
          return;
        }
        state.introWait = i16(state.introWait) + 1;
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
          state.introWait = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.captionWait = impactWord;
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
        if ((i16(a.animation.cursor.index) == 2) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(3);
          return;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.introWait = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.captionWait = impactWord;
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
        if ((i16(a.animation.cursor.index) == 2) && (double(a.motion.velocityY) < double(2.0f))) {
          selectFrame(3);
          return;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.introWait = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.captionWait = impactWord;
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
          state.introWait = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.captionWait = impactWord;
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
          state.introWait = 0;
          return;
        }
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          events.system_sound(0x16);
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.introWait = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.captionWait = impactWord;
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
        if (i16(state.introWait) == 0) {
          state.introWait = i16(state.introWait) + 1;
          a.begin_pose(0.0f,95.0f,0.0f);
        }
        a.pose.rotation[2] = double(a.pose.rotation[2]) + double(30.0f);
        a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
        if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
          events.system_sound(0x16);
          impactWord = i16(i32(double(a.motion.velocityX)*double(100.0f)));
          state.introWait = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.captionWait = impactWord;
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
          state.introWait = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.captionWait = impactWord;
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
            state.introWait = impactWord;
            impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
            state.captionWait = impactWord;
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
        a.motion.velocityX = (float)(int)i16(state.introWait) * double(0.0025f);
        a.motion.velocityY = (float)-(int)i16(state.captionWait) * double(0.01f);
        a.motion.accelerationY = 0.699999988079071f;
        a.begin_pose(0.0f,95.0f,0.0f);
        return;
      case 0x5d:
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 11.0f;
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
        if (i16(a.animation.cursor.index) != 0xe) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 0.0f;
        return;
      case 0x5e:
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -11.0f;
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
        if (i16(a.animation.cursor.index) != 0xe) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 0.0f;
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
          state.introWait = 1;
        }
        if (((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 0) &&
           (((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
            ((a.controls.current.buttons[2] != 0 || (a.controls.current.buttons[3] != 0)))))) {
          state.introWait = 2;
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
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          selectFrame(6);
          return;
        }
        advancedFrame = a.animation.advance();
        if ((advancedFrame == i16(a.animation.cursor.frameCount)) &&
           (a.select_action(0x62), u8(a.transitionReady) == (u8)0x0)) {
          a.select_action(99);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = (float)(int)i16(state.introWait) * double(0.005f);
          a.motion.velocityY = (float)-(int)i16(state.captionWait) * double(0.005f);
          a.motion.accelerationY = 2.0f;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(5);
        return;
      case 0x61:
        if ((0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) &&
           (((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
            ((a.controls.current.buttons[2] != 0 || (a.controls.current.buttons[3] != 0)))))) {
          state.introWait = 1;
        }
        if (((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 0) &&
           ((((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
             (a.controls.current.buttons[2] != 0)) || (a.controls.current.buttons[3] != 0)))) {
          state.introWait = 2;
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
        if ((advancedFrame == i16(a.animation.cursor.frameCount)) &&
           (a.select_action(0x62), u8(a.transitionReady) == (u8)0x0)) {
          a.select_action(99);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = (float)(int)i16(state.introWait) * double(0.005f);
          a.motion.velocityY = (float)-(int)i16(state.captionWait) * double(0.005f);
          a.motion.accelerationY = 2.0f;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(5);
        return;
      case 0x62:
        if ((0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) &&
           ((((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
             (a.controls.current.buttons[2] != 0)) || (a.controls.current.buttons[3] != 0)))) {
          state.introWait = 1;
        }
        if (((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 0) &&
           (((a.controls.current.buttons[0] != 0 || (a.controls.current.buttons[1] != 0)) ||
            ((a.controls.current.buttons[2] != 0 || (a.controls.current.buttons[3] != 0)))))) {
          state.introWait = 2;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0x5f);
        if (i16(state.introWait) == 1) {
          a.select_action(0x5d);
        }
        if (i16(state.introWait) != 2) {
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
            if (a.phaseVariants[a.combat.spellPhase] != (u8)0x5) {
              return;
            }
            a.select_action(0x31d);
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
          a.remainingHits = 8;
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
        if ((i16(a.animation.cursor.elapsed) == 0) && (i16(a.animation.cursor.index) < 0xc)) {
          a.response = 0;
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
      switch(step) {
      case 0x8f:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -12.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
          state.introWait = 0;
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
        if (6 < i16(state.introWait)) {
          return;
        }
        state.introWait = i16(state.introWait) + 1;
        selectFrame(1);
        return;
      case 0x90:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -12.0f;
          a.motion.velocityY = 0.0f;
          a.response = 0;
          state.introWait = 0;
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
        if (6 < i16(state.introWait)) {
          return;
        }
        state.introWait = i16(state.introWait) + 1;
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
          state.introWait = impactWord;
          impactWord = i16(i32(double(a.motion.velocityY)*double(100.0f)));
          state.captionWait = impactWord;
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
        selectFrame(8);
        return;
      case 0x96:
        a.combat.spellCharge = double(a.combat.spellCharge) + double(1.0f);
        if (double(1000.0f) < double(a.combat.spellCharge)) {
          a.combat.spellCharge = 1000.0f;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
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
          state.introWait = 0;
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
          state.introWait = 0;
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
          state.introWait = 0;
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
          state.introWait = 0;
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
          state.introWait = 0;
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
          state.introWait = 0;
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
          state.introWait = 0;
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
          state.introWait = 0;
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
          consumeSpirit(a,0xfa);
          events.system_sound(0x2c);
          state.introWait = 0xb4;
          shared(0x38,(float)((i8)u8(a.motion.facing) * 0x1e) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        a.motion.opponentPush = -double(state.introWait)*double(0.1f);
        if (0 < i16(state.introWait)) {
          state.introWait = i16(state.introWait) + -10;
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
          consumeSpirit(a,0xfa);
          events.system_sound(0x2c);
          state.introWait = 0xb4;
          shared(0x38,(float)((i8)u8(a.motion.facing) * 0x1e) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        }
        if ((double(a.motion.velocityX) < double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) + double(0.6f),
           double(0.0f) < double(a.motion.velocityX))) {
          a.motion.velocityX = 0.0f;
        }
        a.motion.opponentPush = -double(state.introWait)*double(0.1f);
        if (0 < i16(state.introWait)) {
          state.introWait = i16(state.introWait) + -10;
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
        trail(3,4,0xff);
        a.carryMomentum = (u8)0x1;
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.motion.velocityX = 9.0f;
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
        if ((5 < i16(a.animation.cursor.index)) &&
           ((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 1)) {
          a.select_action(0xcf);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        if ((0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) &&
           (i16(state.introWait) < 4)) {
          selectFrame(5);
          state.introWait = i16(state.introWait) + 1;
          return;
        }
        a.select_action(0xcf);
        return;
      case 0xc9:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = -9.0f;
          state.introWait = 0;
        }
        if ((5 < i16(a.animation.cursor.index)) &&
           (-1 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal)) {
          a.select_action(0xcf);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        if (((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 0) &&
           (i16(state.introWait) < 4)) {
          selectFrame(6);
          state.introWait = i16(state.introWait) + 1;
          return;
        }
        a.select_action(0xcf);
        return;
      case 0xca:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          state.introWait = 0;
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
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 8.5f;
          a.motion.velocityY = 4.5f;
          a.motion.accelerationY = 0.44999998807907104f;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (0 < i16(state.introWait)) {
          return;
        }
        selectFrame(3);
        state.introWait = i16(state.introWait) + 1;
        return;
      case 0xcb:
        trail(3,4,0xff);
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
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
          a.motion.velocityY = 4.0f;
          a.motion.accelerationY = 0.44999998807907104f;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (0 < i16(state.introWait)) {
          return;
        }
        selectFrame(3);
        state.introWait = i16(state.introWait) + 1;
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
      }
    }
    else if (step < 0x187) {
      if (step == 0x186) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 2;
          a.response = 0;
          a.motion.velocityX = 7.0f;
        }
        if ((0x10 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.6f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 0x11) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.response = 0;
        return;
      }
      switch(step) {
      case 0x12d:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 1;
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
          state.introWait = 0;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(800,(float)((int)(i8)u8(a.motion.facing) << 7) + double(a.motion.x),double(a.motion.y) - double(66.0f),u8(a.motion.facing),1);
        events.sound(10);
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
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1d);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(5.0f);
        owned(0x340,(float)((i8)u8(a.motion.facing) * 0xbe) + double(a.motion.x),0.0f,u8(a.motion.facing),0);
        shared(0x81,(float)((i8)u8(a.motion.facing) * 0xbe) + double(a.motion.x),0.0f,1,0);
        events.sound(0x11);
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
        if ((2 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.8f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(2);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.carryMomentum = (u8)0x0;
          a.motion.velocityX = 8.0f;
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
        }
        if (i16(a.animation.cursor.index) != 5) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1b);
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
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(5.0f);
        owned(0x342,(float)((i8)u8(a.motion.facing) * 0x5a) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        owned(0x343,(float)((i8)u8(a.motion.facing) * 0x5a) + double(a.motion.x),double(a.motion.y) + double(80.0f),u8(a.motion.facing),1);
        events.sound(0x18);
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
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0x20);
        return;
      case 0x133:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.remainingHits = 1;
          a.response = 0;
        }
        if ((((double(0.0f) < double(a.motion.y)) && (2 < i16(a.animation.cursor.index))) &&
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
          events.shake(10.0f);
          owned(0x340,double(a.motion.x),0.0f,u8(a.motion.facing),0);
          shared(0x81,double(a.motion.x),0.0f,1,0);
          shared(0x81,double(a.motion.x),0.0f,0xffffffff,0);
          events.sound(0x11);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 8.0f;
          a.motion.velocityY = 14.0f;
          a.motion.accelerationY = 1.5f;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(5);
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
          state.introWait = 0;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0x20);
        return;
      case 0x135:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
        }
        if ((double(a.motion.y) == double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.8f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        if ((((double(0.0f) < double(a.motion.y)) && (3 < i16(a.animation.cursor.index))) &&
            (i16(a.animation.cursor.index) < 6)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
           (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f)))) {
          selectFrame(6);
          a.motion.velocityY = 0.0f;
          a.motion.y = 0.0f;
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          state.introWait = 0;
          return;
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityX = 13.5f;
        a.motion.velocityY = 6.5f;
        a.motion.accelerationY = 1.0f;
        return;
      case 0x136:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 1;
          a.response = 0;
        }
        if ((double(a.motion.y) == double(0.0f)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.8f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        if ((((double(0.0f) < double(a.motion.y)) && (4 < i16(a.animation.cursor.index))) &&
            (i16(a.animation.cursor.index) < 7)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
           (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f)))) {
          selectFrame(7);
          a.motion.y = 0.0f;
          a.motion.velocityY = 0.0f;
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
        a.motion.velocityX = 15.0f;
        a.motion.velocityY = 6.0f;
        a.motion.accelerationY = 0.8999999761581421f;
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
          state.introWait = 0;
        }
        if (9 < i16(a.animation.cursor.index)) {
          if (double(a.motion.velocityX) <= double(0.0f)) {
            a.motion.velocityX = 0.0f;
          }
          else {
            a.motion.velocityX = double(a.motion.velocityX) - double(0.5f);
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = 0.0f;
          owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x82) + double(a.motion.x),double(a.motion.y) + double(70.0f),u8(a.motion.facing),1);
          consumeSpirit(a,0xfa);
          events.shake(5.0f);
          events.sound(1);
          shared(0x7f,double(a.motion.x),0.0f,u8(a.motion.facing),0);
        }
        if (i16(a.animation.cursor.index) != 0xd) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (i16(state.introWait) < 5) {
          return;
        }
        state.introWait = i16(state.introWait) + 1;
        selectFrame(0xc);
        return;
      }
      switch(step) {
      case 0x187:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.carryMomentum = (u8)0x0;
          a.remainingHits = 1;
          a.response = 0;
          a.motion.accelerationY = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.velocityX = 10.0f;
        }
        if (((double(0.0f) < double(a.motion.velocityX)) && (double(a.motion.y) == double(0.0f))) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.15f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
        }
        if (((double(0.0f) < double(a.motion.y)) && (3 < i16(a.animation.cursor.index))) &&
           (i16(a.animation.cursor.index) < 0x11)) {
          if (a.response != 0) {
            selectFrame(0x11);
            a.motion.velocityX = -3.0f;
            a.motion.velocityY = 13.0f;
            a.motion.accelerationY = 0.75f;
            a.response = 1;
            return;
          }
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if (double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
              (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f))) {
            a.motion.clear();
            a.motion.y = 0.0f;
            selectFrame(0x17);
            return;
          }
        }
        if (((double(0.0f) < double(a.motion.y)) && (0x10 < i16(a.animation.cursor.index))) &&
           ((i16(a.animation.cursor.index) < 0x17 &&
            (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
            double(a.motion.velocityY) + double(a.motion.y) < double(0.0f) !=
            (double(a.motion.velocityY) + double(a.motion.y) == double(0.0f)))))) {
          a.motion.velocityY = 0.0f;
          a.motion.accelerationY = 0.0f;
          a.motion.y = 0.0f;
          selectFrame(0x17);
          a.motion.clear();
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 12.0f;
          a.motion.velocityY = 9.0f;
          a.motion.accelerationY = 0.5f;
        }
        if ((i16(a.animation.cursor.index) == 0x11) && (i16(a.animation.cursor.elapsed) == 0)) {
          selectFrame(9);
        }
        if (i16(a.animation.cursor.index) != 0x17) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x15);
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
          a.remainingHits = 6;
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.elapsed) == 0) && (i16(a.animation.cursor.index) < 0xb)) {
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1b);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.system_sound(0x1b);
        }
        if (i16(a.animation.cursor.index) != 9) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.system_sound(0x1b);
        return;
      case 0x189:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (u8(a.carryMomentum) == (u8)0x0) {
            a.motion.clear();
          }
          a.remainingHits = 3;
          a.response = 0;
          a.motion.velocityX = 10.0f;
        }
        if ((7 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityX = double(a.motion.velocityX) - double(0.7f),
           double(a.motion.velocityX) < double(0.0f))) {
          a.motion.velocityX = 0.0f;
          a.carryMomentum = (u8)0x0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (8 < i16(a.animation.cursor.index)) {
          return;
        }
        a.response = 0;
        return;
      case 0x18a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityX = 15.0f;
          state.introWait = 0;
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
          state.introWait = 0;
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
           (i16(state.introWait) < 2)) {
          state.introWait = i16(state.introWait) + 1;
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
        owned(0x323,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        consumeSpirit(a,0x96);
        events.sound(0);
        return;
      case 0x191:
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
        owned(0x327,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        consumeSpirit(a,0x96);
        events.sound(0);
        return;
      case 0x192:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
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
        owned(0x328,(float)((i8)u8(a.motion.facing) * 0x14) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),1);
        consumeSpirit(a,0x96);
        events.sound(0);
        return;
      case 0x194:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          a.response = 1;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x323,(float)((i8)u8(a.motion.facing) * 0x55) + double(a.motion.x),double(a.motion.y) + double(95.0f),u8(a.motion.facing),1);
          consumeSpirit(a,0x96);
          events.sound(0);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
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
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          owned(0x327,(float)((i8)u8(a.motion.facing) * 0x55) + double(a.motion.x),double(a.motion.y) + double(95.0f),u8(a.motion.facing),1);
          consumeSpirit(a,0x96);
          events.sound(0);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
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
        a.launchParameters[0] = 0.0f;
        owned(0x338,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        consumeSpirit(a,200);
        events.sound(0);
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
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.launchParameters[0] = 30.0f;
        owned(0x338,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        consumeSpirit(a,200);
        events.sound(0);
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
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.launchParameters[0] = 20.0f;
        owned(0x339,(float)((i8)u8(a.motion.facing) * 0x14) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),0);
        consumeSpirit(a,300);
        events.sound(0);
        return;
      case 0x19a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          a.response = 1;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = 0.0f;
          owned(0x338,(float)((i8)u8(a.motion.facing) * 0x55) + double(a.motion.x),double(a.motion.y) + double(95.0f),u8(a.motion.facing),1);
          consumeSpirit(a,200);
          events.sound(0);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x19b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          a.response = 1;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = -30.0f;
          owned(0x338,(float)((i8)u8(a.motion.facing) * 0x55) + double(a.motion.x),double(a.motion.y) + double(95.0f),u8(a.motion.facing),1);
          consumeSpirit(a,200);
          events.sound(0);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      case 0x19c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
          state.introWait = 0;
        }
        if ((((6 < i16(a.animation.cursor.index)) && (i16(state.introWait) < 2)) &&
            (a.controls.current.buttons[1] == 1)) && (0 < i16(a.combat.spirit))) {
          state.introWait = i16(state.introWait) + 1;
          selectFrame(1);
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
        owned(0x33d,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
        consumeSpirit(a,100);
        events.sound(0);
        return;
      case 0x19e:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.response = 1;
          a.motion.clear();
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
        a.launchParameters[0] = 35.0f;
        owned(0x33c,(float)((i8)u8(a.motion.facing) * 0x14) + double(a.motion.x),double(a.motion.y) + double(90.0f),u8(a.motion.facing),0);
        consumeSpirit(a,300);
        events.sound(0);
        return;
      case 0x1a0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = double(a.motion.velocityX) * double(0.25f);
          a.motion.velocityY = double(a.motion.velocityY) * double(0.25f);
          a.response = 1;
          state.introWait = 0;
        }
        if ((((5 < i16(a.animation.cursor.index)) && (i16(state.introWait) < 2)) &&
            (a.controls.current.buttons[1] == 1)) && (0 < i16(a.combat.spirit))) {
          state.introWait = i16(state.introWait) + 1;
          selectFrame(1);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.launchParameters[0] = 0.0f;
          owned(0x33d,(float)((i8)u8(a.motion.facing) * 0x55) + double(a.motion.x),double(a.motion.y) + double(95.0f),u8(a.motion.facing),1);
          consumeSpirit(a,100);
          events.sound(0);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(9);
        return;
      }
    }
    else if (step < 0x2d3) {
      if (step == 0x2d2) {
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 5;
        }
        if (((i16(a.animation.cursor.index) == 0xb) && (a.response == 1)) &&
           (u8(a.transitionReady) == (u8)0x1)) {
          a.select_action(0x2d3);
          other.select_action(0x66);
          if ((double(750.0f) < double(a.motion.x)) && (u8(a.motion.facing) == (u8)0x1)) {
            a.motion.x = 750.0f;
          }
          if ((double(a.motion.x) < double(550.0f)) && (u8(a.motion.facing) == (u8)0xff)) {
            a.motion.x = 550.0f;
          }
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x1fe) + double(a.motion.x);
          other.motion.velocityX = 0.0f;
          other.motion.y = 0.0f;
          other.motion.clear();
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x74,(float)((i8)u8(a.motion.facing) * 0x34) + double(a.motion.x),double(a.motion.y) + double(160.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(6);
        }
        if (((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.introWait) < 5)) {
          state.introWait = i16(state.introWait) + 1;
          selectFrame(5);
          return;
        }
        if ((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(7);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        consumeSpirit(a,500);
        return;
      }
      switch(step) {
      case 0x1f5:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.introWait = 0;
        }
        if (9 < i16(a.animation.cursor.index)) {
          if (double(a.motion.velocityX) <= double(0.0f)) {
            a.motion.velocityX = 0.0f;
          }
          else {
            a.motion.velocityX = double(a.motion.velocityX) - double(0.5f);
          }
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
        a.launchParameters[0] = 75.0f;
        owned(0x32b,(float)((i8)u8(a.motion.facing) * 0x53) + double(a.motion.x),double(a.motion.y) + double(170.0f),u8(a.motion.facing),1);
        consumeSpirit(a,0xfa);
        events.shake(5.0f);
        events.sound(1);
        shared(0x7f,double(a.motion.x),0.0f,u8(a.motion.facing),0);
        return;
      case 0x1f6:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 8) &&
           (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          events.sound(3);
          a.motion.velocityX = -4.0f;
          a.motion.velocityY = 10.0f;
          a.motion.y = 0.0f;
          events.shake(5.0f);
          owned(0x32e,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          selectFrame(9);
          return;
        }
        if ((8 < i16(a.animation.cursor.index)) &&
           ((i16(a.animation.cursor.index) < 0x1d &&
            (a.motion.velocityY = double(a.motion.velocityY) - double(0.8f),
            double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))))) {
          selectFrame(0x1d);
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.y = 0.0f;
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(a,200);
          a.motion.velocityX = 16.0f;
          a.motion.velocityY = -16.0f;
          events.sound(2);
        }
        if (i16(a.animation.cursor.index) != 0x1d) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x1b);
        return;
      case 0x1f7:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
        }
        if ((i16(a.animation.cursor.index) == 8) &&
           (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          events.sound(3);
          a.motion.velocityX = -3.0f;
          a.motion.velocityY = 8.0f;
          a.motion.y = 0.0f;
          events.shake(5.0f);
          owned(0x32e,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          selectFrame(9);
          return;
        }
        if ((8 < i16(a.animation.cursor.index)) &&
           ((i16(a.animation.cursor.index) < 0x1d &&
            (a.motion.velocityY = double(a.motion.velocityY) - double(0.8f),
            double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))))) {
          selectFrame(0x1d);
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          a.motion.y = 0.0f;
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          consumeSpirit(a,200);
          a.motion.velocityX = 16.0f;
          a.motion.velocityY = -16.0f;
          events.sound(2);
        }
        if (i16(a.animation.cursor.index) != 0x1d) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0x1b);
        return;
      case 0x252:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.selectedSpell = (u8)((i8)u8(a.selectedSpell) + -1);
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.introWait = 0;
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
          state.introWait = 0;
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
      case 0x254:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.selectedSpell = (u8)((i8)u8(a.selectedSpell) + -1);
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.introWait = 0;
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
      case 0x255:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.introWait = 0;
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
        if (0 < i16(state.introWait)) {
          return;
        }
        state.introWait = i16(state.introWait) + 1;
        selectFrame(6);
        return;
      case 0x256:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
          a.carryMomentum = (u8)0x0;
          state.introWait = 0;
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
          state.introWait = 0;
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
        if (0 < i16(state.introWait)) {
          return;
        }
        state.introWait = i16(state.introWait) + 1;
        selectFrame(6);
        return;
      case 600:
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
        consumeSpirit(a,0xfa);
        events.sound(4);
        for (volley = 0; volley < 8; volley = volley + 1) {
          step = random.range(0,0x78);
          a.launchParameters[0] = (float)(step + -0x3c);
          step = random.range(0,2);
          a.launchParameters[1] = (float)(step + 6);
          owned(0x321,(float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x),double(a.motion.y) + double(95.0f),u8(a.motion.facing),1);
        }
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
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(9);
        consumeSpirit(a,0xfa);
        for (emission = 0; emission < 0x10; emission = emission + 1) {
          step = random.range(0x28,0x46);
          a.launchParameters[0] = (float)-step;
          step = random.range(0,6);
          a.launchParameters[1] = (float)(step + 6);
          owned(0x322,(float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x),double(a.motion.y) + double(95.0f),u8(a.motion.facing),1);
        }
        return;
      case 0x25a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 0;
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
        events.sound(5);
        events.shake(5.0f);
        consumeSpirit(a,0xfa);
        for (local_1c = 0; local_1c < 0xc; local_1c = local_1c + 1) {
          step = random.range(0,0x28);
          a.launchParameters[0] = (float)(step + -0x5a);
          step = random.range(0,0x16);
          a.launchParameters[1] = (float)(step + 5);
          layer = 1;
          uVar10 = u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          step = random.range(0,0xbe);
          owned(0x32c,(float)((step + 10) * (int)(i8)u8(a.motion.facing)) +
                             double(a.motion.x),particleCoordinateOrLayer,uVar10,layer);
        }
        owned(0x340,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),0.0f,u8(a.motion.facing),0);
        shared(0x7f,double(a.motion.x),0.0f,u8(a.motion.facing),0);
        return;
      case 0x25b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          a.response = 1;
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
        events.sound(5);
        events.shake(5.0f);
        consumeSpirit(a,0xfa);
        owned(0x340,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),0.0f,u8(a.motion.facing),0);
        owned(0x341,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),0.0f,u8(a.motion.facing),0);
        shared(0x7f,double(a.motion.x),0.0f,u8(a.motion.facing),0);
        return;
      case 700:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          state.captionWait = 0;
          state.impactX = 0;
          state.impactY = 0;
          state.spellLoops = 0;
          a.remainingHits = 0;
          a.response = 0;
          a.motion.clear();
        }
        if (((i16(state.spellLoops) == 0) &&
            (i16(other.animation.cursor.action) == 0x61)) &&
           (0x20 < i16(a.animation.cursor.index))) {
          events.shake(15.0f);
          state.spellLoops = i16(state.spellLoops) + 1;
          events.sound(0x17);
          scriptedDamage(2000,100,1);
        }
        if ((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x11)) {
          if (double(other.motion.y) == double(0.0f)) {
            a.remainingHits = 1;
          }
          else {
            a.remainingHits = 0;
          }
          if (a.response == 1) {
            a.motion.clear();
            selectFrame(0x11);
            other.select_action(0x65);
            other.motion.velocityX = 0.0f;
            other.motion.velocityY = 0.0f;
            other.motion.x =
                 (float)((i8)u8(a.motion.facing) * 0x2d) + double(a.motion.x);
            other.motion.y =
                 double(a.motion.y) + double(120.0f) + double(a.motion.velocityY);
            return;
          }
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.motion.clear();
            a.motion.y = 0.0f;
            a.select_action(0x2bd);
            return;
          }
        }
        if ((0x10 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x23)) {
          if (i16(a.animation.cursor.index) < 0x15) {
            biasCamera(0);
            a.motion.velocityY = double(a.motion.velocityY) - double(0.5f);
            if (double(a.motion.velocityY) < double(0.0f)) {
              a.motion.velocityY = 0.0f;
            }
            a.motion.velocityX = 0.0f;
            other.motion.x =
                 (float)((i8)u8(a.motion.facing) * 0x2d) + double(a.motion.x);
            other.motion.y =
                 double(a.motion.y) + double(120.0f) + double(a.motion.velocityY);
          }
          if ((0x14 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x1f)) {
            biasCamera(0);
            state.captionWait = i16(state.captionWait) + -1;
            events.shake(2.0f);
            if (i16(state.impactX) < 0x3c) {
              if (i16(state.captionWait) < 1) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y);
                step = random.range(0,0x780);
                owned(0x331,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x3c0),double(float(double(effectY)+960.0))-step,direction,particleCoordinateOrLayer);
                state.captionWait = 8;
                state.impactX = i16(state.impactX) + 1;
              }
              if (i16(state.captionWait) == 2) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y);
                step = random.range(0,0x780);
                owned(0x331,(float)((i8)u8(a.motion.facing) * 0x3c0) + double(a.motion.x),double(float(double(effectY)+960.0))-step,direction,particleCoordinateOrLayer);
                state.impactX = i16(state.impactX) + 1;
              }
              if (i16(state.captionWait) == 4) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y) + double(960.0f);
                step = random.range(0,0x780);
                owned(0x331,(float)((0x3c0 - step) * (int)(i8)u8(a.motion.facing)) +
                                   double(a.motion.x),effectY,direction,particleCoordinateOrLayer);
                state.impactX = i16(state.impactX) + 1;
              }
              if (i16(state.captionWait) == 6) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y) - double(960.0f);
                step = random.range(0,0x780);
                owned(0x331,(float)((0x3c0 - step) * (int)(i8)u8(a.motion.facing)) +
                                   double(a.motion.x),effectY,direction,particleCoordinateOrLayer);
                state.impactX = i16(state.impactX) + 1;
              }
            }
            if (i16(a.animation.cursor.index) < 0x17) {
              other.motion.x =
                   (float)((i8)u8(a.motion.facing) * 0x2d) + double(a.motion.x);
              other.motion.y =
                   double(a.motion.y) + double(120.0f) + double(a.motion.velocityY);
            }
          }
        }
        if ((0x22 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.6f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          a.select_action(10);
          return;
        }
        if ((i16(state.impactY) == 0) &&
           (i16(other.animation.cursor.action) == 0x61)) {
          state.impactY = i16(state.impactY) + 1;
          events.shake(15.0f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          selectFrame(0x2a);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x73,(float)((i8)u8(a.motion.facing) * 0x34) + double(a.motion.x),double(a.motion.y) + double(160.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 15.0f;
          a.motion.velocityY = 10.0f;
          a.motion.accelerationY = 0.6000000238418579f;
        }
        if ((i16(a.animation.cursor.index) == 0x11) && (i16(a.animation.cursor.elapsed) == 0)) {
          selectFrame(9);
        }
        if ((i16(a.animation.cursor.index) == 0x13) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 12.5f;
        }
        if ((i16(a.animation.cursor.index) == 0x15) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.introWait) < 2) {
            state.introWait = i16(state.introWait) + 1;
            selectFrame(0x13);
            return;
          }
          state.introWait = 0;
        }
        if ((i16(a.animation.cursor.index) == 0x17) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.introWait) < 0) {
            state.introWait = i16(state.introWait) + 1;
            selectFrame(0x15);
            return;
          }
          events.sound(0x13);
          state.introWait = 0;
          other.select_action(0x6d);
          other.motion.x = double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(140.0f);
          other.motion.facing = u8(a.motion.facing);
        }
        if ((i16(a.animation.cursor.index) == 0x18) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 45.0f;
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x2c) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(128.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x19) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 5.0f;
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(85.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1a) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 25.0f;
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x19) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(65.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1b) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 85.0f;
          other.motion.x =
               double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x14);
          other.motion.y = double(a.motion.y) + double(50.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1c) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 160.0f;
          other.motion.x =
               double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x3c);
          other.motion.y = double(a.motion.y) + double(90.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1d) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = -165.0f;
          other.motion.x =
               double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x2d);
          other.motion.y = double(a.motion.y) + double(145.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1e) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = -110.0f;
          other.motion.x = double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(150.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1f) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.introWait) < 0xb) {
            state.introWait = i16(state.introWait) + 1;
            selectFrame(0x18);
            other.begin_pose(0.0f,0.0f,0.0f);
            other.pose.rotation[2] = -45.0f;
            other.motion.x =
                 (float)((i8)u8(a.motion.facing) * 0x2c) + double(a.motion.x);
            other.motion.y = double(a.motion.y) + double(128.0f);
            return;
          }
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x2c) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(110.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x20) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.select_action(0x6f);
          other.motion.facing = -(i8)u8(a.motion.facing);
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(85.0f);
        }
        if (i16(a.animation.cursor.index) != 0x21) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        other.motion.x =
             (float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x);
        other.motion.y = double(a.motion.y) + double(85.0f);
        other.select_action(0x49);
        other.impulseX = 36.0f;
        other.impulseY = -18.0f;
        other.motion.facing = -(i8)u8(a.motion.facing);
        events.sound(0x14);
        return;
      case 0x2bd:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 0;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      case 0x2be:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 5;
        }
        if (((i16(a.animation.cursor.index) == 0xb) && (a.response == 1)) &&
           (u8(a.transitionReady) == (u8)0x1)) {
          a.select_action(0x2bf);
          other.select_action(0x66);
          if ((double(750.0f) < double(a.motion.x)) && (u8(a.motion.facing) == (u8)0x1)) {
            a.motion.x = 750.0f;
          }
          if ((double(a.motion.x) < double(550.0f)) && (u8(a.motion.facing) == (u8)0xff)) {
            a.motion.x = 550.0f;
          }
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x1fe) + double(a.motion.x);
          other.motion.velocityX = 0.0f;
          other.motion.y = 0.0f;
          other.motion.clear();
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x73,(float)((i8)u8(a.motion.facing) * 0x34) + double(a.motion.x),double(a.motion.y) + double(160.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(6);
        }
        if (((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) &&
           (i16(state.introWait) < 5)) {
          state.introWait = i16(state.introWait) + 1;
          selectFrame(5);
          return;
        }
        if ((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(7);
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        consumeSpirit(a,500);
        return;
      case 0x2bf:
        if (i16(a.animation.cursor.index) == 2) {
          events.shake(2.0f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.shake(5.0f);
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x1d6) + double(a.motion.x);
          owned(0x333,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          events.sound(8);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x1c2) + double(a.motion.x);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 400) + double(a.motion.x);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x14a) + double(a.motion.x);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        other.select_action(0x35);
        other.motion.facing = u8(a.motion.facing);
        state.summonTimer = 0x1c2;
        return;
      case 0x2c0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 1;
        }
        if ((6 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x10)) {
          if (double(2.0f) <= double(a.pose.scale[0])) {
            a.pose.scale[0] = 2.0f;
            a.pose.scale[1] = 2.0f;
          }
          else {
            a.pose.scale[0] = double(a.pose.scale[0]) + double(0.2f);
            a.pose.scale[1] = double(a.pose.scale[1]) + double(0.2f);
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x2c1);
          state.giantMode = 900;
          return;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x73,(float)((i8)u8(a.motion.facing) * 0x34) + double(a.motion.x),double(a.motion.y) + double(160.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0xb);
          owned(0x332,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),0);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.begin_pose(0.0f,0.0f,0.0f);
        }
        if (i16(a.animation.cursor.index) != 0x10) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.begin_pose(0.0f,0.0f,0.0f);
        return;
      case 0x2c1:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
        }
        a.motion.clear();
        if (i16(state.giantMode) < 1) {
          a.select_action(0x2c9);
          return;
        }
        if (a.controls.current.buttons[0] == 1) {
          a.select_action(0x2c4);
          return;
        }
        if (a.controls.current.buttons[1] == 1) {
          a.select_action(0x2c5);
          return;
        }
        if (a.controls.current.buttons[2] == 1) {
          a.select_action(0x2c6);
          return;
        }
        if (a.controls.current.vertical < 0) {
          a.select_action(0x2c3);
          if (0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) {
            state.introWait = 1;
            return;
          }
          state.introWait = 0;
          return;
        }
        if (1 < a.controls.current.horizontal) {
          a.motion.facing = (u8)0x1;
          a.select_action(0x2c2);
          return;
        }
        if (a.controls.current.horizontal < -1) {
          a.motion.facing = (u8)0xff;
          a.select_action(0x2c2);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(0);
        return;
      case 0x2c2:
        if (i16(state.giantMode) < 1) {
          a.select_action(0x2c9);
          return;
        }
        a.motion.velocityX = 3.25f;
        if (a.controls.current.buttons[0] == 1) {
          a.select_action(0x2c4);
          return;
        }
        if (a.controls.current.buttons[1] == 1) {
          a.select_action(0x2c5);
          return;
        }
        if (a.controls.current.buttons[2] == 1) {
          a.select_action(0x2c6);
          return;
        }
        if (a.controls.current.vertical < 0) {
          a.select_action(0x2c3);
          if (0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) {
            state.introWait = 1;
            return;
          }
          state.introWait = 0;
          return;
        }
        if ((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 1) {
          a.select_action(0x2c1);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          selectFrame(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.shake(3.0f);
          events.sound(0x11);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(3.0f);
        events.sound(0x11);
        return;
      case 0x2c3:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.motion.clear();
        }
        if (double(0.0f) < double(a.motion.y)) {
          if (a.controls.current.buttons[0] == 1) {
            a.select_action(0x2c7);
            return;
          }
          if (a.controls.current.buttons[1] == 1) {
            a.select_action(0x2c8);
            return;
          }
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            selectFrame(0xf);
            a.motion.y = 0.0f;
            a.motion.clear();
            events.sound(0x11);
            events.shake(20.0f);
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x2c1);
          if (i16(state.giantMode) < 1) {
            a.select_action(0x2c9);
          }
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 10.0f;
          a.motion.accelerationY = 0.30000001192092896f;
          if (i16(state.introWait) == 1) {
            a.motion.velocityX = 7.0f;
          }
          else {
            a.motion.velocityX = 0.0f;
          }
        }
        if (i16(a.animation.cursor.index) != 0xf) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xd);
        return;
      case 0x2c4:
        a.motion.clear();
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x2c1);
          if (i16(state.giantMode) < 1) {
            a.select_action(0x2c9);
          }
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(10.0f);
        events.sound(0xc);
        return;
      case 0x2c5:
        a.motion.clear();
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x2c1);
          if (i16(state.giantMode) < 1) {
            a.select_action(0x2c9);
          }
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(10.0f);
        events.sound(0xd);
        return;
      case 0x2c6:
        a.motion.clear();
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x2c1);
          if (i16(state.giantMode) < 1) {
            a.select_action(0x2c9);
          }
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(10.0f);
        events.sound(0xe);
        return;
      case 0x2c7:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) < 6) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(6);
          a.motion.y = 0.0f;
          a.motion.clear();
          events.sound(0x11);
          events.shake(10.0f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x2c1);
          if (i16(state.giantMode) < 1) {
            a.select_action(0x2c9);
          }
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0xc);
        return;
      case 0x2c8:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityY = 5.0f;
          a.motion.accelerationY = 0.44999998807907104f;
        }
        if ((i16(a.animation.cursor.index) < 3) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(3);
          a.motion.y = 0.0f;
          a.motion.clear();
          events.sound(0xf);
          events.shake(20.0f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0x2c1);
        if (i16(state.giantMode) < 1) {
          a.select_action(0x2c9);
        }
        a.response = 0;
        return;
      case 0x2c9:
        a.motion.clear();
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 0;
          a.remainingHits = 1;
          a.begin_pose(0.0f,0.0f,0.0f);
          events.sound(0x10);
          state.giantMode = 0;
        }
        if (i16(a.animation.cursor.index) == 0) {
          if (double(a.pose.scale[0]) <= double(0.5f)) {
            a.pose.scale[0] = 0.5f;
            a.pose.scale[1] = 0.5f;
            selectFrame(1);
            return;
          }
          a.pose.scale[0] = double(a.pose.scale[0]) - double(0.025f);
          a.pose.scale[1] = double(a.pose.scale[1]) - double(0.025f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        a.begin_pose(0.0f,0.0f,0.0f);
        return;
      case 0x2d0:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          state.captionWait = 0;
          state.impactX = 0;
          state.impactY = 0;
          state.spellLoops = 0;
          a.remainingHits = 0;
          a.response = 0;
          a.motion.clear();
        }
        if (((i16(state.spellLoops) == 0) &&
            (i16(other.animation.cursor.action) == 0x59)) &&
           (0x20 < i16(a.animation.cursor.index))) {
          events.shake(25.0f);
          state.spellLoops = i16(state.spellLoops) + 1;
          events.sound(0x18);
          scriptedDamage(3000,0x32,1);
        }
        if ((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x11)) {
          if (double(other.motion.y) == double(0.0f)) {
            a.remainingHits = 1;
          }
          else {
            a.remainingHits = 0;
          }
          if (a.response == 1) {
            a.motion.clear();
            selectFrame(0x11);
            other.select_action(0x65);
            other.motion.velocityX = 0.0f;
            other.motion.velocityY = 0.0f;
            other.motion.x =
                 (float)((i8)u8(a.motion.facing) * 0x2d) + double(a.motion.x);
            other.motion.y =
                 double(a.motion.y) + double(120.0f) + double(a.motion.velocityY);
            return;
          }
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.motion.clear();
            a.motion.y = 0.0f;
            a.select_action(0x2bd);
            return;
          }
        }
        if ((0x10 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x23)) {
          if (i16(a.animation.cursor.index) < 0x15) {
            biasCamera(0);
            a.motion.velocityY = double(a.motion.velocityY) - double(0.5f);
            if (double(a.motion.velocityY) < double(0.0f)) {
              a.motion.velocityY = 0.0f;
            }
            a.motion.velocityX = 0.0f;
            other.motion.x =
                 (float)((i8)u8(a.motion.facing) * 0x2d) + double(a.motion.x);
            other.motion.y =
                 double(a.motion.y) + double(120.0f) + double(a.motion.velocityY);
          }
          if ((0x14 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x1f)) {
            biasCamera(0);
            state.captionWait = i16(state.captionWait) + -1;
            events.shake(2.0f);
            if (i16(state.impactX) < 0x3c) {
              if (i16(state.captionWait) < 1) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y);
                step = random.range(0,0x780);
                owned(0x334,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x3c0),double(float(double(effectY)+960.0))-step,direction,particleCoordinateOrLayer);
                state.captionWait = 8;
                state.impactX = i16(state.impactX) + 1;
              }
              if (i16(state.captionWait) == 2) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y);
                step = random.range(0,0x780);
                owned(0x334,(float)((i8)u8(a.motion.facing) * 0x3c0) + double(a.motion.x),double(float(double(effectY)+960.0))-step,direction,particleCoordinateOrLayer);
                state.impactX = i16(state.impactX) + 1;
              }
              if (i16(state.captionWait) == 4) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y) + double(960.0f);
                step = random.range(0,0x780);
                owned(0x334,(float)((0x3c0 - step) * (int)(i8)u8(a.motion.facing)) +
                                   double(a.motion.x),effectY,direction,particleCoordinateOrLayer);
                state.impactX = i16(state.impactX) + 1;
              }
              if (i16(state.captionWait) == 6) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y) - double(960.0f);
                step = random.range(0,0x780);
                owned(0x334,(float)((0x3c0 - step) * (int)(i8)u8(a.motion.facing)) +
                                   double(a.motion.x),effectY,direction,particleCoordinateOrLayer);
                state.impactX = i16(state.impactX) + 1;
              }
            }
            if (i16(a.animation.cursor.index) < 0x17) {
              other.motion.x =
                   (float)((i8)u8(a.motion.facing) * 0x2d) + double(a.motion.x);
              other.motion.y =
                   double(a.motion.y) + double(120.0f) + double(a.motion.velocityY);
            }
          }
        }
        if ((0x22 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.6f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          a.select_action(10);
          return;
        }
        if ((i16(state.impactY) == 0) &&
           (i16(other.animation.cursor.action) == 0x61)) {
          state.impactY = i16(state.impactY) + 1;
          events.shake(15.0f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          selectFrame(0x2a);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x74,(float)((i8)u8(a.motion.facing) * 0x34) + double(a.motion.x),double(a.motion.y) + double(160.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 15.0f;
          a.motion.velocityY = 10.0f;
          a.motion.accelerationY = 0.6000000238418579f;
        }
        if ((i16(a.animation.cursor.index) == 0x11) && (i16(a.animation.cursor.elapsed) == 0)) {
          selectFrame(9);
        }
        if ((i16(a.animation.cursor.index) == 0x13) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 12.5f;
        }
        if ((i16(a.animation.cursor.index) == 0x15) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.introWait) < 2) {
            state.introWait = i16(state.introWait) + 1;
            selectFrame(0x13);
            return;
          }
          state.introWait = 0;
        }
        if ((i16(a.animation.cursor.index) == 0x17) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.introWait) < 0) {
            state.introWait = i16(state.introWait) + 1;
            selectFrame(0x15);
            return;
          }
          events.sound(0x15);
          state.introWait = 0;
          other.select_action(0x6d);
          other.motion.x = double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(140.0f);
          other.motion.facing = u8(a.motion.facing);
        }
        if ((i16(a.animation.cursor.index) == 0x18) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 45.0f;
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x2c) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(128.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x19) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 5.0f;
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(85.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1a) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 25.0f;
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x19) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(65.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1b) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 85.0f;
          other.motion.x =
               double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x14);
          other.motion.y = double(a.motion.y) + double(50.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1c) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 160.0f;
          other.motion.x =
               double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x3c);
          other.motion.y = double(a.motion.y) + double(90.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1d) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = -165.0f;
          other.motion.x =
               double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x2d);
          other.motion.y = double(a.motion.y) + double(145.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1e) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = -110.0f;
          other.motion.x = double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(150.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1f) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.introWait) < 0x21) {
            state.introWait = i16(state.introWait) + 1;
            selectFrame(0x18);
            other.begin_pose(0.0f,0.0f,0.0f);
            other.pose.rotation[2] = -45.0f;
            other.motion.x =
                 (float)((i8)u8(a.motion.facing) * 0x2c) + double(a.motion.x);
            other.motion.y = double(a.motion.y) + double(128.0f);
            return;
          }
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x2c) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(110.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x20) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.select_action(0x6f);
          other.motion.facing = -(i8)u8(a.motion.facing);
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(85.0f);
        }
        if (i16(a.animation.cursor.index) != 0x21) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        other.motion.x =
             (float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x);
        other.motion.y = double(a.motion.y) + double(85.0f);
        other.select_action(0x58);
        other.impulseX = 18.0f;
        other.impulseY = -36.0f;
        other.motion.facing = -(i8)u8(a.motion.facing);
        events.sound(0x16);
        return;
      case 0x2d1:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 0;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        return;
      }
    }
    else {
      switch(step) {
      case 0x2d3:
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x1d6) + double(a.motion.x);
          owned(0x335,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
          owned(0x33f,double(other.motion.x),double(other.motion.y) + double(100.0f),u8(a.motion.facing),1);
          events.sound(8);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x1c2) + double(a.motion.x);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 400) + double(a.motion.x);
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x14a) + double(a.motion.x);
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        other.select_action(0x35);
        other.motion.facing = u8(a.motion.facing);
        state.summonTimer = 0x1c2;
        return;
      case 0x2d4:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 0;
          a.motion.clear();
          a.remainingHits = 1;
        }
        if ((6 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x10)) {
          if (double(3.0f) <= double(a.pose.scale[0])) {
            a.pose.scale[0] = 3.0f;
            a.pose.scale[1] = 3.0f;
          }
          else {
            a.pose.scale[0] = double(a.pose.scale[0]) + double(0.2f);
            a.pose.scale[1] = double(a.pose.scale[1]) + double(0.2f);
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x2d5);
          state.giantMode = 900;
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x74,(float)((i8)u8(a.motion.facing) * 0x34) + double(a.motion.x),double(a.motion.y) + double(160.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.sound(0xb);
          owned(0x332,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),0);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.begin_pose(0.0f,0.0f,0.0f);
        }
        if (i16(a.animation.cursor.index) != 0x10) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.begin_pose(0.0f,0.0f,0.0f);
        return;
      case 0x2d5:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
        }
        a.motion.clear();
        if (i16(state.giantMode) < 1) {
          a.select_action(0x2dd);
          return;
        }
        if (a.controls.current.buttons[0] == 1) {
          a.select_action(0x2d8);
          return;
        }
        if (a.controls.current.buttons[1] == 1) {
          a.select_action(0x2d9);
          return;
        }
        if (a.controls.current.buttons[2] == 1) {
          a.select_action(0x2da);
          return;
        }
        if (a.controls.current.vertical < 0) {
          a.select_action(0x2d7);
          if (0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) {
            state.introWait = 1;
            return;
          }
          state.introWait = 0;
          return;
        }
        if (1 < a.controls.current.horizontal) {
          a.motion.facing = (u8)0x1;
          a.select_action(0x2d6);
          return;
        }
        if (a.controls.current.horizontal < -1) {
          a.motion.facing = (u8)0xff;
          a.select_action(0x2d6);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(0);
        return;
      case 0x2d6:
        if (i16(state.giantMode) < 1) {
          a.select_action(0x2dd);
          return;
        }
        a.motion.velocityX = 2.0f;
        if (a.controls.current.buttons[0] == 1) {
          a.select_action(0x2d8);
          return;
        }
        if (a.controls.current.buttons[1] == 1) {
          a.select_action(0x2d9);
          return;
        }
        if (a.controls.current.buttons[2] == 1) {
          a.select_action(0x2da);
          return;
        }
        if (a.controls.current.vertical < 0) {
          a.select_action(0x2d7);
          if (0 < (int)(i8)u8(a.motion.facing) * a.controls.current.horizontal) {
            state.introWait = 1;
            return;
          }
          state.introWait = 0;
          return;
        }
        if ((int)(i8)u8(a.motion.facing) * a.controls.current.horizontal < 1) {
          a.select_action(0x2d5);
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          selectFrame(0);
        }
        if ((i16(a.animation.cursor.index) == 3) && (i16(a.animation.cursor.elapsed) == 0)) {
          events.shake(3.0f);
          events.sound(0x11);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(3.0f);
        events.sound(0x11);
        return;
      case 0x2d7:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.motion.clear();
        }
        if (double(0.0f) < double(a.motion.y)) {
          if (a.controls.current.buttons[0] == 1) {
            a.select_action(0x2db);
            return;
          }
          if (a.controls.current.buttons[1] == 1) {
            a.select_action(0x2dc);
            return;
          }
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            selectFrame(0xf);
            a.motion.y = 0.0f;
            a.motion.clear();
            events.sound(0x11);
            events.shake(20.0f);
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x2d5);
          if (i16(state.giantMode) < 1) {
            a.select_action(0x2dd);
          }
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 10.0f;
          a.motion.accelerationY = 0.30000001192092896f;
          if (i16(state.introWait) == 1) {
            a.motion.velocityX = 5.0f;
          }
          else {
            a.motion.velocityX = 0.0f;
          }
        }
        if (i16(a.animation.cursor.index) != 0xf) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0xd);
        return;
      case 0x2d8:
        a.motion.clear();
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x2d5);
          if (i16(state.giantMode) < 1) {
            a.select_action(0x2dd);
          }
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 0xb) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(10.0f);
        events.sound(0xc);
        return;
      case 0x2d9:
        a.motion.clear();
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x2d5);
          if (i16(state.giantMode) < 1) {
            a.select_action(0x2dd);
          }
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(10.0f);
        events.sound(0xd);
        return;
      case 0x2da:
        a.motion.clear();
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x2d5);
          if (i16(state.giantMode) < 1) {
            a.select_action(0x2dd);
          }
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.shake(10.0f);
        events.sound(0xe);
        return;
      case 0x2db:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
        }
        if ((i16(a.animation.cursor.index) < 6) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(6);
          a.motion.y = 0.0f;
          a.motion.clear();
          events.sound(0x11);
          events.shake(10.0f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x2d5);
          if (i16(state.giantMode) < 1) {
            a.select_action(0x2dd);
          }
          a.response = 0;
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(0xc);
        return;
      case 0x2dc:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.remainingHits = 1;
          a.response = 0;
          a.motion.velocityY = 5.0f;
          a.motion.accelerationY = 0.44999998807907104f;
        }
        if ((i16(a.animation.cursor.index) < 3) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(3);
          a.motion.y = 0.0f;
          a.motion.clear();
          events.sound(0xf);
          events.shake(20.0f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0x2d5);
        if (i16(state.giantMode) < 1) {
          a.select_action(0x2dd);
        }
        a.response = 0;
        return;
      case 0x2dd:
        a.motion.clear();
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 0;
          a.remainingHits = 1;
          a.begin_pose(0.0f,0.0f,0.0f);
          events.sound(0x10);
          state.giantMode = 0;
        }
        if (i16(a.animation.cursor.index) == 0) {
          if (double(a.pose.scale[0]) <= double(0.333f)) {
            a.pose.scale[0] = 0.3330000042915344f;
            a.pose.scale[1] = 0.3330000042915344f;
            selectFrame(1);
            return;
          }
          a.pose.scale[0] = double(a.pose.scale[0]) - double(0.025f);
          a.pose.scale[1] = double(a.pose.scale[1]) - double(0.025f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        a.begin_pose(0.0f,0.0f,0.0f);
        return;
      case 0x2e4:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 0;
          a.motion.clear();
        }
        if (4 < i16(a.animation.cursor.index)) {
          biasCamera(1);
          state.introWait = i16(state.introWait) + 1;
          if (i16(state.introWait) == 0x3c) {
            events.sound(0x19);
          }
          if (0xb4 < i16(state.introWait)) {
            a.select_action(0x2e5);
            a.motion.x = double(other.motion.x);
            a.motion.y = 640.0f;
            return;
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          a.begin_pose(0.0f,0.0f,0.0f);
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.velocityY = 24.0f;
        return;
      case 0x2e5:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 0;
          a.remainingHits = 1;
          a.motion.velocityY = (float)(state.difficulty * -2 + -0x18);
        }
        if ((i16(a.animation.cursor.index) == 0) &&
           (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
            (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.clear();
          a.motion.y = 0.0f;
          selectFrame(1);
          events.shake(10.0f);
          events.sound(0x1a);
          return;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 0;
          a.remainingHits = 1;
          a.begin_pose(0.0f,0.0f,0.0f);
          events.sound(0x10);
        }
        if (i16(a.animation.cursor.index) == 6) {
          if (double(a.pose.scale[0]) <= double(0.333f)) {
            a.pose.scale[0] = 0.3330000042915344f;
            a.pose.scale[1] = 0.3330000042915344f;
            selectFrame(7);
            return;
          }
          a.pose.scale[0] = double(a.pose.scale[0]) - double(0.025f);
          a.pose.scale[1] = double(a.pose.scale[1]) - double(0.025f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.begin_pose(0.0f,0.0f,0.0f);
        a.select_action(0);
        return;
      case 0x2ef:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 0;
          a.motion.clear();
        }
        if ((i16(a.animation.cursor.index) == 4) && (9 < i16(a.animation.cursor.elapsed))) {
          if ((u8)u8(a.pose.channel(24)) < 0xb) {
            a.pose.set_channel(24,u8((u8)0x0));
          }
          else {
            a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + -10)));
          }
          step = random.range(0,100);
          if (step < 0x32) {
            particleCoordinateOrLayer = 1;
            direction = (u32)(u8)u8(a.motion.facing);
            effectY = double(a.motion.y);
            step = random.range(0,200);
            position = double(float(double(effectY)+100.0))-step;
            effectY = double(a.motion.x);
            step = random.range(0,200);
            owned(0x388,double(float(double(effectY)+100.0))-step,position,direction,particleCoordinateOrLayer);
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x2f0);
          events.sound(0x1d);
          if (state.difficulty < '\x01') {
            for (local_24 = 0; local_24 < 2; local_24 = local_24 + 1) {
              step = local_24 * 0x3c + 0x3c;
              a.launchParameters[0] = (float)step;
              owned(0x389,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
            }
          }
          else {
            for (local_20 = 0; local_20 < 3; local_20 = local_20 + 1) {
              step = local_20 * 0x3c + 0x1e;
              a.launchParameters[0] = (float)step;
              owned(0x389,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
            }
          }
          for (local_28 = 0; local_28 < 0x12; local_28 = local_28 + 1) {
            a.launchParameters[0] = (float)(local_28 * 0x14);
            a.launchParameters[1] = 3.0f;
            owned(0x38b,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
            if ('\x01' < state.difficulty) {
              a.launchParameters[0] = (float)(local_28 * 0x14);
              a.launchParameters[1] = 15.0f;
              owned(0x38b,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
            }
            if ('\x02' < state.difficulty) {
              a.launchParameters[0] = (float)(local_28 * 0x14);
              a.launchParameters[1] = 27.0f;
              owned(0x38b,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
            }
          }
        }
        if (i16(a.animation.cursor.index) != 4) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x387,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        a.begin_pose(0.0f,0.0f,0.0f);
        events.sound(0x1b);
        return;
      case 0x2f0:
        if ((((0 < i16(other.combat.health)) &&
             (i16(other.animation.cursor.index) == 2)) &&
            ('\0' < i8(other.combat.spellPhase))) &&
           (i16(other.animation.cursor.action) == 99)) {
          other.select_action(0x5f);
          other.transitionReady = 1;
          return;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 0;
          a.motion.clear();
          advancedFrame = random.range(0,5);
          state.impactX = advancedFrame + 5;
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          advancedFrame = random.range(0,0x3c);
          state.captionWait = advancedFrame + 100;
          if (double(a.motion.y) < double(180.0f)) {
            if (double(480.0f) <= double(a.motion.x)) {
              if (u8(a.motion.facing) == (u8)0x1) {
                state.impactX = 0xff5b;
              }
              else {
                state.impactX = 0xfff1;
              }
            }
            else if (u8(a.motion.facing) == (u8)0x1) {
              state.impactX = 0xfff1;
            }
            else {
              state.impactX = 0xff5b;
            }
          }
          if (double(250.0f) < double(a.motion.y)) {
            if (double(a.motion.x) <= double(800.0f)) {
              if (u8(a.motion.facing) == (u8)0x1) {
                state.impactX = 0xf;
              }
              else {
                state.impactX = 0xa5;
              }
            }
            else if (u8(a.motion.facing) == (u8)0x1) {
              state.impactX = 0xa5;
            }
            else {
              state.impactX = 0xf;
            }
          }
          if (double(a.motion.y) == double(0.0f)) {
            advancedFrame = random.range(0,0x32);
            state.impactX = -0x41 - advancedFrame;
          }
        }
        if (i16(a.animation.cursor.index) == 1) {
          if (0 < i16(state.captionWait)) {
            state.captionWait = i16(state.captionWait) + -2;
          }
          a.set_polar_velocity((float)(int)i16(state.impactX),(float)(int)i16(state.captionWait) * double(0.1f));
        }
        if ((i16(a.animation.cursor.index) == 3) || (i16(a.animation.cursor.index) == 4)) {
          if (0xf4 < (u8)u8(a.pose.channel(24))) {
            a.pose.set_channel(24,u8((u8)0xff));
            selectFrame(i16(a.animation.cursor.index) + 2);
            return;
          }
          a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + '\n')));
        }
        if (((i16(a.animation.cursor.index) == 5) || (i16(a.animation.cursor.index) == 6)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.15f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          selectFrame(7);
          a.motion.clear();
          a.motion.y = 0.0f;
          return;
        }
        if ((i16(a.animation.cursor.index) < 5) && (step = random.range(0,100), step < 0x32)) {
          particleCoordinateOrLayer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          effectY = double(a.motion.y);
          step = random.range(0,100);
          position = double(float(double(effectY)+50.0))-step;
          effectY = double(a.motion.x);
          step = random.range(0,100);
          owned(0x388,double(float(double(effectY)+50.0))-step,position,direction,particleCoordinateOrLayer);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          a.commandTimers[2] = state.difficulty * -0x1e + 0xb4;
        }
        if ((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) == 0)) {
          if ((int)i16(state.introWait) < state.difficulty + 1) {
            state.introWait = i16(state.introWait) + 1;
            selectFrame(1);
            return;
          }
          if (double(a.motion.x) <= double(other.motion.x)) {
            a.motion.facing = (u8)0x1;
          }
          else {
            a.motion.facing = (u8)0xff;
          }
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
        }
        if ((i16(a.animation.cursor.index) == 5) && (i16(a.animation.cursor.elapsed) == 0)) {
          selectFrame(3);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(5);
        return;
      case 0x2f9:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
          advancedFrame = random.range(0,0x3c);
          a.commandTimers[2] = advancedFrame + 300 + state.difficulty * -0x28;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        events.sound(5);
        owned(0x393,(float)((i8)u8(a.motion.facing) * 0x6b) + double(a.motion.x),double(a.motion.y) + double(94.0f),u8(a.motion.facing),1);
        return;
      case 0x303:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          state.captionWait = 0;
          state.impactX = 0;
          state.impactY = 0;
          state.spellLoops = 0;
          a.remainingHits = 0;
          a.response = 0;
          a.motion.clear();
        }
        if (((i16(state.spellLoops) == 0) &&
            (i16(other.animation.cursor.action) == 0x61)) &&
           (0x20 < i16(a.animation.cursor.index))) {
          events.shake(15.0f);
          state.spellLoops = i16(state.spellLoops) + 1;
          events.sound(0x17);
          scriptedDamage(0x708,100,1);
        }
        if (((i16(a.animation.cursor.index) < 5) && (i16(a.animation.cursor.elapsed) < 0x14)) &&
           (state.spellLoops = i16(state.spellLoops) + 1,
           4 < i16(state.spellLoops))) {
          shared(0x40,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 3),double(a.motion.y) + double(104.0f),1,1);
          state.spellLoops = 0;
        }
        if ((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x11)) {
          if (double(other.motion.y) == double(0.0f)) {
            a.remainingHits = 1;
          }
          else {
            a.remainingHits = 0;
          }
          if (a.response == 1) {
            a.motion.clear();
            selectFrame(0x11);
            other.select_action(0x65);
            other.motion.velocityX = 0.0f;
            other.motion.velocityY = 0.0f;
            other.motion.x =
                 (float)((i8)u8(a.motion.facing) * 0x2d) + double(a.motion.x);
            other.motion.y =
                 double(a.motion.y) + double(120.0f) + double(a.motion.velocityY);
            return;
          }
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.motion.clear();
            a.motion.y = 0.0f;
            a.select_action(0x304);
            return;
          }
        }
        if ((0x10 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x23)) {
          if (i16(a.animation.cursor.index) < 0x15) {
            biasCamera(0);
            a.motion.velocityY = double(a.motion.velocityY) - double(0.5f);
            if (double(a.motion.velocityY) < double(0.0f)) {
              a.motion.velocityY = 0.0f;
            }
            a.motion.velocityX = 0.0f;
            other.motion.x =
                 (float)((i8)u8(a.motion.facing) * 0x2d) + double(a.motion.x);
            other.motion.y =
                 double(a.motion.y) + double(120.0f) + double(a.motion.velocityY);
          }
          if ((0x14 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x1f)) {
            biasCamera(0);
            state.captionWait = i16(state.captionWait) + -1;
            events.shake(2.0f);
            if (i16(state.impactX) < 0x3c) {
              if (i16(state.captionWait) < 1) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y);
                step = random.range(0,0x780);
                owned(0x331,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x3c0),double(float(double(effectY)+960.0))-step,direction,particleCoordinateOrLayer);
                state.captionWait = 8;
                state.impactX = i16(state.impactX) + 1;
              }
              if (i16(state.captionWait) == 2) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y);
                step = random.range(0,0x780);
                owned(0x331,(float)((i8)u8(a.motion.facing) * 0x3c0) + double(a.motion.x),double(float(double(effectY)+960.0))-step,direction,particleCoordinateOrLayer);
                state.impactX = i16(state.impactX) + 1;
              }
              if (i16(state.captionWait) == 4) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y) + double(960.0f);
                step = random.range(0,0x780);
                owned(0x331,(float)((0x3c0 - step) * (int)(i8)u8(a.motion.facing)) +
                                   double(a.motion.x),effectY,direction,particleCoordinateOrLayer);
                state.impactX = i16(state.impactX) + 1;
              }
              if (i16(state.captionWait) == 6) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y) - double(960.0f);
                step = random.range(0,0x780);
                owned(0x331,(float)((0x3c0 - step) * (int)(i8)u8(a.motion.facing)) +
                                   double(a.motion.x),effectY,direction,particleCoordinateOrLayer);
                state.impactX = i16(state.impactX) + 1;
              }
            }
            if (i16(a.animation.cursor.index) < 0x17) {
              other.motion.x =
                   (float)((i8)u8(a.motion.facing) * 0x2d) + double(a.motion.x);
              other.motion.y =
                   double(a.motion.y) + double(120.0f) + double(a.motion.velocityY);
            }
          }
        }
        if ((0x22 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.6f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          a.select_action(10);
          advancedFrame = random.range(0,0x78);
          a.commandTimers[2] = advancedFrame + 0x78 + state.difficulty * -0x14;
          return;
        }
        if ((i16(state.impactY) == 0) &&
           (i16(other.animation.cursor.action) == 0x61)) {
          state.impactY = i16(state.impactY) + 1;
          events.shake(15.0f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          selectFrame(0x2a);
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 10.0f;
          a.motion.velocityY = 10.0f;
          a.motion.accelerationY = 0.6000000238418579f;
          state.spellLoops = 0;
        }
        if ((i16(a.animation.cursor.index) == 0x11) && (i16(a.animation.cursor.elapsed) == 0)) {
          selectFrame(9);
        }
        if ((i16(a.animation.cursor.index) == 0x13) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 12.5f;
        }
        if ((i16(a.animation.cursor.index) == 0x15) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.introWait) < 2) {
            state.introWait = i16(state.introWait) + 1;
            selectFrame(0x13);
            return;
          }
          state.introWait = 0;
        }
        if ((i16(a.animation.cursor.index) == 0x17) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.introWait) < 0) {
            state.introWait = i16(state.introWait) + 1;
            selectFrame(0x15);
            return;
          }
          events.sound(0x13);
          state.introWait = 0;
          other.select_action(0x6d);
          other.motion.x = double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(140.0f);
          other.motion.facing = u8(a.motion.facing);
        }
        if ((i16(a.animation.cursor.index) == 0x18) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 45.0f;
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x2c) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(128.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x19) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 5.0f;
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(85.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1a) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 25.0f;
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x19) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(65.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1b) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 85.0f;
          other.motion.x =
               double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x14);
          other.motion.y = double(a.motion.y) + double(50.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1c) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 160.0f;
          other.motion.x =
               double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x3c);
          other.motion.y = double(a.motion.y) + double(90.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1d) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = -165.0f;
          other.motion.x =
               double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x2d);
          other.motion.y = double(a.motion.y) + double(145.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1e) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = -110.0f;
          other.motion.x = double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(150.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1f) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.introWait) < 0xb) {
            state.introWait = i16(state.introWait) + 1;
            selectFrame(0x18);
            other.begin_pose(0.0f,0.0f,0.0f);
            other.pose.rotation[2] = -45.0f;
            other.motion.x =
                 (float)((i8)u8(a.motion.facing) * 0x2c) + double(a.motion.x);
            other.motion.y = double(a.motion.y) + double(128.0f);
            return;
          }
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x2c) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(110.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x20) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.select_action(0x6f);
          other.motion.facing = -(i8)u8(a.motion.facing);
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(85.0f);
        }
        if (i16(a.animation.cursor.index) != 0x21) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        other.motion.x =
             (float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x);
        other.motion.y = double(a.motion.y) + double(85.0f);
        other.select_action(0x49);
        other.impulseX = 36.0f;
        other.impulseY = -18.0f;
        other.motion.facing = -(i8)u8(a.motion.facing);
        events.sound(0x14);
        return;
      case 0x304:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 0;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        a.select_action(0);
        advancedFrame = random.range(0x3c,0x78);
        a.commandTimers[2] = advancedFrame + state.difficulty * -0x14;
        return;
      case 0x305:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          state.captionWait = 0;
          state.impactX = 0;
          state.impactY = 0;
          state.spellLoops = 0;
          a.remainingHits = 0;
          a.response = 0;
          a.motion.clear();
        }
        if ((i16(state.spellLoops) == 0) &&
           (i16(other.animation.cursor.action) == 0x59)) {
          events.shake(25.0f);
          state.spellLoops = i16(state.spellLoops) + 1;
          events.sound(0x18);
          scriptedDamage(3000,0x32,1);
        }
        if ((8 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x11)) {
          if (double(other.motion.y) == double(0.0f)) {
            a.remainingHits = 1;
          }
          else {
            a.remainingHits = 0;
          }
          if (a.response == 1) {
            a.motion.clear();
            selectFrame(0x11);
            other.select_action(0x65);
            other.motion.velocityX = 0.0f;
            other.motion.velocityY = 0.0f;
            other.motion.x =
                 (float)((i8)u8(a.motion.facing) * 0x2d) + double(a.motion.x);
            other.motion.y =
                 double(a.motion.y) + double(120.0f) + double(a.motion.velocityY);
            return;
          }
          a.motion.velocityY = double(a.motion.velocityY) - double(a.motion.accelerationY);
          if (double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
              (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f))) {
            a.motion.clear();
            a.motion.y = 0.0f;
            a.select_action(0x304);
            return;
          }
        }
        if ((0x10 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x23)) {
          if (i16(a.animation.cursor.index) < 0x15) {
            biasCamera(0);
            a.motion.velocityY = double(a.motion.velocityY) - double(0.5f);
            if (double(a.motion.velocityY) < double(0.0f)) {
              a.motion.velocityY = 0.0f;
            }
            a.motion.velocityX = 0.0f;
            other.motion.x =
                 (float)((i8)u8(a.motion.facing) * 0x2d) + double(a.motion.x);
            other.motion.y =
                 double(a.motion.y) + double(120.0f) + double(a.motion.velocityY);
          }
          if ((0x14 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0x1f)) {
            biasCamera(0);
            state.captionWait = i16(state.captionWait) + -1;
            events.shake(2.0f);
            if (i16(state.impactX) < 0x3c) {
              if (i16(state.captionWait) < 1) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y);
                step = random.range(0,0x780);
                owned(0x334,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x3c0),double(float(double(effectY)+960.0))-step,direction,particleCoordinateOrLayer);
                state.captionWait = 8;
                state.impactX = i16(state.impactX) + 1;
              }
              if (i16(state.captionWait) == 2) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y);
                step = random.range(0,0x780);
                owned(0x334,(float)((i8)u8(a.motion.facing) * 0x3c0) + double(a.motion.x),double(float(double(effectY)+960.0))-step,direction,particleCoordinateOrLayer);
                state.impactX = i16(state.impactX) + 1;
              }
              if (i16(state.captionWait) == 4) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y) + double(960.0f);
                step = random.range(0,0x780);
                owned(0x334,(float)((0x3c0 - step) * (int)(i8)u8(a.motion.facing)) +
                                   double(a.motion.x),effectY,direction,particleCoordinateOrLayer);
                state.impactX = i16(state.impactX) + 1;
              }
              if (i16(state.captionWait) == 6) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y) - double(960.0f);
                step = random.range(0,0x780);
                owned(0x334,(float)((0x3c0 - step) * (int)(i8)u8(a.motion.facing)) +
                                   double(a.motion.x),effectY,direction,particleCoordinateOrLayer);
                state.impactX = i16(state.impactX) + 1;
              }
            }
            if (i16(a.animation.cursor.index) < 0x17) {
              other.motion.x =
                   (float)((i8)u8(a.motion.facing) * 0x2d) + double(a.motion.x);
              other.motion.y =
                   double(a.motion.y) + double(120.0f) + double(a.motion.velocityY);
            }
          }
        }
        if ((0x22 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.6f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          a.select_action(10);
          return;
        }
        if ((i16(state.impactY) == 0) &&
           (i16(other.animation.cursor.action) == 0x61)) {
          state.impactY = i16(state.impactY) + 1;
          events.shake(15.0f);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          selectFrame(0x2a);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          hitFreeze(0x28);
          events.system_sound(0x17);
          shared(0x73,(float)((i8)u8(a.motion.facing) * 0x34) + double(a.motion.x),double(a.motion.y) + double(160.0f),u8(a.motion.facing),1);
          a.combat.spellExtent = double(a.combat.spellExtent) - double(100.0f);
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityX = 10.0f;
          a.motion.velocityY = 10.0f;
          a.motion.accelerationY = 0.6000000238418579f;
        }
        if ((i16(a.animation.cursor.index) == 0x11) && (i16(a.animation.cursor.elapsed) == 0)) {
          selectFrame(9);
        }
        if ((i16(a.animation.cursor.index) == 0x13) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 12.5f;
        }
        if ((i16(a.animation.cursor.index) == 0x15) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.introWait) < 2) {
            state.introWait = i16(state.introWait) + 1;
            selectFrame(0x13);
            return;
          }
          state.introWait = 0;
        }
        if ((i16(a.animation.cursor.index) == 0x17) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.introWait) < 0) {
            state.introWait = i16(state.introWait) + 1;
            selectFrame(0x15);
            return;
          }
          events.sound(0x15);
          state.introWait = 0;
          other.select_action(0x6d);
          other.motion.x = double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(140.0f);
          other.motion.facing = u8(a.motion.facing);
        }
        if ((i16(a.animation.cursor.index) == 0x18) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 45.0f;
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x2c) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(128.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x19) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 5.0f;
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x32) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(85.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1a) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 25.0f;
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x19) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(65.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1b) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 85.0f;
          other.motion.x =
               double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x14);
          other.motion.y = double(a.motion.y) + double(50.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1c) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = 160.0f;
          other.motion.x =
               double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x3c);
          other.motion.y = double(a.motion.y) + double(90.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1d) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = -165.0f;
          other.motion.x =
               double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x2d);
          other.motion.y = double(a.motion.y) + double(145.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1e) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.pose.rotation[2] = -110.0f;
          other.motion.x = double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(150.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x1f) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.introWait) < 0x21) {
            state.introWait = i16(state.introWait) + 1;
            selectFrame(0x18);
            other.begin_pose(0.0f,0.0f,0.0f);
            other.pose.rotation[2] = -45.0f;
            other.motion.x =
                 (float)((i8)u8(a.motion.facing) * 0x2c) + double(a.motion.x);
            other.motion.y = double(a.motion.y) + double(128.0f);
            return;
          }
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x2c) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(110.0f);
        }
        if ((i16(a.animation.cursor.index) == 0x20) && (i16(a.animation.cursor.elapsed) == 0)) {
          other.begin_pose(0.0f,0.0f,0.0f);
          other.select_action(0x6f);
          other.motion.facing = -(i8)u8(a.motion.facing);
          other.motion.x =
               (float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x);
          other.motion.y = double(a.motion.y) + double(85.0f);
        }
        if (i16(a.animation.cursor.index) != 0x21) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        other.motion.x =
             (float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x);
        other.motion.y = double(a.motion.y) + double(85.0f);
        other.select_action(0x58);
        other.impulseX = 36.0f;
        other.impulseY = -18.0f;
        other.motion.facing = -(i8)u8(a.motion.facing);
        events.sound(0x16);
        return;
      case 0x306:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          state.captionWait = 0;
          state.impactX = 0;
          state.impactY = 0;
          state.spellLoops = 0;
          a.remainingHits = 0;
          a.response = 0;
          a.motion.clear();
        }
        if (((6 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xe)) &&
           ((i16(state.impactY) < 0xf &&
            (state.spellLoops = i16(state.spellLoops) + 1,
            4 < i16(state.spellLoops))))) {
          shared(0x40,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0xc),double(a.motion.y) + double(109.0f),1,1);
          state.spellLoops = 0;
          state.impactY = i16(state.impactY) + 1;
        }
        if ((3 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xe)) {
          a.motion.velocityY = double(a.motion.velocityY) - double(0.7f);
          if (double(a.motion.velocityY) < double(1.0f)) {
            a.motion.velocityY = 1.0f;
          }
          a.motion.velocityX = 0.0f;
          if ((6 < i16(a.animation.cursor.index)) && (i16(a.animation.cursor.index) < 0xe)) {
            state.captionWait = i16(state.captionWait) + -1;
            events.shake(2.0f);
            if (i16(state.impactX) < 0x2d) {
              if (i16(state.captionWait) < 1) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y);
                step = random.range(0,0x780);
                owned(0x38f,double(a.motion.x) - (float)((i8)u8(a.motion.facing) * 0x3c0),double(float(double(effectY)+960.0))-step,direction,particleCoordinateOrLayer);
                state.captionWait = 8;
                state.impactX = i16(state.impactX) + 1;
              }
              if (i16(state.captionWait) == 2) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y);
                step = random.range(0,0x780);
                owned(0x38f,(float)((i8)u8(a.motion.facing) * 0x3c0) + double(a.motion.x),double(float(double(effectY)+960.0))-step,direction,particleCoordinateOrLayer);
                state.impactX = i16(state.impactX) + 1;
              }
              if (i16(state.captionWait) == 4) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y) + double(960.0f);
                step = random.range(0,0x780);
                owned(0x38f,(float)((0x3c0 - step) * (int)(i8)u8(a.motion.facing)) +
                                   double(a.motion.x),effectY,direction,particleCoordinateOrLayer);
                state.impactX = i16(state.impactX) + 1;
              }
              if (i16(state.captionWait) == 6) {
                particleCoordinateOrLayer = 1;
                direction = (u32)(u8)u8(a.motion.facing);
                effectY = double(a.motion.y) - double(960.0f);
                step = random.range(0,0x780);
                owned(0x38f,(float)((0x3c0 - step) * (int)(i8)u8(a.motion.facing)) +
                                   double(a.motion.x),effectY,direction,particleCoordinateOrLayer);
                state.impactX = i16(state.impactX) + 1;
              }
            }
          }
        }
        if ((0x11 < i16(a.animation.cursor.index)) &&
           (a.motion.velocityY = double(a.motion.velocityY) - double(0.7f),
           double(a.motion.y) + double(a.motion.velocityY) < double(0.0f) !=
           (double(a.motion.y) + double(a.motion.velocityY) == double(0.0f)))) {
          a.motion.y = 0.0f;
          a.motion.clear();
          a.select_action(10);
          advancedFrame = random.range(0,0x3c);
          a.commandTimers[2] = advancedFrame + 100 + state.difficulty * -0x14;
          return;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          selectFrame(0x19);
        }
        if ((i16(a.animation.cursor.index) == 4) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.velocityY = 12.5f;
        }
        if ((i16(a.animation.cursor.index) == 6) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          impactWord = i16(i32(double(a.motion.x)+double(a.motion.facing)*0));
          state.anchorX = impactWord;
          impactWord = i16(i32(double(a.motion.y)+140));
          state.anchorY = impactWord;
          events.sound(0x13);
        }
        if ((i16(a.animation.cursor.index) == 7) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.x)+double(a.motion.facing)*44));
          state.anchorX = impactWord;
          impactWord = i16(i32(double(a.motion.y)+128));
          state.anchorY = impactWord;
        }
        if ((i16(a.animation.cursor.index) == 8) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.x)+double(a.motion.facing)*50));
          state.anchorX = impactWord;
          impactWord = i16(i32(double(a.motion.y)+85));
          state.anchorY = impactWord;
        }
        if ((i16(a.animation.cursor.index) == 9) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.x)+double(a.motion.facing)*25));
          state.anchorX = impactWord;
          impactWord = i16(i32(double(a.motion.y)+65));
          state.anchorY = impactWord;
        }
        if ((i16(a.animation.cursor.index) == 10) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.x)+double(a.motion.facing)*-20));
          state.anchorX = impactWord;
          impactWord = i16(i32(double(a.motion.y)+50));
          state.anchorY = impactWord;
        }
        if ((i16(a.animation.cursor.index) == 0xb) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.x)+double(a.motion.facing)*-60));
          state.anchorX = impactWord;
          impactWord = i16(i32(double(a.motion.y)+90));
          state.anchorY = impactWord;
        }
        if ((i16(a.animation.cursor.index) == 0xc) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.x)+double(a.motion.facing)*-45));
          state.anchorX = impactWord;
          impactWord = i16(i32(double(a.motion.y)+145));
          state.anchorY = impactWord;
        }
        if ((i16(a.animation.cursor.index) == 0xd) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.x)+double(a.motion.facing)*0));
          state.anchorX = impactWord;
          impactWord = i16(i32(double(a.motion.y)+150));
          state.anchorY = impactWord;
        }
        if ((i16(a.animation.cursor.index) == 0xe) && (i16(a.animation.cursor.elapsed) == 0)) {
          if (i16(state.introWait) < 0xb) {
            state.introWait = i16(state.introWait) + 1;
            selectFrame(7);
            impactWord = i16(i32(double(a.motion.x)+double(a.motion.facing)*44));
            state.anchorX = impactWord;
            impactWord = i16(i32(double(a.motion.y)+128));
            state.anchorY = impactWord;
            return;
          }
          impactWord = i16(i32(double(a.motion.x)+double(a.motion.facing)*44));
          state.anchorX = impactWord;
          impactWord = i16(i32(double(a.motion.y)+110));
          state.anchorY = impactWord;
        }
        if ((i16(a.animation.cursor.index) == 0xf) && (i16(a.animation.cursor.elapsed) == 0)) {
          impactWord = i16(i32(double(a.motion.x)+double(a.motion.facing)*80));
          state.anchorX = impactWord;
          impactWord = i16(i32(double(a.motion.y)+85));
          state.anchorY = impactWord;
        }
        if (i16(a.animation.cursor.index) != 0x10) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        impactWord = i16(i32(double(a.motion.x)+double(a.motion.facing)*80));
        state.anchorX = impactWord;
        impactWord = i16(i32(double(a.motion.y)+85));
        state.anchorY = impactWord;
        events.sound(0x14);
        owned(0x390,(float)((i8)u8(a.motion.facing) * 0x50) + double(a.motion.x),double(a.motion.y) + double(85.0f),u8(a.motion.facing),1);
        return;
      case 0x30c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          a.remainingHits = 1;
          state.captionWait = 0;
          a.response = 0;
        }
        if (((i16(a.animation.cursor.index) == 2) && (i16(a.animation.cursor.elapsed) < 0x1e)) &&
           (state.captionWait = i16(state.captionWait) + 1,
           4 < i16(state.captionWait))) {
          shared(0x3e,(float)((i8)u8(a.motion.facing) * 0xe) + double(a.motion.x),double(a.motion.y) + double(145.0f),1,1);
          state.captionWait = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(9);
          advancedFrame = random.range(0,0x3c);
          a.commandTimers[2] = advancedFrame + 300 + state.difficulty * -0x28;
        }
        if (i16(a.animation.cursor.index) != 6) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        owned(0x340,(float)((i8)u8(a.motion.facing) * 100) + double(a.motion.x),0.0f,u8(a.motion.facing),0);
        owned(0x340,double(other.motion.x),0.0f,u8(a.motion.facing),0);
        shared(0x7f,double(a.motion.x),0.0f,u8(a.motion.facing),0);
        events.sound(5);
        events.shake(15.0f);
        if (state.difficulty == '\x03') {
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39b,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39b,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39b,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39b,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39d,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39d,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
        }
        if (state.difficulty == '\x02') {
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39b,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39b,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39d,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39d,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39d,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39d,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
        }
        if (state.difficulty == '\x01') {
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39d,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39d,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39d,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39d,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39f,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
          uVar10 = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          particleCoordinateOrLayer = double(a.motion.y);
          effectY = double(other.motion.x);
          step = random.range(0,300);
          owned(0x39f,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
        }
        if (state.difficulty != '\0') {
          return;
        }
        uVar10 = 1;
        direction = (u32)(u8)u8(a.motion.facing);
        particleCoordinateOrLayer = double(a.motion.y);
        effectY = double(other.motion.x);
        step = random.range(0,300);
        owned(0x39f,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
        uVar10 = 1;
        direction = (u32)(u8)u8(a.motion.facing);
        particleCoordinateOrLayer = double(a.motion.y);
        effectY = double(other.motion.x);
        step = random.range(0,300);
        owned(0x39f,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
        uVar10 = 1;
        direction = (u32)(u8)u8(a.motion.facing);
        particleCoordinateOrLayer = double(a.motion.y);
        effectY = double(other.motion.x);
        step = random.range(0,300);
        owned(0x39f,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
        uVar10 = 1;
        direction = (u32)(u8)u8(a.motion.facing);
        particleCoordinateOrLayer = double(a.motion.y);
        effectY = double(other.motion.x);
        step = random.range(0,300);
        owned(0x39f,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
        uVar10 = 1;
        direction = (u32)(u8)u8(a.motion.facing);
        particleCoordinateOrLayer = double(a.motion.y);
        effectY = double(other.motion.x);
        step = random.range(0,300);
        owned(0x39d,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
        uVar10 = 1;
        direction = (u32)(u8)u8(a.motion.facing);
        particleCoordinateOrLayer = double(a.motion.y);
        effectY = double(other.motion.x);
        step = random.range(0,300);
        owned(0x39d,double(float(double(effectY)+150.0))-step,particleCoordinateOrLayer,direction,uVar10);
        return;
      case 0x316:
        a.motion.x = 640.0f;
        other.motion.opponentPush = 0.0f;
        a.motion.facing = (u8)0xff;
        if (((0 < i16(other.combat.health)) &&
            (i16(other.animation.cursor.index) == 2)) &&
           (('\0' < i8(other.combat.spellPhase) &&
            (i16(other.animation.cursor.action) == 99)))) {
          other.select_action(0x5f);
          other.transitionReady = 1;
          return;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
          state.introWait = 0;
          state.captionWait = 0;
          state.impactX = 0;
          state.impactY = 0;
          state.spellLoops = 0;
        }
        if (i16(a.animation.cursor.index) < 2) {
          step = random.range(0,100);
          if (step < 0x32) {
            particleCoordinateOrLayer = 1;
            direction = (u32)(u8)u8(a.motion.facing);
            effectY = double(a.motion.y);
            step = random.range(0,200);
            position = double(float(double(effectY)+100.0))-step;
            effectY = double(a.motion.x);
            step = random.range(0,200);
            owned(0x388,double(float(double(effectY)+100.0))-step,position,direction,particleCoordinateOrLayer);
          }
          if (0xfe < (u8)u8(a.pose.channel(24))) {
            a.pose.set_channel(24,u8((u8)0xff));
            selectFrame(i16(a.animation.cursor.index) + 2);
            state.introWait = 0;
            return;
          }
          a.pose.set_channel(24,u8((u8)((i8)u8(a.pose.channel(24)) + '\x05')));
        }
        if (1 < i16(a.animation.cursor.index)) {
          state.captionWait = i16(state.captionWait) + 1;
          state.impactX = i16(state.impactX) + 1;
          if (4 < i16(state.captionWait)) {
            shared(0x3e,double(a.motion.x),double(a.motion.y) + double(90.0f),1,1)
            ;
            state.captionWait = 0;
          }
          if (i16(a.combat.health) < 0x1d4c) {
            a.select_action(0x317);
            state.introWait = 0;
            return;
          }
          if (state.difficulty * -0x1e + 0xf0 < (int)i16(state.impactX)) {
            events.sound(0x1d);
            for (local_2c = 0; local_2c < 0x12; local_2c = local_2c + 1) {
              a.launchParameters[0] = (float)(local_2c * 0x14);
              a.launchParameters[1] = 12.0f;
              owned(0x38b,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
            }
            state.impactX = 0;
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          selectFrame(2);
          return;
        }
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(0);
        return;
      case 0x317:
        a.motion.x = 640.0f;
        other.motion.opponentPush = 0.0f;
        state.captionWait = i16(state.captionWait) + 1;
        if ((((0 < i16(other.combat.health)) &&
             (i16(other.animation.cursor.index) == 2)) &&
            ('\0' < i8(other.combat.spellPhase))) &&
           (i16(other.animation.cursor.action) == 99)) {
          other.select_action(0x5f);
          other.transitionReady = 1;
          return;
        }
        if (4 < i16(state.captionWait)) {
          shared(0x40,double(a.motion.x),double(a.motion.y) + double(90.0f),1,1);
          state.captionWait = 0;
        }
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
        }
        if (1 < i16(a.animation.cursor.index)) {
          state.impactX = i16(state.impactX) + 1;
          hudAlpha[1] = 0;
          if (i16(a.combat.health) < 5000) {
            a.select_action(0x318);
            state.introWait = 0;
            return;
          }
          if (state.difficulty * -0x1e + 0xf0 < (int)i16(state.impactX)) {
            events.sound(0x1d);
            for (local_30 = 0; local_30 < 0x12; local_30 = local_30 + 1) {
              a.launchParameters[0] = (float)(local_30 * 0x14);
              a.launchParameters[1] = 12.0f;
              owned(0x38b,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
            }
            state.impactX = 0;
          }
          if (i16(a.animation.cursor.elapsed) == 0) {
            events.sound(0x1c);
            a.launchParameters[0] = (float)(int)i16(state.impactY);
            a.launchParameters[1] = 15.0f;
            a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
            owned(0x397,(double(a.motion.velocityX) + double(5.0f)) * (float)(int)(i8)u8(a.motion.facing)
                               + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
            if (state.difficulty == '\x01') {
              a.launchParameters[0] = (float)(i16(state.impactY) + 0xb4);
              a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
              owned(0x397,(double(a.motion.velocityX) + double(5.0f)) *
                                 (float)(int)(i8)u8(a.motion.facing) + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
            }
            if (state.difficulty == '\x02') {
              a.launchParameters[0] = (float)(i16(state.impactY) + 0x78);
              a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
              owned(0x397,(double(a.motion.velocityX) + double(5.0f)) *
                                 (float)(int)(i8)u8(a.motion.facing) + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
              a.launchParameters[0] = (float)(i16(state.impactY) + -0x78);
              a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
              owned(0x397,(double(a.motion.velocityX) + double(5.0f)) *
                                 (float)(int)(i8)u8(a.motion.facing) + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
            }
            if (state.difficulty == '\x03') {
              a.launchParameters[0] = (float)(i16(state.impactY) + 0xb4);
              a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
              owned(0x397,(double(a.motion.velocityX) + double(5.0f)) *
                                 (float)(int)(i8)u8(a.motion.facing) + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
              a.launchParameters[0] = (float)(i16(state.impactY) + 0x5a);
              a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
              owned(0x397,(double(a.motion.velocityX) + double(5.0f)) *
                                 (float)(int)(i8)u8(a.motion.facing) + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
              a.launchParameters[0] = (float)(i16(state.impactY) + -0x5a);
              a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
              owned(0x397,(double(a.motion.velocityX) + double(5.0f)) *
                                 (float)(int)(i8)u8(a.motion.facing) + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
            }
            a.motion.velocityX = 0.0f;
            a.motion.velocityY = 0.0f;
            state.impactY = i16(state.impactY) + 5;
            if (0xb4 < i16(state.impactY)) {
              state.impactY = i16(state.impactY) + -0x168;
            }
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          selectFrame(2);
          return;
        }
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (0x13 < i16(state.introWait)) {
          events.sound(0x21);
          ownedWithDraw(0x386,0.0f,480.0f,1,3,1,0,0);
          return;
        }
        selectFrame(0);
        state.introWait = i16(state.introWait) + 1;
        return;
      case 0x318:
        a.motion.x = 640.0f;
        other.motion.opponentPush = 0.0f;
        state.captionWait = i16(state.captionWait) + 1;
        if (((0 < i16(other.combat.health)) &&
            (i16(other.animation.cursor.index) == 2)) &&
           (('\0' < i8(other.combat.spellPhase) &&
            (i16(other.animation.cursor.action) == 99)))) {
          other.select_action(0x5f);
          other.transitionReady = 1;
          return;
        }
        if (4 < i16(state.captionWait)) {
          shared(0x3e,double(a.motion.x),double(a.motion.y) + double(90.0f),1,1);
          state.captionWait = 0;
        }
        hudAlpha[1] = 0;
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
        }
        if (1 < i16(a.animation.cursor.index)) {
          state.impactX = i16(state.impactX) + 1;
          state.spellLoops = i16(state.spellLoops) + 1;
          hudAlpha[0] = 0;
          if (i16(a.combat.health) < 0x9c4) {
            a.select_action(0x319);
            owned(0x398,double(a.motion.x),double(a.motion.y) + double(100.0f),1,0);
            state.introWait = 0;
            return;
          }
          if (state.difficulty * -0x1e + 0xf0 < (int)i16(state.impactX)) {
            events.sound(0x1d);
            for (local_34 = 0; local_34 < 0x12; local_34 = local_34 + 1) {
              a.launchParameters[0] = (float)(local_34 * 0x14);
              a.launchParameters[1] = 12.0f;
              owned(0x38b,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
            }
            state.impactX = 0;
          }
          step = state.difficulty * -0x28 + 0x168;
          if (step < i16(state.spellLoops)) {
            if (state.difficulty < '\x02') {
              owned(0x3a1,double(a.motion.x),double(a.motion.y) + double(250.0f),u8(a.motion.facing),1);
            }
            if (state.difficulty == '\x02') {
              owned(0x3a0,double(a.motion.x),double(a.motion.y) + double(250.0f),u8(a.motion.facing),1);
            }
            if (state.difficulty == '\x03') {
              owned(0x396,double(a.motion.x),double(a.motion.y) + double(250.0f),u8(a.motion.facing),1);
            }
            state.spellLoops = 0;
          }
          if (i16(a.animation.cursor.elapsed) == 0) {
            events.sound(0x1c);
            a.launchParameters[0] = (float)(int)i16(state.impactY);
            a.launchParameters[1] = 15.0f;
            a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
            owned(0x397,(double(a.motion.velocityX) + double(5.0f)) * (float)(int)(i8)u8(a.motion.facing)
                               + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
            if (state.difficulty == '\x01') {
              a.launchParameters[0] = (float)(i16(state.impactY) + 0xb4);
              a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
              owned(0x397,(double(a.motion.velocityX) + double(5.0f)) *
                                 (float)(int)(i8)u8(a.motion.facing) + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
            }
            if (state.difficulty == '\x02') {
              a.launchParameters[0] = (float)(i16(state.impactY) + 0x78);
              a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
              owned(0x397,(double(a.motion.velocityX) + double(5.0f)) *
                                 (float)(int)(i8)u8(a.motion.facing) + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
              a.launchParameters[0] = (float)(i16(state.impactY) + -0x78);
              a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
              owned(0x397,(double(a.motion.velocityX) + double(5.0f)) *
                                 (float)(int)(i8)u8(a.motion.facing) + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
            }
            if (state.difficulty == '\x03') {
              a.launchParameters[0] = (float)(i16(state.impactY) + 0xb4);
              a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
              owned(0x397,(double(a.motion.velocityX) + double(5.0f)) *
                                 (float)(int)(i8)u8(a.motion.facing) + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
              a.launchParameters[0] = (float)(i16(state.impactY) + 0x5a);
              a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
              owned(0x397,(double(a.motion.velocityX) + double(5.0f)) *
                                 (float)(int)(i8)u8(a.motion.facing) + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
              a.launchParameters[0] = (float)(i16(state.impactY) + -0x5a);
              a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
              owned(0x397,(double(a.motion.velocityX) + double(5.0f)) *
                                 (float)(int)(i8)u8(a.motion.facing) + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
            }
            a.motion.velocityX = 0.0f;
            a.motion.velocityY = 0.0f;
            state.impactY = i16(state.impactY) + 5;
            if (0xb4 < i16(state.impactY)) {
              state.impactY = i16(state.impactY) + -0x168;
            }
          }
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          selectFrame(2);
          return;
        }
        if (i16(a.animation.cursor.index) != 2) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (0x13 < i16(state.introWait)) {
          events.sound(0x21);
          ownedWithDraw(0x385,0.0f,0.0f,1,3,1,0,0);
          return;
        }
        selectFrame(0);
        state.introWait = i16(state.introWait) + 1;
        return;
      case 0x319:
        a.motion.x = 640.0f;
        other.motion.opponentPush = 0.0f;
        state.captionWait = i16(state.captionWait) + 1;
        state.impactX = i16(state.impactX) + 1;
        state.spellLoops = i16(state.spellLoops) + 1;
        if ((((0 < i16(other.combat.health)) &&
             (i16(other.animation.cursor.index) == 2)) &&
            ('\0' < i8(other.combat.spellPhase))) &&
           (i16(other.animation.cursor.action) == 99)) {
          other.select_action(0x5f);
          other.transitionReady = 1;
          return;
        }
        if (4 < i16(state.captionWait)) {
          shared(0x3e,double(a.motion.x),double(a.motion.y) + double(90.0f),1,1);
          state.captionWait = 0;
        }
        if (state.difficulty * -0x1e + 0xf0 < (int)i16(state.impactX)) {
          events.sound(0x1d);
          for (local_38 = 0; local_38 < 0x12; local_38 = local_38 + 1) {
            a.launchParameters[0] = (float)(local_38 * 0x14);
            a.launchParameters[1] = 12.0f;
            owned(0x38b,double(a.motion.x),double(a.motion.y) + double(100.0f),u8(a.motion.facing),1);
          }
          state.impactX = 0;
        }
        step = state.difficulty * -0x28 + 0x168;
        if (step < i16(state.spellLoops)) {
          if (state.difficulty < '\x02') {
            owned(0x3a1,double(a.motion.x),double(a.motion.y) + double(250.0f),u8(a.motion.facing),1);
          }
          if (state.difficulty == '\x02') {
            owned(0x3a0,double(a.motion.x),double(a.motion.y) + double(250.0f),u8(a.motion.facing),1);
          }
          if (state.difficulty == '\x03') {
            owned(0x396,double(a.motion.x),double(a.motion.y) + double(250.0f),u8(a.motion.facing),1);
          }
          state.spellLoops = 0;
        }
        if (i16(a.animation.cursor.elapsed) == 0) {
          events.sound(0x1c);
          a.launchParameters[0] = (float)(int)i16(state.impactY);
          a.launchParameters[1] = 15.0f;
          a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
          owned(0x397,(double(a.motion.velocityX) + double(5.0f)) * (float)(int)(i8)u8(a.motion.facing) +
                             double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
          if (state.difficulty == '\x01') {
            a.launchParameters[0] = (float)(i16(state.impactY) + 0xb4);
            a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
            owned(0x397,(double(a.motion.velocityX) + double(5.0f)) * (float)(int)(i8)u8(a.motion.facing)
                               + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
          }
          if (state.difficulty == '\x02') {
            a.launchParameters[0] = (float)(i16(state.impactY) + 0x78);
            a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
            owned(0x397,(double(a.motion.velocityX) + double(5.0f)) * (float)(int)(i8)u8(a.motion.facing)
                               + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(i16(state.impactY) + -0x78);
            a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
            owned(0x397,(double(a.motion.velocityX) + double(5.0f)) * (float)(int)(i8)u8(a.motion.facing)
                               + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
          }
          if (state.difficulty == '\x03') {
            a.launchParameters[0] = (float)(i16(state.impactY) + 0xb4);
            a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
            owned(0x397,(double(a.motion.velocityX) + double(5.0f)) * (float)(int)(i8)u8(a.motion.facing)
                               + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(i16(state.impactY) + 0x5a);
            a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
            owned(0x397,(double(a.motion.velocityX) + double(5.0f)) * (float)(int)(i8)u8(a.motion.facing)
                               + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
            a.launchParameters[0] = (float)(i16(state.impactY) + -0x5a);
            a.set_polar_velocity(double(a.launchParameters[0]),200.0f);
            owned(0x397,(double(a.motion.velocityX) + double(5.0f)) * (float)(int)(i8)u8(a.motion.facing)
                               + double(a.motion.x),(double(a.motion.y) + double(114.0f)) - double(a.motion.velocityY),u8(a.motion.facing),1);
          }
          a.motion.velocityX = 0.0f;
          a.motion.velocityY = 0.0f;
          state.impactY = i16(state.impactY) + 5;
          if (0xb4 < i16(state.impactY)) {
            state.impactY = i16(state.impactY) + -0x168;
          }
        }
        hudAlpha[1] = 0;
        hudAlpha[0] = 0;
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame != i16(a.animation.cursor.frameCount)) {
          return;
        }
        selectFrame(0);
        return;
      case 0x31a:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 0;
          a.motion.clear();
        }
        if (0x14 < i16(state.introWait)) {
          hudAlpha[1] = 0;
          other.selectedSpell = 0;
          consumeSpirit(other,1000);
        }
        if (0x28 < i16(state.introWait)) {
          hudAlpha[0] = 0;
          route = 0;
          completedStages = 0;
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if (i16(a.animation.cursor.index) != 9) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        selectFrame(7);
        state.introWait = i16(state.introWait) + 1;
        if (i16(state.introWait) == 0x15) {
          ownedWithDraw(0x386,0.0f,480.0f,1,3,1,0,0);
        }
        if (i16(state.introWait) != 0x29) {
          return;
        }
        ownedWithDraw(0x385,0.0f,0.0f,1,3,1,0,0);
        return;
      case 0x31b:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
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
          owned(0x3e5,double(a.motion.x),double(a.motion.y),u8(a.motion.facing),1);
        }
        if (i16(a.animation.cursor.index) != 7) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (7 < i16(state.introWait)) {
          return;
        }
        state.introWait = i16(state.introWait) + 1;
        selectFrame(5);
        return;
      case 0x31c:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
          a.response = 1;
          a.motion.clear();
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0);
        }
        if ((i16(a.animation.cursor.index) == 0x14) && (i16(a.animation.cursor.elapsed) == 0)) {
          fade(0);
          a.begin_spell(mode,replayMode,core);
          events.system_sound(0x17);
        }
        if (i16(a.animation.cursor.index) != 0x17) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        if (7 < i16(state.introWait)) {
          return;
        }
        state.introWait = i16(state.introWait) + 1;
        selectFrame(0x15);
        a.transitionReady = (u8)0x1;
        return;
      case 0x31d:
        if ((i16(a.animation.cursor.index) == 0) && (i16(a.animation.cursor.elapsed) == 0)) {
          state.introWait = 0;
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
        if ((0 < i16(a.animation.cursor.index)) && (step = random.range(0,100), step < 0x32)) {
          particleCoordinateOrLayer = 1;
          direction = (u32)(u8)u8(a.motion.facing);
          effectY = double(a.motion.y);
          step = random.range(0,200);
          position = double(float(double(effectY)+100.0))-step;
          effectY = double(a.motion.x);
          step = random.range(0,200);
          owned(0x388,double(float(double(effectY)+100.0))-step,position,direction,particleCoordinateOrLayer);
        }
        advancedFrame = a.animation.advance();
        if (advancedFrame == i16(a.animation.cursor.frameCount)) {
          a.select_action(0x316);
          a.transitionReady = (u8)0x1;
          a.motion.facing = (u8)0x1;
          return;
        }
        if ((i16(a.animation.cursor.index) == 1) && (i16(a.animation.cursor.elapsed) == 0)) {
          a.begin_pose(0.0f,0.0f,0.0f);
          fade(0);
          a.begin_spell(mode,replayMode,core);
          events.system_sound(0x17);
        }
        if (i16(a.animation.cursor.index) != 3) {
          return;
        }
        if (i16(a.animation.cursor.elapsed) != 0) {
          return;
        }
        a.motion.facing = (u8)0x1;
        a.motion.velocityX = (double(640.0f) - double(a.motion.x)) / double(60.0f);
        a.motion.velocityY = 3.0f;
        return;
      }
    }
switchD_005aef1a_default:
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
    goto LAB_005c0674;
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
    goto LAB_005c0674;
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
    a.motion.velocityX = 5.5f;
    goto LAB_005c0674;
  case 5:
    a.motion.velocityX = -5.5f;
LAB_005c0674:
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
            a.motion.velocityY = 16.0f;
            a.motion.accelerationY = 0.75f;
            if ('\0' < (i8)u8(a.carryMomentum)) {
              a.motion.velocityX = 3.0f;
              a.motion.velocityY = 15.0f;
              a.motion.accelerationY = 1.25f;
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
            a.motion.velocityX = 8.0f;
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
            a.motion.velocityX = -8.0f;
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
    goto switchD_005aef1a_default;
  }
  return;

}
}
