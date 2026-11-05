#include "SuikaObjects.hpp"
#include <cmath>
namespace th075 {void advance_suika_object(SuikaObject& p,FighterActor& owner,FighterActor& other,SuikaState& state,ReplayStreams& random,FighterActionEvents& events){
i32 lastRoll=0;const auto nextRandom=[&](i32 low,i32 high){return lastRoll=random.range(low,high);};
auto& sceneFadeColor=events.fade_color();auto& sceneFadeAlpha=events.fade_alpha();
double aimRadians=0;const auto atan=[](float y,float x){return std::atan2(double(y),double(x));};
const auto channel=[&](u32 shift){return p.pose.channel(shift);};const auto setChannel=[&](u32 shift,u8 value){p.pose.set_channel(shift,value);};
const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters);};
const auto shared=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Shared,id,x,y,facing,layer,owner.launchParameters);};

  u8 uVar1;
  u16 uVar2;
  i16 sVar3;
  u32 uVar4;
  int iVar5;
  double fVar6;
  float fVar7;
  float fVar8;
  u32 uVar9;
  float particleSpeedOrLayer;
  u32 local_3d8;
  u32 local_390;
  u32 local_24c;
  u32 local_238;
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
        if ((i16(owner.animation.cursor.action) != 0x12d) &&
           (i16(p.animation.cursor.index) < 0x12)) {
          p.frame(0x12);
        }
        if (i16(owner.animation.cursor.action) == 0x12d) {
          p.motion.x =
               (float)((int)i8(p.motion.facing) << 7) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(66.0f);
        }
        if (p.response == 6) {
          p.response = 0;
          p.hits = 1;
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.2000000476837158f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.hits = 1;
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x321:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (8 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 1;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 0.10000000149011612f;
            p.pose.scale[1] = 0.10000000149011612f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) < 5) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
            if (double(0.5f) <= double(p.pose.scale[0])) {
              p.frame(i16(p.animation.cursor.index) + 4);
              p.pose.scale[0] = 0.5f;
              p.pose.scale[1] = 0.5f;
            }
          }
          if ((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 9)) {
            p.frame(9);
            iVar5 = nextRandom(0,4);
            p.motion.velocityX = -double(2.0f) - (float)iVar5;
            iVar5 = nextRandom(0,6);
            p.motion.velocityY = (float)iVar5 + double(8.0f);
          }
          if (i16(p.animation.cursor.index) < 9) {
            if (double(3.0f) < double(p.motion.velocityY)) {
              p.counter = 1;
            }
            if (double(p.motion.velocityY) < -double(3.0f)) {
              p.counter = 0xffff;
            }
            if (i16(p.counter) == 1) {
              p.motion.velocityY = double(p.motion.velocityY) - double(0.2f);
            }
            if (i16(p.counter) == -1) {
              p.motion.velocityY = double(p.motion.velocityY) + double(0.2f);
            }
          }
          if (((i16(p.animation.cursor.index) < 9) || (0x12 < i16(p.animation.cursor.index))) ||
             (p.motion.velocityY = double(p.motion.velocityY) - double(0.8f),
             fVar8 = double(p.motion.y) + double(p.motion.velocityY),
             fVar8 < double(0.0f) == (fVar8 == double(0.0f)))) {
            if (i16(p.animation.cursor.index) == 0x1d) {
              if (u8(channel(24)) < 0x14) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x14));
            }
            if ((double(1480.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(200.0f))) {
              p.destroy();
            }
            else {
              sVar3 = p.advance();
              if (sVar3 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
              if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.frame(1);
              }
              if ((i16(p.animation.cursor.index) == 9) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.frame(5);
              }
              if ((i16(p.animation.cursor.index) == 0x13) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.frame(0x11);
              }
            }
          }
          else {
            p.frame(0x13);
            p.motion.velocityY = 0.0f;
            p.motion.velocityX = 0.0f;
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_8 = 0; local_8 < 6; local_8 = local_8 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x322:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (0xf < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 1;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 0.10000000149011612f;
            p.pose.scale[1] = 0.10000000149011612f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) < 6) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
            if (double(0.5f) <= double(p.pose.scale[0])) {
              p.pose.scale[0] = 0.5f;
              p.pose.scale[1] = 0.5f;
            }
            p.motion.velocityY = double(p.motion.velocityY) - double(0.6f);
            fVar8 = double(p.motion.y) + double(p.motion.velocityY);
            if (fVar8 < double(0.0f) != (fVar8 == double(0.0f))) {
              p.frame(6);
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
            }
          }
          if ((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 0x10)) {
            p.frame(0x10);
            iVar5 = nextRandom(0,4);
            p.motion.velocityX = -double(2.0f) - (float)iVar5;
            iVar5 = nextRandom(0,6);
            p.motion.velocityY = (float)iVar5 + double(8.0f);
          }
          if (((i16(p.animation.cursor.index) < 0x10) || (0x19 < i16(p.animation.cursor.index))) ||
             (p.motion.velocityY = double(p.motion.velocityY) - double(0.8f),
             fVar8 = double(p.motion.y) + double(p.motion.velocityY),
             fVar8 < double(0.0f) == (fVar8 == double(0.0f)))) {
            if (i16(p.animation.cursor.index) == 0x24) {
              if (u8(channel(24)) < 0x14) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x14));
            }
            if ((double(1480.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(200.0f))) {
              p.destroy();
            }
            else {
              sVar3 = p.advance();
              if (sVar3 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
              if ((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.frame(4);
              }
              if ((i16(p.animation.cursor.index) == 8) && (i16(p.animation.cursor.elapsed) == 0)) {
                iVar5 = nextRandom(0,3);
                p.motion.velocityX = (float)(iVar5 + 5);
              }
              if ((i16(p.animation.cursor.index) == 0x10) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.frame(8);
              }
              if ((i16(p.animation.cursor.index) == 0x1a) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.frame(0x18);
              }
            }
          }
          else {
            p.frame(0x1a);
            p.motion.velocityY = 0.0f;
            p.motion.velocityX = 0.0f;
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_c = 0; local_c < 6; local_c = local_c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x323:
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 10.0f;
            p.motion.velocityY = 8.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (double(p.motion.velocityX) <= double(2.5f)) {
            p.motion.velocityX = 2.5f;
          }
          else {
            p.motion.velocityX = double(p.motion.velocityX) - double(1.0f);
          }
          p.motion.velocityY = double(p.motion.velocityY) - double(0.6f);
          owned(0x324,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
            events.sound(0x12);
            events.shake(5.0f);
            for (local_14 = 0; local_14 < 0x12; local_14 = local_14 + 1) {
              owner.launchParameters[0] = (float)(local_14 * 0x14);
              owner.launchParameters[1] = 17.0f;
              owned(0x325,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0);
            }
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_10 = 0; local_10 < 0xf; local_10 = local_10 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x324:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.pose.scale[0] = 0.6000000238418579f;
          p.pose.scale[1] = 0.6000000238418579f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          iVar5 = nextRandom(0,5);
          fVar8 = (float)(iVar5 + 5);
          iVar5 = nextRandom(0,0x167);
          p.polar((float)iVar5,fVar8);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x325:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (0 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 2;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 1)) {
            p.counter = 0;
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
          p.polar(double(p.parameters[0]),double(p.parameters[1]));
          if (double(2.0f) < double(p.parameters[1])) {
            p.parameters[1] = double(p.parameters[1]) - double(1.0f);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 0) {
            owned(0x326,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            iVar5 = nextRandom(0,4);
            p.pose.scale[0] = (float)iVar5 * double(0.1f) + double(0.8f);
            p.pose.scale[1] = double(p.pose.scale[0]);
            if (i16(p.hits) == 0) {
              p.frame(1);
            }
          }
          if (i16(p.animation.cursor.index) == 1) {
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_18 = 0; local_18 < 2; local_18 = local_18 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x326:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.pose.scale[0] = 0.75f;
          p.pose.scale[1] = 0.75f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
        if (u8(channel(24)) < 0x10) {
          p.destroy();
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -0xf));
          setChannel(0,u8(i8(channel(0)) + -0xf));
          setChannel(8,u8(i8(channel(8)) + -0xf));
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x327:
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if (((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
              (i16(p.counter) == 0)) &&
             (((i8(p.motion.facing) == -1 && (double(0.0f) < double(p.motion.velocityX))) ||
              ((i8(p.motion.facing) == '\x01' && (double(p.motion.velocityX) < double(0.0f))))))) {
            p.motion.facing = -i8(p.motion.facing);
            p.counter = i16(p.counter) + 1;
          }
          if (((double(owner.combat.screenBounds[1]) < double(p.motion.x)) &&
              (i16(p.counter) == 0)) &&
             (((i8(p.motion.facing) == -1 && (double(p.motion.velocityX) < double(0.0f))) ||
              ((i8(p.motion.facing) == '\x01' && (double(0.0f) < double(p.motion.velocityX))))))) {
            p.motion.facing = -i8(p.motion.facing);
            p.counter = i16(p.counter) + 1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 20.0f;
            p.motion.velocityY = 10.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (double(p.motion.velocityX) <= double(10.0f)) {
            p.motion.velocityX = 10.0f;
          }
          else {
            p.motion.velocityX = double(p.motion.velocityX) - double(1.0f);
          }
          p.motion.velocityY = double(p.motion.velocityY) - double(0.6f);
          owned(0x324,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
            events.sound(0x12);
            events.shake(5.0f);
            for (local_20 = 0; local_20 < 0x12; local_20 = local_20 + 1) {
              owner.launchParameters[0] = (float)(local_20 * 0x14);
              owner.launchParameters[1] = 17.0f;
              owned(0x325,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_1c = 0; local_1c < 0xf; local_1c = local_1c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x328:
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 2.5f;
            p.motion.velocityY = 15.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          p.motion.velocityY = double(p.motion.velocityY) - double(0.45f);
          owned(0x324,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
            events.sound(0x12);
            events.shake(5.0f);
            for (local_28 = 0; local_28 < 0x12; local_28 = local_28 + 1) {
              owner.launchParameters[0] = (float)(local_28 * 0x14);
              owner.launchParameters[1] = 17.0f;
              owned(0x325,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_24 = 0; local_24 < 0xf; local_24 = local_24 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x329:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 2.0f;
            p.pose.scale[1] = 2.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_2c = 0; local_2c < 10; local_2c = local_2c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x32a:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          iVar5 = nextRandom(0,0x78);
          p.pose.rotation[0] = (float)(iVar5 + -0x3c);
          iVar5 = nextRandom(0,0x78);
          p.pose.rotation[1] = (float)(iVar5 + -0x3c);
          iVar5 = nextRandom(0,0x78);
          p.pose.rotation[2] = (float)(iVar5 + -0x3c);
          p.counter = 0;
        }
        p.pose.scale[0] =
             (double(0.2f) - (float)(int)i16(p.counter) * double(0.05f) *
                    (float)(i16(p.counter) < 5)) + double(0.005f) + double(p.pose.scale[0]);
        p.pose.scale[1] =
             (double(0.2f) - (float)(int)i16(p.counter) * double(0.05f) *
                    (float)(i16(p.counter) < 5)) + double(0.005f) + double(p.pose.scale[1]);
        p.counter = i16(p.counter) + 1;
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0x15) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x14));
          setChannel(8,u8(i8(channel(8)) + -0x14));
          setChannel(0,u8(i8(channel(0)) + -0x14));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x32b:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.counter = 0;
          p.pose.scale[0] = 2.0f;
          p.pose.scale[1] = 2.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.polar(double(p.parameters[0]),10.0f);
        }
        if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
          sVar3 = nextRandom(0,0x3c);
          p.timer = sVar3 + -0x1e;
          sVar3 = nextRandom(0,0x3c);
          p.angle = sVar3 + -0x1e;
          owned(0x329,(float)(int)i16(p.timer) + double(p.motion.x),(float)(int)i16(p.angle) + double(p.motion.y),u8(p.motion.facing),1);
          owner.launchParameters[0] = double(p.parameters[0]);
          owned(0x32a,(float)(int)i16(p.timer) + double(p.motion.x),(float)(int)i16(p.angle) + double(p.motion.y),u8(p.motion.facing),1);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        if ((double(1480.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(200.0f))) {
          p.destroy();
        }
        else {
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            if (i16(p.counter) < 6) {
              p.frame(1);
              p.counter = i16(p.counter) + 1;
            }
            else {
              p.destroy();
            }
          }
        }
        break;
      case 0x32c:
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.5f;
            p.pose.scale[1] = 0.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.hits = 1;
            p.response = 0;
          }
          p.polar(double(p.parameters[0]),double(p.parameters[1]));
          if (double(2.0f) < double(p.parameters[1])) {
            p.parameters[1] = double(p.parameters[1]) - double(1.0f);
          }
          if ((i16(p.animation.cursor.index) < 2) && (double(p.parameters[1]) == double(2.0f))) {
            p.frame(2);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          p.angle = i16(p.angle) + -1;
          if (i16(p.angle) < 1) {
            particleSpeedOrLayer = 1;
            uVar4 = (u32)u8(p.motion.facing);
            fVar8 = double(p.motion.y);
            iVar5 = nextRandom(0,0x14);
            fVar7 = (float)iVar5 + double(float(double(fVar8)-10.0));
            fVar8 = double(p.motion.x);
            iVar5 = nextRandom(0,0x14);
            owned(0x32d,(float)iVar5 + double(float(double(fVar8)-10.0)),fVar7,uVar4,particleSpeedOrLayer);
            p.angle = 4;
          }
          if (i16(p.animation.cursor.elapsed) == 0) {
            uVar2 = nextRandom(0,0x14);
            p.timer = uVar2;
            p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
            p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(1);
          }
          if (((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) < 0x19)) {
            p.frame(2);
            p.counter = i16(p.counter) + 1;
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_30 = 0; local_30 < 4; local_30 = local_30 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x32d:
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.800000011920929f;
          p.pose.scale[1] = 0.800000011920929f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 1;
          p.motion.velocityY = 4.0f;
          p.motion.velocityX = 0.0f;
        }
        p.motion.velocityY = double(p.motion.velocityY) - double(0.15f);
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        if (u8(channel(24)) < 0xe) {
          setChannel(24,u8(0));
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -0xd));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.94f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.94f);
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x32e:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
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
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x32f:
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) != 1)) {
          p.begin_pose(0.0f,0.0f,0.0f);
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
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.08f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.08f);
        setChannel(24,u8(i8(channel(24)) + -7));
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x330:
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) != 1)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          iVar5 = nextRandom(0x19,0x4b);
          p.pose.scale[0] = (float)iVar5 * double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          setChannel(24,u8(0xff));
          uVar1 = nextRandom(0,0xff);
          setChannel(16,u8(uVar1));
          uVar1 = nextRandom(0,0xff);
          setChannel(8,u8(uVar1));
          uVar1 = nextRandom(0,0xff);
          setChannel(0,u8(uVar1));
        }
        p.motion.x = double(p.motion.x) - double(0.1f);
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
        setChannel(24,u8(i8(channel(24)) + -7));
        p.pose.scale[0] = double(p.pose.scale[0]) - double(0.01f);
        p.pose.scale[1] = double(p.pose.scale[1]) - double(0.01f);
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x331:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          iVar5 = nextRandom(0x4b,0x96);
          p.pose.scale[0] = (float)iVar5 * double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          iVar5 = nextRandom(0,0x167);
          p.pose.rotation[2] = (float)iVar5;
          (aimRadians = atan((double(other.motion.y) + double(20.0f)) -
                       double(p.motion.y),(double(other.motion.x) -
                       double(p.motion.x)) * (float)(int)i8(p.motion.facing)));
          uVar2 = i16(i32(aimRadians/double(0.017453289f)));
          p.angle = uVar2;
          p.polar((float)(int)i16(p.angle),36.0f);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        if (i16(p.animation.cursor.index) < 2) {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          fVar6 = (double)std::abs(float(double(p.motion.x) -
                                        double(other.motion.x)));
          if ((fVar6 < (double)double(60.0f)) &&
             (fVar6 = (double)std::abs(float((double(p.motion.y) -
                                            double(other.motion.y)) + double(20.0f))), fVar6 < (double)double(60.0f))) {
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            events.system_sound(4);
            uVar2 = i16(i32(double(p.motion.x)-other.motion.x));
            p.counter = uVar2;
            uVar2 = i16(i32(double(p.motion.y)-other.motion.y+double(20.0f)));
            p.timer = uVar2;
            p.frame(2);
          }
        }
        if ((i16(p.animation.cursor.index) == 2) &&
           ((i16(owner.animation.cursor.action) == 700 ||
            (i16(owner.animation.cursor.action) == 0x303)))) {
          if (i16(owner.animation.cursor.index) == 0x17) {
            p.motion.x =
                 (float)((i16(p.counter) + 0x3c) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0x8c) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x18) {
            p.motion.x =
                 (float)((i16(p.counter) + 0x5e) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0xb2) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x19) {
            p.motion.x =
                 (float)((i16(p.counter) + 0x96) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0x52) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x1a) {
            p.motion.x =
                 (float)((i16(p.counter) + 0x4b) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0x11) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x1b) {
            p.motion.x =
                 (float)((i16(p.counter) + -0x1e) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 10) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x1c) {
            p.motion.x =
                 (float)((i16(p.counter) + -0x6e) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0x55) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x1d) {
            p.motion.x =
                 (float)((i16(p.counter) + -0x5f) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0xb9) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x1e) {
            p.motion.x =
                 (float)((i16(p.counter) + -10) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0xe6) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x1f) {
            p.motion.x =
                 (float)((i16(p.counter) + 0x36) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0xa0) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x20) {
            p.motion.x =
                 (float)((i16(p.counter) + 0x50) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0x87) +
                 double(owner.motion.y);
          }
          if (0x20 < i16(owner.animation.cursor.index)) {
            p.motion.x =
                 (float)((int)i16(p.counter) * (int)i8(p.motion.facing)) +
                 double(other.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0x46) +
                 double(other.motion.y);
            if (i16(other.animation.cursor.action) == 0x61) {
              p.frame(3);
              iVar5 = nextRandom(0,0x14);
              p.motion.velocityX = (float)(iVar5 + -10);
              iVar5 = nextRandom(5,0x14);
              p.motion.velocityY = (float)-iVar5;
              return;
            }
          }
        }
        if (i16(p.animation.cursor.index) == 3) {
          p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
          fVar8 = double(p.motion.y) + double(p.motion.velocityY);
          if ((fVar8 < double(0.0f) != (fVar8 == double(0.0f))) && (double(0.0f) < double(p.motion.velocityY))) {
            p.motion.velocityY = -double(p.motion.velocityY);
          }
          if (u8(channel(24)) < 6) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -5));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x332:
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) != 1)) {
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
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.08f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.08f);
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
        break;
      case 0x333:
        if (i16(owner.animation.cursor.action) != 0x2bf) {
          p.destroy();
        }
        if (i16(owner.animation.cursor.action) == 0x2bf) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.timer = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.timer = i16(p.timer) + 1;
        if (i16(p.counter) == 0) {
          if (u8(channel(24)) < 0xeb) {
            setChannel(24,u8(i8(channel(24)) + '\x14'));
          }
          else {
            setChannel(24,u8(0xff));
            p.counter = 1;
          }
        }
        if ((i16(p.counter) == 1) && (0x5a < i16(p.timer))) {
          if (u8(channel(24)) < 0x15) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
        }
        break;
      case 0x334:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          iVar5 = nextRandom(100,300);
          p.pose.scale[0] = (float)iVar5 * double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          iVar5 = nextRandom(0,0x167);
          p.pose.rotation[2] = (float)iVar5;
          (aimRadians = atan((double(other.motion.y) + double(20.0f)) -
                       double(p.motion.y),(double(other.motion.x) -
                       double(p.motion.x)) * (float)(int)i8(p.motion.facing)));
          uVar2 = i16(i32(aimRadians/double(0.017453289f)));
          p.angle = uVar2;
          p.polar((float)(int)i16(p.angle),36.0f);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        if (i16(p.animation.cursor.index) < 2) {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          fVar6 = (double)std::abs(float(double(p.motion.x) -
                                        double(other.motion.x)));
          if ((fVar6 < (double)double(80.0f)) &&
             (fVar6 = (double)std::abs(float((double(p.motion.y) -
                                            double(other.motion.y)) + double(20.0f))), fVar6 < (double)double(80.0f))) {
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            events.system_sound(4);
            uVar2 = i16(i32(double(p.motion.x)-other.motion.x));
            p.counter = uVar2;
            uVar2 = i16(i32(double(p.motion.y)-other.motion.y+double(20.0f)));
            p.timer = uVar2;
            p.frame(2);
          }
        }
        if ((i16(p.animation.cursor.index) == 2) &&
           ((i16(owner.animation.cursor.action) == 0x2d0 ||
            (i16(owner.animation.cursor.action) == 0x305)))) {
          if (i16(owner.animation.cursor.index) == 0x17) {
            p.motion.x =
                 (float)((i16(p.counter) + 0x3c) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0x8c) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x18) {
            p.motion.x =
                 (float)((i16(p.counter) + 0x5e) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0xb2) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x19) {
            p.motion.x =
                 (float)((i16(p.counter) + 0x96) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0x52) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x1a) {
            p.motion.x =
                 (float)((i16(p.counter) + 0x4b) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0x11) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x1b) {
            p.motion.x =
                 (float)((i16(p.counter) + -0x1e) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 10) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x1c) {
            p.motion.x =
                 (float)((i16(p.counter) + -0x6e) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0x55) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x1d) {
            p.motion.x =
                 (float)((i16(p.counter) + -0x5f) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0xb9) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x1e) {
            p.motion.x =
                 (float)((i16(p.counter) + -10) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0xe6) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x1f) {
            p.motion.x =
                 (float)((i16(p.counter) + 0x36) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0xa0) +
                 double(owner.motion.y);
          }
          if (i16(owner.animation.cursor.index) == 0x20) {
            p.motion.x =
                 (float)((i16(p.counter) + 0x50) * (int)i8(p.motion.facing)) +
                 double(owner.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0x87) +
                 double(owner.motion.y);
          }
          if (0x20 < i16(owner.animation.cursor.index)) {
            p.motion.x =
                 (float)((int)i16(p.counter) * (int)i8(p.motion.facing)) +
                 double(other.motion.x);
            p.motion.y =
                 (float)(i16(p.timer) + 0x46) +
                 double(other.motion.y);
            if (i16(other.animation.cursor.action) == 0x59) {
              p.frame(3);
              iVar5 = nextRandom(0,0x14);
              p.motion.velocityX = (float)(iVar5 + -10);
              iVar5 = nextRandom(5,0x14);
              p.motion.velocityY = (float)-iVar5;
              return;
            }
          }
        }
        if (i16(p.animation.cursor.index) == 3) {
          p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
          fVar8 = double(p.motion.y) + double(p.motion.velocityY);
          if ((fVar8 < double(0.0f) != (fVar8 == double(0.0f))) && (double(0.0f) < double(p.motion.velocityY))) {
            p.motion.velocityY = -double(p.motion.velocityY);
          }
          if (u8(channel(24)) < 6) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -5));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x335:
        if (i16(owner.animation.cursor.action) != 0x2d3) {
          p.destroy();
        }
        if (i16(owner.animation.cursor.action) == 0x2d3) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.timer = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.timer = i16(p.timer) + 1;
        if (i16(p.counter) == 0) {
          if (u8(channel(24)) < 0xeb) {
            setChannel(24,u8(i8(channel(24)) + '\x14'));
          }
          else {
            setChannel(24,u8(0xff));
            p.counter = 1;
          }
        }
        if ((i16(p.counter) == 1) && (0x5a < i16(p.timer))) {
          if (u8(channel(24)) < 0x15) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
        }
        break;
      case 0x336:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.timer = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          iVar5 = nextRandom(0,5);
          p.pose.scale[0] = double(1.0f) - (float)iVar5 * double(0.1f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityY = 0.0f;
          iVar5 = nextRandom(0,10);
          p.motion.velocityX = (float)(iVar5 + -5);
        }
        if (i16(p.animation.cursor.index) == 1) {
          if (u8(channel(24)) < 0xb) {
            setChannel(24,u8(0));
            p.destroy();
          }
          else {
            setChannel(24,u8(i8(channel(24)) + -10));
          }
        }
        p.motion.velocityY = double(p.motion.velocityY) - double(0.3f);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x337:
        p.motion.facing = 1;
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          iVar5 = nextRandom(0,5);
          p.pose.scale[0] = double(1.0f) - (float)iVar5 * double(0.1f);
          p.pose.scale[1] = double(p.pose.scale[0]);
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
          (aimRadians = atan((double(owner.motion.y) + double(100.0f)) -
                       double(p.motion.y),double(owner.motion.x) - double(p.motion.x)));
          uVar2 = i16(i32(aimRadians/double(0.017453289f)));
          p.counter = uVar2;
          p.polar((float)(int)i16(p.counter),20.0f);
          if (((double(p.motion.x) - double(owner.motion.x) < double(20.0f)) &&
              (-double(20.0f) < double(p.motion.x) - double(owner.motion.x))) &&
             ((double(p.motion.y) - double(owner.motion.y) < double(120.0f) &&
              (double(80.0f) < double(p.motion.y) - double(owner.motion.y))))) {
            p.destroy();
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
      case 0x338:
        if (p.response == 5) {
          p.destroy();
        }
        else {
          iVar5 = (int)i16(owner.animation.cursor.action);
          if ((iVar5 < 0x32) ||
             (iVar5 = (int)i16(owner.animation.cursor.action), 0x95 < iVar5)) {
            uVar9 = 1;
            particleSpeedOrLayer = u8(p.motion.facing);
            fVar8 = double(p.motion.y);
            iVar5 = nextRandom(0,0x14);
            fVar7 = double(float(double(fVar8)+10.0))-iVar5;
            fVar8 = double(p.motion.x);
            iVar5 = nextRandom(0,0x14);
            owned(0x326,double(float(double(fVar8)+10.0))-iVar5,fVar7,particleSpeedOrLayer,uVar9);
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
              p.polar(double(p.parameters[0]),30.0f);
              fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
              p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
              p.response = 0;
              p.hits = 1;
            }
            if ((i16(p.animation.cursor.index) == 1) &&
               (((p.response != 0 && (p.response != 6)) ||
                (fVar8 = double(p.motion.y) + double(p.motion.velocityY),
                fVar8 < double(0.0f) != (fVar8 == double(0.0f)))))) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.hits = 1;
              p.response = 0;
              static_cast<void>(1);
              p.frame(2);
              owned(0x33b,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              p.pose.rotation[2] = 0.0f;
            }
            else {
              if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
                particleSpeedOrLayer = 1;
                uVar4 = (u32)u8(p.motion.facing);
                fVar8 = double(p.motion.y);
                iVar5 = nextRandom(0,100);
                fVar7 = double(float(double(fVar8)+50.0))-iVar5;
                fVar8 = double(p.motion.x);
                iVar5 = nextRandom(0,100);
                owned(0x33b,double(float(double(fVar8)+50.0))-iVar5,fVar7,uVar4,particleSpeedOrLayer);
              }
              if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
                particleSpeedOrLayer = 1;
                uVar4 = (u32)u8(p.motion.facing);
                fVar8 = double(p.motion.y);
                iVar5 = nextRandom(0,100);
                fVar7 = double(float(double(fVar8)+50.0))-iVar5;
                fVar8 = double(p.motion.x);
                iVar5 = nextRandom(0,100);
                owned(0x33b,double(float(double(fVar8)+50.0))-iVar5,fVar7,uVar4,particleSpeedOrLayer);
              }
              if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f)))
                  || (double(960.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(100.0f)))
              {
                p.destroy();
              }
              else {
                p.motion.x =
                     (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                     double(p.motion.x);
                p.motion.y = double(p.motion.y) - double(p.motion.velocityY)
                ;
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
            for (local_34 = 0; local_34 < 7; local_34 = local_34 + 1) {
              owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
            }
          }
        }
        break;
      case 0x339:
        if ((i16(owner.animation.cursor.action) < 0x46) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.timer = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = double(p.parameters[0]);
            p.motion.velocityY = 10.0f;
            p.cycle = 0;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          if (((i16(p.animation.cursor.index) < 2) &&
              (i16(owner.animation.cursor.index) == 0)) &&
             ((i16(owner.animation.cursor.action) == 0x198 ||
              (i16(owner.animation.cursor.action) == 0x19e)))) {
            p.frame(2);
            p.motion.velocityY = 0.0f;
            p.motion.velocityX = 0.0f;
          }
          else {
            if (i16(p.animation.cursor.index) == 0) {
              if (double(p.motion.velocityX) <= double(2.5f)) {
                p.motion.velocityX = 2.5f;
              }
              else {
                p.motion.velocityX = double(p.motion.velocityX) - double(1.0f);
              }
              p.motion.velocityY = double(p.motion.velocityY) - double(0.6f);
            }
            if (i16(p.animation.cursor.index) == 1) {
              p.cycle = i16(p.cycle) + -1;
              local_238 = (u32)(double(0.0f) < (double(other.motion.x) - double(p.motion.x)) *
                                       (float)(int)i8(p.motion.facing));
              owner.motion.opponentPush =
                   (float)((int)i8(p.motion.facing) *
                          (int)i8(other.motion.facing)) * -double(6.0f)
                   * (float)(int)(local_238 * 2 + -1);
              if (i16(p.cycle) < 1) {
                owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0);
                p.cycle = 0xf;
              }
            }
            if (i16(p.animation.cursor.index) == 2) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
            }
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              events.sound(0x1f);
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
            }
            if (((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.timer) < 0x2d)) {
              p.timer = i16(p.timer) + 1;
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_38 = 0; local_38 < 0xf; local_38 = local_38 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x33a:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) - double(0.05f);
        p.pose.scale[1] = double(p.pose.scale[1]) - double(0.05f);
        if (5 < i16(p.animation.cursor.elapsed)) {
          setChannel(24,u8(i8(channel(24)) + -0xf));
        }
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(10.0f);
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33b:
        if (p.response == 5) {
          p.destroy();
        }
        else if ((i16(owner.animation.cursor.action) < 0x32) ||
                (0x95 < i16(owner.animation.cursor.action))) {
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
            p.response = 0;
            p.hits = 1;
          }
          if (((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
             ((double(960.0f) < double(p.motion.y) || (double(p.motion.y) < -double(100.0f))))) {
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
          for (local_3c = 0; local_3c < 7; local_3c = local_3c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x33c:
        if ((i16(owner.animation.cursor.action) < 0x46) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.timer = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = double(p.parameters[0]);
            p.motion.velocityY = 10.0f;
            p.cycle = 0;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (((i16(p.animation.cursor.index) < 2) &&
              (i16(owner.animation.cursor.index) == 0)) &&
             ((i16(owner.animation.cursor.action) == 0x198 ||
              (i16(owner.animation.cursor.action) == 0x19e)))) {
            p.frame(2);
            p.motion.velocityY = 0.0f;
            p.motion.velocityX = 0.0f;
          }
          else {
            if (i16(p.animation.cursor.index) == 0) {
              if (double(p.motion.velocityX) <= double(2.5f)) {
                p.motion.velocityX = 2.5f;
              }
              else {
                p.motion.velocityX = double(p.motion.velocityX) - double(1.0f);
              }
              p.motion.velocityY = double(p.motion.velocityY) - double(0.6f);
            }
            if (i16(p.animation.cursor.index) == 1) {
              p.cycle = i16(p.cycle) + -1;
              local_24c = (u32)(double(0.0f) < (double(other.motion.x) - double(p.motion.x)) *
                                       (float)(int)i8(p.motion.facing));
              owner.motion.opponentPush =
                   (float)((int)i8(p.motion.facing) *
                          (int)i8(other.motion.facing)) * -double(6.0f)
                   * (float)(int)(local_24c * 2 + -1);
              if (i16(p.cycle) < 1) {
                owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0);
                p.cycle = 0xf;
              }
            }
            if (i16(p.animation.cursor.index) == 2) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
            }
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              events.sound(0x1f);
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
            }
            if (((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.timer) < 0x2d)) {
              p.timer = i16(p.timer) + 1;
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_40 = 0; local_40 < 0xf; local_40 = local_40 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x33d:
        if (p.response == 5) {
          p.destroy();
        }
        else if ((i16(owner.animation.cursor.action) < 0x32) ||
                (0x95 < i16(owner.animation.cursor.action))) {
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
            p.motion.velocityX = 15.0f;
            p.motion.velocityY = 10.0f;
            p.response = 0;
            p.hits = 1;
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          p.motion.velocityY = double(p.motion.velocityY) - double(1.5f);
          if (double(p.motion.y) + double(p.motion.velocityY) < double(0.0f)) {
            p.motion.y = 0.0f;
            p.motion.velocityY = -double(p.motion.velocityY);
          }
          if (((i16(p.animation.cursor.index) == 1) && (p.response != 0)) &&
             (p.response != 6)) {
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.hits = 1;
            p.response = 0;
            static_cast<void>(1);
            p.destroy();
            owned(0x33e,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            p.pose.rotation[2] = 0.0f;
          }
          else if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f)))
                   || (double(960.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(100.0f))
                  ) {
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
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_44 = 0; local_44 < 5; local_44 = local_44 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x33e:
        if (p.response == 5) {
          p.destroy();
        }
        else if ((i16(owner.animation.cursor.action) < 0x32) ||
                (0x95 < i16(owner.animation.cursor.action))) {
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
            p.response = 0;
            p.hits = 1;
          }
          if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
              (double(960.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(100.0f))) {
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
          for (local_48 = 0; local_48 < 5; local_48 = local_48 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x33f:
        if (i16(owner.animation.cursor.action) != 0x2d3) {
          p.destroy();
        }
        if (i16(owner.animation.cursor.action) == 0x2d3) {
          p.motion.x =
               double(other.motion.x);
          p.motion.y =
               double(other.motion.y) + double(100.0f);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.timer = 0;
        }
        p.timer = i16(p.timer) + 1;
        if (0x5a < i16(p.timer)) {
          if (u8(channel(24)) < 0x15) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
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
          p.counter = 0;
        }
        p.pose.scale[0] =
             (double(0.2f) - (float)(int)i16(p.counter) * double(0.02f) *
                    (float)(i16(p.counter) < 5)) + double(0.005f) + double(p.pose.scale[0]);
        p.pose.scale[1] =
             (double(0.2f) - (float)(int)i16(p.counter) * double(0.02f) *
                    (float)(i16(p.counter) < 5)) + double(0.005f) + double(p.pose.scale[1]);
        p.counter = i16(p.counter) + 1;
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0x15) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x14));
          setChannel(8,u8(i8(channel(8)) + -0x14));
          setChannel(0,u8(i8(channel(0)) + -0x14));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x341:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
        }
        if (0 < i16(p.animation.cursor.index)) {
          p.pose.scale[0] =
               (double(0.2f) - (float)(int)i16(p.counter) * double(0.02f) *
                      (float)(i16(p.counter) < 5)) + double(0.005f) + double(p.pose.scale[0]);
          p.pose.scale[1] =
               (double(0.2f) - (float)(int)i16(p.counter) * double(0.02f) *
                      (float)(i16(p.counter) < 5)) + double(0.005f) + double(p.pose.scale[1]);
          p.counter = i16(p.counter) + 1;
        }
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0x15) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x14));
          setChannel(8,u8(i8(channel(8)) + -0x14));
          setChannel(0,u8(i8(channel(0)) + -0x14));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
          setChannel(24,u8(0xff));
          for (local_4c = 0; local_4c < 0xc; local_4c = local_4c + 1) {
            iVar5 = nextRandom(0,0x28);
            owner.launchParameters[0] = (float)(iVar5 + -0x5a);
            iVar5 = nextRandom(0,0x16);
            owner.launchParameters[1] = (float)(iVar5 + 5);
            uVar9 = 1;
            uVar4 = (u32)u8(p.motion.facing);
            particleSpeedOrLayer = 0;
            iVar5 = nextRandom(0,200);
            owned(0x32c,(float)((iVar5 + -100) * (int)i8(p.motion.facing)) +
                               double(p.motion.x),particleSpeedOrLayer,uVar4,uVar9);
          }
        }
        break;
      case 0x342:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[2] = -80.0f;
          p.counter = 0;
        }
        p.pose.scale[0] =
             (double(0.3f) - (float)(int)i16(p.counter) * double(0.05f) *
                    (float)(i16(p.counter) < 5)) + double(0.005f) + double(p.pose.scale[0]);
        p.pose.scale[1] =
             (double(0.3f) - (float)(int)i16(p.counter) * double(0.05f) *
                    (float)(i16(p.counter) < 5)) + double(0.005f) + double(p.pose.scale[1]);
        p.counter = i16(p.counter) + 1;
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0x15) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x14));
          setChannel(8,u8(i8(channel(8)) + -0x14));
          setChannel(0,u8(i8(channel(0)) + -0x14));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x343:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 2.0f;
          p.pose.scale[1] = 2.0f;
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
      default:
        p.advance();
        break;
      case 0x352:
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
          state.guardEffectSent = 0;
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
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.frame(0);
        }
        break;
      case 900:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.response = 0;
          iVar5 = nextRandom(0,0x1e);
          p.motion.velocityX = (float)(iVar5 + -0xf);
          iVar5 = nextRandom(0,0xf);
          p.motion.velocityY = (float)iVar5;
        }
        if ((i16(p.animation.cursor.index) < 3) &&
           (p.motion.velocityY = double(p.motion.velocityY) - double(0.6f),
           fVar8 = double(p.motion.velocityY) + double(p.motion.y),
           fVar8 < double(0.0f) != (fVar8 == double(0.0f)))) {
          p.motion.y = 0.0f;
          p.frame(3);
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
        }
        else if (double(p.motion.y) <= double(960.0f)) {
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.action(0);
            p.begin_pose(0.0f,0.0f,0.0f);
          }
          if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(1);
          }
          if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.motion.velocityY = 24.0f;
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x385:
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x386:
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x387:
        p.motion.x = double(owner.motion.x);
        p.motion.y = double(owner.motion.y);
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x388:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          iVar5 = nextRandom(0,4);
          p.pose.scale[0] = (float)iVar5 * double(0.1f) + double(0.4f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          setChannel(24,u8(0x40));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.01f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.01f);
        setChannel(24,u8(i8(channel(24)) + -1));
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x389:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          iVar5 = nextRandom(0,100);
          if (iVar5 < 0x14) {
            particleSpeedOrLayer = 1;
            uVar4 = (u32)u8(p.motion.facing);
            fVar8 = double(p.motion.y);
            iVar5 = nextRandom(0,100);
            fVar7 = double(float(double(fVar8)+50.0))-iVar5;
            fVar8 = double(p.motion.x);
            iVar5 = nextRandom(0,100);
            owned(0x388,double(float(double(fVar8)+50.0))-iVar5,fVar7,uVar4,particleSpeedOrLayer);
          }
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
            p.counter = 100;
            p.timer = 0;
            uVar2 = i16(i32(p.parameters[0]));
            p.angle = uVar2;
            p.cycle = 0x78;
            p.variant = state.difficulty * 2 + 1;
          }
          if (i16(owner.animation.cursor.index) < 2) {
            p.cycle = i16(p.cycle) + -1;
            if (((((i16(p.cycle) < 1) &&
                  (0 < i16(other.combat.health))) &&
                 (fVar6 = (double)std::sqrt(double(float((double(other.motion.y) -
                                                double(p.motion.y)) *
                                                (double(other.motion.y) -
                                                double(p.motion.y)) +
                                                (double(other.motion.x) -
                                                double(p.motion.x)) *
                                                (double(other.motion.x) -
                                                double(p.motion.x))))),
                 (double)(state.difficulty * -0x30 + 0x100) < fVar6)) &&
                ((double(0.0f) < double(p.motion.y) && (double(0.0f) < double(p.motion.x))))) &&
               (double(p.motion.x) < double(1280.0f))) {
              if (i16(p.variant) < 1) {
                owned(0x38a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
                events.sound(0x1c);
                p.variant = state.difficulty * 2 + 1;
                p.cycle = state.difficulty * -0x1e + 0xb4;
              }
              else {
                owned(0x38a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
                events.sound(0x1c);
                p.variant = i16(p.variant) + -1;
                p.cycle = 8;
              }
            }
            (aimRadians = atan((double(other.motion.y) -
                         double(p.motion.y)) + double(100.0f),(double(other.motion.x) -
                         double(p.motion.x)) * (float)(int)i8(p.motion.facing)));
            uVar2 = i16(i32(aimRadians/double(0.017453289f)));
            p.timer = uVar2;
            if (i16(p.timer) < i16(p.angle)) {
              if ((int)i16(p.angle) - (int)i16(p.timer) < 0xb5) {
                p.angle = i16(p.angle) + -1;
                if (i16(p.angle) < -0xb4) {
                  p.angle = i16(p.angle) + 0x168;
                }
              }
              else {
                p.angle = i16(p.angle) + 1;
                if (0xb4 < i16(p.angle)) {
                  p.angle = i16(p.angle) + -0x168;
                }
              }
            }
            if (i16(p.angle) <= i16(p.timer)) {
              if ((int)i16(p.angle) - (int)i16(p.timer) < -0xb4) {
                p.angle = i16(p.angle) + -1;
                if (i16(p.angle) < -0xb4) {
                  p.angle = i16(p.angle) + 0x168;
                }
              }
              else {
                p.angle = i16(p.angle) + 1;
                if (0xb4 < i16(p.angle)) {
                  p.angle = i16(p.angle) + -0x168;
                }
              }
            }
          }
          if (1 < i16(owner.animation.cursor.index)) {
            p.cycle = i16(p.cycle) + -1;
            if (((i16(p.cycle) < 1) &&
                (fVar6 = (double)std::sqrt(double(float((double(other.motion.y) -
                                               double(p.motion.y)) *
                                               (double(other.motion.y) -
                                               double(p.motion.y)) +
                                               (double(other.motion.x) -
                                               double(p.motion.x)) *
                                               (double(other.motion.x) -
                                               double(p.motion.x))))), (double)double(128.0f) < fVar6)
                ) && ((double(0.0f) < double(p.motion.y) &&
                      ((double(0.0f) < double(p.motion.x) && (double(p.motion.x) < double(1280.0f)))))
                     )) {
              if (i16(p.variant) < 1) {
                owned(0x38a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
                events.sound(0x1c);
                p.variant = 5;
                p.cycle = state.difficulty * -0x1e + 0xb4;
              }
              else {
                owned(0x38a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
                events.sound(0x1c);
                p.variant = i16(p.variant) + -1;
                p.cycle = 8;
              }
            }
            (aimRadians = atan(double(owner.motion.y) - double(p.motion.y),(double(owner.motion.x) - double(p.motion.x))
                         * (float)(int)i8(p.motion.facing)));
            uVar2 = i16(i32(aimRadians/double(0.017453289f)));
            p.timer = uVar2;
            if (i16(p.timer) < i16(p.angle)) {
              if ((int)i16(p.angle) - (int)i16(p.timer) < 0xb5) {
                p.angle = i16(p.angle) + -3;
                if (i16(p.angle) < -0xb4) {
                  p.angle = i16(p.angle) + 0x168;
                }
              }
              else {
                p.angle = i16(p.angle) + 3;
                if (0xb4 < i16(p.angle)) {
                  p.angle = i16(p.angle) + -0x168;
                }
              }
            }
            if (i16(p.angle) <= i16(p.timer)) {
              if ((int)i16(p.angle) - (int)i16(p.timer) < -0xb4) {
                p.angle = i16(p.angle) + -3;
                if (i16(p.angle) < -0xb4) {
                  p.angle = i16(p.angle) + 0x168;
                }
              }
              else {
                p.angle = i16(p.angle) + 3;
                if (0xb4 < i16(p.angle)) {
                  p.angle = i16(p.angle) + -0x168;
                }
              }
            }
          }
          if (0x32 < i16(p.counter)) {
            p.counter = i16(p.counter) + -1;
          }
          p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter) * double(0.1f));
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            if (i16(owner.animation.cursor.index) < 3) {
              p.counter = i16(p.counter) + 1;
              p.frame(1);
            }
            else {
              p.destroy();
            }
          }
        }
        break;
      case 0x38a:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 6.0f;
            p.pose.scale[1] = 6.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if (i16(p.animation.cursor.index) == 0) {
            if (double(p.pose.scale[0]) <= double(1.0f)) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
            }
            else {
              p.pose.scale[0] = double(p.pose.scale[0]) - double(0.2f);
              p.pose.scale[1] = double(p.pose.scale[1]) - double(0.2f);
            }
            if (u8(channel(24)) < 0xeb) {
              setChannel(24,u8(i8(channel(24)) + '\x14'));
            }
            else {
              setChannel(24,u8(0xff));
            }
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
              else if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
                particleSpeedOrLayer = 6.0f;
                fVar6 = (double)(aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                                              double(p.motion.y),(double(other.motion.x) - double(p.motion.x)) *
                                              (float)(int)i8(p.motion.facing)));
                p.polar((float)(fVar6 / (double)double(0.017453289f)),particleSpeedOrLayer);
                fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
                p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
                p.response = 0;
                p.hits = 1;
              }
            }
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
            p.response = 0;
          }
          p.parameters[1] = double(p.parameters[1]) + double(1.0f);
          p.polar(double(p.parameters[0]),double(p.parameters[1]) * double(0.1f));
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
        break;
      case 0x38e:
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
          }
          if (i16(owner.animation.cursor.action) == 0x2f9) {
            p.motion.x =
                 (float)(i8(p.motion.facing) * 0x8c) +
                 double(owner.motion.x);
            p.motion.y = double(owner.motion.y) + double(35.0f);
          }
          if ((i16(owner.animation.cursor.action) == 0x2f9) &&
             ((i16(owner.animation.cursor.action) != 0x2f9 ||
              (i16(owner.animation.cursor.index) < 0xd)))) {
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.9f);
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(4);
            }
          }
          else {
            p.destroy();
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
            iVar5 = nextRandom(0x4b,0x96);
            p.pose.scale[0] = (float)iVar5 * double(0.01f);
            p.pose.scale[1] = double(p.pose.scale[0]);
            iVar5 = nextRandom(0,0x167);
            p.pose.rotation[2] = (float)iVar5;
            (aimRadians = atan((float)(int)i16(state.anchorY) -
                         double(p.motion.y),((float)(int)i16(state.anchorX) -
                         double(p.motion.x)) * (float)(int)i8(p.motion.facing)));
            uVar2 = i16(i32(aimRadians/double(0.017453289f)));
            p.angle = uVar2;
            p.polar((float)(int)i16(p.angle),36.0f);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) < 2) {
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
            fVar6 = (double)std::abs(float(double(p.motion.x) -
                                          (float)(int)i16(state.anchorX)));
            if ((fVar6 < (double)double(60.0f)) &&
               (fVar6 = (double)std::abs(float(double(p.motion.y) -
                                              (float)(int)i16(state.anchorY))), fVar6 < (double)double(60.0f)))
            {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              events.system_sound(4);
              uVar2 = i16(i32(double(p.motion.x)-state.anchorX));
              p.counter = uVar2;
              uVar2 = i16(i32(double(p.motion.y)-state.anchorY));
              p.timer = uVar2;
              p.frame(2);
            }
          }
          if ((i16(p.animation.cursor.index) == 2) &&
             (i16(owner.animation.cursor.action) == 0x306)) {
            if (i16(owner.animation.cursor.index) == 6) {
              p.motion.x =
                   (float)((i16(p.counter) + 0x3c) * (int)i8(p.motion.facing)) +
                   double(owner.motion.x);
              p.motion.y =
                   (float)(i16(p.timer) + 0x8c) +
                   double(owner.motion.y);
            }
            if (i16(owner.animation.cursor.index) == 7) {
              p.motion.x =
                   (float)((i16(p.counter) + 0x5e) * (int)i8(p.motion.facing)) +
                   double(owner.motion.x);
              p.motion.y =
                   (float)(i16(p.timer) + 0xb2) +
                   double(owner.motion.y);
            }
            if (i16(owner.animation.cursor.index) == 8) {
              p.motion.x =
                   (float)((i16(p.counter) + 0x96) * (int)i8(p.motion.facing)) +
                   double(owner.motion.x);
              p.motion.y =
                   (float)(i16(p.timer) + 0x52) +
                   double(owner.motion.y);
            }
            if (i16(owner.animation.cursor.index) == 9) {
              p.motion.x =
                   (float)((i16(p.counter) + 0x4b) * (int)i8(p.motion.facing)) +
                   double(owner.motion.x);
              p.motion.y =
                   (float)(i16(p.timer) + 0x11) +
                   double(owner.motion.y);
            }
            if (i16(owner.animation.cursor.index) == 10) {
              p.motion.x =
                   (float)((i16(p.counter) + -0x1e) * (int)i8(p.motion.facing)) +
                   double(owner.motion.x);
              p.motion.y =
                   (float)(i16(p.timer) + 10) +
                   double(owner.motion.y);
            }
            if (i16(owner.animation.cursor.index) == 0xb) {
              p.motion.x =
                   (float)((i16(p.counter) + -0x6e) * (int)i8(p.motion.facing)) +
                   double(owner.motion.x);
              p.motion.y =
                   (float)(i16(p.timer) + 0x55) +
                   double(owner.motion.y);
            }
            if (i16(owner.animation.cursor.index) == 0xc) {
              p.motion.x =
                   (float)((i16(p.counter) + -0x5f) * (int)i8(p.motion.facing)) +
                   double(owner.motion.x);
              p.motion.y =
                   (float)(i16(p.timer) + 0xb9) +
                   double(owner.motion.y);
            }
            if (i16(owner.animation.cursor.index) == 0xd) {
              p.motion.x =
                   (float)((i16(p.counter) + -10) * (int)i8(p.motion.facing)) +
                   double(owner.motion.x);
              p.motion.y =
                   (float)(i16(p.timer) + 0xe6) +
                   double(owner.motion.y);
            }
            if (i16(owner.animation.cursor.index) == 0xe) {
              p.motion.x =
                   (float)((i16(p.counter) + 0x36) * (int)i8(p.motion.facing)) +
                   double(owner.motion.x);
              p.motion.y =
                   (float)(i16(p.timer) + 0xa0) +
                   double(owner.motion.y);
            }
            if (i16(owner.animation.cursor.index) == 0xf) {
              p.motion.x =
                   (float)((i16(p.counter) + 0x50) * (int)i8(p.motion.facing)) +
                   double(owner.motion.x);
              p.motion.y =
                   (float)(i16(p.timer) + 0x87) +
                   double(owner.motion.y);
            }
            if (i16(owner.animation.cursor.index) == 0x10) {
              p.frame(3);
            }
          }
          if (i16(p.animation.cursor.index) == 3) {
            if ((((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(200.0f))) ||
                (double(960.0f) < double(p.motion.y))) ||
               ((i16(owner.animation.cursor.action) == 0x306 &&
                (i16(owner.animation.cursor.index) == 5)))) {
              p.destroy();
              return;
            }
            p.motion.x =
                 (float)((int)i16(state.anchorX) +
                        (int)i16(p.counter) * (int)i8(p.motion.facing));
            p.motion.y =
                 (float)((int)i16(state.anchorY) +
                        (int)i16(p.timer));
            if (i16(state.anchorY) < 1) {
              p.frame(4);
              iVar5 = nextRandom(0,0x14);
              p.motion.velocityX = (float)(iVar5 + -10);
              iVar5 = nextRandom(5,0x14);
              p.motion.velocityY = (float)-iVar5;
              return;
            }
          }
          if (i16(p.animation.cursor.index) == 4) {
            p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
            fVar8 = double(p.motion.y) + double(p.motion.velocityY);
            if ((fVar8 < double(0.0f) != (fVar8 == double(0.0f))) && (double(0.0f) < double(p.motion.velocityY))) {
              p.motion.velocityY = -double(p.motion.velocityY);
            }
            if (u8(channel(24)) < 6) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -5));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x390:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          uVar2 = i16(i32(p.motion.x));
          state.anchorX = uVar2;
          uVar2 = i16(i32(p.motion.y));
          state.anchorY = uVar2;
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.hits = 1;
            p.response = 0;
          }
          if (((i16(owner.animation.cursor.action) == 0x306) &&
              (0xf < i16(owner.animation.cursor.index))) &&
             (i16(p.animation.cursor.index) == 0)) {
            p.motion.x =
                 (float)(i8(p.motion.facing) * 0x8c) +
                 double(owner.motion.x);
            p.motion.y = double(owner.motion.y) + double(35.0f);
            p.frame(1);
            (aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                         double(p.motion.y),(double(other.motion.x) -
                         double(p.motion.x)) * (float)(int)i8(p.motion.facing)));
            uVar2 = i16(i32(aimRadians/double(0.017453289f)));
            p.angle = uVar2;
            p.polar((float)(int)i16(p.angle),(float)(state.difficulty * 3 + 0xf));
          }
          if ((i16(p.animation.cursor.index) != 1) || (double(0.0f) <= double(p.motion.y))) {
            if ((double(1700.0f) < double(p.motion.x)) ||
               ((double(p.motion.x) < -double(500.0f) || (double(1280.0f) < double(p.motion.y))))) {
              p.destroy();
            }
            else {
              p.motion.x =
                   (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                   double(p.motion.x);
              p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
              sVar3 = p.advance();
              if (sVar3 == i16(p.animation.cursor.frameCount)) {
                p.frame(1);
              }
            }
          }
          else {
            events.sound(0x17);
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
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            owned(0x392,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0);
          }
          if (i16(p.counter) < 0x3c) {
            p.counter = i16(p.counter) + 1;
          }
          p.angle = i16(p.angle) + -1;
          p.pose.rotation[2] = double(p.pose.rotation[2]) - double(5.0f);
          if (i16(p.animation.cursor.index) == 1) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.15f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.15f);
            p.cycle = i16(p.cycle) + -1;
          }
          if (i16(p.animation.cursor.index) == 2) {
            local_390 = (u32)(double(0.0f) < (double(other.motion.x)
                                     - double(p.motion.x)) *
                                     (float)(int)i8(p.motion.facing));
            owner.motion.opponentPush =
                 -((float)(int)i16(p.counter) * double(0.1f)) *
                 (float)((int)i8(p.motion.facing) *
                        (int)i8(other.motion.facing)) *
                 (float)(int)(local_390 * 2 + -1);
            events.shake(2.0f);
            if (i16(p.angle) < 1) {
              iVar5 = nextRandom(0,0x167);
              owner.launchParameters[2] = (float)iVar5;
              p.polar(double(owner.launchParameters[2]),960.0f);
              owned(0x394,(float)(int)i8(p.motion.facing) * double(p.motion.velocityX)
                                 + double(p.motion.x),double(p.motion.y) + double(p.motion.velocityY),u8(p.motion.facing),1);
              iVar5 = nextRandom(0,0x167);
              owner.launchParameters[2] = (float)iVar5;
              p.polar(double(owner.launchParameters[2]),960.0f);
              owned(0x394,(float)(int)i8(p.motion.facing) * double(p.motion.velocityX)
                                 + double(p.motion.x),double(p.motion.y) + double(p.motion.velocityY),u8(p.motion.facing),1);
              p.angle = 6 - state.difficulty;
            }
          }
          if (i16(p.animation.cursor.index) == 4) {
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            static_cast<void>(0x1f);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if (((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) &&
             ((int)i16(p.timer) < state.difficulty + 2)) {
            p.timer = i16(p.timer) + 1;
            p.frame(2);
          }
        }
        break;
      case 0x392:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.timer = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          p.counter = i16(p.counter) + 1;
          p.pose.rotation[2] = double(p.pose.rotation[2]) - double(6.0f);
          if (i16(p.animation.cursor.index) == 4) {
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if (((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) &&
             ((int)i16(p.timer) < state.difficulty + 2)) {
            p.timer = i16(p.timer) + 1;
            p.frame(2);
          }
        }
        break;
      case 0x393:
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
            iVar5 = nextRandom(0,8);
            p.motion.velocityX = (float)(iVar5 + 6);
          }
          if ((double(1180.0f) < double(p.motion.x)) || (double(p.motion.x) < double(100.0f))) {
            p.destroy();
            owned(0x391,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0);
          }
          else {
            if (i16(p.animation.cursor.index) == 1) {
              iVar5 = nextRandom(0,5);
              p.pose.scale[0] = (float)iVar5 * double(0.1f) + double(0.8f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
              owned(0x391,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0);
            }
          }
        }
        break;
      case 0x394:
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
            p.polar(-double(p.parameters[2]) + double(180.0f),30.0f);
            fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
            p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (((p.response == 0) &&
              (double(p.parameters[2]) < double(0.0f) == (double(p.parameters[2]) == double(0.0f)))) ||
             (i16(p.animation.cursor.index) != 1)) {
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
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
            }
          }
          else {
            p.frame(2);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
        }
        break;
      case 0x396:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 99;
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if (i16(p.animation.cursor.index) == 1) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.033f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.033f);
            if (double(1.0f) < double(p.pose.scale[0])) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
            }
          }
          if (i16(p.animation.cursor.index) == 3) {
            iVar5 = nextRandom(0,5);
            p.pose.scale[0] = (float)iVar5 * double(0.025f) + 0.95;
            p.pose.scale[1] = double(p.pose.scale[0]);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
          if ((((double(1800.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(680.0f))) ||
              (double(1280.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(380.0f))) {
            p.destroy();
          }
          else {
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(3);
              p.response = 0;
            }
            else if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
              events.sound(0x10);
              particleSpeedOrLayer = 5.0f;
              fVar6 = (double)(aimRadians = atan((double(other.motion.y) + double(100.0f)) - double(p.motion.y),(double(other.motion.x) - double(p.motion.x)) *
                                            (float)(int)i8(p.motion.facing)));
              p.polar((float)(fVar6 / (double)double(0.017453289f)),particleSpeedOrLayer);
            }
          }
        }
        break;
      case 0x397:
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
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if (i16(p.animation.cursor.index) == 0) {
            if (u8(channel(24)) < 0xdc) {
              setChannel(24,u8(i8(channel(24)) + '#'));
            }
            else {
              setChannel(24,u8(0xff));
            }
            p.pose.scale[0] = double(p.pose.scale[0]) - double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) - double(0.1f);
          }
          if (i16(p.animation.cursor.index) == 1) {
            if (i16(p.counter) < 0xf) {
              p.counter = i16(p.counter) + 1;
            }
            p.polar(double(p.parameters[0]),(float)(0x14 - i16(p.counter)));
          }
          if ((i16(p.animation.cursor.index) == 1) && (p.response != 0)) {
            p.frame(2);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
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
              else if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.polar(double(p.parameters[0]),20.0f);
                fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
                p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
              }
            }
          }
        }
        break;
      case 0x398:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            owned(0x399,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0);
          }
          if ((int)i16(p.counter) < state.difficulty * 8 + 10) {
            p.counter = i16(p.counter) + 1;
          }
          p.angle = i16(p.angle) + -1;
          p.pose.rotation[2] = double(p.pose.rotation[2]) - double(5.0f);
          if (i16(p.animation.cursor.index) == 1) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.15f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.15f);
            p.cycle = i16(p.cycle) + -1;
          }
          if (i16(p.animation.cursor.index) == 2) {
            local_3d8 = (u32)(double(0.0f) < (double(other.motion.x)
                                     - double(p.motion.x)) *
                                     (float)(int)i8(p.motion.facing));
            owner.motion.opponentPush =
                 -((float)(int)i16(p.counter) * double(0.1f)) *
                 (float)((int)i8(p.motion.facing) *
                        (int)i8(other.motion.facing)) *
                 (float)(int)(local_3d8 * 2 + -1);
            events.shake(2.0f);
          }
          if (i16(p.animation.cursor.index) == 4) {
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            static_cast<void>(0x1f);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(2);
          }
        }
        break;
      case 0x399:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.timer = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          p.counter = i16(p.counter) + 1;
          p.pose.rotation[2] = double(p.pose.rotation[2]) - double(6.0f);
          if (i16(p.animation.cursor.index) == 4) {
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(2);
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
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            iVar5 = nextRandom(0,0x167);
            p.pose.rotation[2] = (float)iVar5;
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
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
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            iVar5 = nextRandom(0,8);
            p.motion.velocityX = (float)(iVar5 + -4);
            iVar5 = nextRandom(0,3);
            p.motion.velocityY = (float)(-0xd - iVar5);
          }
          if (i16(p.animation.cursor.elapsed) == 0) {
            particleSpeedOrLayer = 1;
            uVar4 = (u32)u8(p.motion.facing);
            fVar8 = double(p.motion.y);
            iVar5 = nextRandom(0,0x14);
            fVar7 = double(float(double(fVar8)+10.0))-iVar5;
            fVar8 = double(p.motion.x);
            iVar5 = nextRandom(0,0x14);
            owned(0x39a,double(float(double(fVar8)+10.0))-iVar5,fVar7,uVar4,particleSpeedOrLayer);
          }
          if ((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) {
            p.destroy();
          }
          else {
            p.motion.velocityY = double(p.motion.velocityY) + double(0.2f);
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            fVar8 = double(p.motion.y) - double(p.motion.velocityY);
            if (fVar8 < double(0.0f) != (fVar8 == double(0.0f))) {
              p.destroy();
              owned(0x39d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x39d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(1);
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
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            iVar5 = nextRandom(0,0x167);
            p.pose.rotation[2] = (float)iVar5;
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
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
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            iVar5 = nextRandom(0,8);
            p.motion.velocityX = (float)(iVar5 + -4);
            iVar5 = nextRandom(0,3);
            p.motion.velocityY = (float)(-0xd - iVar5);
          }
          if (i16(p.animation.cursor.elapsed) == 0) {
            particleSpeedOrLayer = 1;
            uVar4 = (u32)u8(p.motion.facing);
            fVar8 = double(p.motion.y);
            iVar5 = nextRandom(0,10);
            fVar7 = double(float(double(fVar8)+5.0))-iVar5;
            fVar8 = double(p.motion.x);
            iVar5 = nextRandom(0,10);
            owned(0x39c,double(float(double(fVar8)+5.0))-iVar5,fVar7,uVar4,particleSpeedOrLayer);
          }
          if ((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) {
            p.destroy();
          }
          else {
            p.motion.velocityY = double(p.motion.velocityY) + double(0.2f);
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            fVar8 = double(p.motion.y) - double(p.motion.velocityY);
            if (fVar8 < double(0.0f) != (fVar8 == double(0.0f))) {
              p.destroy();
              owned(0x39f,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x39f,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(1);
            }
          }
        }
        break;
      case 0x39e:
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
            iVar5 = nextRandom(0,0x167);
            p.pose.rotation[2] = (float)iVar5;
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x39f:
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
            iVar5 = nextRandom(0,8);
            p.motion.velocityX = (float)(iVar5 + -4);
            iVar5 = nextRandom(0,3);
            p.motion.velocityY = (float)(-0xd - iVar5);
          }
          if (i16(p.animation.cursor.elapsed) == 0) {
            owned(0x39e,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          if ((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) {
            p.destroy();
          }
          else {
            p.motion.velocityY = double(p.motion.velocityY) + double(0.2f);
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            fVar8 = double(p.motion.y) - double(p.motion.velocityY);
            if (fVar8 < double(0.0f) != (fVar8 == double(0.0f))) {
              if (i16(p.counter) == 0) {
                p.motion.velocityY = -double(p.motion.velocityY) * double(0.75f);
                p.hits = 1;
                p.response = 0;
                p.counter = i16(p.counter) + 1;
                return;
              }
              p.destroy();
            }
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(1);
            }
          }
        }
        break;
      case 0x3a0:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 99;
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if (i16(p.animation.cursor.index) == 1) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.033f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.033f);
            if (double(1.0f) < double(p.pose.scale[0])) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
            }
          }
          if (i16(p.animation.cursor.index) == 3) {
            iVar5 = nextRandom(0,5);
            p.pose.scale[0] = (float)iVar5 * double(0.025f) + 0.95;
            p.pose.scale[1] = double(p.pose.scale[0]);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
          if ((((double(1800.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(680.0f))) ||
              (double(1280.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(380.0f))) {
            p.destroy();
          }
          else {
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(3);
              p.response = 0;
            }
            else if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
              events.sound(0x10);
              particleSpeedOrLayer = 5.0f;
              fVar6 = (double)(aimRadians = atan((double(other.motion.y) + double(100.0f)) - double(p.motion.y),(double(other.motion.x) - double(p.motion.x)) *
                                            (float)(int)i8(p.motion.facing)));
              p.polar((float)(fVar6 / (double)double(0.017453289f)),particleSpeedOrLayer);
            }
          }
        }
        break;
      case 0x3a1:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 99;
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if (i16(p.animation.cursor.index) == 1) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.033f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.033f);
            if (double(1.0f) < double(p.pose.scale[0])) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
            }
          }
          if (i16(p.animation.cursor.index) == 3) {
            iVar5 = nextRandom(0,5);
            p.pose.scale[0] = (float)iVar5 * double(0.025f) + 0.95;
            p.pose.scale[1] = double(p.pose.scale[0]);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
          if ((((double(1800.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(680.0f))) ||
              (double(1280.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(380.0f))) {
            p.destroy();
          }
          else {
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(3);
              p.response = 0;
            }
            else if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
              events.sound(0x10);
              particleSpeedOrLayer = 5.0f;
              fVar6 = (double)(aimRadians = atan((double(other.motion.y) + double(100.0f)) - double(p.motion.y),(double(other.motion.x) - double(p.motion.x)) *
                                            (float)(int)i8(p.motion.facing)));
              p.polar((float)(fVar6 / (double)double(0.017453289f)),particleSpeedOrLayer);
            }
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
          fVar8 = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[0] = fVar8 + fVar8;
          fVar8 = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[1] = fVar8 + fVar8;
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(10);
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
