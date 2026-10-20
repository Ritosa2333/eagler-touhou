#include "MarisaObjects.hpp"
#include <cmath>
namespace th075 {
void advance_marisa_orbit(MarisaObject& p,FighterActor& owner,FighterActor& other,MarisaState& state,ReplayStreams& random,FighterActionEvents& events){

    double aimRadians=0;
    const auto atan=[&](float y,float x){return std::atan2(double(y),double(x));};
    const auto channel=[&](u32 shift){return u8(p.pose.color>>shift);};
    const auto setChannel=[&](u32 shift,u8 value){p.pose.color=(p.pose.color&~(255u<<shift))|(u32(value)<<shift);};
    const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters);};
    const auto shared=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Shared,id,x,y,facing,layer,owner.launchParameters);};

  u16 byteValue;
  i16 advancedFrame;
  int coordinate;
  
  if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
    p.counter = 0;
    p.timer = 0;
    p.angle = 0;
    p.cycle = 0;
    p.variant = 0;
    p.begin_pose(0.0f,0.0f,0.0f);
    p.hits = 1;
    p.response = 0;
    p.pose.scale[0] = 1.0f;
    p.pose.scale[1] = 1.0f;
    setChannel(24,u8(0xff));
    setChannel(16,u8(0xff));
    setChannel(8,u8(0xff));
    setChannel(0,u8(0xff));
  }
  if (i16(p.animation.cursor.action) == 0x33c) {
    byteValue = i32(p.motion.x);
    state.spellCounters[1] = byteValue;
    byteValue = i32(p.motion.y);
    state.spellCounters[7] = byteValue;
  }
  if (i16(p.animation.cursor.action) == 0x33d) {
    byteValue = i32(p.motion.x);
    state.spellCounters[2] = byteValue;
    byteValue = i32(p.motion.y);
    state.spellCounters[8] = byteValue;
  }
  if (i16(p.animation.cursor.action) == 0x33e) {
    byteValue = i32(p.motion.x);
    state.spellCounters[3] = byteValue;
    byteValue = i32(p.motion.y);
    state.spellCounters[9] = byteValue;
  }
  if (i16(p.animation.cursor.action) == 0x33f) {
    byteValue = i32(p.motion.x);
    state.spellCounters[4] = byteValue;
    byteValue = i32(p.motion.y);
    state.spellCounters[10] = byteValue;
  }
  if (i16(p.animation.cursor.action) == 0x340) {
    byteValue = i32(p.motion.x);
    state.spellCounters[5] = byteValue;
    byteValue = i32(p.motion.y);
    state.spellCounters[11] = byteValue;
  }
  if (i16(p.animation.cursor.action) == 0x341) {
    byteValue = i32(p.motion.x);
    state.spellCounters[6] = byteValue;
    byteValue = i32(p.motion.y);
    state.spellCounters[12] = byteValue;
  }
  if ((((((i16(owner.animation.cursor.action) == 0x2c5) ||
         (i16(owner.animation.cursor.action) == 0x2c6)) &&
        (i16(owner.animation.cursor.index) == 5)) &&
       ((i16(owner.animation.cursor.elapsed) == 0 && (i16(p.angle) == 0)))) &&
      (i16(p.animation.cursor.index) == 1)) &&
     (((((i16(p.animation.cursor.action) == 0x341 && (i16(state.chargeRemaining) == 6)
         ) || ((i16(p.animation.cursor.action) == 0x340 &&
               (i16(state.chargeRemaining) == 5)))) ||
       (((i16(p.animation.cursor.action) == 0x33f && (i16(state.chargeRemaining) == 4)
         ) || ((i16(p.animation.cursor.action) == 0x33e &&
               (i16(state.chargeRemaining) == 3)))))) ||
      (((i16(p.animation.cursor.action) == 0x33d && (i16(state.chargeRemaining) == 2))
       || ((i16(p.animation.cursor.action) == 0x33c &&
           (i16(state.chargeRemaining) == 1)))))))) {
    p.angle = 1;
    aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                 double(p.motion.y),(double(other.motion.x) -
                 double(p.motion.x)) * (float)(int)i8(p.motion.facing));
    byteValue = i32(aimRadians/double(0.017453289f));
    p.cycle = byteValue;
    p.frame(2);
    state.chargeCount = i16(state.chargeCount) + -1
    ;
    state.chargeRemaining = i16(state.chargeRemaining) + -1
    ;
  }
  if (((((i16(owner.animation.cursor.action) == 0x2cc) ||
        (i16(owner.animation.cursor.action) == 0x2cd)) &&
       (i16(owner.animation.cursor.index) == 5)) &&
      ((i16(owner.animation.cursor.elapsed) == 0 &&
       ((i16(p.animation.cursor.index) == 1 || (i16(p.animation.cursor.index) == 2)))))) &&
     ((((((i16(p.animation.cursor.action) == 0x341 &&
          (i16(state.chargeRemaining) == 6)) ||
         ((i16(p.animation.cursor.action) == 0x340 &&
          (i16(state.chargeRemaining) == 5)))) ||
        (((i16(p.animation.cursor.action) == 0x33f &&
          (i16(state.chargeRemaining) == 4)) ||
         ((i16(p.animation.cursor.action) == 0x33e &&
          (i16(state.chargeRemaining) == 3)))))) ||
       ((i16(p.animation.cursor.action) == 0x33d && (i16(state.chargeRemaining) == 2))
       )) || ((i16(p.animation.cursor.action) == 0x33c &&
              (i16(state.chargeRemaining) == 1)))))) {
    p.frame(3);
    state.chargeRemaining = i16(state.chargeRemaining) + -1
    ;
    p.motion.velocityX = 0.0f;
    p.motion.velocityY = 0.0f;
  }
  if (i16(state.chargeMode) == 2) {
    owned(0x344,double(p.motion.x),double(p.motion.y),1,0);
  }
  if (((i16(state.chargeMode) == 99) && (i16(p.animation.cursor.index) < 7)) &&
     (i16(p.angle) == 0)) {
    p.frame(7);
  }
  if ((i16(state.chargeMode) == 3) && (i16(p.animation.cursor.index) < 2)) {
    p.frame(2);
  }
  byteValue = i32(double(p.parameters[0])+state.effectCounters[2]-180);
  p.counter = byteValue;
  if (0xb4 < i16(p.counter)) {
    p.counter = i16(p.counter) + -0x168;
  }
  if (i16(p.counter) < -0xb4) {
    p.counter = i16(p.counter) + 0x168;
  }
  p.timer = i16(p.timer) + 5;
  if (((i16(state.chargeMode) != 6) &&
      ('\x01' < i8(owner.combat.spellPhase))) && (0x78 < i16(p.timer))
     ) {
    p.timer = 0x78;
  }
  if ((i8(owner.combat.spellPhase) == '\x01') && (0x8c < i16(p.timer))
     ) {
    p.timer = 0x8c;
  }
  if ((i16(state.chargeMode) == 6) && (0xb4 < i16(p.timer))) {
    p.timer = 0xb4;
  }
  if (i16(p.animation.cursor.index) == 1) {
    p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer));
    coordinate = i32(double(state.effectCounters[0])+p.motion.facing*double(p.motion.velocityX));
    p.motion.x = (float)coordinate;
    coordinate = i32(double(state.effectCounters[1])+p.motion.velocityY);
    p.motion.y = (float)coordinate;
    p.hits = 1;
    p.response = 0;
  }
  if (((i16(p.animation.cursor.index) == 2) && (i16(state.chargeMode) == 3)) &&
     (i16(p.angle) == 0)) {
    p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer));
    coordinate = i32(double(state.effectCounters[0])+p.motion.facing*double(p.motion.velocityX));
    p.motion.x = (float)coordinate;
    coordinate = i32(double(state.effectCounters[1])+p.motion.velocityY);
    p.motion.y = (float)coordinate;
    owned(0x344,double(p.motion.x),double(p.motion.y),1,0);
    if (((p.response != 0) && (p.response != 4)) &&
       ((p.response != 5 && (p.response != 6)))) {
      p.frame(7);
      return;
    }
    if (((p.response == 4) || (p.response == 5)) ||
       (p.response == 6)) {
      p.hits = 1;
      p.response = 0;
    }
  }
  if ((i16(p.animation.cursor.index) == 2) && (0 < i16(p.angle))) {
    p.polar((float)(int)i16(p.cycle),(float)(int)i16(p.angle) * double(0.5f) + double(5.0f));
    p.angle = i16(p.angle) + 1;
    p.variant = 1;
    p.motion.x =
         (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
         double(p.motion.x);
    p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
    owned(0x344,double(p.motion.x),double(p.motion.y),1,0);
    if ((p.response == 4) ||
       ((((p.response != 0 && (p.response != 4)) &&
         (p.response != 5)) && (p.response != 6)))) {
      p.frame(7);
      return;
    }
    if ((p.response == 5) || (p.response == 6)) {
      p.hits = 1;
      p.response = 0;
    }
  }
  if (i16(p.animation.cursor.index) == 3) {
    p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer));
    coordinate = i32(double(state.effectCounters[0])+p.motion.facing*double(p.motion.velocityX));
    p.motion.x = (float)coordinate;
    coordinate = i32(double(state.effectCounters[1])+p.motion.velocityY);
    p.motion.y = (float)coordinate;
    owner.launchParameters[0] = (float)(i16(p.animation.cursor.action) + -0x33c);
    if (i16(p.angle) == 0) {
      owned(0x349,(float)(i8(owner.motion.facing) * 0x14) +
                         double(p.motion.x),double(p.motion.y),u8(owner.motion.facing),1);
    }
    if (((0x78 < i16(p.angle)) || (p.response == 4)) ||
       ((p.response != 0 &&
        (((p.response != 4 && (p.response != 5)) &&
         (p.response != 6)))))) {
      p.frame(7);
      p.angle = 0;
      return;
    }
    p.angle = i16(p.angle) + 1;
    p.variant = 1;
  }
  if (i16(p.animation.cursor.index) == 4) {
    if (i16(p.angle) == 1) {
      owned(0x349,(float)(i8(owner.motion.facing) * 0x14) +
                         double(p.motion.x),double(p.motion.y),u8(owner.motion.facing),1);
    }
    if (((0x78 < i16(p.angle)) || (p.response == 4)) ||
       (((p.response != 0 &&
         ((p.response != 4 && (p.response != 5)))) &&
        (p.response != 6)))) {
      p.frame(7);
      return;
    }
    p.angle = i16(p.angle) + 1;
  }
  if (i16(p.animation.cursor.index) == 7) {
    setChannel(24,u8(i8(channel(24)) + -0x19));
  }
  if (i16(state.chargeMode) == 6) {
    owned(0x34b,double(p.motion.x),double(p.motion.y),1,1);
  }
  advancedFrame = p.advance();
  if (advancedFrame == i16(p.animation.cursor.frameCount)) {
    p.destroy();
    if (i16(p.angle) == 0) {
      state.chargeCount =
           i16(state.chargeCount) + -1;
    }
    if (i16(p.variant) == 0) {
      state.chargeRemaining =
           i16(state.chargeRemaining) + -1;
    }
  }
  else {
    if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
      p.frame(1);
    }
    if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
      p.frame(2);
    }
    if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
      p.frame(3);
    }
    if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
      p.frame(4);
    }
    if ((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) {
      p.frame(5);
    }
    if ((i16(p.animation.cursor.index) == 7) && (i16(p.animation.cursor.elapsed) == 0)) {
      p.frame(6);
    }
  }
  return;

}
void advance_marisa_object(MarisaObject& p,FighterActor& owner,FighterActor& other,MarisaState& state,ReplayStreams& random,FighterActionEvents& events){

    double aimRadians=0;
    const auto atan=[&](float y,float x){return std::atan2(double(y),double(x));};
    const auto channel=[&](u32 shift){return u8(p.pose.color>>shift);};
    const auto setChannel=[&](u32 shift,u8 value){p.pose.color=(p.pose.color&~(255u<<shift))|(u32(value)<<shift);};
    const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters);};
    const auto shared=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Shared,id,x,y,facing,layer,owner.launchParameters);};

  u8 byteValue;
  u16 wordValue;
  i16 advancedFrame;
  int roll;
  u32 direction;
  double angleRadians;
  float position;
  float auxiliaryValue;
  u32 layer;
  u32 direction1;
  u32 direction0;
  
  if ((i16(other.combat.opponentStop) == 0) &&
     ((i16(owner.combat.opponentStop) == 0 || (i16(p.contactDelay) != 0)))) {
    if (i16(p.freeze) == 0) {
      switch(p.animation.cursor.action) {
      case 800:
        if ((i16(owner.animation.cursor.action) == 400) &&
           (i16(p.counter) == 0)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 100) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
        }
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (0x13 < i16(p.animation.cursor.index))) {
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          if (p.response == 4) {
            p.response = 0;
            p.timer = i16(p.timer) + 1;
            if (1 < i16(p.timer)) {
              p.hits = i16(p.hits) + 1;
              p.timer = 0;
            }
          }
          if (i16(owner.animation.cursor.action) != 400) {
            p.counter = 1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.timer = 0;
            p.begin_pose(-20.0f,0.0f,0.0f);
            p.hits = 5;
            p.response = 0;
            p.pose.scale[0] = 2.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (p.response != 0) {
            if (i16(p.timer) < 2) {
              p.timer = i16(p.timer) + 1;
            }
            else {
              p.response = 0;
              p.timer = 0;
            }
          }
          if (0x13 < i16(p.animation.cursor.index)) {
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          if (0x13 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0x15) {
              setChannel(24,u8(0));
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x14));
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(0x14);
          }
          if (((i16(p.animation.cursor.index) == 0x14) &&
              (i16(owner.animation.cursor.action) == 400)) &&
             ((5 < i16(owner.animation.cursor.index) &&
              (i16(owner.animation.cursor.index) < 10)))) {
            p.frame(0xc);
          }
        }
        else {
          p.frame(0x14);
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          owned(0x3e2,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x28) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x50) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x78) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0xa0) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 200) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0xf0) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x118) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x140) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x168) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 400) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x1b8) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x1e0) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x208) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x230) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 600) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x280) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x2a8) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x2d0) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x2f8) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 800) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x348) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x370) + double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x321:
        if ((i16(owner.animation.cursor.action) == 0x192) &&
           (i16(p.counter) == 0)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 100) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(66.0f);
        }
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (0x13 < i16(p.animation.cursor.index))) {
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          if (p.response == 4) {
            p.response = 0;
            p.timer = i16(p.timer) + 1;
            if (1 < i16(p.timer)) {
              p.hits = i16(p.hits) + 1;
              p.timer = 0;
            }
          }
          if (i16(owner.animation.cursor.action) != 400) {
            p.counter = 1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.timer = 0;
            p.begin_pose(-20.0f,0.0f,0.0f);
            p.hits = 5;
            p.response = 0;
            p.pose.scale[0] = 2.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (p.response != 0) {
            if (i16(p.timer) < 2) {
              p.timer = i16(p.timer) + 1;
            }
            else {
              p.response = 0;
              p.timer = 0;
            }
          }
          if (0x13 < i16(p.animation.cursor.index)) {
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          if (0x13 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0x15) {
              setChannel(24,u8(0));
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x14));
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(0x14);
          }
          if (((i16(p.animation.cursor.index) == 0x14) &&
              (i16(owner.animation.cursor.action) == 400)) &&
             ((5 < i16(owner.animation.cursor.index) &&
              (i16(owner.animation.cursor.index) < 10)))) {
            p.frame(0xc);
          }
        }
        else {
          p.frame(0x14);
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          owned(0x3e2,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x28) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x50) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x78) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0xa0) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 200) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0xf0) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x118) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x140) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x168) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 400) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x1b8) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x1e0) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x208) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x230) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 600) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x280) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x2a8) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x2d0) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x2f8) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 800) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x348) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x370) + double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x322:
        if ((i16(owner.animation.cursor.action) == 0x194) &&
           (i16(p.counter) == 0)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x46) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(116.0f);
        }
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (0x13 < i16(p.animation.cursor.index))) {
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          if (p.response == 4) {
            p.response = 0;
            p.timer = i16(p.timer) + 1;
            if (1 < i16(p.timer)) {
              p.hits = i16(p.hits) + 1;
              p.timer = 0;
            }
          }
          if (i16(owner.animation.cursor.action) != 0x194) {
            p.counter = 1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.timer = 0;
            p.begin_pose(-20.0f,0.0f,0.0f);
            p.hits = 5;
            p.response = 0;
            p.pose.scale[0] = 2.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (p.response != 0) {
            if (i16(p.timer) < 2) {
              p.timer = i16(p.timer) + 1;
            }
            else {
              p.response = 0;
              p.timer = 0;
            }
          }
          if (0x13 < i16(p.animation.cursor.index)) {
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          if (0x13 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0x15) {
              setChannel(24,u8(0));
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x14));
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(0x14);
          }
          if (((i16(p.animation.cursor.index) == 0x14) &&
              (i16(owner.animation.cursor.action) == 0x194)) &&
             ((5 < i16(owner.animation.cursor.index) &&
              (i16(owner.animation.cursor.index) < 10)))) {
            p.frame(0xc);
          }
        }
        else {
          p.frame(0x14);
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          owned(0x3e2,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x28) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x50) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x78) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0xa0) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 200) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0xf0) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x118) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x140) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x168) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 400) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x1b8) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x1e0) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x208) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x230) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 600) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x280) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x2a8) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x2d0) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x2f8) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 800) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x348) + double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e2,(float)(i8(p.motion.facing) * 0x370) + double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x323:
        if (i16(owner.animation.cursor.action) == 0x131) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x26) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(3.0f);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 3.0f;
          p.pose.scale[1] = 3.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.8f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.8f);
        if ((1 < i16(p.animation.cursor.index)) && (10 < u8(channel(24)))) {
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x324:
        if (i16(owner.animation.cursor.action) == 0x131) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x26) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(3.0f);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 2.0f;
          p.pose.scale[1] = 2.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 60.0f;
          p.pose.rotation[1] = (float)(i8(p.motion.facing) * 0x3c);
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
        if (0x14 < u8(channel(24))) {
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x325:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 4.0f;
          p.pose.scale[1] = 4.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 75.0f;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.3f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
        if (0x14 < u8(channel(24))) {
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x326:
        if (((i16(owner.animation.cursor.action) == 700) &&
            (i16(p.animation.cursor.index) < 6)) ||
           (i16(owner.animation.cursor.action) == 0x2bd)) {
          p.motion.x =
               (float)((int)i8(p.motion.facing) << 5) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(90.0f);
        }
        if (((i16(owner.animation.cursor.action) == 0x2bd) ||
            ((i16(owner.animation.cursor.action) == 700 &&
             (i16(owner.animation.cursor.index) < 6)))) ||
           (9 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.hits = 1;
            p.response = 0;
            if (i16(p.counter) != 1) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.pose.scale[0] = 1.5f;
              p.pose.scale[1] = 2.0f;
              p.counter = 1;
              p.timer = 100;
              p.angle = 0x514;
              setChannel(24,u8(0xff));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
            }
          }
          if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.hits = 1;
            p.response = 0;
          }
          if (i16(p.animation.cursor.index) < 10) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.005f);
            wordValue = i32(double(p.timer)*double(0.98f));
            p.timer = wordValue;
            p.angle = i16(p.angle) + -1;
            p.pose.scale[1] =
                 (float)(int)i16(p.timer) * 0.01 +
                 (float)(int)i16(p.angle) * double(0.001f);
          }
          if (9 < i16(p.animation.cursor.index)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.15f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
            setChannel(24,u8(i8(channel(24)) + -5));
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(10);
          }
          else if ((i16(p.animation.cursor.index) == 10) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(0);
          }
        }
        else {
          p.frame(i16(p.animation.cursor.index) + 10);
          p.hits = 1;
          p.response = 0;
        }
        break;
      case 0x327:
        if (i16(owner.animation.cursor.action) == 0x18a) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x97) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(104.0f);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 3.0f;
          p.pose.scale[1] = 3.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.8f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.8f);
        if ((1 < i16(p.animation.cursor.index)) && (10 < u8(channel(24)))) {
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x328:
        if (i16(owner.animation.cursor.action) == 0x18b) {
          p.motion.x =
               (float)((int)i8(p.motion.facing) << 7) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(70.0f);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 3.0f;
          p.pose.scale[1] = 3.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.8f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.8f);
        if ((1 < i16(p.animation.cursor.index)) && (10 < u8(channel(24)))) {
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x329:
        if ((((i16(owner.animation.cursor.action) == 700) &&
             (i16(p.animation.cursor.index) < 6)) ||
            (i16(owner.animation.cursor.action) == 0x2bd)) ||
           (((i16(owner.animation.cursor.action) == 0x2ef &&
             (i16(owner.animation.cursor.index) < 0xc)) ||
            ((i16(owner.animation.cursor.action) == 0x2f0 &&
             (i16(owner.animation.cursor.index) < 0xc)))))) {
          p.motion.x =
               (float)((int)i8(p.motion.facing) << 6) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(90.0f);
        }
        if ((((i16(owner.animation.cursor.action) != 0x2bd) &&
             ((i16(owner.animation.cursor.action) != 700 ||
              (5 < i16(owner.animation.cursor.index))))) &&
            ((i16(owner.animation.cursor.action) != 0x2ef ||
             (0xb < i16(owner.animation.cursor.index))))) &&
           (((i16(owner.animation.cursor.action) != 0x2f0 ||
             (0xb < i16(owner.animation.cursor.index))) &&
            (i16(p.animation.cursor.index) == 0)))) {
          p.frame(1);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 2.0f;
          p.pose.scale[1] = 2.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.8f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.8f);
        if ((0 < i16(p.animation.cursor.index)) && (10 < u8(channel(24)))) {
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.frame(0);
        }
        break;
      case 0x32a:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[1] = (float)(i8(p.motion.facing) * 0x46);
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.75f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.75f);
        if (0x14 < u8(channel(24))) {
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x32b:
        if (p.response == 5) {
          p.destroy();
        }
        else if ((i16(owner.animation.cursor.action) < 0x32) ||
                (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) &&
             ((i16(p.animation.cursor.elapsed) == 0 && (i16(p.counter) != 1)))) {
            p.counter = 1;
            p.timer = 200;
            wordValue = i32(p.parameters[0]);
            p.angle = wordValue;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer) * double(0.01f));
            p.pose.scale[0] = 2.0f;
            p.pose.scale[1] = 2.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
          }
          if (p.response == 5) {
            p.destroy();
          }
          else {
            random.range(0,100);
            if ((((p.response == 1) || (p.response == 2)) ||
                (p.response == 4)) ||
               (position = double(p.motion.y) + double(p.motion.velocityY),
               position < double(0.0f) != (position == double(0.0f)))) {
              p.action(0x32d);
              owned(0x32f,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              p.pose.rotation[0] = 0.0f;
              p.pose.rotation[1] = 0.0f;
              p.pose.rotation[2] = 0.0f;
              p.pose.scale[0] = 0.5f;
              p.pose.scale[1] = 0.5f;
            }
            else {
              p.timer = i16(p.timer) + 0x32;
              p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer) * double(0.01f));
              p.motion.x =
                   (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                   double(p.motion.x);
              p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
              if (((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f)))
                 || (double(1000.0f) < double(p.motion.y))) {
                p.destroy();
              }
              else {
                advancedFrame = p.advance();
                if (advancedFrame == i16(p.animation.cursor.frameCount)) {
                  p.frame(0x14);
                }
              }
            }
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x32c:
        if (p.response == 5) {
          p.destroy();
        }
        else if ((i16(owner.animation.cursor.action) < 0x46) ||
                (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) &&
             ((i16(p.animation.cursor.elapsed) == 0 && (i16(p.counter) != 1)))) {
            p.counter = 1;
            p.timer = 100;
            wordValue = i32(p.parameters[0]);
            p.angle = wordValue;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer) * double(0.01f));
            p.pose.scale[0] = 2.0f;
            p.pose.scale[1] = 2.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(2.0f);
          if (p.response == 5) {
            p.destroy();
          }
          else {
            random.range(0,100);
            if ((((p.response == 1) || (p.response == 2)) ||
                (p.response == 4)) ||
               (position = double(p.motion.y) + double(p.motion.velocityY),
               position < double(0.0f) != (position == double(0.0f)))) {
              p.action(0x32e);
              owned(0x34d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x34d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x34d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              p.pose.rotation[0] = 0.0f;
              p.pose.rotation[1] = 0.0f;
              p.pose.rotation[2] = 0.0f;
              p.pose.scale[0] = 0.5f;
              p.pose.scale[1] = 0.5f;
            }
            else {
              p.timer = i16(p.timer) + 10;
              p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer) * double(0.01f));
              p.motion.x =
                   (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                   double(p.motion.x);
              p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
              if (((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f)))
                 || (double(1000.0f) < double(p.motion.y))) {
                p.destroy();
              }
              else {
                advancedFrame = p.advance();
                if (advancedFrame == i16(p.animation.cursor.frameCount)) {
                  p.frame(1);
                }
              }
            }
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x32d:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          static_cast<void>(7);
          p.begin_pose(0.0f,0.0f,0.0f);
          p.counter = 0;
          p.hits = 1;
          p.response = 0;
          p.pose.scale[0] = 0.25f;
          p.pose.scale[1] = 0.25f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
        }
        p.counter = i16(p.counter) + 1;
        p.pose.scale[0] =
             (float)(i16(p.counter) < 5) *
             (double(0.3f) - (float)(int)i16(p.counter) * 0.07) + double(0.01f) +
             double(p.pose.scale[0]);
        p.pose.scale[1] =
             (float)(i16(p.counter) < 5) *
             (double(0.3f) - (float)(int)i16(p.counter) * 0.07) + double(0.01f) +
             double(p.pose.scale[1]);
        if (u8(channel(24)) < 0xb) {
          setChannel(24,u8(0));
          p.destroy();
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -10));
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x32e:
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) == 1)) {
          p.counter = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX));
          wordValue = i32(aimRadians/double(0.017453289f));
          p.counter = wordValue;
          auxiliaryValue = 2.0f;
          advancedFrame = i16(p.counter);
          roll = roll = random.range(0,0x28);
          p.polar((float)(advancedFrame + -0x14 + roll),auxiliaryValue);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
          static_cast<void>(8);
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
        if (1 < i16(p.animation.cursor.index)) {
          setChannel(24,u8(i8(channel(24)) + -0x10));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x32f:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.counter = 0;
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
        }
        p.counter = i16(p.counter) + 1;
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
        p.pose.scale[0] =
             (float)(i16(p.counter) < 5) *
             (double(0.3f) - (float)(int)i16(p.counter) * 0.07) + double(0.01f) +
             double(p.pose.scale[0]);
        p.pose.scale[1] =
             (float)(i16(p.counter) < 5) *
             (double(0.3f) - (float)(int)i16(p.counter) * 0.07) + double(0.01f) +
             double(p.pose.scale[1]);
        if (0 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0xb) {
            setChannel(24,u8(0));
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x330:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          roll = random.range(0x19,0x4b);
          p.pose.scale[0] = (float)roll * double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          byteValue = random.range(0,0xff);
          setChannel(16,u8(byteValue));
          byteValue = random.range(0,0xff);
          setChannel(8,u8(byteValue));
          byteValue = random.range(0,0xff);
          setChannel(0,u8(byteValue));
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
        }
        p.motion.x = double(p.motion.x) - double(0.1f);
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
        setChannel(24,u8(i8(channel(24)) + -7));
        p.pose.scale[0] = double(p.pose.scale[0]) - double(0.01f);
        p.pose.scale[1] = double(p.pose.scale[1]) - double(0.01f);
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x331:
        p.motion.facing = u8(owner.motion.facing);
        if (((0x95 < i16(owner.animation.cursor.action)) &&
            (i16(owner.animation.cursor.action) < 0x9a)) &&
           (i16(p.animation.cursor.index) == 0)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x39) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(94.0f);
        }
        if (((0x99 < i16(owner.animation.cursor.action)) &&
            (i16(owner.animation.cursor.action) < 0x9e)) &&
           (i16(p.animation.cursor.index) == 0)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x39) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(60.0f);
        }
        if ((i16(owner.animation.cursor.action) == 0x9e) &&
           (i16(p.animation.cursor.index) == 0)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x39) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
        }
        if (((i16(owner.animation.cursor.action) < 0x96) ||
            (0x9e < i16(owner.animation.cursor.action))) &&
           (i16(p.animation.cursor.index) == 0)) {
          p.frame(1);
          state.frame.blockEffectSent = 0;
        }
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[1] = 50.0f;
          p.counter = 1;
        }
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(2.0f);
        if (0 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0x1f) {
            setChannel(24,u8(0));
          }
          else {
            setChannel(24,u8(i8(channel(24)) + -0x1e));
          }
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.frame(0);
        }
        break;
      case 0x332:
        if (p.response == 5) {
          p.destroy();
        }
        else if ((i16(owner.animation.cursor.action) < 0x32) ||
                (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.motion.velocityX = double(p.parameters[0]) * double(0.1f);
            p.motion.velocityY = double(p.parameters[1]) * double(0.1f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          p.motion.velocityY = double(p.motion.velocityY) - double(0.6f);
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          roll = random.range(0,100);
          if (roll < 0x1e) {
            auxiliaryValue = 1;
            direction = (u32)u8(p.motion.facing);
            roll = random.range(0xffffffec,0x14);
            position = (float)roll + double(p.motion.y);
            roll = random.range(0xffffffec,0x14);
            owned(0x330,(float)roll + double(p.motion.x),position,direction,auxiliaryValue);
          }
          if ((double(p.motion.y) < double(0.0f)) || (p.response == 4)) {
            p.action(0x356);
            owned(0x32f,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            owned(0x335,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            p.pose.rotation[0] = 0.0f;
            p.pose.rotation[1] = 0.0f;
            p.pose.rotation[2] = 0.0f;
            p.pose.scale[0] = 0.5f;
            p.pose.scale[1] = 0.5f;
          }
          else {
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.frame(0);
            }
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x333:
        if (p.response == 5) {
          p.destroy();
        }
        else if ((i16(owner.animation.cursor.action) < 0x32) ||
                (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.polar(-60.0f,15.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          p.motion.velocityY = double(p.motion.velocityY) - double(0.75f);
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          roll = random.range(0,100);
          if (roll < 0x1e) {
            auxiliaryValue = 1;
            direction = (u32)u8(p.motion.facing);
            roll = random.range(0xffffffec,0x14);
            position = (float)roll + double(p.motion.y);
            roll = random.range(0xffffffec,0x14);
            owned(0x330,(float)roll + double(p.motion.x),position,direction,auxiliaryValue);
          }
          if (double(0.0f) <= double(p.motion.y)) {
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.frame(0);
            }
          }
          else {
            p.action(0x336);
            owned(0x32f,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            owned(0x335,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            p.pose.rotation[0] = 0.0f;
            p.pose.rotation[1] = 0.0f;
            p.pose.rotation[2] = 0.0f;
            p.pose.scale[0] = 0.5f;
            p.pose.scale[1] = 0.5f;
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x334:
        if (((i16(owner.animation.cursor.action) == 700) &&
            (i16(owner.animation.cursor.index) < 6)) ||
           (i16(owner.animation.cursor.action) == 0x2bd)) {
          p.motion.x =
               (float)((int)i8(p.motion.facing) << 5) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(90.0f);
        }
        if ((i16(owner.animation.cursor.action) == 0x2bd) ||
           (((i16(owner.animation.cursor.action) == 700 &&
             (i16(owner.animation.cursor.index) < 6)) ||
            (0x18 < i16(p.animation.cursor.index))))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.hits = 1;
            p.response = 0;
            if (i16(p.counter) != 1) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.pose.scale[0] = 4.0f;
              p.pose.scale[1] = 2.0f;
              p.counter = 1;
              p.timer = 100;
              p.angle = 2000;
              setChannel(24,u8(0xff));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
            }
          }
          if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.hits = 1;
            p.response = 0;
          }
          if (i16(p.animation.cursor.index) < 0x19) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.005f);
            wordValue = i32(double(p.timer)*double(0.98f));
            p.timer = wordValue;
            p.angle = i16(p.angle) + -1;
            p.pose.scale[1] =
                 (float)(int)i16(p.timer) * 0.01 +
                 (float)(int)i16(p.angle) * double(0.001f);
          }
          if (9 < i16(p.animation.cursor.index)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.15f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
            setChannel(24,u8(i8(channel(24)) + -5));
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(0x19);
          }
          else if ((i16(p.animation.cursor.index) == 10) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(0);
          }
        }
        else {
          p.frame(i16(p.animation.cursor.index) + 0x19);
          p.hits = 1;
          p.response = 0;
        }
        break;
      case 0x335:
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x336:
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) == 1)) {
          p.counter = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.motion.velocityY = 0.0f;
          p.motion.velocityX = 0.0f;
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
          static_cast<void>(9);
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.07f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.07f);
        setChannel(24,u8(i8(channel(24)) + -10));
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x337:
        if ((i16(owner.animation.cursor.action) == 400) &&
           (i16(p.counter) == 0)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x78) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
        }
        if (p.response == 6) {
          p.response = 0;
          p.hits = 1;
        }
        if (i16(owner.animation.cursor.action) != 400) {
          p.counter = 1;
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.begin_pose(-20.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.pose.scale[0] = 1.7000000476837158f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.hits = 1;
          p.response = 0;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.01f);
        if (i16(p.animation.cursor.index) == 4) {
          p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
        }
        if ((3 < i16(p.animation.cursor.index)) && (3 < u8(channel(24)))) {
          setChannel(24,u8(i8(channel(24)) + -3));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if (((i16(p.animation.cursor.index) == 4) &&
            (i16(owner.animation.cursor.action) == 400)) &&
           (i16(owner.animation.cursor.index) == 6)) {
          p.frame(3);
        }
        break;
      case 0x338:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.begin_pose(-20.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = -30.0f;
          p.motion.velocityY = 10.0f;
          p.motion.accelerationY = 0.009999999776482582f;
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        if ((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(200.0f))) {
          p.destroy();
        }
        p.motion.velocityY = double(p.motion.velocityY) - double(p.motion.accelerationY);
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.frame(0xc);
        }
        break;
      case 0x339:
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) == 0)) {
          p.counter = 1;
          p.begin_pose(-20.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 80.0f;
          p.motion.velocityY = 0.0f;
        }
        owned(0x34c,double(p.motion.x) - (float)(i8(p.motion.facing) * 0x4b),double(p.motion.y) + double(220.0f),-(int)i8(p.motion.facing),1);
        owned(0x34c,double(p.motion.x) - (float)(i8(p.motion.facing) * 0x4b),double(p.motion.y) + double(220.0f),-(int)i8(p.motion.facing),1);
        shared(0x82,double(p.motion.x),0.0f,u8(p.motion.facing),1);
        if ((('\0' < i8(p.motion.facing)) &&
            (double(owner.motion.x) < double(p.motion.x))) &&
           ((i16(owner.animation.cursor.index) == 4 &&
            (i16(owner.animation.cursor.action) == 0x2c0)))) {
          owner.motion.velocityX = 80.0f;
        }
        if (((i8(p.motion.facing) < '\0') &&
            (double(p.motion.x) < double(owner.motion.x))) &&
           ((i16(owner.animation.cursor.index) == 4 &&
            (i16(owner.animation.cursor.action) == 0x2c0)))) {
          owner.motion.velocityX = 80.0f;
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        if (((double(2000.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(720.0f))) ||
           (4 < i16(owner.animation.cursor.index))) {
          p.destroy();
          state.actionStage = 1;
        }
        else {
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(0);
            p.response = 0;
          }
        }
        break;
      case 0x33a:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.begin_pose(-20.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 80.0f;
          p.motion.velocityY = 0.0f;
        }
        owned(0x348,double(p.motion.x) - (float)(i8(p.motion.facing) * 0x1e1),250.0f,1,1);
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        if ((double(6000.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(3720.0f))) {
          p.destroy();
        }
        else {
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(0);
          }
        }
        break;
      case 0x33b:
        if (i16(owner.animation.cursor.action) == 0x2bf) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
          if (0x16 < i16(owner.animation.cursor.index)) {
            p.destroy();
            return;
          }
        }
        if (i16(owner.animation.cursor.action) == 0x2bf) {
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(0);
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x33c:
        advance_marisa_orbit(p,owner,other,state,random,events);
        break;
      case 0x33d:
        advance_marisa_orbit(p,owner,other,state,random,events);
        break;
      case 0x33e:
        advance_marisa_orbit(p,owner,other,state,random,events);
        break;
      case 0x33f:
        advance_marisa_orbit(p,owner,other,state,random,events);
        break;
      case 0x340:
        advance_marisa_orbit(p,owner,other,state,random,events);
        break;
      case 0x341:
        advance_marisa_orbit(p,owner,other,state,random,events);
        break;
      case 0x342:
        if ((i16(p.animation.cursor.index) < 5) || (i16(p.animation.cursor.index) == 6)) {
          p.motion.x = double(owner.motion.x);
        }
        if ((i16(p.animation.cursor.index) < 5) || (i16(p.animation.cursor.index) == 6)) {
          p.motion.y = double(owner.motion.y) + double(100.0f);
        }
        wordValue = i32(p.motion.x);
        state.effectCounters[0] = wordValue;
        wordValue = i32(p.motion.y);
        state.effectCounters[1] = wordValue;
        state.effectCounters[2] =
             i16(state.effectCounters[2]) + 3;
        if (i16(state.chargeMode) == 2) {
          state.effectCounters[2] =
               i16(state.effectCounters[2]) + 7;
        }
        if (i16(state.chargeMode) == 3) {
          state.effectCounters[2] =
               i16(state.effectCounters[2]) + 3;
        }
        if (i16(state.effectCounters[2]) < 0) {
          state.effectCounters[2] = 0;
        }
        if (0x167 < i16(state.effectCounters[2])) {
          state.effectCounters[2] = 0;
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          owner.launchParameters[0] = 0.0f;
          owned(0x33c,double(p.motion.x),double(p.motion.y),1,1);
        }
        if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
          owner.launchParameters[0] = 90.0f;
          owned(0x33d,double(p.motion.x),double(p.motion.y),1,1);
        }
        if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
          owner.launchParameters[0] = 180.0f;
          owned(0x33e,double(p.motion.x),double(p.motion.y),1,1);
        }
        if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
          owner.launchParameters[0] = 270.0f;
          owned(0x33f,double(p.motion.x),double(p.motion.y),1,1);
        }
        if (i16(p.animation.cursor.index) == 4) {
          state.chargeMode = 1;
        }
        if ((i16(state.chargeCount) == 0) && (i16(p.animation.cursor.index) < 7))
        {
          state.chargeMode = 99;
          p.frame(7);
        }
        else {
          if ((i16(p.animation.cursor.index) == 4) &&
             ((i16(owner.animation.cursor.action) == 0x2c3 &&
              (i16(owner.animation.cursor.index) == 5)))) {
            p.frame(5);
            p.hits = 0xf;
            state.chargeMode = 2;
            p.motion.facing = u8(owner.motion.facing);
          }
          if (((i16(p.animation.cursor.index) == 4) &&
              (i16(owner.animation.cursor.action) == 0x2c4)) &&
             (i16(owner.animation.cursor.index) == 5)) {
            p.frame(6);
            state.chargeMode = 3;
            p.motion.facing = u8(owner.motion.facing);
          }
          if ((i16(owner.combat.spellTimer) < 1) && (i16(p.animation.cursor.index) < 7)
             ) {
            state.chargeMode = 99;
            p.frame(7);
          }
          else {
            if (i16(p.animation.cursor.index) == 5) {
              state.chargeMode = 2;
              if ((double(1580.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(300.0f))) {
                state.chargeMode = 99;
                p.frame(7);
                return;
              }
              if (((p.response == 0) || (p.response == 6)) ||
                 (p.response == 4)) {
                p.motion.x =
                     (float)(int)i8(p.motion.facing) * double(10.0f) + double(p.motion.x);
              }
              else {
                p.motion.x =
                     (float)(int)i8(p.motion.facing) * double(5.0f) + double(p.motion.x);
              }
            }
            if (i16(p.animation.cursor.index) == 6) {
              state.chargeMode = 3;
              state.spellCounters[0] =
                   i16(state.spellCounters[0]) + -1;
              if ((i16(state.chargeCount) == 0) ||
                 (i16(state.spellCounters[0]) < 1)) {
                state.chargeMode = 99;
                p.frame(7);
                return;
              }
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
              state.chargeMode = 0;
              state.chargeCount = 0;
            }
            else {
              if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.frame(4);
              }
              if ((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.frame(5);
                p.response = 0;
              }
              if ((i16(p.animation.cursor.index) == 7) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.frame(6);
              }
            }
          }
        }
        break;
      case 0x343:
        if ((i16(p.animation.cursor.index) < 7) || (i16(p.animation.cursor.index) == 8)) {
          p.motion.x = double(owner.motion.x);
        }
        if ((i16(p.animation.cursor.index) < 7) || (i16(p.animation.cursor.index) == 8)) {
          p.motion.y = double(owner.motion.y) + double(100.0f);
        }
        wordValue = i32(p.motion.x);
        state.effectCounters[0] = wordValue;
        wordValue = i32(p.motion.y);
        state.effectCounters[1] = wordValue;
        state.effectCounters[2] =
             i16(state.effectCounters[2]) + 3;
        if (i16(state.chargeMode) == 2) {
          state.effectCounters[2] =
               i16(state.effectCounters[2]) + 7;
        }
        if (i16(state.chargeMode) == 3) {
          state.effectCounters[2] =
               i16(state.effectCounters[2]) + 3;
        }
        if (i16(state.chargeMode) == 6) {
          state.effectCounters[2] =
               i16(state.effectCounters[2]) + 7;
        }
        if (i16(state.effectCounters[2]) < 0) {
          state.effectCounters[2] = 0;
        }
        if (0x167 < i16(state.effectCounters[2])) {
          state.effectCounters[2] = 0;
        }
        if (i16(state.spellCounters[0]) < 0) {
          state.spellCounters[0] = 0;
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.cycle = 0;
          owner.launchParameters[0] = 0.0f;
          owned(0x33c,double(p.motion.x),double(p.motion.y),1,1);
        }
        if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
          owner.launchParameters[0] = 60.0f;
          owned(0x33d,double(p.motion.x),double(p.motion.y),1,1);
        }
        if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
          owner.launchParameters[0] = 120.0f;
          owned(0x33e,double(p.motion.x),double(p.motion.y),1,1);
        }
        if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
          owner.launchParameters[0] = 180.0f;
          owned(0x33f,double(p.motion.x),double(p.motion.y),1,1);
        }
        if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
          owner.launchParameters[0] = 240.0f;
          owned(0x340,double(p.motion.x),double(p.motion.y),1,1);
        }
        if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
          owner.launchParameters[0] = 300.0f;
          owned(0x341,double(p.motion.x),double(p.motion.y),1,1);
        }
        if (i16(p.animation.cursor.index) == 6) {
          state.chargeMode = 1;
        }
        if ((i16(state.chargeCount) == 0) && (i16(p.animation.cursor.index) < 9))
        {
          state.chargeMode = 99;
          p.frame(9);
        }
        else {
          if ((i16(p.animation.cursor.index) == 6) &&
             ((i16(owner.animation.cursor.action) == 0x2ca &&
              (i16(owner.animation.cursor.index) == 5)))) {
            p.frame(7);
            p.hits = 0x14;
            state.chargeMode = 6;
            p.motion.facing = u8(owner.motion.facing);
            aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                         double(p.motion.y),(double(other.motion.x) -
                         double(p.motion.x)) * (float)(int)i8(p.motion.facing));
            wordValue = i32(aimRadians/double(0.017453289f));
            p.angle = wordValue;
            p.timer = 0x50;
          }
          if (((i16(p.animation.cursor.index) == 6) &&
              (i16(owner.animation.cursor.action) == 0x2cb)) &&
             (i16(owner.animation.cursor.index) == 5)) {
            p.frame(8);
            state.chargeMode = 3;
            p.motion.facing = u8(owner.motion.facing);
          }
          if ((i16(owner.combat.spellTimer) < 1) && (i16(p.animation.cursor.index) < 9)
             ) {
            state.chargeMode = 99;
            p.frame(9);
          }
          else {
            if (i16(p.animation.cursor.index) == 7) {
              state.chargeMode = 6;
              if ((((((double(1580.0f) < double(p.motion.x)) &&
                     (double(0.0f) < (float)(int)i8(p.motion.facing) * double(p.motion.velocityX))) ||
                    ((double(p.motion.x) < -double(300.0f) &&
                     ((float)(int)i8(p.motion.facing) * double(p.motion.velocityX) < double(0.0f)))))
                   || ((double(p.motion.y) < -double(300.0f) && (double(0.0f) < double(p.motion.velocityY)))))
                  || ((double(1280.0f) < double(p.motion.y) && (double(p.motion.velocityY) < double(0.0f)))))
                 && (0 < i16(p.cycle))) {
                state.chargeMode = 99;
                p.frame(9);
                return;
              }
              if ((p.response == 6) && (p.response != 4)) {
                p.response = 0;
              }
              if (i16(p.timer) == 0) {
                aimRadians = atan((double(other.motion.y) + double(100.0f))
                             - double(p.motion.y),(double(other.motion.x) -
                             double(p.motion.x)) * (float)(int)i8(p.motion.facing));
                wordValue = i32(aimRadians/double(0.017453289f));
                p.angle = wordValue;
                p.cycle = 1;
              }
              p.timer = i16(p.timer) + -1;
              if (i16(p.cycle) == 1) {
                p.timer = i16(p.timer) + 2;
              }
              p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer) * double(0.2f));
              p.motion.x =
                   (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                   double(p.motion.x);
              p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            }
            if (i16(p.animation.cursor.index) == 8) {
              state.chargeMode = 3;
              state.spellCounters[0] =
                   i16(state.spellCounters[0]) + -1;
              if ((i16(state.chargeCount) == 0) ||
                 (i16(state.spellCounters[0]) < 1)) {
                state.chargeMode = 99;
                p.frame(9);
                return;
              }
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
              state.chargeMode = 0;
              state.chargeCount = 0;
            }
            else {
              if ((i16(p.animation.cursor.index) == 7) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.frame(6);
              }
              if ((i16(p.animation.cursor.index) == 8) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.frame(7);
                p.response = 0;
              }
              if ((i16(p.animation.cursor.index) == 9) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.frame(8);
              }
            }
          }
        }
        break;
      case 0x344:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.05f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.05f);
        if (u8(channel(24)) < 0x15) {
          setChannel(24,u8(0));
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -0x14));
          setChannel(16,u8(i8(channel(16)) + -0x14));
          setChannel(8,u8(i8(channel(8)) + -0x14));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x345:
        if (((i16(owner.animation.cursor.action) == 0x2c8) &&
            (i16(owner.animation.cursor.index) < 6)) ||
           (i16(owner.animation.cursor.action) == 0x2c9)) {
          p.motion.x =
               (float)((int)i8(p.motion.facing) << 5) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(90.0f);
        }
        if (((i16(owner.animation.cursor.action) == 0x2c9) ||
            ((i16(owner.animation.cursor.action) == 0x2c8 &&
             (i16(owner.animation.cursor.index) < 6)))) ||
           (0x18 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            state.laser.visible = 1;
            state.laser.x = double(p.motion.x);
            state.laser.y = double(p.motion.y);
            state.laser.facing = u8(p.motion.facing);
            p.hits = 1;
            p.response = 0;
            if (i16(p.counter) != 1) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.pose.scale[0] = 4.0f;
              p.pose.scale[1] = 4.0f;
              p.counter = 1;
              p.timer = 100;
              p.angle = 0xdac;
              setChannel(24,u8(0xff));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
            }
          }
          if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.hits = 1;
            p.response = 0;
          }
          if (i16(p.animation.cursor.index) < 0x19) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.005f);
            wordValue = i32(double(p.timer)*double(0.98f));
            p.timer = wordValue;
            p.angle = i16(p.angle) + -1;
            p.pose.scale[1] =
                 (float)(int)i16(p.timer) * 0.005 +
                 (float)(int)i16(p.angle) * double(0.001f);
            state.laser.width = double(p.pose.scale[1]);
          }
          if (9 < i16(p.animation.cursor.index)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.15f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
            setChannel(24,u8(i8(channel(24)) + -5));
            state.laser.width = double(p.pose.scale[1]);
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            state.laser.visible = 0;
            p.frame(0x19);
          }
          else if ((i16(p.animation.cursor.index) == 10) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(0);
          }
        }
        else {
          p.frame(i16(p.animation.cursor.index) + 0x19);
          p.hits = 1;
          p.response = 0;
        }
        break;
      case 0x346:
        if (((i16(owner.animation.cursor.action) == 0x2c8) &&
            (i16(p.animation.cursor.index) < 6)) ||
           (i16(owner.animation.cursor.action) == 0x2c9)) {
          p.motion.x =
               (float)((int)i8(p.motion.facing) << 6) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(90.0f);
        }
        if (((i16(owner.animation.cursor.action) != 0x2c8) &&
            ((i16(owner.animation.cursor.action) != 0x2c9 ||
             (5 < i16(owner.animation.cursor.index))))) &&
           (i16(p.animation.cursor.index) == 0)) {
          p.frame(1);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 2.0f;
          p.pose.scale[1] = 2.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.8f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.8f);
        if ((0 < i16(p.animation.cursor.index)) && (10 < u8(channel(24)))) {
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.frame(0);
        }
        break;
      case 0x347:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[1] = (float)(i8(p.motion.facing) * 0x46);
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.75f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.75f);
        if (0x14 < u8(channel(24))) {
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x348:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if (u8(channel(24)) < 0x10) {
          setChannel(24,u8(0));
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -0xf));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x349:
        if (double(p.parameters[0]) == double(0.0f)) {
          p.motion.x = (float)(int)i16(state.spellCounters[1]);
          p.motion.y = (float)(int)i16(state.spellCounters[7]);
        }
        if (double(p.parameters[0]) == double(1.0f)) {
          p.motion.x = (float)(int)i16(state.spellCounters[2]);
          p.motion.y = (float)(int)i16(state.spellCounters[8]);
        }
        if (double(p.parameters[0]) == double(2.0f)) {
          p.motion.x = (float)(int)i16(state.spellCounters[3]);
          p.motion.y = (float)(int)i16(state.spellCounters[9]);
        }
        if (double(p.parameters[0]) == double(3.0f)) {
          p.motion.x = (float)(int)i16(state.spellCounters[4]);
          p.motion.y = (float)(int)i16(state.spellCounters[10]);
        }
        if (double(p.parameters[0]) == double(4.0f)) {
          p.motion.x = (float)(int)i16(state.spellCounters[5]);
          p.motion.y = (float)(int)i16(state.spellCounters[11]);
        }
        if (double(p.parameters[0]) == double(5.0f)) {
          p.motion.x = (float)(int)i16(state.spellCounters[6]);
          p.motion.y = (float)(int)i16(state.spellCounters[12]);
        }
        if (p.response == 6) {
          p.response = 0;
          p.hits = i16(p.hits) + 1;
        }
        if (p.response == 4) {
          p.response = 0;
          p.timer = i16(p.timer) + 1;
          if (1 < i16(p.timer)) {
            p.hits = i16(p.hits) + 1;
            p.timer = 0;
          }
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.timer = 0;
          p.begin_pose(-20.0f,0.0f,0.0f);
          p.hits = 0x14;
          p.response = 0;
          p.pose.scale[0] = 2.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
        }
        if (p.response != 0) {
          if (i16(p.timer) < 0xf) {
            p.timer = i16(p.timer) + 1;
          }
          else {
            p.response = 0;
            p.timer = 0;
          }
        }
        if (0x13 < i16(p.animation.cursor.index)) {
          p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
        }
        if (0x13 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0x15) {
            setChannel(24,u8(0));
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.frame(0x14);
        }
        if ((i16(p.animation.cursor.index) == 0x14) && (i16(p.counter) < 10)) {
          p.frame(0xc);
          p.counter = i16(p.counter) + 1;
        }
        break;
      default:
        p.advance();
        break;
      case 0x34b:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 2.5f;
          p.pose.scale[1] = 2.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) - double(0.25f);
        p.pose.scale[1] = double(p.pose.scale[1]) - double(0.25f);
        if (u8(channel(24)) < 0x1a) {
          setChannel(24,u8(0));
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -0x19));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x34c:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          roll = random.range(0,10);
          p.pose.scale[0] = (float)roll * double(0.1f) + double(0.5f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          advancedFrame = random.range(0,0x3c);
          p.counter = advancedFrame + -0x1e;
          wordValue = random.range(0xf,0x1e);
          p.timer = wordValue;
        }
        p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer));
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        if (0 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0xd) {
            setChannel(24,u8(0));
          }
          else {
            setChannel(24,u8(i8(channel(24)) + -0xc));
            setChannel(16,u8(i8(channel(16)) + -0xc));
            setChannel(8,u8(i8(channel(8)) + -0xc));
          }
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x34d:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.counter = 0;
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          roll = random.range(0,0x168);
          p.pose.rotation[2] = (float)roll;
        }
        p.counter = i16(p.counter) + 1;
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
        p.pose.scale[0] =
             (float)(i16(p.counter) < 0xd) *
             (double(0.4f) - (float)(int)i16(p.counter) * 0.03) + double(0.01f) +
             double(p.pose.scale[0]);
        p.pose.scale[1] =
             (float)(i16(p.counter) < 0xd) *
             (double(0.4f) - (float)(int)i16(p.counter) * 0.03) + double(0.01f) +
             double(p.pose.scale[1]);
        if (0 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0xb) {
            setChannel(24,u8(0));
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x34e:
        if (((i16(owner.animation.cursor.action) == 0x2bf) &&
            (0xe < i16(owner.animation.cursor.index))) &&
           (i16(owner.animation.cursor.index) < 0x17)) {
          p.motion.x =
               double(owner.motion.x) -
               (float)(i8(p.motion.facing) * 0x6e);
          p.motion.y = double(owner.motion.y) + double(95.0f);
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 2.5f;
            p.pose.scale[1] = 1.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(0);
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x34f:
        if ((i16(owner.animation.cursor.action) == 0x2bd) ||
           (i16(owner.animation.cursor.action) == 0x2c9)) {
          if ((i16(owner.animation.cursor.action) == 0x2bd) ||
             (i16(owner.animation.cursor.action) == 0x2c9)) {
            p.motion.x =
                 double(owner.motion.x) -
                 (float)(i8(p.motion.facing) * 0x52);
            p.motion.y = double(owner.motion.y) + double(132.0f);
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(0);
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x352:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (3 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.5f) + double(5.0f));
            roll = random.range(0,10);
            p.pose.scale[0] = (float)roll * double(0.025f) + double(0.5f);
            p.pose.scale[1] = double(p.pose.scale[0]);
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
          }
          p.counter = i16(p.counter) + 1;
          if (i16(p.animation.cursor.index) < 2) {
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.5f) + double(5.0f));
          }
          if ((p.response == 0) || (i16(p.animation.cursor.index) != 1)) {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            position = double(p.motion.y) - double(p.motion.velocityY);
            if ((position < double(0.0f) == (position == double(0.0f))) || (i16(p.animation.cursor.index) != 1)) {
              if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f)))
                  || (double(640.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(120.0f)))
              {
                p.destroy();
              }
              else {
                if (1 < i16(p.animation.cursor.index)) {
                  if (u8(channel(24)) < 0x19) {
                    setChannel(24,u8(0));
                  }
                  else {
                    setChannel(24,u8(i8(channel(24)) + -0x19));
                  }
                }
                advancedFrame = p.advance();
                if (advancedFrame == i16(p.animation.cursor.frameCount)) {
                  p.destroy();
                }
              }
            }
            else {
              p.frame(2);
              owned(0x32f,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.motion.y = 0.0f;
            }
          }
          else {
            p.motion.velocityY = 0.0f;
            p.motion.velocityX = 0.0f;
            p.frame(2);
            owned(0x32f,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
        }
        else {
          p.destroy();
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x353:
        if (((i16(owner.animation.cursor.action) == 0x2ef) &&
            ((i16(owner.animation.cursor.action) != 0x2ef ||
             (i16(owner.animation.cursor.index) < 0xc)))) ||
           ((i16(owner.animation.cursor.action) == 0x2f0 &&
            ((i16(owner.animation.cursor.action) != 0x2f0 ||
             (i16(owner.animation.cursor.index) < 0xc)))))) {
          if (((i16(owner.animation.cursor.action) == 0x2ef) &&
              (i16(owner.animation.cursor.index) < 0xc)) ||
             ((i16(owner.animation.cursor.action) == 0x2f0 &&
              (i16(owner.animation.cursor.index) < 0xc)))) {
            p.motion.x =
                 double(owner.motion.x) -
                 (float)(i8(p.motion.facing) * 0x52);
            p.motion.y = double(owner.motion.y) + double(132.0f);
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(0);
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x354:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          roll = random.range(0,0x168);
          p.pose.rotation[2] = (float)roll;
          p.counter = 10;
        }
        p.pose.scale[0] =
             (float)(int)i16(p.counter) * double(0.01f) + double(0.05f) + double(p.pose.scale[0]);
        p.pose.scale[1] = double(p.pose.scale[0]);
        if (0 < i16(p.counter)) {
          p.counter = i16(p.counter) + -1;
        }
        if (1 < i16(p.animation.cursor.index)) {
          setChannel(24,u8(i8(channel(24)) + -0x19));
          setChannel(16,u8(i8(channel(16)) + -0x14));
          setChannel(8,u8(i8(channel(8)) + -0x14));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x355:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
        p.pose.scale[1] = double(p.pose.scale[0]);
        if (0 < i16(p.counter)) {
          p.counter = i16(p.counter) + -1;
        }
        if (1 < i16(p.animation.cursor.index)) {
          setChannel(24,u8(i8(channel(24)) + -0x19));
          setChannel(16,u8(i8(channel(16)) + -0x14));
          setChannel(8,u8(i8(channel(8)) + -0x14));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x356:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.hits = 1;
          p.counter = 0;
          events.system_sound(4);
        }
        if (p.response == 6) {
          p.response = 0;
          p.hits = i16(p.hits) + 1;
        }
        if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
          auxiliaryValue = 1;
          direction = (u32)u8(p.motion.facing);
          roll = random.range(0,0xa0);
          position = ((float)roll + double(p.motion.y)) - double(80.0f);
          roll = random.range(0,0xa0);
          owned(0x354,((float)roll + double(p.motion.x)) - double(80.0f),position,direction,auxiliaryValue);
          auxiliaryValue = 1;
          direction = (u32)u8(p.motion.facing);
          roll = random.range(0,0xdc);
          position = ((float)roll + double(p.motion.y)) - double(110.0f);
          roll = random.range(0,0xdc);
          owned(0x355,((float)roll + double(p.motion.x)) - double(110.0f),position,direction,auxiliaryValue);
          p.counter = i16(p.counter) + 1;
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          if ((i16(p.counter) < 0x10) &&
             ((i16(p.counter) < 6 || (p.response == 0)))) {
            p.frame(1);
          }
          else {
            p.destroy();
          }
        }
        break;
      case 900:
        if (((i16(owner.animation.cursor.action) == 0x2db) &&
            (8 < i16(owner.animation.cursor.index))) &&
           (i16(owner.animation.cursor.index) < 0x11)) {
          p.motion.x =
               double(owner.motion.x) -
               (float)(i8(p.motion.facing) * 0x6e);
          p.motion.y = double(owner.motion.y) + double(95.0f);
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 2.5f;
            p.pose.scale[1] = 1.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(0);
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x385:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          roll = random.range(0,5);
          p.pose.scale[0] = (float)roll * double(0.1f) + double(0.5f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          if (state.difficulty == '\0') {
            p.hits = 0;
          }
          else {
            p.hits = 1;
          }
          p.response = 0;
          advancedFrame = random.range(0,0x3c);
          p.counter = advancedFrame + -0x1e;
          wordValue = random.range(5,0xf);
          p.timer = wordValue;
        }
        p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer));
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0xd) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0xc));
          setChannel(16,u8(i8(channel(16)) + -0xc));
          setChannel(8,u8(i8(channel(8)) + -0xc));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x38e:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = double(p.pose.scale[0]);
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          roll = random.range(0,0x167);
          p.pose.rotation[2] = (float)roll;
          p.response = 0;
          p.hits = 1;
          p.counter = 0;
          p.polar(double(p.parameters[0]) + double(p.parameters[1]),(float)(int)i16(p.counter) * double(0.05f) + double(p.parameters[2]));
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.counter = i16(p.counter) + 1;
        if ((i16(p.hits) == 0) && (i16(p.animation.cursor.index) == 1)) {
          p.frame(2);
        }
        else {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          if (1 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          if ((((double(p.motion.x) < -double(80.0f)) || (double(1360.0f) < double(p.motion.x))) ||
              (double(p.motion.y) < double(0.0f))) || (double(960.0f) < double(p.motion.y))) {
            p.destroy();
          }
          else {
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
        }
        break;
      case 0x38f:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = double(p.pose.scale[0]);
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          roll = random.range(0,0x167);
          p.pose.rotation[2] = (float)roll;
          p.response = 0;
          p.hits = 1;
          p.counter = 0;
          p.polar(double(p.parameters[0]) + double(p.parameters[1]),(float)(int)i16(p.counter) * double(0.05f) + double(p.parameters[2]));
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.counter = i16(p.counter) + 1;
        if ((i16(p.hits) == 0) && (i16(p.animation.cursor.index) == 1)) {
          p.frame(2);
        }
        else {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          if (1 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          if ((((double(p.motion.x) < -double(80.0f)) || (double(1360.0f) < double(p.motion.x))) ||
              (double(p.motion.y) < double(0.0f))) || (double(960.0f) < double(p.motion.y))) {
            p.destroy();
          }
          else {
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
        }
        break;
      case 0x390:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = double(p.pose.scale[0]);
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          roll = random.range(0,0x167);
          p.pose.rotation[2] = (float)roll;
          p.response = 0;
          p.hits = 1;
          p.counter = 0;
          p.polar(double(p.parameters[0]) + double(p.parameters[1]),(float)(int)i16(p.counter) * double(0.05f) + double(p.parameters[2]));
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.counter = i16(p.counter) + 1;
        if ((i16(p.hits) == 0) && (i16(p.animation.cursor.index) == 1)) {
          p.frame(2);
        }
        else {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          if (1 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          if ((((double(p.motion.x) < -double(80.0f)) || (double(1360.0f) < double(p.motion.x))) ||
              (double(p.motion.y) < double(0.0f))) || (double(960.0f) < double(p.motion.y))) {
            p.destroy();
          }
          else {
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
        }
        break;
      case 0x391:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = double(p.pose.scale[0]);
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          roll = random.range(0,0x167);
          p.pose.rotation[2] = (float)roll;
          p.response = 0;
          p.hits = 1;
          p.counter = 0;
          p.polar(double(p.parameters[0]) + double(p.parameters[1]),(float)(int)i16(p.counter) * double(0.05f) + double(p.parameters[2]));
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.counter = i16(p.counter) + 1;
        if ((i16(p.hits) == 0) && (i16(p.animation.cursor.index) == 1)) {
          p.frame(2);
        }
        else {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          if (1 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          if ((((double(p.motion.x) < -double(80.0f)) || (double(1360.0f) < double(p.motion.x))) ||
              (double(p.motion.y) < double(0.0f))) || (double(960.0f) < double(p.motion.y))) {
            p.destroy();
          }
          else {
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
        }
        break;
      case 0x399:
        if ((i16(owner.animation.cursor.action) == 0x2ef) &&
           (i16(owner.animation.cursor.index) < 0xc)) {
          p.motion.x =
               (float)((int)i8(p.motion.facing) << 5) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(90.0f);
          direction0 = (u32)(double(0.0f) < (double(other.motion.x) -
                                   double(owner.motion.x)) *
                                   (float)(int)i8(p.motion.facing));
          owner.motion.opponentPush =
               (float)((int)i8(p.motion.facing) *
                      (int)i8(other.motion.facing)) * double(6.0f) *
               (float)(int)(direction0 * 2 + -1);
          events.shake(3.0f);
        }
        if ((((i16(owner.animation.cursor.action) == 0x2ef) &&
             (i16(owner.animation.cursor.action) == 0x2ef)) &&
            (i16(owner.animation.cursor.index) < 0xc)) ||
           (0x18 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.hits = 1;
            p.response = 0;
            if (i16(p.counter) != 1) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.pose.scale[0] = 4.0f;
              p.pose.scale[1] = 1.0f;
              p.counter = 1;
              p.timer = 100;
              p.angle = 2000;
              setChannel(24,u8(0xff));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
            }
          }
          if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.hits = 1;
            p.response = 0;
          }
          if (i16(p.animation.cursor.index) < 0x19) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.005f);
            wordValue = i32(double(p.timer)*double(0.98f));
            p.timer = wordValue;
            p.angle = i16(p.angle) + -1;
            p.pose.scale[1] =
                 (float)(int)i16(p.timer) * 0.01 +
                 (float)(int)i16(p.angle) * double(0.001f);
          }
          if (9 < i16(p.animation.cursor.index)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.15f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
            setChannel(24,u8(i8(channel(24)) + -5));
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(0x19);
          }
          else if ((i16(p.animation.cursor.index) == 10) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(0);
          }
        }
        else {
          p.frame(i16(p.animation.cursor.index) + 0x19);
          p.hits = 1;
          p.response = 0;
        }
        break;
      case 0x39a:
        if ((i16(owner.animation.cursor.action) == 0x2f0) &&
           (i16(owner.animation.cursor.index) < 0xc)) {
          p.motion.x =
               (float)((int)i8(p.motion.facing) << 5) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(90.0f);
          direction1 = (u32)(double(0.0f) < (double(other.motion.x) -
                                   double(owner.motion.x)) *
                                   (float)(int)i8(p.motion.facing));
          owner.motion.opponentPush =
               (float)((int)i8(p.motion.facing) *
                      (int)i8(other.motion.facing)) * double(6.0f) *
               (float)(int)(direction1 * 2 + -1);
          events.shake(3.0f);
        }
        if ((((i16(owner.animation.cursor.action) == 0x2f0) &&
             (i16(owner.animation.cursor.action) == 0x2f0)) &&
            (i16(owner.animation.cursor.index) < 0xc)) ||
           (0x18 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            state.laser.visible = 1;
            state.laser.x = double(p.motion.x);
            state.laser.y = double(p.motion.y);
            state.laser.facing = u8(p.motion.facing);
            p.hits = 1;
            p.response = 0;
            if (i16(p.counter) != 1) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.pose.scale[0] = 4.0f;
              p.pose.scale[1] = 2.0f;
              p.counter = 1;
              p.timer = 100;
              p.angle = 0xdac;
              setChannel(24,u8(0xff));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
            }
          }
          if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.hits = 1;
            p.response = 0;
          }
          if (i16(p.animation.cursor.index) < 0x19) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.005f);
            wordValue = i32(double(p.timer)*double(0.98f));
            p.timer = wordValue;
            p.angle = i16(p.angle) + -1;
            p.pose.scale[1] =
                 (float)(int)i16(p.timer) * 0.005 +
                 (float)(int)i16(p.angle) * double(0.001f);
            state.laser.width = double(p.pose.scale[1]);
          }
          if (9 < i16(p.animation.cursor.index)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.15f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
            setChannel(24,u8(i8(channel(24)) + -5));
            state.laser.width = double(p.pose.scale[1]);
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            state.laser.visible = 0;
            p.frame(0x19);
          }
          else if ((i16(p.animation.cursor.index) == 10) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(0);
          }
        }
        else {
          p.frame(i16(p.animation.cursor.index) + 0x19);
          p.hits = 1;
          p.response = 0;
        }
        break;
      case 0x39b:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          roll = random.range(0,0x167);
          p.pose.rotation[2] = (float)roll;
          p.response = 0;
          p.hits = 1;
          p.counter = 0;
          roll = random.range(0,0x28);
          wordValue = i32(double(p.parameters[0])-roll+20);
          p.timer = wordValue;
          advancedFrame = random.range(0,0x3c);
          p.angle = advancedFrame + 0x96;
        }
        p.polar((float)(int)i16(p.timer),(float)(int)i16(p.angle) * double(0.1f));
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.counter = i16(p.counter) + 1;
        if (0x1e < i16(p.angle)) {
          p.angle = i16(p.angle) + -2;
        }
        if (((i16(p.hits) == 0) ||
            (position = double(p.motion.y) + double(p.motion.velocityY),
            position < double(0.0f) != (position == double(0.0f)))) && (i16(p.animation.cursor.index) == 1)) {
          p.frame(2);
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
        }
        else {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          if (1 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          if (((double(p.motion.x) < -double(80.0f)) || (double(1360.0f) < double(p.motion.x))) ||
             ((double(p.motion.y) < double(0.0f) || (double(960.0f) < double(p.motion.y))))) {
            p.destroy();
          }
          else {
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
        }
        break;
      case 0x39c:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = double(p.pose.scale[0]);
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          roll = random.range(0,0x167);
          p.pose.rotation[2] = (float)roll;
          p.response = 0;
          p.hits = 1;
          p.counter = 0;
          position = (float)(int)i16(p.counter) * double(0.05f) + double(p.parameters[2]);
          roll = roll = random.range(0,0x28);
          p.polar((float)(roll + 0x46),position);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.counter = i16(p.counter) + 1;
        if (((i16(p.hits) == 0) ||
            (position = double(p.motion.y) + double(p.motion.velocityY),
            position < double(0.0f) != (position == double(0.0f)))) && (i16(p.animation.cursor.index) == 1)) {
          p.frame(2);
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
        }
        else {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          if (1 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          if (((double(p.motion.x) < -double(80.0f)) || (double(1360.0f) < double(p.motion.x))) ||
             ((double(p.motion.y) < double(0.0f) || (double(960.0f) < double(p.motion.y))))) {
            p.destroy();
          }
          else {
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
        }
        break;
      case 0x39d:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = double(p.pose.scale[0]);
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          roll = random.range(0,0x167);
          p.pose.rotation[2] = (float)roll;
          p.response = 0;
          p.hits = 1;
          p.counter = 0;
          position = (float)(int)i16(p.counter) * double(0.05f) + double(p.parameters[2]);
          roll = roll = random.range(0,0x28);
          p.polar((float)(roll + 0x46),position);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.counter = i16(p.counter) + 1;
        if (((i16(p.hits) == 0) ||
            (position = double(p.motion.y) + double(p.motion.velocityY),
            position < double(0.0f) != (position == double(0.0f)))) && (i16(p.animation.cursor.index) == 1)) {
          p.frame(2);
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
        }
        else {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          if (1 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          if (((double(p.motion.x) < -double(80.0f)) || (double(1360.0f) < double(p.motion.x))) ||
             ((double(p.motion.y) < double(0.0f) || (double(960.0f) < double(p.motion.y))))) {
            p.destroy();
          }
          else {
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
        }
        break;
      case 0x39e:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = double(p.pose.scale[0]);
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          roll = random.range(0,0x167);
          p.pose.rotation[2] = (float)roll;
          p.response = 0;
          p.hits = 1;
          p.counter = 0;
          position = (float)(int)i16(p.counter) * double(0.05f) + double(p.parameters[2]);
          roll = roll = random.range(0,0x28);
          p.polar((float)(roll + 0x46),position);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.counter = i16(p.counter) + 1;
        if (((i16(p.hits) == 0) ||
            (position = double(p.motion.y) + double(p.motion.velocityY),
            position < double(0.0f) != (position == double(0.0f)))) && (i16(p.animation.cursor.index) == 1)) {
          p.frame(2);
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
        }
        else {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          if (1 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          if (((double(p.motion.x) < -double(80.0f)) || (double(1360.0f) < double(p.motion.x))) ||
             ((double(p.motion.y) < double(0.0f) || (double(960.0f) < double(p.motion.y))))) {
            p.destroy();
          }
          else {
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
        }
        break;
      case 0x39f:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.004999999888241291f;
          p.pose.scale[1] = 4.0f;
          p.response = 0;
          p.hits = 0;
          p.counter = 0;
          p.timer = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          events.sound(0);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.counter = i16(p.counter) + 1;
        p.timer = i16(p.timer) + -1;
        p.motion.velocityX = 0.0f;
        if (double(p.motion.x) <
            double(other.motion.x) - double(50.0f)) {
          p.motion.velocityX = (float)(int)state.difficulty * double(0.5f) + double(1.0f);
        }
        if (double(other.motion.x) + double(50.0f) <
            double(p.motion.x)) {
          p.motion.velocityX = -double(1.0f) - (float)(int)state.difficulty * double(0.5f);
        }
        if ((i16(p.counter) < 0x78) && (double(p.pose.scale[0]) < double(0.025f))) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.005f);
        }
        if (i16(p.counter) == 0x78) {
          p.hits = 0x100;
        }
        if ((0x78 < i16(p.counter)) && (i16(p.counter) < 0xf1)) {
          if (double(p.pose.scale[0]) < double(1.5f)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.15f);
          }
          events.shake(4.0f);
          owner.launchParameters[0] = -90.0f;
          layer = 1;
          direction = (u32)u8(p.motion.facing);
          auxiliaryValue = 0;
          position = double(p.motion.x);
          roll = random.range(0,0xf0);
          owned(0x39b,(position + double(120.0f)) - (float)roll,auxiliaryValue,direction,layer);
          if (i16(p.timer) < 1) {
            p.timer = 5;
            p.response = 0;
          }
        }
        if (i16(p.counter) == 0xf0) {
          state.actionStage =
               i16(state.actionStage) + 1;
        }
        if (0xf0 < i16(p.counter)) {
          p.motion.velocityX = 0.0f;
          p.hits = 0;
          p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
        }
        break;
      case 0x3a0:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.004999999888241291f;
          p.pose.scale[1] = 4.0f;
          p.response = 0;
          p.hits = 0;
          p.counter = 0;
          p.timer = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          events.sound(0);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.counter = i16(p.counter) + 1;
        p.timer = i16(p.timer) + -1;
        p.motion.velocityX = 0.0f;
        if (double(p.motion.x) <
            double(other.motion.x) - double(50.0f)) {
          p.motion.velocityX = (float)(int)state.difficulty * double(0.5f) + double(1.0f);
        }
        if (double(other.motion.x) + double(50.0f) <
            double(p.motion.x)) {
          p.motion.velocityX = -double(1.0f) - (float)(int)state.difficulty * double(0.5f);
        }
        if ((i16(p.counter) < 0x78) && (double(p.pose.scale[0]) < double(0.025f))) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.005f);
        }
        if (i16(p.counter) == 0x78) {
          p.hits = 0x100;
        }
        if ((0x78 < i16(p.counter)) && (i16(p.counter) < 0x141)) {
          if (double(p.pose.scale[0]) < double(1.5f)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.15f);
          }
          events.shake(4.0f);
          owner.launchParameters[0] = -90.0f;
          layer = 1;
          direction = (u32)u8(p.motion.facing);
          auxiliaryValue = 0;
          position = double(p.motion.x);
          roll = random.range(0,0xf0);
          owned(0x39b,(position + double(120.0f)) - (float)roll,auxiliaryValue,direction,layer);
          if (i16(p.timer) < 1) {
            p.timer = 5;
            p.response = 0;
          }
        }
        if (i16(p.counter) == 0x140) {
          state.actionStage =
               i16(state.actionStage) + 1;
        }
        if (0x140 < i16(p.counter)) {
          p.motion.velocityX = 0.0f;
          p.hits = 0;
          p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
        }
        break;
      case 0x3a1:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.004999999888241291f;
          p.pose.scale[1] = 4.0f;
          p.response = 0;
          p.hits = 0;
          p.counter = 0;
          p.timer = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          events.sound(0);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.counter = i16(p.counter) + 1;
        p.timer = i16(p.timer) + -1;
        p.motion.velocityX = 0.0f;
        if (double(p.motion.x) <
            double(other.motion.x) - double(50.0f)) {
          p.motion.velocityX = (float)(int)state.difficulty * double(0.5f) + double(1.0f);
        }
        if (double(other.motion.x) + double(50.0f) <
            double(p.motion.x)) {
          p.motion.velocityX = -double(1.0f) - (float)(int)state.difficulty * double(0.5f);
        }
        if ((i16(p.counter) < 0x78) && (double(p.pose.scale[0]) < double(0.025f))) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.005f);
        }
        if (i16(p.counter) == 0x78) {
          p.hits = 0x100;
        }
        if ((0x78 < i16(p.counter)) && (i16(p.counter) < 0x141)) {
          if (double(p.pose.scale[0]) < double(1.5f)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.15f);
          }
          events.shake(4.0f);
          owner.launchParameters[0] = -90.0f;
          layer = 1;
          direction = (u32)u8(p.motion.facing);
          auxiliaryValue = 0;
          position = double(p.motion.x);
          roll = random.range(0,0xf0);
          owned(0x39b,(position + double(120.0f)) - (float)roll,auxiliaryValue,direction,layer);
          if (i16(p.timer) < 1) {
            p.timer = 5;
            p.response = 0;
          }
        }
        if (i16(p.counter) == 0x140) {
          state.actionStage =
               i16(state.actionStage) + 1;
        }
        if (0x140 < i16(p.counter)) {
          p.motion.velocityX = 0.0f;
          p.hits = 0;
          p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
        }
        break;
      case 0x3a2:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.004999999888241291f;
          p.pose.scale[1] = 4.0f;
          p.response = 0;
          p.hits = 0;
          p.counter = 0;
          p.timer = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          events.sound(0);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.counter = i16(p.counter) + 1;
        p.timer = i16(p.timer) + -1;
        p.motion.velocityX = 0.0f;
        if (double(p.motion.x) <
            double(other.motion.x) - double(50.0f)) {
          p.motion.velocityX = (float)(int)state.difficulty * double(0.5f) + double(1.0f);
        }
        if (double(other.motion.x) + double(50.0f) <
            double(p.motion.x)) {
          p.motion.velocityX = -double(1.0f) - (float)(int)state.difficulty * double(0.5f);
        }
        if ((i16(p.counter) < 0x78) && (double(p.pose.scale[0]) < double(0.025f))) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.005f);
        }
        if (i16(p.counter) == 0x78) {
          p.hits = 0x100;
        }
        if ((0x78 < i16(p.counter)) && (i16(p.counter) < 0x155)) {
          if (double(p.pose.scale[0]) < double(2.5f)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
          }
          events.shake(4.0f);
          owner.launchParameters[0] = -90.0f;
          layer = 1;
          direction = (u32)u8(p.motion.facing);
          auxiliaryValue = 0;
          position = double(p.motion.x);
          roll = random.range(0,0xf0);
          owned(0x39b,(position + double(120.0f)) - (float)roll,auxiliaryValue,direction,layer);
          if (i16(p.timer) < 1) {
            p.timer = 5;
            p.response = 0;
          }
        }
        if (i16(p.counter) == 0x154) {
          state.actionStage =
               i16(state.actionStage) + 1;
        }
        if (0x154 < i16(p.counter)) {
          p.motion.velocityX = 0.0f;
          p.hits = 0;
          p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
        }
        break;
      case 0x3de:
        if ((i16(state.frame.farRangeFrames) == 0) && (i16(p.animation.cursor.index) < 2))
        {
          p.frame(2);
        }
        else {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
          p.contactDelay = 1;
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 2.0f;
            p.pose.scale[1] = 2.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
          if (i16(p.animation.cursor.index) == 0) {
            if (u8(channel(24)) < 0xb4) {
              setChannel(24,u8(i8(channel(24)) + '\x19'));
            }
            else {
              setChannel(24,u8(0xb4));
            }
            if (0x77 < i16(state.frame.farRangeFrames)) {
              p.frame(1);
              setChannel(24,u8(0xb4));
              return;
            }
          }
          if (i16(p.animation.cursor.index) == 2) {
            if (u8(channel(24)) < 0x1e) {
              setChannel(24,u8(0));
            }
            else {
              setChannel(24,u8(i8(channel(24)) + -0x1e));
            }
            if (i8(channel(24)) == '\0') {
              p.destroy();
              return;
            }
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x3df:
        if ((i16(state.frame.farRangeFrames) == 0) && (i16(p.animation.cursor.index) < 2))
        {
          p.frame(2);
        }
        else {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
          p.contactDelay = 1;
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          p.pose.scale[0] =
               (float)(int)i16(state.frame.farRangeFrames) / double(60.0f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          if (i16(p.animation.cursor.index) == 0) {
            if (u8(channel(24)) < 0xe6) {
              setChannel(24,u8(i8(channel(24)) + '\x19'));
            }
            else {
              setChannel(24,u8(0xff));
            }
            if (0x77 < i16(state.frame.farRangeFrames)) {
              p.frame(1);
              setChannel(24,u8(0xff));
              return;
            }
          }
          if (i16(p.animation.cursor.index) == 2) {
            if (u8(channel(24)) < 10) {
              setChannel(24,u8(0));
            }
            else {
              setChannel(24,u8(i8(channel(24)) + -10));
            }
            if (i8(channel(24)) == '\0') {
              p.destroy();
              return;
            }
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x3e2:
        p.motion.facing = 1;
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = -1.0f;
          advancedFrame = roll = random.range(0,0x28);
          p.counter = 0x6e - advancedFrame;
          advancedFrame = random.range(0,7);
          p.timer = 0xf - advancedFrame;
        }
        if (i16(p.animation.cursor.index) == 0) {
          p.timer = i16(p.timer) + -1;
          if (i16(p.timer) < 3) {
            p.timer = 3;
          }
          p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer) * double(0.8f));
        }
        if (i16(p.animation.cursor.index) == 1) {
          aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                       double(p.motion.y),double(other.motion.x) -
                       double(p.motion.x));
          wordValue = i32(aimRadians/double(0.017453289f));
          p.counter = wordValue;
          p.polar((float)(int)i16(p.counter),20.0f);
          if ((((double(p.motion.x) -
                 double(other.motion.x) < double(20.0f)) &&
               (-double(20.0f) < double(p.motion.x) -
                        double(other.motion.x))) &&
              (double(p.motion.y) -
               double(other.motion.y) < double(120.0f))) &&
             (double(80.0f) < double(p.motion.y) -
                     double(other.motion.y))) {
            owner.knockdownTime =
                 i16(owner.knockdownTime) + 1;
            events.system_sound(0x2e);
            p.destroy();
            if (0 < i16(other.combat.spiritBreak)) {
              if (5 < i16(other.combat.spiritBreak)) {
                other.combat.spiritBreak =
                     i16(other.combat.spiritBreak) + -5;
                return;
              }
              other.combat.spiritBreak = 0;
              return;
            }
            if (i16(other.combat.spirit) < 0x3de) {
              other.combat.spirit =
                   i16(other.combat.spirit) + 10;
              return;
            }
            other.combat.spirit = 1000;
            if (i8(other.combat.spellEnded) != '\0') {
              return;
            }
            if (double(other.combat.spellExtent) < double(998.9f)) {
              other.combat.spellExtent =
                   double(other.combat.spellExtent) + double(0.5f);
              return;
            }
            other.combat.spellExtent = 900.0f;
            return;
          }
        }
        p.motion.x = double(p.motion.x) + double(p.motion.velocityX);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
        }
        break;
      case 0x3e3:
        p.motion.facing = 1;
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = -1.0f;
          advancedFrame = random.range(0,0x168);
          p.counter = advancedFrame + -0xb4;
          p.timer = 0xf;
        }
        if (i16(p.animation.cursor.index) == 0) {
          p.timer = i16(p.timer) + -1;
          if (i16(p.timer) < 3) {
            p.timer = 3;
          }
          p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer) * double(0.8f));
        }
        if (i16(p.animation.cursor.index) == 1) {
          aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                       double(p.motion.y),double(other.motion.x) -
                       double(p.motion.x));
          wordValue = i32(aimRadians/double(0.017453289f));
          p.counter = wordValue;
          p.polar((float)(int)i16(p.counter),20.0f);
          if (((double(p.motion.x) -
                double(other.motion.x) < double(20.0f)) &&
              (-double(20.0f) < double(p.motion.x) -
                       double(other.motion.x))) &&
             ((double(p.motion.y) -
               double(other.motion.y) < double(120.0f) &&
              (double(80.0f) < double(p.motion.y) -
                      double(other.motion.y))))) {
            p.destroy();
            if (i8(other.selectedSpell) < '\x02') {
              other.selectedSpell =
                   i8(other.selectedSpell) + '\x01';
            }
            else {
              other.score =
                   other.score + 20000;
            }
            events.system_sound(0x2e);
            return;
          }
        }
        p.motion.x = double(p.motion.x) + double(p.motion.velocityX);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
        }
        break;
      case 0x3e4:
        p.motion.facing = 1;
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = -1.0f;
          advancedFrame = random.range(0,0x168);
          p.counter = advancedFrame + -0xb4;
          p.timer = 0xf;
        }
        if (i16(p.animation.cursor.index) == 0) {
          p.timer = i16(p.timer) + -1;
          if (i16(p.timer) < 3) {
            p.timer = 3;
          }
          p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer) * double(0.8f));
        }
        if (i16(p.animation.cursor.index) == 1) {
          aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                       double(p.motion.y),double(other.motion.x) -
                       double(p.motion.x));
          wordValue = i32(aimRadians/double(0.017453289f));
          p.counter = wordValue;
          p.polar((float)(int)i16(p.counter),20.0f);
          if ((((double(p.motion.x) -
                 double(other.motion.x) < double(20.0f)) &&
               (-double(20.0f) < double(p.motion.x) -
                        double(other.motion.x))) &&
              (double(p.motion.y) -
               double(other.motion.y) < double(120.0f))) &&
             (double(80.0f) < double(p.motion.y) -
                     double(other.motion.y))) {
            owner.knockdownTime =
                 i16(owner.knockdownTime) + 1;
            events.system_sound(0x2e);
            p.destroy();
            if (0 < i16(other.combat.spiritBreak)) {
              if (5 < i16(other.combat.spiritBreak)) {
                other.combat.spiritBreak =
                     i16(other.combat.spiritBreak) + -5;
                return;
              }
              other.combat.spiritBreak = 0;
              other.combat.spirit = 1000;
              return;
            }
            if (i16(other.combat.spirit) < 0x3de) {
              other.combat.spirit =
                   i16(other.combat.spirit) + 10;
              return;
            }
            other.combat.spirit = 1000;
            if (i8(other.combat.spellEnded) != '\0') {
              return;
            }
            if (double(other.combat.spellExtent) < double(998.9f)) {
              other.combat.spellExtent =
                   double(other.combat.spellExtent) + double(0.5f);
              return;
            }
            other.combat.spellExtent = 900.0f;
            return;
          }
        }
        p.motion.x = double(p.motion.x) + double(p.motion.velocityX);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
        }
        break;
      case 0x3e5:
        p.motion.x = double(owner.motion.x);
        p.motion.y = 0.0f;
        p.contactDelay = 1;
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 3.0f;
          p.pose.scale[1] = 3.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.projection[2] = 0x4e2;
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
          p.pose.rotation[0] = -80.0f;
          p.counter = owner.combat.spellTimer;
        }
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          position = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[0] = position + position;
          position = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[1] = position + position;
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(10);
          }
        }
        break;
      case 0x3e6:
        if (((i16(owner.animation.cursor.action) == 0xd1) ||
            (i16(owner.animation.cursor.action) == 0xd2)) ||
           (1 < i16(p.animation.cursor.index))) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[0] = 0.0f;
            p.pose.rotation[1] = 0.0f;
            p.pose.rotation[2] = 0.0f;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) - double(0.0025f);
          p.pose.scale[1] = double(p.pose.scale[1]) - double(0.0025f);
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
          if (i16(p.animation.cursor.index) == 2) {
            setChannel(24,u8(i8(channel(24)) + -0xc));
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(1);
          }
        }
        else {
          p.frame(2);
        }
      }
    }
    else {
      p.freeze = i16(p.freeze) + -1;
    }
  }
  return;

}
}
