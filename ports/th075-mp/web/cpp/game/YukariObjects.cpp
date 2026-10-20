#include "YukariObjects.hpp"
#include <cmath>
namespace th075 {void advance_yukari_object(YukariObject& p,FighterActor& owner,FighterActor& other,YukariState& state,ReplayStreams& random,FighterActionEvents& events){
i32 lastRoll=0;const auto nextRandom=[&](i32 low,i32 high){return lastRoll=random.range(low,high);};
auto& sceneFadeColor=events.fade_color();auto& sceneFadeAlpha=events.fade_alpha();
double aimRadians=0;const auto atan=[](float y,float x){return std::atan2(double(y),double(x));};
const auto channel=[&](u32 shift){return p.pose.channel(shift);};const auto setChannel=[&](u32 shift,u8 value){p.pose.set_channel(shift,value);};
const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters);};
const auto shared=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Shared,id,x,y,facing,layer,owner.launchParameters);};

  float fVar1;
  u16 uVar2;
  i16 sVar3;
  u32 uVar4;
  int iVar5;
  double fVar6;
  float particleCoordinateOrLayer;
  u32 uVar8;
  u32 local_6c;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  
  if ((i16(other.combat.opponentStop) == 0) &&
     ((i16(owner.combat.opponentStop) == 0 || (i16(p.contactDelay) != 0)))) {
    if (i16(p.freeze) == 0) {
      switch(p.animation.cursor.action) {
      case 800:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            p.counter = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
          }
          p.counter = i16(p.counter) + 1;
          if (i16(p.animation.cursor.index) < 2) {
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.1f) + double(p.parameters[1]));
          }
          if ((((p.response == 1) || (p.response == 2)) ||
              ((p.response == 4 || (p.response == 6)))) &&
             (i16(p.animation.cursor.index) == 1)) {
            p.frame(2);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          else {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
                (double(640.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(120.0f))) {
              p.destroy();
            }
            else {
              if (1 < i16(p.animation.cursor.index)) {
                p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f);
                p.pose.scale[0] = double(p.pose.scale[0]) * double(0.9f);
                p.pose.scale[1] = double(p.pose.scale[1]) * double(0.9f);
              }
              sVar3 = p.advance();
              if (sVar3 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
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
          for (local_8 = 0; local_8 < 3; local_8 = local_8 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x321:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 75.0f;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
        if (0x14 < u8(channel(24))) {
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x322:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
        if (0 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0x15) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x323:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.response = 0;
          p.hits = 1;
          p.motion.velocityX = 25.0f;
        }
        p.motion.velocityX = double(p.motion.velocityX) - double(0.8f);
        if (double(p.motion.velocityX) < double(2.5f)) {
          p.motion.velocityX = 2.5f;
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0x15) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x324:
        if ((i16(owner.animation.cursor.action) == 700) &&
           (i16(owner.animation.cursor.index) < 7)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x7e) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(139.0f);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.0f;
          p.pose.scale[1] = 0.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = -30.0f;
          p.pose.rotation[1] = (float)(i8(p.motion.facing) * 0x3c);
          p.counter = 0;
        }
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
        if (double(p.pose.scale[0]) < double(1.0f)) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
        }
        if ((double(p.pose.scale[0]) < double(1.0f) != (double(p.pose.scale[0]) == double(1.0f))) &&
           (i16(p.counter) == 0)) {
          owned(0x325,double(owner.motion.x),double(owner.motion.y),u8(p.motion.facing),0);
          p.counter = i16(p.counter) + 1;
        }
        if (((6 < i16(owner.animation.cursor.index)) &&
            (i16(owner.animation.cursor.action) == 700)) ||
           (i16(owner.animation.cursor.action) != 700)) {
          if (u8(channel(24)) < 0x15) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x325:
        if ((i16(owner.animation.cursor.action) == 700) &&
           (i16(owner.animation.cursor.index) < 7)) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.hits = 99;
        }
        if (((i16(owner.animation.cursor.index) < 7) ||
            (i16(owner.animation.cursor.action) != 700)) &&
           (i16(owner.animation.cursor.action) == 700)) {
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(1);
            p.response = 0;
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x326:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.polar(double(p.parameters[0]),10.0f);
          }
          if ((i16(p.animation.cursor.index) == 0) &&
             (double(p.parameters[1]) <= (float)(int)i16(p.animation.cursor.elapsed))) {
            p.frame(1);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.hits = 1;
            p.response = 0;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 1) {
            p.polar(double(p.parameters[2]),(float)(int)i16(p.counter) * double(0.1f));
            p.counter = i16(p.counter) + 10;
            owner.launchParameters[0] = double(p.parameters[2]);
            owner.launchParameters[1] = (float)(int)i16(p.counter);
            owned(0x327,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            if (p.response != 0) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.frame(2);
              return;
            }
          }
          if (i16(p.animation.cursor.index) == 2) {
            p.counter = i16(p.counter) + -0x14;
            if (i16(p.counter) < 0) {
              p.counter = 0;
            }
            owner.launchParameters[0] = double(p.parameters[2]);
            owner.launchParameters[1] = (float)(int)i16(p.counter);
            owned(0x327,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.95f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.95f);
          }
          if ((((double(1800.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(520.0f))) ||
              (double(1400.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(520.0f))) {
            p.destroy();
          }
          else {
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_c = 0; local_c < 3; local_c = local_c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x327:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = double(p.parameters[1]) * double(0.01f) + double(0.0f);
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.response = 0;
          p.hits = 1;
          p.pose.rotation[2] = double(p.parameters[0]);
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x328:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.hits = 0xff;
          p.response = 0;
          p.counter = 0;
        }
        if ((i16(p.animation.cursor.index) < 6) &&
           (p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f),
           double(1.5f) < double(p.pose.scale[0]))) {
          p.pose.scale[0] = 1.5f;
        }
        if ((5 < i16(p.animation.cursor.index)) && (p.response == 5)) {
          p.response = 0;
          state.particleDelay =
               i16(state.particleDelay) + 1;
          if (10 < i16(state.particleDelay)) {
            state.particleDelay = 10;
          }
        }
        if (i16(p.hits) == 0) {
          if (double(p.pose.scale[0]) <= double(0.15f)) {
            p.destroy();
            return;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) - double(0.15f);
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.frame(6);
          p.counter = i16(p.counter) + 1;
          if (1 < i16(p.counter)) {
            p.hits = 0;
          }
        }
        break;
      case 0x329:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.hits = 0xff;
          p.response = 0;
          p.counter = 0;
        }
        if ((i16(p.animation.cursor.index) < 6) &&
           (p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f),
           double(1.5f) < double(p.pose.scale[0]))) {
          p.pose.scale[0] = 1.5f;
        }
        if ((5 < i16(p.animation.cursor.index)) && (p.response == 5)) {
          p.response = 0;
          state.particleDelay =
               i16(state.particleDelay) + 1;
          if (10 < i16(state.particleDelay)) {
            state.particleDelay = 10;
          }
        }
        if (i16(p.hits) == 0) {
          if (double(p.pose.scale[0]) <= double(0.15f)) {
            p.destroy();
            return;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) - double(0.15f);
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.frame(6);
          p.counter = i16(p.counter) + 1;
          if (2 < i16(p.counter)) {
            p.hits = 0;
          }
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
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
          owned(0x32b,double(p.motion.x),double(p.motion.y) - double(220.0f),u8(p.motion.facing),1);
        }
        break;
      case 0x32b:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        fVar1 = double(p.motion.y) + double(p.motion.velocityY);
        if ((fVar1 < double(0.0f) == (fVar1 == double(0.0f))) || (6 < i16(p.animation.cursor.index))) {
          if (i16(p.animation.cursor.index) == 8) {
            if (u8(channel(24)) < 0x10) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0xf));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.motion.velocityY = -18.0f;
            events.sound(5);
          }
        }
        else {
          p.motion.velocityY = 0.0f;
          p.motion.y = 0.0f;
          events.shake(5.0f);
          p.frame(7);
          events.sound(10);
          shared(0x81,double(p.motion.x) - double(50.0f),0.0f,1,1);
          shared(0x81,double(p.motion.x) + double(50.0f),0.0f,0xffffffff,1);
        }
        break;
      default:
        p.advance();
        break;
      case 0x32d:
        state.portalX = 1;
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 0x14;
          p.response = 0;
          p.pose.scale[0] = 0.0f;
          p.pose.scale[1] = 2.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.counter = 0;
        }
        if (i16(p.animation.cursor.index) == 1) {
          if (double(p.pose.scale[0]) < double(1.0f)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.25f);
          }
          if (double(1.0f) < double(p.pose.scale[1])) {
            p.pose.scale[1] = double(p.pose.scale[1]) - double(0.25f);
          }
        }
        if ((i16(p.animation.cursor.index) != 2) ||
           (fVar6 = (double)std::abs(float(double(p.motion.x) -
                                          double(other.motion.x))), (double)double(100.0f) <= fVar6)) {
          if ((i16(p.animation.cursor.index) == 3) &&
             (p.pose.rotation[2] = double(p.pose.rotation[2]) - double(30.0f),
             double(1.0f) < double(p.motion.velocityX))) {
            p.motion.velocityX = double(p.motion.velocityX) - double(1.0f);
          }
          if (i16(p.animation.cursor.index) == 4) {
            p.motion.velocityY = double(p.motion.velocityY) - double(0.4f);
            p.pose.rotation[2] = double(p.pose.rotation[2]) - double(30.0f);
          }
          if (i16(p.animation.cursor.index) == 5) {
            p.pose.rotation[2] = double(p.pose.rotation[2]) - double(30.0f);
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              state.portalX = 0;
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if ((double(1480.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(200.0f))) {
            p.destroy();
            state.portalX = 0;
          }
          else {
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
              state.portalX = 0;
            }
            else {
              if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.motion.velocityX = 30.0f;
              }
              if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
                if ((i16(p.counter) < 0x14) && (0 < i16(p.hits))) {
                  p.frame(3);
                  p.response = 0;
                  p.counter = i16(p.counter) + 1;
                  return;
                }
                if (p.response == 0) {
                  p.frame(5);
                }
                else {
                  p.frame(4);
                  p.motion.velocityX = -3.0f;
                  p.motion.velocityY = 15.0f;
                }
              }
              if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
                static_cast<void>(9);
              }
            }
          }
        }
        else {
          p.frame(3);
          p.motion.velocityX = 10.0f;
          static_cast<void>(8);
        }
        break;
      case 0x32e:
        state.portalX = 1;
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 0x1e;
          p.response = 0;
          p.pose.scale[0] = 0.0f;
          p.pose.scale[1] = 2.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.counter = 0;
        }
        if ((0 < i16(p.contactDelay)) && (i16(owner.motion.hitFreeze) == 0))
        {
          p.contactDelay = 0;
        }
        if (i16(p.animation.cursor.index) == 1) {
          if (double(p.pose.scale[0]) < double(1.0f)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.25f);
          }
          if (double(1.0f) < double(p.pose.scale[1])) {
            p.pose.scale[1] = double(p.pose.scale[1]) - double(0.25f);
          }
        }
        if ((i16(p.animation.cursor.index) != 2) ||
           (fVar6 = (double)std::abs(float(double(p.motion.x) -
                                          double(other.motion.x))), (double)double(100.0f) <= fVar6)) {
          if ((i16(p.animation.cursor.index) == 3) &&
             (p.pose.rotation[2] = double(p.pose.rotation[2]) - double(30.0f),
             double(1.0f) < double(p.motion.velocityX))) {
            p.motion.velocityX = double(p.motion.velocityX) - double(1.0f);
          }
          if (i16(p.animation.cursor.index) == 4) {
            p.motion.velocityY = double(p.motion.velocityY) - double(0.4f);
            p.pose.rotation[2] = double(p.pose.rotation[2]) - double(30.0f);
          }
          if (i16(p.animation.cursor.index) == 5) {
            p.pose.rotation[2] = double(p.pose.rotation[2]) - double(30.0f);
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              state.portalX = 0;
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if ((double(1480.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(200.0f))) {
            p.destroy();
            state.portalX = 0;
          }
          else {
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
              state.portalX = 0;
            }
            else {
              if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.motion.velocityX = 30.0f;
              }
              if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
                if ((i16(p.counter) < 0x1e) && (0 < i16(p.hits))) {
                  p.frame(3);
                  p.response = 0;
                  p.counter = i16(p.counter) + 1;
                  return;
                }
                if (p.response == 0) {
                  p.frame(5);
                }
                else {
                  p.frame(4);
                  p.motion.velocityX = -2.0f;
                  p.motion.velocityY = 15.0f;
                }
              }
              if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
                static_cast<void>(9);
              }
            }
          }
        }
        else {
          p.frame(3);
          p.motion.velocityX = 10.0f;
          static_cast<void>(8);
        }
        break;
      case 0x32f:
        state.portalY = 1;
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 0x14;
          p.response = 0;
          p.pose.scale[0] = 0.0f;
          p.pose.scale[1] = 2.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.counter = 0;
        }
        if ((0 < i16(p.contactDelay)) && (i16(owner.motion.hitFreeze) == 0))
        {
          p.contactDelay = 0;
        }
        if (i16(p.animation.cursor.index) == 1) {
          if (double(p.pose.scale[0]) < double(1.0f)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.25f);
          }
          if (double(1.0f) < double(p.pose.scale[1])) {
            p.pose.scale[1] = double(p.pose.scale[1]) - double(0.25f);
          }
        }
        if (1 < i16(p.animation.cursor.index)) {
          if (((i8(p.motion.facing) == '\x01') &&
              (((double(0.0f) < double(p.motion.velocityX) &&
                (double(owner.combat.screenBounds[1]) < double(p.motion.x))) ||
               ((double(p.motion.velocityX) < double(0.0f) &&
                (double(p.motion.x) < double(owner.combat.screenBounds[0]))))))) ||
             ((i8(p.motion.facing) == -1 &&
              (((double(p.motion.velocityX) < double(0.0f) &&
                (double(owner.combat.screenBounds[1]) < double(p.motion.x))) ||
               ((double(0.0f) < double(p.motion.velocityX) &&
                (double(p.motion.x) < double(owner.combat.screenBounds[0]))))))))) {
            p.motion.velocityX = -double(p.motion.velocityX);
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) - double(30.0f);
          p.motion.velocityY = double(p.motion.velocityY) - double(0.8f);
          if ((double(p.motion.y) + double(p.motion.velocityY) < double(0.0f)) &&
             (double(p.motion.velocityY) < double(0.0f))) {
            p.counter = i16(p.counter) + 1;
            p.response = 0;
            p.hits = 1;
            p.motion.velocityY = -double(p.motion.velocityY);
            events.sound(0xd);
          }
        }
        if (i16(p.animation.cursor.index) == 5) {
          p.pose.rotation[2] = double(p.pose.rotation[2]) - double(30.0f);
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            state.portalY = 0;
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
          state.portalY = 0;
        }
        else {
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.motion.velocityX = 6.0f;
            p.motion.velocityY = 16.0f;
          }
          if (((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) < 5)) {
            p.frame(3);
          }
        }
        break;
      case 0x330:
        if ((i16(owner.animation.cursor.action) == 0x2bf) &&
           (i16(owner.animation.cursor.index) < 7)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x7e) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(140.0f);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.0f;
          p.pose.scale[1] = 0.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 20.0f;
          p.pose.rotation[1] = (float)(i8(p.motion.facing) * -0x32);
          p.counter = 0;
          p.timer = 0;
          p.hits = 99;
          p.response = 0;
        }
        if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
          owner.motion.opponentPush = 0.0f;
        }
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
        if (double(1.0f) <= double(p.pose.scale[0])) {
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          if (i16(p.animation.cursor.index) < 2) {
            p.frame(2);
            return;
          }
        }
        else {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
        }
        if ((((i16(owner.animation.cursor.index) < 7) ||
             (i16(owner.animation.cursor.action) != 0x2bf)) &&
            (i16(owner.animation.cursor.action) == 0x2bf)) ||
           (i16(p.animation.cursor.index) != 2)) {
          if (i16(p.animation.cursor.index) == 2) {
            p.timer = i16(p.timer) + 1;
            if (0x50 < i16(p.timer)) {
              p.timer = 0x50;
            }
            local_6c = (u32)(double(0.0f) < (double(other.motion.x) -
                                    double(p.motion.x)) *
                                    (float)(int)i8(p.motion.facing));
            owner.motion.opponentPush =
                 -((float)(int)i16(p.timer) * double(0.1f)) *
                 (float)((int)i8(p.motion.facing) *
                        (int)i8(other.motion.facing)) *
                 (float)(int)(local_6c * 2 + -1);
          }
          if (i16(p.animation.cursor.index) == 3) {
            if (u8(channel(24)) < 0x15) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x14));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(2);
            p.response = 0;
          }
        }
        else {
          p.frame(3);
        }
        break;
      case 0x331:
        if ((i16(owner.animation.cursor.action) == 0x2bf) &&
           (i16(owner.animation.cursor.index) < 7)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x7f) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(140.0f);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.0f;
          p.pose.scale[1] = 0.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 20.0f;
          p.pose.rotation[1] = (float)(i8(p.motion.facing) * -0x32);
          p.counter = 0;
          p.timer = 0;
          p.hits = 99;
          p.response = 0;
        }
        p.pose.rotation[2] = double(p.pose.rotation[2]) - double(5.0f);
        if (double(1.0f) <= double(p.pose.scale[0])) {
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          if (i16(p.animation.cursor.index) < 2) {
            p.frame(2);
            return;
          }
        }
        else {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
        }
        if ((((i16(owner.animation.cursor.index) < 7) ||
             (i16(owner.animation.cursor.action) != 0x2bf)) &&
            (i16(owner.animation.cursor.action) == 0x2bf)) ||
           (i16(p.animation.cursor.index) != 2)) {
          if (i16(p.animation.cursor.index) == 3) {
            if (u8(channel(24)) < 0x15) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x14));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(2);
            p.response = 0;
          }
        }
        else {
          p.frame(3);
        }
        break;
      case 0x332:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.5f;
          p.pose.scale[1] = 1.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.polar(double(p.parameters[0]),12.0f);
          p.pose.rotation[2] = double(p.parameters[0]);
        }
        if (double(p.parameters[0]) == double(0.0f)) {
          p.motion.y = 0.0f;
        }
        if (double(p.parameters[0]) == -double(90.0f)) {
          if (i8(p.motion.facing) == '\x01') {
            p.motion.x = double(owner.combat.screenBounds[1]);
          }
          if (i8(p.motion.facing) == -1) {
            p.motion.x = double(owner.combat.screenBounds[0]);
          }
        }
        if (double(p.parameters[0]) == -double(180.0f)) {
          p.motion.y = double(owner.combat.screenBounds[2]);
        }
        if (double(p.parameters[0]) == double(90.0f)) {
          if (i8(p.motion.facing) == -1) {
            p.motion.x = double(owner.combat.screenBounds[1]);
          }
          if (i8(p.motion.facing) == '\x01') {
            p.motion.x = double(owner.combat.screenBounds[0]);
          }
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x333:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.counter = 0;
        }
        if ((i16(p.counter) < 6) &&
           ((i16(owner.animation.cursor.action) != 0x2be ||
            (i16(owner.animation.cursor.index) != 3)))) {
          if (i16(p.animation.cursor.index) == 1) {
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = 0.0f;
              owned(0x332,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            p.motion.velocityX = 12.0f;
            p.motion.y = 0.0f;
            if (((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
                (i8(p.motion.facing) == -1)) ||
               ((double(owner.combat.screenBounds[1]) < double(p.motion.x) &&
                (i8(p.motion.facing) == '\x01')))) {
              p.frame(2);
              p.counter = i16(p.counter) + 1;
              p.response = 0;
              p.hits = 1;
            }
          }
          if (i16(p.animation.cursor.index) == 2) {
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = -90.0f;
              owned(0x332,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 12.0f;
            if (i8(p.motion.facing) == '\x01') {
              p.motion.x = double(owner.combat.screenBounds[1]);
            }
            if (i8(p.motion.facing) == -1) {
              p.motion.x = double(owner.combat.screenBounds[0]);
            }
            if (double(owner.combat.screenBounds[2]) < double(p.motion.y)) {
              p.frame(3);
              p.counter = i16(p.counter) + 1;
              p.response = 0;
              p.hits = 1;
            }
          }
          if (i16(p.animation.cursor.index) == 3) {
            if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = -180.0f;
              owned(0x332,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            p.motion.velocityX = -12.0f;
            p.motion.velocityY = 0.0f;
            p.motion.y = double(owner.combat.screenBounds[2]);
            if (((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
                (i8(p.motion.facing) == '\x01')) ||
               ((double(owner.combat.screenBounds[1]) < double(p.motion.x) &&
                (i8(p.motion.facing) == -1)))) {
              p.frame(4);
              p.counter = i16(p.counter) + 1;
              p.response = 0;
              p.hits = 1;
            }
          }
          if (i16(p.animation.cursor.index) == 4) {
            if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = 90.0f;
              owned(0x332,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = -12.0f;
            if (i8(p.motion.facing) == -1) {
              p.motion.x = double(owner.combat.screenBounds[1]);
            }
            if (i8(p.motion.facing) == '\x01') {
              p.motion.x = double(owner.combat.screenBounds[0]);
            }
            if (double(p.motion.y) < double(0.0f)) {
              p.frame(1);
              p.motion.velocityX = 8.0f;
              p.motion.velocityY = 0.0f;
              p.motion.y = 0.0f;
              owner.launchParameters[0] = 0.0f;
              owned(0x332,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              p.counter = i16(p.counter) + 1;
              p.response = 0;
              p.hits = 1;
            }
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(4);
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
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x334:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.5f;
          p.pose.scale[1] = 1.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.polar(double(p.parameters[0]),12.0f);
          p.pose.rotation[2] = double(p.parameters[0]);
        }
        if (double(p.parameters[0]) == double(0.0f)) {
          p.motion.y = 0.0f;
        }
        if (double(p.parameters[0]) == -double(90.0f)) {
          if (i8(p.motion.facing) == '\x01') {
            p.motion.x = double(owner.combat.screenBounds[1]);
          }
          if (i8(p.motion.facing) == -1) {
            p.motion.x = double(owner.combat.screenBounds[0]);
          }
        }
        if (double(p.parameters[0]) == -double(180.0f)) {
          p.motion.y = double(owner.combat.screenBounds[2]);
        }
        if (double(p.parameters[0]) == double(90.0f)) {
          if (i8(p.motion.facing) == -1) {
            p.motion.x = double(owner.combat.screenBounds[1]);
          }
          if (i8(p.motion.facing) == '\x01') {
            p.motion.x = double(owner.combat.screenBounds[0]);
          }
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x335:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.counter = 0;
        }
        if ((i16(p.counter) < 6) &&
           ((i16(owner.animation.cursor.action) != 0x2c2 ||
            (i16(owner.animation.cursor.index) != 3)))) {
          if (i16(p.animation.cursor.index) == 1) {
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = 0.0f;
              owned(0x334,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            p.motion.velocityX = 12.0f;
            p.motion.y = 0.0f;
            if (((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
                (i8(p.motion.facing) == -1)) ||
               ((double(owner.combat.screenBounds[1]) < double(p.motion.x) &&
                (i8(p.motion.facing) == '\x01')))) {
              p.frame(2);
              p.counter = i16(p.counter) + 1;
              p.response = 0;
              p.hits = 1;
            }
          }
          if (i16(p.animation.cursor.index) == 2) {
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = -90.0f;
              owned(0x334,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 12.0f;
            if (i8(p.motion.facing) == '\x01') {
              p.motion.x = double(owner.combat.screenBounds[1]);
            }
            if (i8(p.motion.facing) == -1) {
              p.motion.x = double(owner.combat.screenBounds[0]);
            }
            if (double(owner.combat.screenBounds[2]) < double(p.motion.y)) {
              p.frame(3);
              p.counter = i16(p.counter) + 1;
              p.response = 0;
              p.hits = 1;
            }
          }
          if (i16(p.animation.cursor.index) == 3) {
            if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = -180.0f;
              owned(0x334,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            p.motion.velocityX = -12.0f;
            p.motion.velocityY = 0.0f;
            p.motion.y = double(owner.combat.screenBounds[2]);
            if (((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
                (i8(p.motion.facing) == '\x01')) ||
               ((double(owner.combat.screenBounds[1]) < double(p.motion.x) &&
                (i8(p.motion.facing) == -1)))) {
              p.frame(4);
              p.counter = i16(p.counter) + 1;
              p.response = 0;
              p.hits = 1;
            }
          }
          if (i16(p.animation.cursor.index) == 4) {
            if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = 90.0f;
              owned(0x334,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = -12.0f;
            if (i8(p.motion.facing) == -1) {
              p.motion.x = double(owner.combat.screenBounds[1]);
            }
            if (i8(p.motion.facing) == '\x01') {
              p.motion.x = double(owner.combat.screenBounds[0]);
            }
            if (double(p.motion.y) < double(0.0f)) {
              p.frame(1);
              p.motion.velocityX = 8.0f;
              p.motion.velocityY = 0.0f;
              p.motion.y = 0.0f;
              owner.launchParameters[0] = 0.0f;
              owned(0x334,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              p.counter = i16(p.counter) + 1;
              p.response = 0;
              p.hits = 1;
            }
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(4);
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
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x336:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.5f;
          p.pose.scale[1] = 1.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.polar(double(p.parameters[0]),16.0f);
          p.pose.rotation[2] = double(p.parameters[0]);
        }
        if (double(p.parameters[0]) == double(0.0f)) {
          p.motion.y = 0.0f;
        }
        if (double(p.parameters[0]) == -double(90.0f)) {
          if (i8(p.motion.facing) == '\x01') {
            p.motion.x = double(owner.combat.screenBounds[1]);
          }
          if (i8(p.motion.facing) == -1) {
            p.motion.x = double(owner.combat.screenBounds[0]);
          }
        }
        if (double(p.parameters[0]) == -double(180.0f)) {
          p.motion.y = double(owner.combat.screenBounds[2]);
        }
        if (double(p.parameters[0]) == double(90.0f)) {
          if (i8(p.motion.facing) == -1) {
            p.motion.x = double(owner.combat.screenBounds[1]);
          }
          if (i8(p.motion.facing) == '\x01') {
            p.motion.x = double(owner.combat.screenBounds[0]);
          }
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x337:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.counter = 0;
        }
        if ((i16(p.counter) < 6) &&
           ((i16(owner.animation.cursor.action) != 0x2c2 ||
            (i16(owner.animation.cursor.index) != 3)))) {
          if (i16(p.animation.cursor.index) == 1) {
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = 0.0f;
              owned(0x336,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            p.motion.velocityX = 16.0f;
            p.motion.y = 0.0f;
            if (((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
                (i8(p.motion.facing) == -1)) ||
               ((double(owner.combat.screenBounds[1]) < double(p.motion.x) &&
                (i8(p.motion.facing) == '\x01')))) {
              p.frame(2);
              p.counter = i16(p.counter) + 1;
              p.response = 0;
              p.hits = 1;
            }
          }
          if (i16(p.animation.cursor.index) == 2) {
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = -90.0f;
              owned(0x336,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 16.0f;
            if (i8(p.motion.facing) == '\x01') {
              p.motion.x = double(owner.combat.screenBounds[1]);
            }
            if (i8(p.motion.facing) == -1) {
              p.motion.x = double(owner.combat.screenBounds[0]);
            }
            if (double(owner.combat.screenBounds[2]) < double(p.motion.y)) {
              p.frame(3);
              p.counter = i16(p.counter) + 1;
              p.response = 0;
              p.hits = 1;
            }
          }
          if (i16(p.animation.cursor.index) == 3) {
            if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = -180.0f;
              owned(0x336,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            p.motion.velocityX = -16.0f;
            p.motion.velocityY = 0.0f;
            p.motion.y = double(owner.combat.screenBounds[2]);
            if (((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
                (i8(p.motion.facing) == '\x01')) ||
               ((double(owner.combat.screenBounds[1]) < double(p.motion.x) &&
                (i8(p.motion.facing) == -1)))) {
              p.frame(4);
              p.counter = i16(p.counter) + 1;
              p.response = 0;
              p.hits = 1;
            }
          }
          if (i16(p.animation.cursor.index) == 4) {
            if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = 90.0f;
              owned(0x336,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = -16.0f;
            if (i8(p.motion.facing) == -1) {
              p.motion.x = double(owner.combat.screenBounds[1]);
            }
            if (i8(p.motion.facing) == '\x01') {
              p.motion.x = double(owner.combat.screenBounds[0]);
            }
            if (double(p.motion.y) < double(0.0f)) {
              p.frame(1);
              p.motion.velocityX = 8.0f;
              p.motion.velocityY = 0.0f;
              p.motion.y = 0.0f;
              owner.launchParameters[0] = 0.0f;
              owned(0x336,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              p.counter = i16(p.counter) + 1;
              p.response = 0;
              p.hits = 1;
            }
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(4);
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
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x338:
        if ((i16(owner.animation.cursor.action) == 0x12d) ||
           (i16(owner.animation.cursor.action) == 0x187)) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x339:
        if (i16(owner.animation.cursor.action) == 0x141) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33a:
        if (i16(owner.animation.cursor.action) == 0x131) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33b:
        if ((i16(owner.animation.cursor.action) == 500) ||
           (i16(owner.animation.cursor.action) == 0x1f5)) {
          p.motion.x =
               (float)((int)i8(owner.motion.facing) << 3) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(135.0f);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.5f;
          p.pose.scale[1] = 1.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f);
        if ((i16(p.animation.cursor.index) == 0) &&
           ((((i16(owner.animation.cursor.action) != 500 &&
              (i16(owner.animation.cursor.action) != 0x1f5)) ||
             ((i16(owner.animation.cursor.action) == 500 &&
              (8 < i16(owner.animation.cursor.index))))) ||
            ((i16(owner.animation.cursor.action) == 0x1f5 &&
             (8 < i16(owner.animation.cursor.index))))))) {
          p.frame(1);
        }
        else {
          if (i16(p.animation.cursor.index) == 1) {
            if (u8(channel(24)) < 0x15) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x14));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x33c:
        if ((i16(owner.animation.cursor.index) < 0x14) &&
           (((i16(owner.animation.cursor.action) == 0x1f7 ||
             (i16(owner.animation.cursor.action) == 0x1f8)) ||
            (i16(owner.animation.cursor.action) == 0x1f9)))) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33d:
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((0x1f6 < i16(owner.animation.cursor.action)) &&
             ((i16(owner.animation.cursor.action) < 0x1fa &&
              (i16(owner.animation.cursor.index) < 7)))) {
            p.motion.x = double(owner.motion.x);
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[1] = 60.0f;
            p.hits = 0xff;
            p.response = 0;
          }
          if ((i16(p.animation.cursor.index) == 1) &&
             (p.pose.scale[1] = double(p.pose.scale[1]) + double(0.25f),
             double(1.0f) < double(p.pose.scale[1]))) {
            p.pose.scale[1] = 1.0f;
          }
          if (i16(p.animation.cursor.index) == 2) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.25f);
          }
          if ((i16(p.animation.cursor.index) == 3) &&
             (p.pose.scale[0] = double(p.pose.scale[0]) - double(0.25f),
             double(p.pose.scale[0]) < double(0.0f))) {
            p.pose.scale[0] = 0.0f;
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.pose.scale[0] = 0.05000000074505806f;
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x33e:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.0f;
          p.pose.scale[1] = 0.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 20.0f;
          p.pose.rotation[1] = (float)(i8(p.motion.facing) * -0x32);
          p.counter = 0;
          p.timer = 0;
          p.hits = 99;
          p.response = 0;
        }
        p.pose.rotation[2] = double(p.pose.rotation[2]) - double(5.0f);
        if (double(1.0f) <= double(p.pose.scale[0])) {
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          if (i16(p.animation.cursor.index) < 2) {
            p.frame(2);
            return;
          }
        }
        else {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
        }
        if (i16(p.animation.cursor.index) == 3) {
          if (u8(channel(24)) < 0x1f) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x1e));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33f:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.hits = 1;
          p.response = 0;
          p.motion.velocityX = 30.0f;
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.95f);
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x340:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.hits = 1;
          p.response = 0;
          p.motion.velocityY = 30.0f;
        }
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.95f);
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x341:
        if ((0x31 < i16(owner.animation.cursor.action)) &&
           (i16(owner.animation.cursor.action) < 0x96)) {
          p.counter = 0;
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.hits = 0xff;
          p.response = 0;
          p.counter = i16(state.particleDelay) + 3;
          state.particleDelay = 0;
        }
        if ((i16(p.animation.cursor.index) < 6) &&
           (p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f),
           double(1.5f) < double(p.pose.scale[0]))) {
          p.pose.scale[0] = 1.5f;
        }
        if (i16(p.counter) == 0) {
          if (double(p.pose.scale[0]) <= double(0.15f)) {
            p.destroy();
            return;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) - double(0.15f);
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.frame(6);
        }
        if (((6 < i16(p.animation.cursor.index)) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (0 < i16(p.counter))) {
          p.counter = i16(p.counter) + -1;
          events.sound(3);
          iVar5 = nextRandom(0,0x167);
          owner.launchParameters[0] = (float)iVar5;
          owner.launchParameters[1] = 10.0f;
          iVar5 = nextRandom(0,0x1e);
          owner.launchParameters[2] = (float)(iVar5 + -0xf);
          particleCoordinateOrLayer = 1;
          uVar4 = (u32)u8(p.motion.facing);
          fVar1 = double(p.motion.y);
          iVar5 = nextRandom(0,0x3c);
          owned(0x326,double(p.motion.x),double(float(double(fVar1)+30.0))-iVar5,uVar4,particleCoordinateOrLayer);
          p.frame(3);
        }
        break;
      case 0x342:
        if ((0x31 < i16(owner.animation.cursor.action)) &&
           (i16(owner.animation.cursor.action) < 0x96)) {
          p.counter = 0;
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = i16(state.particleDelay) * 2 + 6;
          state.particleDelay = 0;
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        else {
          if (((i16(p.animation.cursor.index) == 0x15) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (0 < i16(p.counter))) {
            p.frame(10);
          }
          if (((9 < i16(p.animation.cursor.index)) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (0 < i16(p.counter))) {
            p.counter = i16(p.counter) + -1;
            events.sound(3);
            owner.launchParameters[1] = 10.0f;
            iVar5 = nextRandom(0,0x1e);
            owner.launchParameters[0] = (float)(-0x4b - iVar5);
            uVar8 = 1;
            uVar4 = (u32)u8(p.motion.facing);
            particleCoordinateOrLayer = double(p.motion.y);
            fVar1 = double(p.motion.x);
            iVar5 = nextRandom(0,0x78);
            owned(0x343,double(float(double(fVar1)+60.0))-iVar5,particleCoordinateOrLayer,uVar4,uVar8);
          }
        }
        break;
      case 0x343:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            p.counter = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
          }
          p.counter = i16(p.counter) + 1;
          if ((i16(p.animation.cursor.index) < 2) &&
             (p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.1f) + double(p.parameters[1])), fVar1 = double(p.motion.y) + double(p.motion.velocityY),
             fVar1 < double(0.0f) != (fVar1 == double(0.0f)))) {
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.frame(2);
          }
          else if (((p.response == 1) ||
                   (((p.response == 2 || (p.response == 4)) ||
                    (p.response == 6)))) && (i16(p.animation.cursor.index) == 1)) {
            p.frame(2);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          else {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
               ((double(640.0f) < double(p.motion.y) || (double(p.motion.y) < -double(120.0f))))) {
              p.destroy();
            }
            else {
              if (1 < i16(p.animation.cursor.index)) {
                p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f);
                p.pose.scale[0] = double(p.pose.scale[0]) * double(0.9f);
                p.pose.scale[1] = double(p.pose.scale[1]) * double(0.9f);
              }
              sVar3 = p.advance();
              if (sVar3 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
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
          for (local_10 = 0; local_10 < 3; local_10 = local_10 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x344:
        if ((0x31 < i16(owner.animation.cursor.action)) &&
           (i16(owner.animation.cursor.action) < 0x96)) {
          p.counter = 0;
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = i16(state.particleDelay) + 3;
          state.particleDelay = 0;
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        else {
          if (((i16(p.animation.cursor.index) == 0x15) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (0 < i16(p.counter))) {
            p.frame(10);
          }
          if (((9 < i16(p.animation.cursor.index)) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (0 < i16(p.counter))) {
            p.counter = i16(p.counter) + -1;
            events.sound(3);
            owner.launchParameters[1] = 10.0f;
            iVar5 = nextRandom(0,0x1e);
            owner.launchParameters[0] = (float)(-0x4b - iVar5);
            uVar8 = 1;
            uVar4 = (u32)u8(p.motion.facing);
            particleCoordinateOrLayer = double(p.motion.y);
            fVar1 = double(p.motion.x);
            iVar5 = nextRandom(0,0x78);
            owned(0x345,double(float(double(fVar1)+60.0))-iVar5,particleCoordinateOrLayer,uVar4,uVar8);
          }
        }
        break;
      case 0x345:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.polar(-90.0f,10.0f);
          }
          if (i16(p.animation.cursor.index) == 0) {
            p.frame(1);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.hits = 1;
            p.response = 0;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 1) {
            p.polar(-90.0f,(float)(int)i16(p.counter) * double(0.1f));
            p.counter = i16(p.counter) + 10;
            owner.launchParameters[0] = -90.0f;
            owner.launchParameters[1] = 0.0f;
            owned(0x327,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            if (p.response != 0) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.frame(2);
              return;
            }
          }
          if (i16(p.animation.cursor.index) == 2) {
            p.counter = i16(p.counter) + -0x14;
            if (i16(p.counter) < 0) {
              p.counter = 0;
            }
            owner.launchParameters[0] = double(p.parameters[2]);
            owner.launchParameters[1] = (float)(int)i16(p.counter);
            owned(0x327,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.95f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.95f);
          }
          if (((double(1800.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(520.0f))) ||
             ((double(1400.0f) < double(p.motion.y) || (double(p.motion.y) < -double(520.0f))))) {
            p.destroy();
          }
          else {
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_14 = 0; local_14 < 3; local_14 = local_14 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x346:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 80.0f;
        }
        if (i16(p.animation.cursor.index) == 1) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
        }
        if (i16(p.animation.cursor.index) == 2) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.01f);
          if (0xf < u8(channel(24))) {
            setChannel(24,u8(i8(channel(24)) + -0xf));
          }
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
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
          p.pose.rotation[0] = 80.0f;
        }
        if (i16(p.animation.cursor.index) == 1) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
        }
        if (i16(p.animation.cursor.index) == 2) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.01f);
          if (0xf < u8(channel(24))) {
            setChannel(24,u8(i8(channel(24)) + -0xf));
          }
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x348:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 80.0f;
        }
        if (i16(p.animation.cursor.index) == 1) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
        }
        if (i16(p.animation.cursor.index) == 2) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.01f);
          if (0xf < u8(channel(24))) {
            setChannel(24,u8(i8(channel(24)) + -0xf));
          }
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x349:
        if (i16(owner.animation.cursor.action) == 0x135) {
          p.motion.x =
               (float)(i8(owner.motion.facing) * 0x9b) +
               double(owner.motion.x);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if (i16(p.animation.cursor.index) == 0) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
        }
        if (i16(p.animation.cursor.index) == 1) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.01f);
          if (0xf < u8(channel(24))) {
            setChannel(24,u8(i8(channel(24)) + -0xf));
          }
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x34a:
        if (i16(owner.animation.cursor.action) == 0x135) {
          p.motion.x =
               (float)(i8(owner.motion.facing) * 0x9b) +
               double(owner.motion.x);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 80.0f;
        }
        if (i16(p.animation.cursor.index) == 1) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
        }
        if (i16(p.animation.cursor.index) == 2) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.01f);
          if (0xf < u8(channel(24))) {
            setChannel(24,u8(i8(channel(24)) + -0xf));
          }
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x34b:
        if (i16(owner.animation.cursor.action) == 0x135) {
          p.motion.x =
               (float)(i8(owner.motion.facing) * 0x9b) +
               double(owner.motion.x);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 80.0f;
        }
        if (i16(p.animation.cursor.index) == 1) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
        }
        if (i16(p.animation.cursor.index) == 2) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.01f);
          if (0xf < u8(channel(24))) {
            setChannel(24,u8(i8(channel(24)) + -0xf));
          }
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x34c:
        if (i16(owner.animation.cursor.action) == 0x12f) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 900:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 0x14;
            p.response = 0;
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 2.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.polar((float)(int)i16(p.timer),30.0f);
            p.response = 0;
            p.hits = 1;
          }
          if (i16(p.animation.cursor.index) == 1) {
            if (double(p.pose.scale[0]) < double(1.0f)) {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.25f);
            }
            if (double(1.0f) < double(p.pose.scale[1])) {
              p.pose.scale[1] = double(p.pose.scale[1]) - double(0.25f);
            }
          }
          if (i16(p.animation.cursor.index) == 3) {
            p.pose.rotation[2] = double(p.pose.rotation[2]) - double(30.0f);
            p.polar((float)(int)i16(p.timer),30.0f);
          }
          if (i16(p.animation.cursor.index) == 4) {
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.pose.rotation[2] = double(p.pose.rotation[2]) - double(30.0f);
          }
          if (i16(p.animation.cursor.index) == 5) {
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.pose.rotation[2] = double(p.pose.rotation[2]) - double(30.0f);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(3);
            p.response = 0;
            p.hits = 1;
          }
          else {
            if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
              if ((int)i16(p.angle) < 3 - state.difficulty) {
                p.frame(4);
                p.angle = i16(p.angle) + 1;
                return;
              }
              p.angle = 0;
            }
            if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
              events.sound(0xf);
              (aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                           double(p.motion.y),(double(other.motion.x) -
                           double(p.motion.x)) * (float)(int)i8(p.motion.facing)));
              uVar2 = i16(i32(aimRadians/double(0.017453289f)));
              p.timer = uVar2;
              owner.launchParameters[0] = (float)(int)i16(p.timer);
              owned(0x387,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
          }
        }
        break;
      case 0x385:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.counter = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
          }
          p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.1f) + double(p.parameters[1]));
          if ((p.response == 0) || (i16(p.animation.cursor.index) != 1)) {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if ((double(1380.0f) < double(p.motion.x)) ||
               (((double(p.motion.x) < -double(100.0f) || (double(640.0f) < double(p.motion.y))) ||
                (double(p.motion.y) < -double(120.0f))))) {
              p.destroy();
            }
            else {
              if (1 < i16(p.animation.cursor.index)) {
                p.motion.velocityX = 0.0f;
                p.motion.velocityY = 0.0f;
                p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
                p.pose.scale[1] = double(p.pose.scale[0]);
              }
              sVar3 = p.advance();
              if (sVar3 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
          else {
            p.frame(2);
          }
        }
        break;
      case 0x386:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.counter = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.1f) + double(p.parameters[1]));
            fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
          }
          if ((p.response == 0) || (i16(p.animation.cursor.index) != 1)) {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if ((double(1380.0f) < double(p.motion.x)) ||
               (((double(p.motion.x) < -double(100.0f) || (double(640.0f) < double(p.motion.y))) ||
                (double(p.motion.y) < -double(120.0f))))) {
              p.destroy();
            }
            else {
              if (1 < i16(p.animation.cursor.index)) {
                if (u8(channel(24)) < 0x15) {
                  p.destroy();
                  return;
                }
                setChannel(24,u8(i8(channel(24)) + -0x14));
              }
              sVar3 = p.advance();
              if (sVar3 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
          else {
            p.frame(2);
          }
        }
        break;
      case 0x387:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 7.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[2] = -double(p.parameters[0]);
          }
          p.pose.scale[1] = double(p.pose.scale[1]) * 0.95;
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x388:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 0x14;
            p.response = 0;
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 2.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.counter = 0;
            p.timer = 0x2d;
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.polar((float)(int)i16(p.timer),30.0f);
            p.response = 0;
            p.hits = 1;
          }
          if (i16(p.animation.cursor.index) == 1) {
            if (double(p.pose.scale[0]) < double(1.0f)) {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.25f);
            }
            if (double(1.0f) < double(p.pose.scale[1])) {
              p.pose.scale[1] = double(p.pose.scale[1]) - double(0.25f);
            }
          }
          if (i16(p.animation.cursor.index) == 3) {
            p.pose.rotation[2] = double(p.pose.rotation[2]) - double(30.0f);
            p.polar((float)(int)i16(p.timer),30.0f);
          }
          if (i16(p.animation.cursor.index) == 4) {
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.pose.rotation[2] = double(p.pose.rotation[2]) - double(30.0f);
          }
          if (i16(p.animation.cursor.index) == 5) {
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.pose.rotation[2] = double(p.pose.rotation[2]) - double(30.0f);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(3);
            p.response = 0;
            p.hits = 1;
          }
          else {
            if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
              if ((int)i16(p.angle) < 3 - state.difficulty) {
                p.frame(4);
                p.angle = i16(p.angle) + 1;
                return;
              }
              p.angle = 0;
            }
            if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
              events.sound(0xf);
              (aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                           double(p.motion.y),(double(other.motion.x) -
                           double(p.motion.x)) * (float)(int)i8(p.motion.facing)));
              nextRandom(0,0x1e);
              uVar2 = i16(i32(double(float(aimRadians/double(0.017453289f)))+lastRoll-double(15.0f)));
              p.timer = uVar2;
              owner.launchParameters[0] = (float)(int)i16(p.timer);
              owned(0x391,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
          }
        }
        break;
      case 0x389:
        if ((i16(owner.animation.cursor.action) == 0x2e5) &&
           (i16(owner.animation.cursor.index) < 0xc)) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
        }
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((i16(owner.animation.cursor.action) != 0x2e5) ||
             ((i16(owner.animation.cursor.action) == 0x2e5 &&
              (0xb < i16(owner.animation.cursor.index))))) {
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(2.0f);
          if (double(p.pose.scale[0]) < double(1.0f)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
            if (double(1.0f) < double(p.pose.scale[0])) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
            }
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(1);
            owner.launchParameters[0] = double(p.pose.rotation[2]);
            owned(0x38d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
        }
        break;
      case 0x38a:
        if ((i16(owner.animation.cursor.action) == 0x2e5) &&
           (i16(owner.animation.cursor.index) < 0xc)) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
        }
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((i16(owner.animation.cursor.action) != 0x2e5) ||
             ((i16(owner.animation.cursor.action) == 0x2e5 &&
              (0xb < i16(owner.animation.cursor.index))))) {
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) - double(2.0f);
          if (double(p.pose.scale[0]) < double(1.0f)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
            if (double(1.0f) < double(p.pose.scale[0])) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
            }
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(1);
            owner.launchParameters[0] = double(p.pose.rotation[2]);
            owned(0x38d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
        }
        break;
      case 0x38b:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.polar(double(p.parameters[0]),7.0f);
            fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
            p.response = 0;
            p.hits = 1;
          }
          if ((i16(p.animation.cursor.index) == 1) && (p.response != 0)) {
            p.frame(2);
          }
          else {
            if (i16(p.animation.cursor.index) == 2) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
            }
            if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
                (double(960.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(100.0f))) {
              p.destroy();
            }
            else {
              p.motion.x =
                   (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                   double(p.motion.x);
              p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
              sVar3 = p.advance();
              if (sVar3 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
        }
        break;
      case 0x38c:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            uVar2 = nextRandom(0,0x167);
            p.timer = uVar2;
            p.polar((float)(int)i16(p.timer),20.0f);
            p.response = 0;
            p.counter = 0;
            p.angle = 0;
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            iVar5 = nextRandom(0,100);
            if (iVar5 < 0x32) {
              p.polar((float)(i16(p.timer) + 0x5a),40.0f);
            }
            else {
              p.polar((float)(i16(p.timer) + -0x5a),40.0f);
            }
            events.sound(0x10);
            fVar6 = (double)(aimRadians = atan((double(other.motion.y) + double(100.0f)) - double(p.motion.y),(double(other.motion.x) - double(p.motion.x)) *
                                          (float)(int)i8(p.motion.facing)));
            owner.launchParameters[0] = (float)(fVar6 / (double)double(0.017453289f));
            for (local_18 = 0; local_18 < state.difficulty * 2 + 3; local_18 = local_18 + 1) {
              if (i16(p.angle) == 0) {
                owned(0x38b,(float)local_18 * double(p.motion.velocityX) +
                                   double(p.motion.x),(float)local_18 * double(p.motion.velocityY) +
                             double(p.motion.y),u8(p.motion.facing),1);
                p.angle = 1;
              }
              if (i16(p.angle) == 1) {
                owned(0x38e,(float)local_18 * double(p.motion.velocityX) +
                                   double(p.motion.x),(float)local_18 * double(p.motion.velocityY) +
                             double(p.motion.y),u8(p.motion.facing),1);
                p.angle = 0;
              }
            }
          }
          if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
              (double(960.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(100.0f))) {
            p.destroy();
          }
          else {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
        }
        break;
      case 0x38d:
        if ((i16(owner.animation.cursor.action) == 0x2e5) &&
           (i16(owner.animation.cursor.index) < 0xc)) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
        }
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[2] = double(p.parameters[0]);
          }
          if ((i16(owner.animation.cursor.action) != 0x2e5) ||
             ((i16(owner.animation.cursor.action) == 0x2e5 &&
              (0xb < i16(owner.animation.cursor.index))))) {
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.05f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.05f);
          if (u8(channel(24)) < 0xb) {
            p.destroy();
          }
          else {
            setChannel(24,u8(i8(channel(24)) + -10));
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(1);
            }
          }
        }
        break;
      case 0x38e:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.polar(double(p.parameters[0]),7.0f);
            fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
            p.response = 0;
            p.hits = 1;
          }
          if ((i16(p.animation.cursor.index) == 1) && (p.response != 0)) {
            p.frame(2);
          }
          else {
            if (i16(p.animation.cursor.index) == 2) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
            }
            if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
                (double(960.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(100.0f))) {
              p.destroy();
            }
            else {
              p.motion.x =
                   (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                   double(p.motion.x);
              p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
              sVar3 = p.advance();
              if (sVar3 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
        }
        break;
      case 0x38f:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f);
          if (i16(p.animation.cursor.index) == 0) {
            if ((i16(owner.animation.cursor.action) == 0x2dc) &&
               (6 < i16(owner.animation.cursor.index))) {
              p.frame(1);
              p.motion.velocityX =
                   (float)(int)i8(owner.motion.facing) * double(8.0f);
              p.motion.velocityY = -20.0f;
              p.hits = 0xf;
              return;
            }
            if ((i16(owner.animation.cursor.action) == 0x2db) ||
               ((i16(owner.animation.cursor.action) == 0x2dc &&
                (i16(owner.animation.cursor.index) < 7)))) {
              p.motion.x =
                   (float)((int)i8(owner.motion.facing) << 3) +
                   double(owner.motion.x);
              p.motion.y = double(owner.motion.y) + double(135.0f);
            }
            if ((((i16(owner.animation.cursor.action) != 0x2db) &&
                 (i16(owner.animation.cursor.action) != 0x2dc)) ||
                ((i16(owner.animation.cursor.action) == 500 &&
                 (8 < i16(owner.animation.cursor.index))))) ||
               ((i16(owner.animation.cursor.action) == 0x2db &&
                (9 < i16(owner.animation.cursor.index))))) {
              p.frame(2);
              return;
            }
          }
          if (i16(p.animation.cursor.index) == 1) {
            fVar1 = double(p.motion.y) + double(p.motion.velocityY);
            if (fVar1 < double(0.0f) != (fVar1 == double(0.0f))) {
              p.motion.y = 0.0f;
              p.motion.velocityY = 0.0f;
            }
            if ((((double(1680.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(400.0f))) ||
                (double(p.motion.y) < -double(500.0f))) || (double(1280.0f) < double(p.motion.y))) {
              p.destroy();
              return;
            }
          }
          if (i16(p.animation.cursor.index) == 2) {
            if (u8(channel(24)) < 0x15) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x14));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(1);
            p.response = 0;
          }
        }
        break;
      case 0x390:
        if (i16(owner.animation.cursor.action) == 0x2dd) {
          p.motion.x =
               (float)((int)i8(owner.motion.facing) << 3) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(135.0f);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.5f;
          p.pose.scale[1] = 1.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f);
        if ((i16(p.animation.cursor.index) == 0) &&
           ((i16(owner.animation.cursor.action) != 0x2dd ||
            ((i16(owner.animation.cursor.action) == 0x2dd &&
             (8 < i16(owner.animation.cursor.index))))))) {
          p.frame(1);
        }
        else {
          if (i16(p.animation.cursor.index) == 1) {
            if (u8(channel(24)) < 0x15) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x14));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x391:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 7.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[2] = -double(p.parameters[0]);
          }
          p.pose.scale[1] = double(p.pose.scale[1]) * 0.95;
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x392:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 7.0f;
            p.pose.scale[1] = 2.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.05f);
          p.pose.scale[1] = double(p.pose.scale[1]) * 0.95;
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
            events.sound(0x17);
            if (state.difficulty == '\0') {
              owned(0x393,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            else {
              owned(0x394,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
          }
        }
        break;
      case 0x393:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            events.shake(5.0f);
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 4.0f;
            p.pose.scale[1] = 2.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.hits = 1;
            p.response = 0;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.5f);
          p.pose.scale[1] = double(p.pose.scale[1]) * 0.95;
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x394:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            events.shake(5.0f);
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 4.0f;
            p.pose.scale[1] = 2.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.hits = 1;
            p.response = 0;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.5f);
          p.pose.scale[1] = double(p.pose.scale[1]) * 0.95;
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x395:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.timer = 1;
          p.angle = 0;
          if (double(0.0f) < double(other.motion.y)) {
            p.angle = i16(p.angle) + 1;
          }
          if (double(250.0f) < double(other.motion.y)) {
            p.angle = i16(p.angle) + 1;
          }
          if (double(400.0f) < double(other.motion.y)) {
            p.angle = i16(p.angle) + 1;
          }
          sVar3 = nextRandom(0,5);
          p.cycle = sVar3 + -1;
          if ((i16(p.cycle) == i16(p.angle)) &&
             (p.cycle = i16(p.cycle) + -1,
             i16(p.cycle) < 0)) {
            p.cycle = 3;
          }
          sVar3 = nextRandom(0,5);
          p.variant = sVar3 + -1;
          if ((i16(p.variant) == i16(p.angle)) &&
             (p.variant = i16(p.variant) + -1,
             i16(p.variant) < 0)) {
            p.variant = 3;
          }
          if ((i16(p.variant) == i16(p.cycle)) &&
             (p.variant = i16(p.variant) + -1,
             i16(p.variant) < 0)) {
            p.variant = 3;
          }
        }
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if (i16(owner.combat.health) < 0xdac) {
            p.counter = i16(p.counter) + 1;
            if ((state.difficulty < '\x02') && (0x8c < i16(p.counter))) {
              events.sound(0xf);
              owned(0x392,(float)((u32)(i16(p.timer) == -1) * 0x500),(float)(i16(p.angle) * 0x96 + 0x32),u8(p.timer),1);
              owned(0x392,(float)((u32)(i16(p.timer) == -1) * 0x500),(float)(i16(p.cycle) * 0x96 + 0x32),u8(p.timer),1);
              p.counter = 0;
              p.timer = -i16(p.timer);
            }
            if ((state.difficulty == '\x02') && (0x78 < i16(p.counter))) {
              events.sound(0xf);
              sVar3 = i16(p.timer);
              p.timer = (i16)-(int)sVar3;
              owned(0x392,(float)((u32)(i16(p.timer) == -1) * 0x500),(float)(i16(p.angle) * 0x96 + 0x32),u8(p.timer),1);
              owned(0x392,(float)((u32)(i16(p.timer) == -1) * 0x500),(float)(i16(p.cycle) * 0x96 + 0x32),u8(p.timer),1);
              owned(0x392,(float)((u32)(i16(p.timer) == -1) * 0x500),(float)(i16(p.variant) * 0x96 + 0x32),u8(p.timer),1);
              p.counter = 0;
            }
            if ((state.difficulty == '\x03') && (0x5a < i16(p.counter))) {
              events.sound(0xf);
              sVar3 = i16(p.timer);
              p.timer = (i16)-(int)sVar3;
              owned(0x392,(float)((u32)(i16(p.timer) == -1) * 0x500),(float)(i16(p.angle) * 0x96 + 0x32),u8(p.timer),1);
              owned(0x392,(float)((u32)(i16(p.timer) == -1) * 0x500),(float)(i16(p.cycle) * 0x96 + 0x32),u8(p.timer),1);
              owned(0x392,(float)((u32)(i16(p.timer) == -1) * 0x500),(float)(i16(p.variant) * 0x96 + 0x32),u8(p.timer),1);
              p.counter = 0;
            }
            p.angle = 0;
            if (double(0.0f) < double(other.motion.y)) {
              p.angle = i16(p.angle) + 1;
            }
            if (double(250.0f) < double(other.motion.y)) {
              p.angle = i16(p.angle) + 1;
            }
            if (double(400.0f) < double(other.motion.y)) {
              p.angle = i16(p.angle) + 1;
            }
            sVar3 = nextRandom(0,5);
            p.cycle = sVar3 + -1;
            if ((i16(p.cycle) == i16(p.angle)) &&
               (p.cycle = i16(p.cycle) + -1,
               i16(p.cycle) < 0)) {
              p.cycle = 3;
            }
            sVar3 = nextRandom(0,5);
            p.variant = sVar3 + -1;
            if ((i16(p.variant) == i16(p.angle)) &&
               (p.variant = i16(p.variant) + -1,
               i16(p.variant) < 0)) {
              p.variant = 3;
            }
            if ((i16(p.variant) == i16(p.cycle)) &&
               (p.variant = i16(p.variant) + -1,
               i16(p.variant) < 0)) {
              p.variant = 3;
            }
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(1);
          }
        }
        break;
      case 0x396:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          p.motion.facing = 1;
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.cycle = 0;
            p.variant = 0;
          }
          p.variant = i16(p.variant) + -1;
          p.polar((float)(int)i16(state.orbitAngle) +
                       double(p.parameters[0]),(float)(int)i16(state.portalCount));
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               (float)(int)i16(state.lastPortalX);
          p.motion.y =
               (float)(int)i16(state.lastPortalY) + double(p.motion.velocityY)
          ;
          owned(0x397,double(p.motion.x),double(p.motion.y),1,1);
          if (i16(p.variant) < 1) {
            events.sound(0x12);
            owner.launchParameters[0] =
                 -((float)(int)i16(state.orbitAngle) +
                  double(p.parameters[0])) * (float)(int)i8(p.motion.facing);
            owner.launchParameters[1] = 5.0f;
            owned(0x399,double(p.motion.x),double(p.motion.y),1,1);
            p.variant = 4;
          }
          if (i16(state.portalAngle) == 2) {
            events.sound(0x10);
            for (local_1c = 0; local_1c < state.difficulty * 2 + 4; local_1c = local_1c + 1) {
              owner.launchParameters[0] =
                   -((float)(int)i16(state.orbitAngle) +
                    double(p.parameters[0])) * (float)(int)i8(p.motion.facing);
              owner.launchParameters[1] = 4.0f;
              owned(0x39a,double(p.motion.x),double(p.motion.y),1,1);
            }
          }
          if (i16(state.portalAngle) == 3) {
            events.sound(0x12);
            owner.launchParameters[0] =
                 -((float)(int)i16(state.orbitAngle) +
                  double(p.parameters[0])) + double(180.0f);
            owner.launchParameters[1] = 5.0f;
            owned(0x39b,double(p.motion.x),double(p.motion.y),1,1);
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(1);
          }
        }
        break;
      case 0x397:
        p.motion.facing = 1;
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if (u8(channel(24)) < 0xe) {
          setChannel(24,u8(0));
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -0xd));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x398:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          p.motion.facing = 1;
          if (i16(owner.motion.hitFreeze) == 0) {
            uVar2 = i16(i32(p.motion.x));
            state.lastPortalX = uVar2;
            uVar2 = i16(i32(p.motion.y));
            state.lastPortalY = uVar2;
          }
          if (0x154 < i16(state.portalCount)) {
            state.portalCount =
                 i16(state.portalCount) + -1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.counter = 0;
            p.timer = 0;
            p.angle = 0x3c;
            p.cycle = 0;
            p.variant = 0xb4;
          }
          p.variant = i16(p.variant) + -1;
          p.angle = i16(p.angle) + -1;
          p.timer = i16(p.timer) + 1;
          if (i16(p.angle) == 1) {
            state.portalAngle = 2;
          }
          if (i16(p.angle) == 0) {
            p.angle = i16(p.angle) + -1;
            p.angle = 0x104;
            state.portalAngle = 0;
          }
          if (i16(p.timer) == 300) {
            state.portalAngle = 3;
          }
          if (i16(p.timer) == 0x12d) {
            p.timer = state.difficulty * 0x14 + -0x3c;
            state.portalAngle = 0;
          }
          if (0 < i16(p.cycle)) {
            p.cycle = i16(p.cycle) + -1;
            state.portalAngle = 1;
            if (i16(p.cycle) == 0) {
              state.portalAngle = 0;
            }
          }
          p.motion.velocityX =
               (double(other.motion.x) -
               double(p.motion.x)) * double(0.02f);
          if ((double(p.motion.velocityX) < double(0.25f)) && (-double(0.25f) < double(p.motion.velocityX))) {
            p.motion.velocityX = 0.0f;
          }
          p.motion.velocityY =
               ((double(other.motion.y) + double(100.0f)) -
               double(p.motion.y)) * double(0.02f);
          if ((double(p.motion.velocityY) < double(0.25f)) && (-double(0.25f) < double(p.motion.velocityY))) {
            p.motion.velocityY = 0.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(1);
          }
        }
        break;
      case 0x399:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
            p.response = 0;
            p.hits = 1;
          }
          if ((i16(p.animation.cursor.index) == 1) && (p.response != 0)) {
            p.frame(2);
          }
          else {
            if (i16(p.animation.cursor.index) == 2) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.8f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.8f);
            }
            if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
                (double(960.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(100.0f))) {
              p.destroy();
            }
            else {
              p.motion.x =
                   (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                   double(p.motion.x);
              p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
              sVar3 = p.advance();
              if (sVar3 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
        }
        break;
      case 0x39a:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 3.0f;
            p.pose.scale[1] = 3.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            uVar2 = nextRandom(0,0x167);
            p.timer = uVar2;
            p.polar((float)(int)i16(p.timer),(float)(int)i16(p.counter) * double(0.05f));
            fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
            p.response = 0;
            p.hits = 1;
          }
          if (i16(p.animation.cursor.index) == 1) {
            p.pose.scale[0] = double(p.pose.scale[0]) - double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) - double(0.1f);
            if (double(p.pose.scale[0]) < double(1.0f)) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
            }
          }
          if ((i16(p.animation.cursor.index) == 2) && (p.response != 0)) {
            p.frame(3);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          else {
            if (i16(p.animation.cursor.index) == 3) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.8f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.8f);
            }
            if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
                (double(960.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(100.0f))) {
              p.destroy();
            }
            else {
              if (i16(p.animation.cursor.index) == 2) {
                p.counter = i16(p.counter) + 1;
                p.motion.x =
                     (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                     double(p.motion.x);
                p.motion.y = double(p.motion.y) - double(p.motion.velocityY)
                ;
                p.polar((float)(int)i16(p.timer),(float)(int)i16(p.counter) * double(0.05f));
              }
              sVar3 = p.advance();
              if (sVar3 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
        }
        break;
      case 0x39b:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 3.0f;
            p.pose.scale[1] = 3.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.response = 0;
            p.hits = 1;
          }
          if (i16(p.animation.cursor.index) == 1) {
            p.pose.scale[0] = double(p.pose.scale[0]) - double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) - double(0.1f);
            if (double(p.pose.scale[0]) < double(1.0f)) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
            }
            if (u8(channel(24)) < 0xf5) {
              setChannel(24,u8(i8(channel(24)) + '\n'));
            }
            else {
              setChannel(24,u8(0xff));
            }
          }
          if ((i16(p.animation.cursor.index) == 2) && (p.response != 0)) {
            p.frame(3);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          else {
            if (i16(p.animation.cursor.index) == 3) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.8f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.8f);
            }
            if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
                (double(960.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(100.0f))) {
              p.destroy();
            }
            else {
              if (i16(p.animation.cursor.index) == 2) {
                p.counter = i16(p.counter) + 1;
                p.polar(double(p.parameters[0]),double(p.parameters[1]));
                p.motion.x =
                     (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                     double(p.motion.x);
                p.motion.y = double(p.motion.y) - double(p.motion.velocityY)
                ;
              }
              sVar3 = p.advance();
              if (sVar3 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
        }
        break;
      case 0x39c:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f);
          if (i16(p.animation.cursor.index) == 0) {
            if ((i16(owner.animation.cursor.action) == 0x2dc) &&
               (6 < i16(owner.animation.cursor.index))) {
              p.frame(1);
              p.motion.velocityX =
                   (float)(int)i8(owner.motion.facing) * double(8.0f);
              p.motion.velocityY = -20.0f;
              p.hits = 0xf;
              return;
            }
            if ((i16(owner.animation.cursor.action) == 0x2db) ||
               ((i16(owner.animation.cursor.action) == 0x2dc &&
                (i16(owner.animation.cursor.index) < 7)))) {
              p.motion.x =
                   (float)((int)i8(owner.motion.facing) << 3) +
                   double(owner.motion.x);
              p.motion.y = double(owner.motion.y) + double(135.0f);
            }
            if (((i16(owner.animation.cursor.action) != 0x2db) &&
                (i16(owner.animation.cursor.action) != 0x2dc)) ||
               ((i16(owner.animation.cursor.action) == 500 &&
                (8 < i16(owner.animation.cursor.index))))) {
              p.frame(2);
              return;
            }
          }
          if (i16(p.animation.cursor.index) == 1) {
            fVar1 = double(p.motion.y) + double(p.motion.velocityY);
            if (fVar1 < double(0.0f) != (fVar1 == double(0.0f))) {
              p.motion.y = 0.0f;
              p.motion.velocityY = 0.0f;
            }
            if ((((double(1680.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(400.0f))) ||
                (double(p.motion.y) < -double(500.0f))) || (double(1280.0f) < double(p.motion.y))) {
              p.destroy();
              return;
            }
          }
          if (i16(p.animation.cursor.index) == 2) {
            if (u8(channel(24)) < 0x15) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x14));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(1);
            p.response = 0;
          }
        }
        break;
      case 0x39d:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f);
          if (i16(p.animation.cursor.index) == 0) {
            if ((i16(owner.animation.cursor.action) == 0x2dc) &&
               (6 < i16(owner.animation.cursor.index))) {
              p.frame(1);
              p.motion.velocityX =
                   (float)(int)i8(owner.motion.facing) * double(10.0f);
              p.motion.velocityY = -20.0f;
              p.hits = 0xf;
              return;
            }
            if ((i16(owner.animation.cursor.action) == 0x2db) ||
               ((i16(owner.animation.cursor.action) == 0x2dc &&
                (i16(owner.animation.cursor.index) < 7)))) {
              p.motion.x =
                   (float)((int)i8(owner.motion.facing) << 3) +
                   double(owner.motion.x);
              p.motion.y = double(owner.motion.y) + double(135.0f);
            }
            if (((i16(owner.animation.cursor.action) != 0x2db) &&
                (i16(owner.animation.cursor.action) != 0x2dc)) ||
               ((i16(owner.animation.cursor.action) == 500 &&
                (8 < i16(owner.animation.cursor.index))))) {
              p.frame(2);
              return;
            }
          }
          if (i16(p.animation.cursor.index) == 1) {
            fVar1 = double(p.motion.y) + double(p.motion.velocityY);
            if (fVar1 < double(0.0f) != (fVar1 == double(0.0f))) {
              p.motion.y = 0.0f;
              p.motion.velocityY = 0.0f;
            }
            if ((((double(1680.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(400.0f))) ||
                (double(p.motion.y) < -double(500.0f))) || (double(1280.0f) < double(p.motion.y))) {
              p.destroy();
              return;
            }
          }
          if (i16(p.animation.cursor.index) == 2) {
            if (u8(channel(24)) < 0x15) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x14));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(1);
            p.response = 0;
          }
        }
        break;
      case 0x3dc:
        if (10 < i16(owner.animation.cursor.index)) {
          p.motion.x = double(owner.motion.x);
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x3dd:
        if (i16(owner.animation.cursor.index) < 9) {
          p.motion.x = double(owner.motion.x);
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
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
          sVar3 = nextRandom(0,0x168);
          p.counter = sVar3 + -0xb4;
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
          (aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                       double(p.motion.y),double(other.motion.x) -
                       double(p.motion.x)));
          uVar2 = i16(i32(aimRadians/double(0.017453289f)));
          p.counter = uVar2;
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
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
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
          sVar3 = nextRandom(0,0x168);
          p.counter = sVar3 + -0xb4;
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
          (aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                       double(p.motion.y),double(other.motion.x) -
                       double(p.motion.x)));
          uVar2 = i16(i32(aimRadians/double(0.017453289f)));
          p.counter = uVar2;
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
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
        }
        break;
      case 0x3e5:
        p.motion.x = double(owner.motion.x);
        p.motion.y = 2.0f;
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
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
          p.pose.rotation[0] = -80.0f;
          p.counter = owner.combat.spellTimer;
          p.timer = 0;
        }
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          fVar1 = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[0] = fVar1 + fVar1;
          fVar1 = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[1] = fVar1 + fVar1;
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(9);
          }
        }
        break;
      case 999:
        if (i16(p.animation.cursor.index) < 1) {
          p.motion.y = double(owner.motion.y);
        }
        p.motion.x = double(owner.motion.x);
        state.portalTimer = 1;
        if ((i16(owner.animation.cursor.index) == 0) &&
           ((((0x45 < i16(owner.animation.cursor.action) &&
              (i16(owner.animation.cursor.action) < 0x59)) &&
             (i16(owner.animation.cursor.action) != 0x4c)) ||
            (((i16(owner.animation.cursor.action) == 0xb5 ||
              (i16(owner.animation.cursor.action) == 0xb4)) ||
             (i16(owner.animation.cursor.action) == 0x91)))))) {
          p.destroy();
          state.portalTimer = 0;
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(60.0f,130.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((i16(p.animation.cursor.index) == 0) &&
             (((i16(owner.animation.cursor.action) == 0x60 ||
               (i16(owner.animation.cursor.action) == 0x61)) &&
              (i16(owner.animation.cursor.index) == 5)))) {
            p.frame(1);
            p.pose.rotation[2] = 0.0f;
          }
          else if ((i16(p.animation.cursor.index) == 1) &&
                  ((((i16(owner.animation.cursor.action) == 0x5f ||
                     (i16(owner.animation.cursor.action) == 0x5e)) ||
                    ((i16(owner.animation.cursor.action) == 0x5d ||
                     (i16(owner.animation.cursor.action) == 0x31c)))) &&
                   (i16(owner.animation.cursor.index) == 3)))) {
            p.frame(2);
          }
          else {
            if (i16(p.animation.cursor.index) == 0) {
              p.pose.rotation[2] = double(p.pose.rotation[2]) - double(35.0f);
            }
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
              state.portalTimer = 0;
            }
          }
        }
      }
    }
    else {
      p.freeze = i16(p.freeze) + -1;
    }
  }
  return;

}}
