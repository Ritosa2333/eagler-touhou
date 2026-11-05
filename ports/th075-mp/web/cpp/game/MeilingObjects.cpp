#include "MeilingObjects.hpp"
#include <cmath>
namespace th075 {void advance_meiling_object(MeilingObject& p,FighterActor& owner,FighterActor& other,MeilingState& state,ReplayStreams& random,FighterActionEvents& events){
i32 lastRoll=0;const auto nextRandom=[&](i32 low,i32 high){return lastRoll=random.range(low,high);};
auto& sceneFadeColor=events.fade_color();auto& sceneFadeAlpha=events.fade_alpha();
double aimRadians=0;const auto atan=[](float y,float x){return std::atan2(double(y),double(x));};
const auto channel=[&](u32 shift){return p.pose.channel(shift);};const auto setChannel=[&](u32 shift,u8 value){p.pose.set_channel(shift,value);};
const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters);};
const auto shared=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Shared,id,x,y,facing,layer,owner.launchParameters);};

  u16 uVar1;
  i16 sVar2;
  u32 uVar3;
  int iVar4;
  double fVar5;
  float fVar6;
  float fVar7;
  u32 uVar8;
  
  if ((i16(other.combat.opponentStop) == 0) &&
     ((i16(owner.combat.opponentStop) == 0 || (i16(p.contactDelay) != 0)))) {
    if (i16(p.freeze) == 0) {
      switch(p.animation.cursor.action) {
      case 800:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (7 < i16(p.animation.cursor.index))) {
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
            p.counter = 10;
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        else {
          p.destroy();
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x321:
        if ((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 0.5f;
            p.pose.scale[1] = 0.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[1] = (float)(i8(p.motion.facing) * 0x3c);
            p.pose.rotation[2] = double(p.parameters[0]);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.counter = 0xf;
            p.response = 0;
          }
          p.pose.rotation[2] =
               (float)(int)i16(p.counter) + double(p.pose.rotation[2]);
          p.counter = i16(p.counter) + -1;
          if (i16(p.counter) < 1) {
            p.counter = 1;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.02f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.02f);
          if (i16(p.animation.cursor.index) == 2) {
            if (u8(channel(24)) < 6) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -5));
            setChannel(16,u8(i8(channel(16)) + -5));
            setChannel(8,u8(i8(channel(8)) + -5));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x322:
        if ((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 0.5f;
            p.pose.scale[1] = 0.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[1] = (float)(i8(p.motion.facing) * 0x46);
            iVar4 = nextRandom(0,0x167);
            p.pose.rotation[2] = (float)iVar4;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.response = 0;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.03f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.03f);
          if (i16(p.animation.cursor.index) == 1) {
            if (u8(channel(24)) < 6) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -5));
            setChannel(16,u8(i8(channel(16)) + -5));
            setChannel(8,u8(i8(channel(8)) + -5));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x323:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 3;
            p.response = 0;
            p.pose.scale[0] = 0.10000000149011612f;
            p.pose.scale[1] = 0.10000000149011612f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0));
            setChannel(8,u8(0));
            setChannel(0,u8(0xff));
            p.counter = 0;
            owner.launchParameters[0] = 0.0f;
            owned(0x321,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            owner.launchParameters[0] = 180.0f;
            owned(0x321,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            owned(0x322,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          if (i16(p.animation.cursor.index) == 1) {
            if (double(p.pose.scale[0]) < double(1.0f)) {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
            }
            if (u8(channel(8)) < 0xe6) {
              setChannel(8,u8(i8(channel(8)) + '\x19'));
              setChannel(16,u8(i8(channel(16)) + '\x19'));
            }
            else {
              setChannel(24,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(16,u8(0xff));
            }
          }
          if (2 < i16(p.animation.cursor.index)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.3f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
            if (u8(channel(24)) < 0x19) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x19));
            setChannel(8,u8(i8(channel(8)) + -0x19));
            setChannel(16,u8(i8(channel(16)) + -0x19));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if (((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) &&
             ((i16(owner.animation.cursor.action) == 600 &&
              (i16(owner.animation.cursor.index) < 8)))) {
            p.frame(2);
            p.counter = i16(p.counter) + 1;
            iVar4 = nextRandom(0,0x14);
            p.pose.scale[0] = double(1.1f) - (float)iVar4 * double(0.01f);
            p.pose.scale[1] = double(p.pose.scale[0]);
            if (4 < i16(p.counter)) {
              p.counter = 0;
              p.response = 0;
            }
          }
        }
        else {
          p.destroy();
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x324:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 10;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 15.0f;
            p.motion.velocityY = 0.0f;
            p.counter = 0;
            p.timer = 0;
          }
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          if (p.response == 4) {
            p.response = 0;
          }
          p.motion.velocityX = double(p.motion.velocityX) - double(0.4f);
          if (double(p.motion.velocityX) < double(2.0f) != (double(p.motion.velocityX) == double(2.0f))) {
            p.motion.velocityX = 2.0f;
          }
          if (1 < i16(p.animation.cursor.index)) {
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.3f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
            if (u8(channel(24)) < 0x19) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x19));
            setChannel(8,u8(i8(channel(8)) + -0x19));
            setChannel(16,u8(i8(channel(16)) + -0x19));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if (((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) &&
             ((0 < i16(p.hits) && (i16(p.timer) < 0x14)))) {
            p.frame(1);
            p.counter = i16(p.counter) + 1;
            p.timer = i16(p.timer) + 1;
            if (2 < i16(p.counter)) {
              owned(0x325,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              p.counter = 0;
            }
            iVar4 = nextRandom(0,0x14);
            p.pose.scale[0] = double(1.1f) - (float)iVar4 * double(0.01f);
            p.pose.scale[1] = double(p.pose.scale[0]);
            if (p.response != 0) {
              p.response = 0;
              p.hits = i16(p.hits) + -3;
            }
          }
        }
        else {
          p.destroy();
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x325:
        if ((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 0.5f;
            p.pose.scale[1] = 0.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            iVar4 = nextRandom(0,0x167);
            p.pose.rotation[0] = (float)iVar4;
            iVar4 = nextRandom(0,0x167);
            p.pose.rotation[1] = (float)iVar4;
            iVar4 = nextRandom(0,0x167);
            p.pose.rotation[2] = (float)iVar4;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.response = 0;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.085f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.085f);
          if (i16(p.animation.cursor.index) == 1) {
            if (u8(channel(24)) < 0x10) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0xf));
            setChannel(16,u8(i8(channel(16)) + -0xf));
            setChannel(8,u8(i8(channel(8)) + -0xf));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x326:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 2.5f;
          p.pose.scale[1] = 2.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if ((i16(owner.animation.cursor.action) == 700) &&
           (i16(owner.animation.cursor.index) < 0xb)) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x327:
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0));
          setChannel(16,u8(0));
          setChannel(8,u8(0));
          setChannel(0,u8(0xff));
          p.pose.rotation[1] = (float)(i8(p.motion.facing) * 0x32);
          p.counter = 1;
        }
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
        p.pose.scale[0] = double(p.pose.scale[0]) - double(0.01f);
        p.pose.scale[1] = double(p.pose.scale[1]) - double(0.01f);
        if (i16(p.animation.cursor.index) == 1) {
          if (u8(channel(24)) < 0xf8) {
            setChannel(24,u8(i8(channel(24)) + '\a'));
            setChannel(8,u8(i8(channel(8)) + '\a'));
            setChannel(16,u8(i8(channel(16)) + '\a'));
          }
          else {
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
          }
        }
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
          setChannel(8,u8(i8(channel(8)) + -10));
          setChannel(16,u8(i8(channel(16)) + -10));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x328:
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.2000000476837158f;
          p.pose.scale[1] = 1.2000000476837158f;
          setChannel(24,u8(0));
          setChannel(16,u8(0));
          setChannel(8,u8(0));
          setChannel(0,u8(0xff));
          p.pose.rotation[1] = (float)(i8(p.motion.facing) * 0x32);
          p.counter = 1;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) - double(0.018f);
        p.pose.scale[1] = double(p.pose.scale[1]) - double(0.018f);
        if (i16(p.animation.cursor.index) == 1) {
          if (u8(channel(24)) < 0xf8) {
            setChannel(24,u8(i8(channel(24)) + '\a'));
            setChannel(8,u8(i8(channel(8)) + '\a'));
            setChannel(16,u8(i8(channel(16)) + '\a'));
          }
          else {
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
          }
        }
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 9) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -8));
          setChannel(8,u8(i8(channel(8)) + -8));
          setChannel(16,u8(i8(channel(16)) + -8));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x329:
        if ((double(1580.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(300.0f))) {
          p.destroy();
        }
        else if ((i16(owner.animation.cursor.action) == 0x2be) ||
                (1 < i16(p.animation.cursor.index))) {
          if ((0 < i16(p.contactDelay)) &&
             (i16(owner.motion.hitFreeze) == 0)) {
            p.contactDelay = 0;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.10000000149011612f;
            p.pose.scale[1] = 0.10000000149011612f;
            setChannel(24,u8(0));
            setChannel(16,u8(0));
            setChannel(8,u8(0));
            setChannel(0,u8(0xff));
            p.hits = 0x14;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.counter = 0;
          }
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.response = 0;
            p.counter = i16(p.counter) + 1;
            iVar4 = nextRandom(0,0x14);
            p.pose.scale[0] = double(1.0f) - (float)iVar4 * double(0.01f);
            p.pose.scale[1] = double(p.pose.scale[0]);
            if (1 < i16(p.counter)) {
              owned(0x32a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              p.counter = 0;
            }
          }
          if (i16(p.animation.cursor.index) == 1) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.03f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.03f);
            if (u8(channel(24)) < 0xeb) {
              setChannel(24,u8(i8(channel(24)) + '\x14'));
              setChannel(8,u8(i8(channel(8)) + '\x14'));
              setChannel(16,u8(i8(channel(16)) + '\x14'));
            }
            else {
              setChannel(24,u8(0xff));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
            }
            if (i16(owner.animation.cursor.index) == 0xf) {
              iVar4 = nextRandom(0,0x14);
              p.pose.scale[0] = double(1.0f) - (float)iVar4 * double(0.01f);
              p.pose.scale[1] = double(p.pose.scale[0]);
              p.frame(2);
            }
          }
          if ((i16(p.animation.cursor.index) == 2) &&
             (p.motion.velocityX = double(p.motion.velocityX) - double(0.75f),
             double(p.motion.velocityX) < double(10.0f))) {
            p.motion.velocityX = 10.0f;
          }
          if (2 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              return;
            }
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.95f);
            p.pose.scale[1] = double(p.pose.scale[0]);
            setChannel(24,u8(i8(channel(24)) + -10));
            setChannel(8,u8(i8(channel(8)) + -10));
            setChannel(16,u8(i8(channel(16)) + -10));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.motion.velocityX = 18.5f;
            p.pose.scale[0] = 0.6000000238418579f;
            p.pose.scale[1] = 0.6000000238418579f;
          }
          if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(2);
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x32a:
        if ((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 0.5f;
            p.pose.scale[1] = 0.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            iVar4 = nextRandom(0,0x167);
            p.pose.rotation[0] = (float)iVar4;
            iVar4 = nextRandom(0,0x167);
            p.pose.rotation[1] = (float)iVar4;
            iVar4 = nextRandom(0,0x167);
            p.pose.rotation[2] = (float)iVar4;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.response = 0;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.065f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.065f);
          if (i16(p.animation.cursor.index) == 1) {
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
            setChannel(16,u8(i8(channel(16)) + -10));
            setChannel(8,u8(i8(channel(8)) + -10));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x32b:
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
            p.angle = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar5 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          if ((i16(p.animation.cursor.index) < 3) && (iVar4 = nextRandom(0,100), iVar4 < 0x28)) {
            uVar8 = 1;
            uVar3 = (u32)u8(p.motion.facing);
            fVar6 = double(p.motion.y);
            iVar4 = nextRandom(0,0x28);
            fVar7 = double(float(double(fVar6)+20.0))-iVar4;
            fVar6 = double(p.motion.x);
            iVar4 = nextRandom(0,0x28);
            owned(0x332,double(float(double(fVar6)+20.0))-iVar4,fVar7,uVar3,uVar8);
          }
          p.counter = i16(p.counter) + 1;
          if (i16(p.animation.cursor.index) < 2) {
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
          }
          if (((((p.response == 1) || (p.response == 2)) ||
               (p.response == 4)) || (p.response == 6)) &&
             (i16(p.animation.cursor.index) == 1)) {
            p.frame(2);
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
              if (i16(p.animation.cursor.index) == 2) {
                if (u8(channel(24)) < 0x3c) {
                  setChannel(24,u8(0));
                  p.destroy();
                  return;
                }
                setChannel(24,u8(i8(channel(24)) + -0x3c));
              }
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x32c:
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
            p.angle = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar5 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          if ((i16(p.animation.cursor.index) < 3) && (iVar4 = nextRandom(0,100), iVar4 < 0x28)) {
            uVar8 = 1;
            uVar3 = (u32)u8(p.motion.facing);
            fVar6 = double(p.motion.y);
            iVar4 = nextRandom(0,0x28);
            fVar7 = double(float(double(fVar6)+20.0))-iVar4;
            fVar6 = double(p.motion.x);
            iVar4 = nextRandom(0,0x28);
            owned(0x333,double(float(double(fVar6)+20.0))-iVar4,fVar7,uVar3,uVar8);
          }
          p.counter = i16(p.counter) + 1;
          if (i16(p.animation.cursor.index) < 2) {
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
          }
          if (((((p.response == 1) || (p.response == 2)) ||
               (p.response == 4)) || (p.response == 6)) &&
             (i16(p.animation.cursor.index) == 1)) {
            p.frame(2);
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
              if (i16(p.animation.cursor.index) == 2) {
                if (u8(channel(24)) < 0x3c) {
                  setChannel(24,u8(0));
                  p.destroy();
                  return;
                }
                setChannel(24,u8(i8(channel(24)) + -0x3c));
              }
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x32d:
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
            p.angle = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar5 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          if ((i16(p.animation.cursor.index) < 3) && (iVar4 = nextRandom(0,100), iVar4 < 0x28)) {
            uVar8 = 1;
            uVar3 = (u32)u8(p.motion.facing);
            fVar6 = double(p.motion.y);
            iVar4 = nextRandom(0,0x28);
            fVar7 = double(float(double(fVar6)+20.0))-iVar4;
            fVar6 = double(p.motion.x);
            iVar4 = nextRandom(0,0x28);
            owned(0x334,double(float(double(fVar6)+20.0))-iVar4,fVar7,uVar3,uVar8);
          }
          p.counter = i16(p.counter) + 1;
          if (i16(p.animation.cursor.index) < 2) {
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
          }
          if (((((p.response == 1) || (p.response == 2)) ||
               (p.response == 4)) || (p.response == 6)) &&
             (i16(p.animation.cursor.index) == 1)) {
            p.frame(2);
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
              if (i16(p.animation.cursor.index) == 2) {
                if (u8(channel(24)) < 0x3c) {
                  setChannel(24,u8(0));
                  p.destroy();
                  return;
                }
                setChannel(24,u8(i8(channel(24)) + -0x3c));
              }
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x32e:
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
            p.angle = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar5 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          if ((i16(p.animation.cursor.index) < 3) && (iVar4 = nextRandom(0,100), iVar4 < 0x28)) {
            uVar8 = 1;
            uVar3 = (u32)u8(p.motion.facing);
            fVar6 = double(p.motion.y);
            iVar4 = nextRandom(0,0x28);
            fVar7 = double(float(double(fVar6)+20.0))-iVar4;
            fVar6 = double(p.motion.x);
            iVar4 = nextRandom(0,0x28);
            owned(0x335,double(float(double(fVar6)+20.0))-iVar4,fVar7,uVar3,uVar8);
          }
          p.counter = i16(p.counter) + 1;
          if (i16(p.animation.cursor.index) < 2) {
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
          }
          if (((((p.response == 1) || (p.response == 2)) ||
               (p.response == 4)) || (p.response == 6)) &&
             (i16(p.animation.cursor.index) == 1)) {
            p.frame(2);
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
              if (i16(p.animation.cursor.index) == 2) {
                if (u8(channel(24)) < 0x3c) {
                  setChannel(24,u8(0));
                  p.destroy();
                  return;
                }
                setChannel(24,u8(i8(channel(24)) + -0x3c));
              }
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x32f:
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
            p.angle = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar5 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          if ((i16(p.animation.cursor.index) < 3) && (iVar4 = nextRandom(0,100), iVar4 < 0x28)) {
            uVar8 = 1;
            uVar3 = (u32)u8(p.motion.facing);
            fVar6 = double(p.motion.y);
            iVar4 = nextRandom(0,0x28);
            fVar7 = double(float(double(fVar6)+20.0))-iVar4;
            fVar6 = double(p.motion.x);
            iVar4 = nextRandom(0,0x28);
            owned(0x336,double(float(double(fVar6)+20.0))-iVar4,fVar7,uVar3,uVar8);
          }
          p.counter = i16(p.counter) + 1;
          if (i16(p.animation.cursor.index) < 2) {
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
          }
          if (((((p.response == 1) || (p.response == 2)) ||
               (p.response == 4)) || (p.response == 6)) &&
             (i16(p.animation.cursor.index) == 1)) {
            p.frame(2);
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
              if (i16(p.animation.cursor.index) == 2) {
                if (u8(channel(24)) < 0x3c) {
                  setChannel(24,u8(0));
                  p.destroy();
                  return;
                }
                setChannel(24,u8(i8(channel(24)) + -0x3c));
              }
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x330:
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
            p.angle = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar5 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          if ((i16(p.animation.cursor.index) < 3) && (iVar4 = nextRandom(0,100), iVar4 < 0x28)) {
            uVar8 = 1;
            uVar3 = (u32)u8(p.motion.facing);
            fVar6 = double(p.motion.y);
            iVar4 = nextRandom(0,0x28);
            fVar7 = double(float(double(fVar6)+20.0))-iVar4;
            fVar6 = double(p.motion.x);
            iVar4 = nextRandom(0,0x28);
            owned(0x337,double(float(double(fVar6)+20.0))-iVar4,fVar7,uVar3,uVar8);
          }
          p.counter = i16(p.counter) + 1;
          if (i16(p.animation.cursor.index) < 2) {
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
          }
          if (((((p.response == 1) || (p.response == 2)) ||
               (p.response == 4)) || (p.response == 6)) &&
             (i16(p.animation.cursor.index) == 1)) {
            p.frame(2);
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
              if (i16(p.animation.cursor.index) == 2) {
                if (u8(channel(24)) < 0x3c) {
                  setChannel(24,u8(0));
                  p.destroy();
                  return;
                }
                setChannel(24,u8(i8(channel(24)) + -0x3c));
              }
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x331:
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
            p.angle = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar5 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          if ((i16(p.animation.cursor.index) < 3) && (iVar4 = nextRandom(0,100), iVar4 < 0x28)) {
            uVar8 = 1;
            uVar3 = (u32)u8(p.motion.facing);
            fVar6 = double(p.motion.y);
            iVar4 = nextRandom(0,0x28);
            fVar7 = double(float(double(fVar6)+20.0))-iVar4;
            fVar6 = double(p.motion.x);
            iVar4 = nextRandom(0,0x28);
            owned(0x338,double(float(double(fVar6)+20.0))-iVar4,fVar7,uVar3,uVar8);
          }
          p.counter = i16(p.counter) + 1;
          if (i16(p.animation.cursor.index) < 2) {
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
          }
          if (((((p.response == 1) || (p.response == 2)) ||
               (p.response == 4)) || (p.response == 6)) &&
             (i16(p.animation.cursor.index) == 1)) {
            p.frame(2);
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
              if (i16(p.animation.cursor.index) == 2) {
                if (u8(channel(24)) < 0x3c) {
                  setChannel(24,u8(0));
                  p.destroy();
                  return;
                }
                setChannel(24,u8(i8(channel(24)) + -0x3c));
              }
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x332:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          iVar4 = nextRandom(0,0x14);
          p.pose.scale[0] = double(1.1f) - (float)iVar4 * double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          iVar4 = nextRandom(0,0x167);
          p.pose.rotation[2] = (float)iVar4;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = -2.0f;
        }
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(2.0f);
        if (0 < i16(p.animation.cursor.index)) {
          if (double(0.05f) < double(p.pose.scale[0])) {
            p.pose.scale[0] = double(p.pose.scale[0]) - double(0.05f);
            p.pose.scale[1] = double(p.pose.scale[1]) - double(0.05f);
          }
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x333:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          iVar4 = nextRandom(0,0x14);
          p.pose.scale[0] = double(1.1f) - (float)iVar4 * double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          iVar4 = nextRandom(0,0x167);
          p.pose.rotation[2] = (float)iVar4;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = -2.0f;
        }
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(2.0f);
        if (0 < i16(p.animation.cursor.index)) {
          if (double(0.05f) < double(p.pose.scale[0])) {
            p.pose.scale[0] = double(p.pose.scale[0]) - double(0.05f);
            p.pose.scale[1] = double(p.pose.scale[1]) - double(0.05f);
          }
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x334:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          iVar4 = nextRandom(0,0x14);
          p.pose.scale[0] = double(1.1f) - (float)iVar4 * double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          iVar4 = nextRandom(0,0x167);
          p.pose.rotation[2] = (float)iVar4;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = -2.0f;
        }
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(2.0f);
        if (0 < i16(p.animation.cursor.index)) {
          if (double(0.05f) < double(p.pose.scale[0])) {
            p.pose.scale[0] = double(p.pose.scale[0]) - double(0.05f);
            p.pose.scale[1] = double(p.pose.scale[1]) - double(0.05f);
          }
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x335:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          iVar4 = nextRandom(0,0x14);
          p.pose.scale[0] = double(1.1f) - (float)iVar4 * double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          iVar4 = nextRandom(0,0x167);
          p.pose.rotation[2] = (float)iVar4;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = -2.0f;
        }
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(2.0f);
        if (0 < i16(p.animation.cursor.index)) {
          if (double(0.05f) < double(p.pose.scale[0])) {
            p.pose.scale[0] = double(p.pose.scale[0]) - double(0.05f);
            p.pose.scale[1] = double(p.pose.scale[1]) - double(0.05f);
          }
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x336:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          iVar4 = nextRandom(0,0x14);
          p.pose.scale[0] = double(1.1f) - (float)iVar4 * double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          iVar4 = nextRandom(0,0x167);
          p.pose.rotation[2] = (float)iVar4;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = -2.0f;
        }
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(2.0f);
        if (0 < i16(p.animation.cursor.index)) {
          if (double(0.05f) < double(p.pose.scale[0])) {
            p.pose.scale[0] = double(p.pose.scale[0]) - double(0.05f);
            p.pose.scale[1] = double(p.pose.scale[1]) - double(0.05f);
          }
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x337:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          iVar4 = nextRandom(0,0x14);
          p.pose.scale[0] = double(1.1f) - (float)iVar4 * double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          iVar4 = nextRandom(0,0x167);
          p.pose.rotation[2] = (float)iVar4;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = -2.0f;
        }
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(2.0f);
        if (0 < i16(p.animation.cursor.index)) {
          if (double(0.05f) < double(p.pose.scale[0])) {
            p.pose.scale[0] = double(p.pose.scale[0]) - double(0.05f);
            p.pose.scale[1] = double(p.pose.scale[1]) - double(0.05f);
          }
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x338:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          iVar4 = nextRandom(0,0x14);
          p.pose.scale[0] = double(1.1f) - (float)iVar4 * double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          iVar4 = nextRandom(0,0x167);
          p.pose.rotation[2] = (float)iVar4;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = -2.0f;
        }
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(2.0f);
        if (0 < i16(p.animation.cursor.index)) {
          if (double(0.05f) < double(p.pose.scale[0])) {
            p.pose.scale[0] = double(p.pose.scale[0]) - double(0.05f);
            p.pose.scale[1] = double(p.pose.scale[1]) - double(0.05f);
          }
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x339:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (0x19 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 10;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.timer = 2000;
            p.polar(0.0f,(float)(int)i16(p.timer) * double(0.01f));
            p.hits = 5;
            p.counter = 0;
          }
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          if (p.response != 0) {
            if (p.response == 4) {
              p.response = 0;
            }
            else {
              p.hits = 0;
            }
          }
          if ((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 0x1a)) {
            p.frame(0x1a);
          }
          else {
            p.timer = i16(p.timer) + -100;
            if (i16(p.timer) < 300) {
              p.timer = 300;
            }
            p.polar(0.0f,(float)(int)i16(p.timer) * double(0.01f));
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if (((i16(p.animation.cursor.index) == 0x1a) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.counter) < 2)) {
              p.counter = i16(p.counter) + 1;
              p.frame(0xd);
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
        }
        break;
      case 0x33a:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (0x19 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.timer = 2000;
            p.polar(0.0f,(float)(int)i16(p.timer) * double(0.01f));
            p.hits = 5;
            p.counter = 0;
          }
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          if (p.response != 0) {
            if (p.response == 4) {
              p.response = 0;
            }
            else {
              p.hits = 0;
            }
          }
          if ((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 0x1a)) {
            p.frame(0x1a);
          }
          else {
            p.timer = i16(p.timer) + -100;
            if (i16(p.timer) < 300) {
              p.timer = 300;
            }
            p.polar(0.0f,(float)(int)i16(p.timer) * double(0.01f));
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if (((i16(p.animation.cursor.index) == 0x1a) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.counter) < 3)) {
              p.counter = i16(p.counter) + 1;
              p.frame(0xd);
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
        }
        break;
      case 0x33b:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (0x19 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.timer = 2000;
            p.polar(-30.0f,(float)(int)i16(p.timer) * double(0.01f));
            p.hits = 5;
            p.counter = 0;
          }
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          if (p.response != 0) {
            if (p.response == 4) {
              p.response = 0;
            }
            else {
              p.hits = 0;
            }
          }
          if ((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 0x1a)) {
            p.frame(0x1a);
          }
          else {
            p.timer = i16(p.timer) + -100;
            if (i16(p.timer) < 300) {
              p.timer = 300;
            }
            p.polar(-30.0f,(float)(int)i16(p.timer) * double(0.01f));
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if (((i16(p.animation.cursor.index) == 0x1a) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.counter) < 3)) {
              p.counter = i16(p.counter) + 1;
              p.frame(0xd);
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
        }
        break;
      case 0x33c:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = (float)(i8(p.motion.facing) * -0x23);
          p.pose.rotation[1] = 40.0f;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.response = 0;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.065f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.065f);
        if (i16(p.animation.cursor.index) == 2) {
          if (u8(channel(24)) < 0x10) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0xf));
          setChannel(0,u8(i8(channel(0)) + -0xf));
          setChannel(8,u8(i8(channel(8)) + -0xf));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33d:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = (float)(i8(p.motion.facing) * -0x23);
          p.pose.rotation[1] = 40.0f;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.response = 0;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.065f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.065f);
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(2.0f);
        if (i16(p.animation.cursor.index) == 2) {
          if (u8(channel(24)) < 0x10) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0xf));
          setChannel(0,u8(i8(channel(0)) + -0xf));
          setChannel(8,u8(i8(channel(8)) + -0xf));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33e:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = (float)(i8(p.motion.facing) * -0x23);
          p.pose.rotation[1] = 40.0f;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.response = 0;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
        if (i16(p.animation.cursor.index) == 2) {
          if (u8(channel(24)) < 0x15) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x14));
          setChannel(0,u8(i8(channel(0)) + -0x14));
          setChannel(8,u8(i8(channel(8)) + -0x14));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33f:
        if ((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.counter = 10;
            p.response = 0;
          }
          p.pose.scale[0] =
               (float)(int)i16(p.counter) * double(0.01f) + double(0.01f) + double(p.pose.scale[0]);
          p.pose.scale[1] =
               (float)(int)i16(p.counter) * double(0.01f) + double(0.01f) + double(p.pose.scale[1]);
          if (0 < i16(p.counter)) {
            p.counter = i16(p.counter) + -1;
          }
          if (i16(p.animation.cursor.index) == 2) {
            if (u8(channel(24)) < 6) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0xf));
            setChannel(16,u8(i8(channel(16)) + -0xf));
            setChannel(0,u8(i8(channel(0)) + -0xf));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x340,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            owned(0x341,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            owned(0x342,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
        }
        break;
      case 0x340:
        if ((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.hits = 1;
            p.response = 0;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) * double(0.9f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.05f);
          if (p.response == 6) {
            p.hits = i16(p.hits) + 1;
            p.response = 0;
          }
          if (i16(p.animation.cursor.index) == 2) {
            if (u8(channel(24)) < 6) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0xf));
            setChannel(16,u8(i8(channel(16)) + -0xf));
            setChannel(0,u8(i8(channel(0)) + -0xf));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x341:
        if ((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.hits = 1;
            p.response = 0;
          }
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.05f);
          if (p.response == 6) {
            p.hits = i16(p.hits) + 1;
            p.response = 0;
          }
          if (i16(p.animation.cursor.index) == 2) {
            if (u8(channel(24)) < 6) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0xf));
            setChannel(16,u8(i8(channel(16)) + -0xf));
            setChannel(0,u8(i8(channel(0)) + -0xf));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x342:
        if ((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.hits = 1;
            p.response = 0;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
          if (i16(p.animation.cursor.index) == 2) {
            if (u8(channel(24)) < 6) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0xf));
            setChannel(16,u8(i8(channel(16)) + -0xf));
            setChannel(0,u8(i8(channel(0)) + -0xf));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x343:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = (float)(i8(p.motion.facing) * -0x50);
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.counter = 10;
          p.response = 0;
          p.hits = 1;
        }
        p.pose.scale[0] =
             (float)(int)i16(p.counter) * 0.03 + double(0.02f) + double(p.pose.scale[0]);
        p.pose.scale[1] =
             (float)(int)i16(p.counter) * 0.03 + double(0.02f) + double(p.pose.scale[1]);
        if (0 < i16(p.counter)) {
          p.counter = i16(p.counter) + -1;
        }
        if (i16(p.animation.cursor.index) == 0x15) {
          if (u8(channel(24)) < 0x10) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0xf));
          setChannel(0,u8(i8(channel(0)) + -0xf));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x344:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 0.6499999761581421f;
          p.pose.scale[1] = 0.6499999761581421f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = (float)(i8(p.motion.facing) * -0x50);
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.counter = 10;
          p.response = 0;
          p.hits = 1;
        }
        p.pose.scale[0] =
             (float)(int)i16(p.counter) * 0.03 + double(0.02f) + double(p.pose.scale[0]);
        p.pose.scale[1] =
             (float)(int)i16(p.counter) * 0.03 + double(0.02f) + double(p.pose.scale[1]);
        if (0 < i16(p.counter)) {
          p.counter = i16(p.counter) + -1;
        }
        if (i16(p.animation.cursor.index) == 0x15) {
          if (u8(channel(24)) < 0x10) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0xf));
          setChannel(0,u8(i8(channel(0)) + -0xf));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x345:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = (float)(i8(p.motion.facing) * -0x3c);
          p.pose.rotation[1] = 35.0f;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.counter = 10;
          p.response = 0;
        }
        p.pose.scale[0] =
             (float)(int)i16(p.counter) * double(0.05f) + double(0.05f) + double(p.pose.scale[0]);
        p.pose.scale[1] =
             (float)(int)i16(p.counter) * double(0.05f) + double(0.05f) + double(p.pose.scale[1]);
        if (0 < i16(p.counter)) {
          p.counter = i16(p.counter) + -1;
        }
        if (i16(p.animation.cursor.index) == 0x14) {
          if (u8(channel(24)) < 0x10) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0xf));
          setChannel(0,u8(i8(channel(0)) + -0xf));
          setChannel(8,u8(i8(channel(8)) + -0xf));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x346:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.response = 0;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.02f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.02f);
        if (i16(p.animation.cursor.index) == 1) {
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
          setChannel(0,u8(i8(channel(0)) + -10));
          setChannel(16,u8(i8(channel(16)) + -10));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x347:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.response = 0;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.05f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.05f);
        if (i16(p.animation.cursor.index) == 1) {
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
          setChannel(0,u8(i8(channel(0)) + -10));
          setChannel(16,u8(i8(channel(16)) + -10));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x348:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 2.5f;
          p.pose.scale[1] = 3.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if ((i16(owner.animation.cursor.action) == 0x2bd) &&
           (i16(owner.animation.cursor.index) < 0xb)) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x349:
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0));
          setChannel(16,u8(0));
          setChannel(8,u8(0));
          setChannel(0,u8(0xff));
          p.pose.rotation[1] = (float)(i8(p.motion.facing) * 0x32);
          p.counter = 1;
        }
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
        p.pose.scale[0] = double(p.pose.scale[0]) - double(0.01f);
        p.pose.scale[1] = double(p.pose.scale[1]) - double(0.01f);
        if (i16(p.animation.cursor.index) == 1) {
          if (u8(channel(24)) < 0xf8) {
            setChannel(24,u8(i8(channel(24)) + '\a'));
            setChannel(8,u8(i8(channel(8)) + '\a'));
            setChannel(16,u8(i8(channel(16)) + '\a'));
          }
          else {
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
          }
        }
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
          setChannel(8,u8(i8(channel(8)) + -10));
          setChannel(16,u8(i8(channel(16)) + -10));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x34a:
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.2000000476837158f;
          p.pose.scale[1] = 1.2000000476837158f;
          setChannel(24,u8(0));
          setChannel(16,u8(0));
          setChannel(8,u8(0));
          setChannel(0,u8(0xff));
          p.pose.rotation[1] = (float)(i8(p.motion.facing) * 0x32);
          p.counter = 1;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) - double(0.018f);
        p.pose.scale[1] = double(p.pose.scale[1]) - double(0.018f);
        if (i16(p.animation.cursor.index) == 1) {
          if (u8(channel(24)) < 0xf8) {
            setChannel(24,u8(i8(channel(24)) + '\a'));
            setChannel(8,u8(i8(channel(8)) + '\a'));
            setChannel(16,u8(i8(channel(16)) + '\a'));
          }
          else {
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
          }
        }
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 9) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -8));
          setChannel(8,u8(i8(channel(8)) + -8));
          setChannel(16,u8(i8(channel(16)) + -8));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x34b:
        if ((double(1580.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(300.0f))) {
          p.destroy();
        }
        else {
          if ((0 < i16(p.contactDelay)) &&
             (i16(owner.motion.hitFreeze) == 0)) {
            p.contactDelay = 0;
          }
          if ((i16(p.animation.cursor.index) < 3) &&
             (i16(owner.animation.cursor.action) != 0x2bf)) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.pose.scale[0] = 0.10000000149011612f;
              p.pose.scale[1] = 0.10000000149011612f;
              setChannel(24,u8(0));
              setChannel(16,u8(0));
              setChannel(8,u8(0));
              setChannel(0,u8(0xff));
              p.hits = 0x14;
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.counter = 0;
            }
            if (i16(p.animation.cursor.index) < 3) {
              iVar4 = nextRandom(0,0x168);
              p.pose.rotation[2] = (float)iVar4;
            }
            if (p.response == 6) {
              p.response = 0;
              p.hits = i16(p.hits) + 1;
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.response = 0;
              iVar4 = nextRandom(0,10);
              p.pose.scale[0] = double(1.0f) - (float)iVar4 * double(0.01f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.response = 0;
              iVar4 = nextRandom(0,0x14);
              p.pose.scale[0] = double(1.0f) - (float)iVar4 * double(0.01f);
              p.pose.scale[1] = double(p.pose.scale[0]);
              p.counter = i16(p.counter) + 1;
              if (0 < i16(p.counter)) {
                owned(0x34c,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
                p.counter = 0;
              }
            }
            if (i16(p.animation.cursor.index) == 1) {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.03f);
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.03f);
              if (u8(channel(24)) < 0xeb) {
                setChannel(24,u8(i8(channel(24)) + '\x14'));
                setChannel(8,u8(i8(channel(8)) + '\x14'));
                setChannel(16,u8(i8(channel(16)) + '\x14'));
              }
              else {
                setChannel(24,u8(0xff));
                setChannel(16,u8(0xff));
                setChannel(8,u8(0xff));
              }
            }
            if ((i16(p.animation.cursor.index) < 3) &&
               (i16(owner.animation.cursor.action) == 0x2bf)) {
              p.motion.x =
                   (float)(i8(owner.motion.facing) * 100) +
                   double(owner.motion.x);
              p.motion.y = double(owner.motion.y) + double(110.0f);
            }
            if (((i16(p.animation.cursor.index) == 2) &&
                (i16(owner.animation.cursor.action) == 0x2bf)) &&
               (0x1c < i16(owner.animation.cursor.index))) {
              p.frame(3);
              p.motion.velocityX = 20.0f;
              p.pose.rotation[2] = 0.0f;
              p.hits = 0x14;
              p.response = 0;
            }
            if (i16(p.animation.cursor.index) == 3) {
              uVar8 = 1;
              uVar3 = (u32)u8(p.motion.facing);
              fVar6 = double(p.motion.y);
              iVar4 = nextRandom(0,300);
              fVar7 = double(float(double(fVar6)+150.0))-iVar4;
              fVar6 = double(p.motion.x);
              iVar4 = nextRandom(0,300);
              fVar6 = double(float(double(fVar6)+150.0))-iVar4;
              iVar4 = nextRandom(0,7);
              owned(iVar4 + 0x332,fVar6,fVar7,uVar3,uVar8);
            }
            if (3 < i16(p.animation.cursor.index)) {
              if (u8(channel(24)) < 0x1f) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x1e));
              setChannel(8,u8(i8(channel(8)) + -0x1e));
              setChannel(16,u8(i8(channel(16)) + -0x1e));
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(2);
              p.response = 0;
            }
            if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(3);
              p.response = 0;
            }
          }
        }
        break;
      case 0x34c:
        if ((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 0.5f;
            p.pose.scale[1] = 0.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            iVar4 = nextRandom(0,0x167);
            p.pose.rotation[0] = (float)iVar4;
            iVar4 = nextRandom(0,0x167);
            p.pose.rotation[1] = (float)iVar4;
            iVar4 = nextRandom(0,0x167);
            p.pose.rotation[2] = (float)iVar4;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.response = 0;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.085f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.085f);
          if (i16(p.animation.cursor.index) == 1) {
            if (u8(channel(24)) < 0xd) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0xc));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x34d:
        if (((i16(owner.animation.cursor.action) == 0x1fc) ||
            (i16(owner.animation.cursor.action) == 0x1fd)) &&
           (double(0.0f) < double(owner.motion.y))) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          p.counter = 1;
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x34e:
        if (i16(owner.animation.cursor.action) == 0x131) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
          if ((i16(owner.combat.freeze) == 0) &&
             (sVar2 = p.advance(), sVar2 == i16(p.animation.cursor.frameCount))) {
            p.destroy();
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x34f:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[1] = 60.0f;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.counter = 10;
          p.response = 0;
        }
        p.pose.scale[0] =
             (float)(int)i16(p.counter) * double(0.02f) + double(0.02f) + double(p.pose.scale[0]);
        p.pose.scale[1] =
             (float)(int)i16(p.counter) * double(0.02f) + double(0.02f) + double(p.pose.scale[1]);
        if (0 < i16(p.counter)) {
          p.counter = i16(p.counter) + -1;
        }
        if (i16(p.animation.cursor.index) == 4) {
          if (u8(channel(24)) < 0x10) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0xf));
          setChannel(0,u8(i8(channel(0)) + -0xf));
          setChannel(8,u8(i8(channel(8)) + -0xf));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x350:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[1] = 60.0f;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.counter = 10;
          p.response = 0;
        }
        p.pose.scale[0] =
             (float)(int)i16(p.counter) * double(0.05f) + double(0.05f) + double(p.pose.scale[0]);
        p.pose.scale[1] =
             (float)(int)i16(p.counter) * double(0.05f) + double(0.05f) + double(p.pose.scale[1]);
        if (0 < i16(p.counter)) {
          p.counter = i16(p.counter) + -1;
        }
        if (i16(p.animation.cursor.index) == 0x14) {
          if (u8(channel(24)) < 0x10) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0xf));
          setChannel(0,u8(i8(channel(0)) + -0xf));
          setChannel(8,u8(i8(channel(8)) + -0xf));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x351:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = (float)(i8(p.motion.facing) * -0x1e);
          p.pose.rotation[1] = 60.0f;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.counter = 10;
          p.response = 0;
        }
        p.pose.scale[0] =
             (float)(int)i16(p.counter) * double(0.05f) + double(0.05f) + double(p.pose.scale[0]);
        p.pose.scale[1] =
             (float)(int)i16(p.counter) * double(0.05f) + double(0.05f) + double(p.pose.scale[1]);
        if (0 < i16(p.counter)) {
          p.counter = i16(p.counter) + -1;
        }
        if (i16(p.animation.cursor.index) == 0x14) {
          if (u8(channel(24)) < 0x10) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0xf));
          setChannel(0,u8(i8(channel(0)) + -0xf));
          setChannel(8,u8(i8(channel(8)) + -0xf));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x352:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.counter = 10;
          p.response = 0;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) - double(0.02f);
        p.pose.scale[1] = double(p.pose.scale[1]) - double(0.02f);
        if (i16(p.animation.cursor.index) == 0) {
          if (u8(channel(24)) < 0x1f) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x1e));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      default:
        p.advance();
        break;
      case 0x3b6:
        if ((((i16(owner.animation.cursor.action) == 700) ||
             (i16(owner.animation.cursor.action) == 0x2bd)) ||
            (i16(owner.animation.cursor.action) == 0x2c2)) ||
           (i16(owner.animation.cursor.action) == 0x2c3)) {
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
            p.motion.velocityY = 15.0f;
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) - double(35.0f);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (double(p.motion.y) <= double(1000.0f)) {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
          else {
            p.destroy();
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x3b7:
        if (i16(p.animation.cursor.index) < 1) {
          p.motion.y = double(owner.motion.y);
        }
        p.motion.x = double(owner.motion.x);
        state.spellLoops = 1;
        if ((i16(owner.animation.cursor.index) == 0) &&
           ((((0x45 < i16(owner.animation.cursor.action) &&
              (i16(owner.animation.cursor.action) < 0x59)) &&
             (i16(owner.animation.cursor.action) != 0x4c)) ||
            (((i16(owner.animation.cursor.action) == 0xb5 ||
              (i16(owner.animation.cursor.action) == 0xb4)) ||
             (i16(owner.animation.cursor.action) == 0x91)))))) {
          p.destroy();
          state.spellLoops = 0;
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(-45.0f,45.0f,0.0f);
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
          else if ((i16(p.animation.cursor.index) == 5) &&
                  ((((i16(owner.animation.cursor.action) == 0x5f ||
                     (i16(owner.animation.cursor.action) == 0x5e)) ||
                    ((i16(owner.animation.cursor.action) == 0x5d ||
                     ((i16(owner.animation.cursor.action) == 0x31c ||
                      (i16(owner.animation.cursor.action) == 0x303)))))) &&
                   (i16(owner.animation.cursor.index) == 3)))) {
            p.frame(6);
          }
          else if ((i16(p.animation.cursor.index) == 7) &&
                  (((((i16(owner.animation.cursor.action) == 0x5f ||
                      (i16(owner.animation.cursor.action) == 0x5e)) ||
                     (i16(owner.animation.cursor.action) == 0x5d)) ||
                    (i16(owner.animation.cursor.action) == 0x31c)) &&
                   (i16(owner.animation.cursor.index) == 0xb)))) {
            p.destroy();
            state.spellLoops = 0;
          }
          else {
            if ((((i16(p.animation.cursor.index) == 7) && (i16(p.animation.cursor.elapsed) == 0)) &&
                (i16(owner.animation.cursor.action) != 0x303)) &&
               (i16(owner.animation.cursor.action) != 0x304)) {
              p.motion.velocityY = 15.0f;
              p.motion.accelerationY = -1.0f;
            }
            if (((i16(p.animation.cursor.index) == 7) &&
                (i16(owner.animation.cursor.action) != 0x304)) &&
               (i16(owner.animation.cursor.action) != 0x303)) {
              p.motion.velocityY = double(p.motion.velocityY) + double(p.motion.accelerationY);
              p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            }
            if ((i16(p.animation.cursor.index) == 7) &&
               ((i16(owner.animation.cursor.action) == 0x304 ||
                (i16(owner.animation.cursor.action) == 0x303)))) {
              p.motion.x =
                   (float)(i8(p.motion.facing) * 0x78) +
                   double(owner.motion.x);
              p.motion.y = double(owner.motion.y) + double(100.0f);
            }
            if ((i16(p.animation.cursor.index) == 0) || (i16(p.animation.cursor.index) == 7)) {
              p.pose.rotation[2] = double(p.pose.rotation[2]) - double(35.0f);
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
        }
        break;
      case 0x3de:
        if ((i16(state.farRangeFrames) == 0) && (i16(p.animation.cursor.index) < 2))
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
            if (0x77 < i16(state.farRangeFrames)) {
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
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x3df:
        if ((i16(state.farRangeFrames) == 0) && (i16(p.animation.cursor.index) < 2))
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
               (float)(int)i16(state.farRangeFrames) / double(45.0f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          if (i16(p.animation.cursor.index) == 0) {
            if (u8(channel(24)) < 0xe6) {
              setChannel(24,u8(i8(channel(24)) + '\x19'));
            }
            else {
              setChannel(24,u8(0xff));
            }
            if (0x77 < i16(state.farRangeFrames)) {
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
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x3e2:
        p.motion.x = double(owner.motion.x);
        p.motion.y = 1.0f;
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
        }
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          fVar6 = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[0] = fVar6 + fVar6 + double(1.0f);
          fVar6 = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[1] = fVar6 + fVar6 + double(1.0f);
          p.pose.rotation[2] = double(p.pose.rotation[2]) - double(1.0f);
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.frame(1);
          }
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
          sVar2 = nextRandom(0,0x168);
          p.counter = sVar2 + -0xb4;
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
          uVar1 = i16(i32(aimRadians/double(0.017453289f)));
          p.counter = uVar1;
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
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
          sVar2 = nextRandom(0,0x168);
          p.counter = sVar2 + -0xb4;
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
          uVar1 = i16(i32(aimRadians/double(0.017453289f)));
          p.counter = uVar1;
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
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
          p.pose.rotation[0] = -80.0f;
          p.counter = owner.combat.spellTimer;
        }
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          fVar6 = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[0] = fVar6 + fVar6;
          fVar6 = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[1] = fVar6 + fVar6;
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.frame(10);
          }
        }
        break;
      case 0x3e6:
        p.motion.facing = u8(owner.motion.facing);
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 2.5f;
          p.pose.scale[1] = 2.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[1] = 50.0f;
          p.counter = 0;
        }
        if (((0x95 < i16(owner.animation.cursor.action)) &&
            (i16(owner.animation.cursor.action) < 0x9a)) &&
           (i16(p.counter) == 0)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x39) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(94.0f);
        }
        if (((0x99 < i16(owner.animation.cursor.action)) &&
            (i16(owner.animation.cursor.action) < 0x9e)) &&
           (i16(p.counter) == 0)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x39) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(60.0f);
        }
        if ((i16(owner.animation.cursor.action) == 0x9e) &&
           (i16(p.counter) == 0)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x39) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
        }
        if (((i16(owner.animation.cursor.action) < 0x96) ||
            (0x9e < i16(owner.animation.cursor.action))) &&
           (i16(p.counter) == 0)) {
          p.counter = 1;
          state.guardEffectSent = 0;
        }
        p.pose.rotation[2] = double(p.pose.rotation[2]) - double(3.0f);
        if (0 < i16(p.counter)) {
          if (u8(channel(24)) < 0x1f) {
            setChannel(24,u8(0));
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x1e));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
        }
      }
    }
    else {
      p.freeze = i16(p.freeze) + -1;
    }
  }
  return;

}}
