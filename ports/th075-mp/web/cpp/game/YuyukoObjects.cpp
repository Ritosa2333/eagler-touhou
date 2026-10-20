#include "YuyukoObjects.hpp"
#include <cmath>
namespace th075 {void advance_yuyuko_object(YuyukoObject& p,FighterActor& owner,FighterActor& other,YuyukoState& state,ReplayStreams& random,FighterActionEvents& events){
auto& sceneFadeColor=events.fade_color();auto& sceneFadeAlpha=events.fade_alpha();
double aimRadians=0;const auto atan=[](float y,float x){return std::atan2(double(y),double(x));};
const auto channel=[&](u32 shift){return p.pose.channel(shift);};const auto setChannel=[&](u32 shift,u8 value){p.pose.set_channel(shift,value);};
const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters);};
const auto shared=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Shared,id,x,y,facing,layer,owner.launchParameters);};

  u16 uVar1;
  i16 sVar2;
  u32 uVar3;
  int iVar4;
  int iVar5;
  double fVar6;
  float fVar7;
  float fVar8;
  u32 uVar9;
  u32 uVar10;
  float launchSpeed;
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
          p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.2f) + double(p.parameters[1]));
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
                if (u8(channel(24)) < 0x19) {
                  setChannel(24,u8(0));
                }
                else {
                  setChannel(24,u8(i8(channel(24)) + -0x19));
                }
              }
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
          else {
            p.frame(2);
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
          p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.2f) + double(p.parameters[1]));
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
                if (u8(channel(24)) < 0x19) {
                  setChannel(24,u8(0));
                }
                else {
                  setChannel(24,u8(i8(channel(24)) + -0x19));
                }
              }
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
          else {
            p.frame(2);
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
      case 0x322:
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
          p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.2f) + double(p.parameters[1]));
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
                if (u8(channel(24)) < 0x19) {
                  setChannel(24,u8(0));
                }
                else {
                  setChannel(24,u8(i8(channel(24)) + -0x19));
                }
              }
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
          else {
            p.frame(2);
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
      case 0x323:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
                ((double(640.0f) < double(p.motion.y) || (double(p.motion.y) < -double(120.0f))))) {
          p.destroy();
        }
        else {
          if (((i16(owner.animation.cursor.action) != 700) ||
              ((i16(owner.animation.cursor.action) == 700 &&
               (9 < i16(owner.animation.cursor.index))))) &&
             (i16(p.animation.cursor.index) < 2)) {
            p.parameters[2] = double(p.parameters[2]) * double(0.9f);
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 3;
            p.response = 0;
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            p.counter = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (i16(p.animation.cursor.index) < 2) {
            p.parameters[0] = double(p.parameters[2]) * double(0.1f) + double(p.parameters[0])
            ;
          }
          p.polar(double(p.parameters[0]),double(p.parameters[1]));
          fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
          p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (1 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 5) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -5));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if (((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) < 0x32)) {
            p.frame(1);
            p.response = 0;
            p.counter = i16(p.counter) + 1;
          }
        }
        break;
      case 0x324:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
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
        if (i16(p.hits) == 0) {
          p.destroy();
        }
        else {
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          launchSpeed = 1;
          uVar3 = (u32)u8(p.motion.facing);
          fVar8 = double(p.motion.y);
          iVar4 = random.range(0,0x14);
          fVar7 = (float)iVar4 + double(float(double(fVar8)-10.0));
          fVar8 = double(p.motion.x);
          iVar4 = random.range(0,0x14);
          owned(0x325,(float)iVar4 + double(float(double(fVar8)-10.0)),fVar7,uVar3,launchSpeed);
          if (i16(p.animation.cursor.elapsed) == 0) {
            uVar1 = random.range(0,0x14);
            p.timer = uVar1;
            p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
            p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(1);
          }
          if (((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) < 0x2d)) {
            p.frame(2);
            p.counter = i16(p.counter) + 1;
          }
        }
        break;
      case 0x325:
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
          p.motion.velocityY = 8.0f;
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
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x326:
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.0f;
          p.pose.scale[1] = 0.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
          p.timer = 0;
          p.motion.velocityX = 8.0f;
          p.motion.velocityY = 0.0f;
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.timer = i16(p.timer) + -1;
        if ((i16(p.animation.cursor.index) == 2) && (i16(p.timer) < 0)) {
          iVar4 = random.range(0,0x28);
          owner.launchParameters[0] = (float)(iVar4 + -0x5a);
          iVar4 = random.range(0,0x16);
          owner.launchParameters[1] = (float)(iVar4 + 5);
          uVar9 = 1;
          uVar3 = (u32)u8(p.motion.facing);
          launchSpeed = double(p.motion.y);
          iVar4 = random.range(0,200);
          owned(0x324,(float)((100 - iVar4) * (int)i8(p.motion.facing)) +
                             double(p.motion.x),launchSpeed,uVar3,uVar9);
          p.timer = 2;
        }
        if (i16(p.animation.cursor.index) == 3) {
          if (u8(channel(24)) < 0xe) {
            setChannel(24,u8(0));
          }
          else {
            setChannel(24,u8(i8(channel(24)) + -0xd));
          }
        }
        if (i16(p.animation.cursor.index) < 2) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.8f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
          events.sound(7);
        }
        break;
      case 0x327:
        if (i16(owner.animation.cursor.action) == 0x130) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 3) + double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(110.0f);
        }
        if (i16(owner.animation.cursor.action) == 0x130) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.5f;
            p.pose.scale[1] = 1.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((i16(owner.combat.freeze) == 0) &&
             (sVar2 = p.advance(), sVar2 == i16(p.animation.cursor.frameCount))) {
            p.destroy();
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x328:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
          p.hits = 1;
          p.response = 0;
          (aimRadians = atan((double(other.motion.y) + double(128.0f)) -
                       double(p.motion.y),(double(other.motion.x) -
                       double(p.motion.x)) * (float)(int)i8(p.motion.facing)));
          uVar1 = i16(i32(aimRadians/double(0.017453289f)));
          p.angle = uVar1;
          if (i16(p.angle) < -0x14) {
            p.angle = 0xffec;
          }
          if (0x14 < i16(p.angle)) {
            p.angle = 0x14;
          }
        }
        if (i16(p.animation.cursor.index) < 3) {
          p.polar((float)(int)i16(p.angle),20.0f);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        if (i16(p.animation.cursor.index) < 3) {
          launchSpeed = 1;
          uVar3 = (u32)u8(p.motion.facing);
          fVar8 = double(p.motion.y);
          iVar4 = random.range(0,0x14);
          fVar7 = (float)iVar4 + double(float(double(fVar8)-10.0));
          fVar8 = double(p.motion.x);
          iVar4 = random.range(0,0x14);
          owned(0x329,(float)iVar4 + double(float(double(fVar8)-10.0)),fVar7,uVar3,launchSpeed);
        }
        if (p.response == 6) {
          p.hits = i16(p.hits) + 1;
          p.response = 0;
        }
        if ((i16(p.animation.cursor.index) == 2) && (p.response == 1)) {
          p.frame(3);
          static_cast<void>(9);
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          state.orbitRadius = 0;
        }
        else {
          if ((i16(p.animation.cursor.index) == 3) &&
             ((i16(p.animation.cursor.elapsed) == 0 && (p.response == 1)))) {
            for (local_14 = 0; local_14 < 8; local_14 = local_14 + 1) {
              owner.launchParameters[0] = (float)local_14;
              owned(0x32a,double(other.motion.x),double(other.motion.y) + double(100.0f),1,1);
              state.orbitRadius =
                   i16(state.orbitRadius) + 1;
            }
          }
          if (i16(p.animation.cursor.elapsed) == 0) {
            uVar1 = random.range(0,0x14);
            p.timer = uVar1;
            p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
            p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(2);
          }
        }
        break;
      case 0x329:
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
          p.motion.velocityY = 0.0f;
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
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x32a:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
        }
        p.polar(double(p.parameters[0]) * double(45.0f) +
                     (float)(int)i16(state.orbitAngle),140.0f);
        p.motion.x =
             double(p.motion.velocityX) +
             double(other.motion.x);
        p.motion.y =
             double(p.motion.velocityY) * double(0.5f) +
             double(other.motion.y) + double(100.0f);
        if (double(p.motion.velocityY) <= double(0.0f)) {
          p.depth = 5.0f;
        }
        else {
          p.depth = -5.0f;
        }
        if (i16(p.animation.cursor.index) < 0xf) {
          if (2 < i16(other.combat.health)) {
            other.combat.health =
                 i16(other.combat.health) + -1;
          }
          if ((0 < i16(other.combat.spellTimer)) &&
             (2 < i16(other.combat.maximumHealth))) {
            other.combat.maximumHealth =
                 i16(other.combat.maximumHealth) + -1;
          }
          if (u8(channel(24)) < 0xeb) {
            setChannel(24,u8(i8(channel(24)) + '\x14'));
          }
          else {
            setChannel(24,u8(0xff));
          }
        }
        if ((((((i16(other.combat.health) < 1) ||
               (i16(owner.combat.spellTimer) < 1)) ||
              (i16(owner.combat.health) < 1)) ||
             (i16(state.orbitRadius) == 0)) ||
            ((0x31 < i16(owner.animation.cursor.action) &&
             (i16(owner.animation.cursor.action) < 0x96)))) &&
           (i16(p.animation.cursor.index) < 0xf)) {
          p.frame(i16(p.animation.cursor.index) + 0xe);
          state.orbitRadius =
               i16(state.orbitRadius) + -1;
          if (i16(state.orbitRadius) < 0) {
            state.orbitRadius = 0;
          }
        }
        if (0xe < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.frame(0xf);
        }
        else if ((i16(p.animation.cursor.index) == 0xf) && (i16(p.animation.cursor.elapsed) == 0)) {
          if (double(p.parameters[0]) + double(p.parameters[0]) <=
              (float)(i16(p.counter) + -1)) {
            state.orbitRadius =
                 i16(state.orbitRadius) + -1;
            if (i16(state.orbitRadius) < 0) {
              state.orbitRadius = 0;
            }
          }
          else {
            p.frame(1);
            p.counter = i16(p.counter) + 1;
          }
        }
        break;
      default:
        p.advance();
        break;
      case 0x32c:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
          p.hits = 1;
          p.response = 0;
        }
        p.polar(double(p.parameters[0]),double(p.parameters[1]));
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        launchSpeed = 1;
        uVar3 = (u32)u8(p.motion.facing);
        fVar8 = double(p.motion.y);
        iVar4 = random.range(0,0x14);
        fVar7 = (float)iVar4 + double(float(double(fVar8)-10.0));
        fVar8 = double(p.motion.x);
        iVar4 = random.range(0,0x14);
        owned(0x33b,(float)iVar4 + double(float(double(fVar8)-10.0)),fVar7,uVar3,launchSpeed);
        if (i16(p.animation.cursor.index) == 1) {
          if ((double(owner.combat.screenBounds[2]) < double(p.motion.y)) &&
             (double(p.motion.velocityY) < double(0.0f))) {
            p.counter = 1;
          }
          if ((double(p.motion.y) < double(0.0f)) && (double(0.0f) < double(p.motion.velocityY))) {
            p.counter = 1;
          }
          if ((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
             ((float)(int)i8(p.motion.facing) * double(p.motion.velocityX) < double(0.0f))) {
            p.counter = 1;
          }
          if ((double(owner.combat.screenBounds[1]) < double(p.motion.x)) &&
             (double(0.0f) < (float)(int)i8(p.motion.facing) * double(p.motion.velocityX))) {
            p.counter = 1;
          }
          if (i16(p.counter) == 1) {
            events.sound(0);
            for (local_18 = 0; local_18 < 1; local_18 = local_18 + 1) {
              owner.launchParameters[1] = (float)(local_18 * 2 + 1);
              fVar6 = (double)(aimRadians = atan((double(other.motion.y) + double(100.0f)) - double(p.motion.y),(double(other.motion.x) - double(p.motion.x)) *
                                            (float)(int)i8(p.motion.facing)));
              owner.launchParameters[0] = (float)(fVar6 / (double)double(0.017453289f));
              owned(0x32d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            p.destroy();
            return;
          }
        }
        if (i16(p.animation.cursor.elapsed) == 0) {
          uVar1 = random.range(0,0x14);
          p.timer = uVar1;
          p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
          p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.frame(1);
        }
        if (((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) < 0x1e)) {
          p.frame(2);
          p.counter = i16(p.counter) + 1;
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
          p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.1f) + double(p.parameters[1]));
          if ((p.response == 0) || (i16(p.animation.cursor.index) != 1)) {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if ((double(1380.0f) < double(p.motion.x)) ||
               (((double(p.motion.x) < -double(100.0f) || (double(960.0f) < double(p.motion.y))) ||
                (double(p.motion.y) < -double(120.0f))))) {
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
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
          else {
            p.frame(2);
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_1c = 0; local_1c < 2; local_1c = local_1c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x32e:
        if (i16(owner.animation.cursor.action) == 0x133) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x14) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(125.0f);
        }
        if (i16(owner.animation.cursor.action) == 0x133) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 2.0f;
            p.pose.scale[1] = 2.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.projection[2] = 0x4e2;
            p.pose.rotation[0] = 110.0f;
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x32f:
        if ((i16(owner.animation.cursor.action) == 0x12d) ||
           (i16(owner.animation.cursor.action) == 0x187)) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
        }
        if ((i16(owner.animation.cursor.action) == 0x12d) ||
           (i16(owner.animation.cursor.action) == 0x187)) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.5f;
            p.pose.scale[1] = 1.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.projection[2] = 0x4e2;
            p.pose.rotation[0] = 110.0f;
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x330:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
          p.hits = 1;
          p.response = 0;
        }
        p.polar(double(p.parameters[0]),double(p.parameters[1]));
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        launchSpeed = 1;
        uVar3 = (u32)u8(p.motion.facing);
        fVar8 = double(p.motion.y);
        iVar4 = random.range(0,0x14);
        fVar7 = (float)iVar4 + double(float(double(fVar8)-10.0));
        fVar8 = double(p.motion.x);
        iVar4 = random.range(0,0x14);
        owned(0x33c,(float)iVar4 + double(float(double(fVar8)-10.0)),fVar7,uVar3,launchSpeed);
        if (i16(p.animation.cursor.index) == 1) {
          if ((double(owner.combat.screenBounds[2]) < double(p.motion.y)) &&
             (double(p.motion.velocityY) < double(0.0f))) {
            p.counter = 1;
          }
          if ((double(p.motion.y) < double(0.0f)) && (double(0.0f) < double(p.motion.velocityY))) {
            p.counter = 1;
          }
          if ((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
             ((float)(int)i8(p.motion.facing) * double(p.motion.velocityX) < double(0.0f))) {
            p.counter = 1;
          }
          if ((double(owner.combat.screenBounds[1]) < double(p.motion.x)) &&
             (double(0.0f) < (float)(int)i8(p.motion.facing) * double(p.motion.velocityX))) {
            p.counter = 1;
          }
          if (i16(p.counter) == 1) {
            events.sound(0);
            for (local_20 = 0; local_20 < 1; local_20 = local_20 + 1) {
              owner.launchParameters[1] = (float)(local_20 * 2 + 1);
              fVar6 = (double)(aimRadians = atan((double(other.motion.y) + double(100.0f)) - double(p.motion.y),(double(other.motion.x) - double(p.motion.x)) *
                                            (float)(int)i8(p.motion.facing)));
              owner.launchParameters[0] = (float)(fVar6 / (double)double(0.017453289f));
              owned(0x332,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            p.destroy();
            return;
          }
        }
        if (i16(p.animation.cursor.elapsed) == 0) {
          uVar1 = random.range(0,0x14);
          p.timer = uVar1;
          p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
          p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.frame(1);
        }
        if (((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) < 0x1e)) {
          p.frame(2);
          p.counter = i16(p.counter) + 1;
        }
        break;
      case 0x331:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
          p.hits = 1;
          p.response = 0;
        }
        p.polar(double(p.parameters[0]),double(p.parameters[1]));
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        launchSpeed = 1;
        uVar3 = (u32)u8(p.motion.facing);
        fVar8 = double(p.motion.y);
        iVar4 = random.range(0,0x14);
        fVar7 = (float)iVar4 + double(float(double(fVar8)-10.0));
        fVar8 = double(p.motion.x);
        iVar4 = random.range(0,0x14);
        owned(0x33d,(float)iVar4 + double(float(double(fVar8)-10.0)),fVar7,uVar3,launchSpeed);
        if (i16(p.animation.cursor.index) == 1) {
          if ((double(owner.combat.screenBounds[2]) < double(p.motion.y)) &&
             (double(p.motion.velocityY) < double(0.0f))) {
            p.counter = 1;
          }
          if ((double(p.motion.y) < double(0.0f)) && (double(0.0f) < double(p.motion.velocityY))) {
            p.counter = 1;
          }
          if ((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
             ((float)(int)i8(p.motion.facing) * double(p.motion.velocityX) < double(0.0f))) {
            p.counter = 1;
          }
          if ((double(owner.combat.screenBounds[1]) < double(p.motion.x)) &&
             (double(0.0f) < (float)(int)i8(p.motion.facing) * double(p.motion.velocityX))) {
            p.counter = 1;
          }
          if (i16(p.counter) == 1) {
            events.sound(0);
            for (local_24 = 0; local_24 < 1; local_24 = local_24 + 1) {
              owner.launchParameters[1] = (float)(local_24 * 2 + 1);
              fVar6 = (double)(aimRadians = atan((double(other.motion.y) + double(100.0f)) - double(p.motion.y),(double(other.motion.x) - double(p.motion.x)) *
                                            (float)(int)i8(p.motion.facing)));
              owner.launchParameters[0] = (float)(fVar6 / (double)double(0.017453289f));
              owned(0x333,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            p.destroy();
            return;
          }
        }
        if (i16(p.animation.cursor.elapsed) == 0) {
          uVar1 = random.range(0,0x14);
          p.timer = uVar1;
          p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
          p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.frame(1);
        }
        if (((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) < 0x1e)) {
          p.frame(2);
          p.counter = i16(p.counter) + 1;
        }
        break;
      case 0x332:
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
          p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.1f) + double(p.parameters[1]));
          if ((p.response == 0) || (i16(p.animation.cursor.index) != 1)) {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if ((double(1380.0f) < double(p.motion.x)) ||
               (((double(p.motion.x) < -double(100.0f) || (double(960.0f) < double(p.motion.y))) ||
                (double(p.motion.y) < -double(120.0f))))) {
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
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
          else {
            p.frame(2);
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_28 = 0; local_28 < 2; local_28 = local_28 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x333:
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
          p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.1f) + double(p.parameters[1]));
          if ((p.response == 0) || (i16(p.animation.cursor.index) != 1)) {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if ((double(1380.0f) < double(p.motion.x)) ||
               (((double(p.motion.x) < -double(100.0f) || (double(960.0f) < double(p.motion.y))) ||
                (double(p.motion.y) < -double(120.0f))))) {
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
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
          else {
            p.frame(2);
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_2c = 0; local_2c < 2; local_2c = local_2c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x334:
        if (i16(owner.animation.cursor.action) == 0x186) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x92) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(80.0f);
        }
        if (i16(owner.animation.cursor.action) == 0x186) {
          uVar9 = 1;
          launchSpeed = u8(p.motion.facing);
          fVar8 = double(p.motion.y);
          iVar4 = random.range(0,0x28);
          fVar7 = (float)iVar4 + double(float(double(fVar8)-10.0));
          fVar8 = double(p.motion.x);
          iVar4 = random.range(0,0x28);
          owned(0x325,(float)iVar4 + double(float(double(fVar8)-10.0)),fVar7,launchSpeed,uVar9);
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.5f;
            p.pose.scale[1] = 1.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (i16(p.animation.cursor.elapsed) == 0) {
            uVar1 = random.range(0,0x14);
            p.timer = uVar1;
            p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(1.5f);
            p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(1.5f);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(1);
          }
          if (((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) < 0x1e)) {
            p.frame(2);
            p.counter = i16(p.counter) + 1;
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x335:
        if ((i16(owner.animation.cursor.action) == 0x136) ||
           (i16(owner.animation.cursor.action) == 0x188)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x14) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
        }
        if ((i16(owner.animation.cursor.action) == 0x136) ||
           (i16(owner.animation.cursor.action) == 0x188)) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 2.0f;
            p.pose.scale[1] = 2.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.projection[2] = 0x4e2;
            p.pose.rotation[0] = 140.0f;
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x336:
        if ((i16(owner.animation.cursor.action) == 0x25f) ||
           (i16(owner.animation.cursor.action) == 0x261)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x69) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(145.0f);
        }
        if ((i16(owner.animation.cursor.action) == 0x25f) ||
           (i16(owner.animation.cursor.action) == 0x261)) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.20000000298023224f;
            p.pose.scale[1] = 0.20000000298023224f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.hits = 1;
            p.response = 0;
            p.counter = 0;
          }
          if (p.response == 6) {
            p.response = 0;
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.counter) == 0)) {
            if (double(1.0f) <= double(p.pose.scale[0])) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              p.counter = i16(p.counter) + 1;
            }
            else {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
            }
          }
          if ((i16(p.animation.cursor.index) < 4) && (0 < i16(p.counter))) {
            iVar4 = random.range(0,8);
            p.pose.scale[0] = (float)iVar4 * double(0.25f) + double(1.0f);
            p.pose.scale[1] = double(p.pose.scale[0]);
          }
          if (i16(p.animation.cursor.index) == 4) {
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[0]);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            for (local_30 = 0; local_30 < 5; local_30 = local_30 + 1) {
              events.sound(2);
              owner.launchParameters[0] = (float)(local_30 * -4 + 8);
              owned(0x337,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x338,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            events.shake(4.0f);
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x337:
        if ((i16(owner.animation.cursor.action) == 0x25f) ||
           (i16(owner.animation.cursor.action) == 0x261)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x69) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(145.0f);
        }
        if ((i16(owner.animation.cursor.action) == 0x25f) ||
           (i16(owner.animation.cursor.action) == 0x261)) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.10000000149011612f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[2] = double(p.parameters[0]);
            p.counter = 0;
          }
          if (i16(p.animation.cursor.index) == 1) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.0025f);
          }
          if (i16(p.animation.cursor.index) == 2) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.pose.scale[1] = double(p.pose.scale[1]) + double(3.0f);
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x338:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[1] = 70.0f;
          p.pose.projection[2] = 0x4e2;
          iVar4 = random.range(0,0x167);
          p.pose.rotation[2] = (float)iVar4;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
        if (u8(channel(24)) < 0x15) {
          p.destroy();
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -0x14));
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x339:
        if ((i16(owner.animation.cursor.action) == 0x260) ||
           (i16(owner.animation.cursor.action) == 0x262)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x69) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(145.0f);
        }
        if ((i16(owner.animation.cursor.action) == 0x260) ||
           (i16(owner.animation.cursor.action) == 0x262)) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.20000000298023224f;
            p.pose.scale[1] = 0.20000000298023224f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.hits = 3;
            p.response = 0;
            p.counter = 0;
          }
          if (p.response == 6) {
            p.response = 0;
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.counter) == 0)) {
            if (double(1.0f) <= double(p.pose.scale[0])) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              p.counter = i16(p.counter) + 1;
            }
            else {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
            }
          }
          if ((i16(p.animation.cursor.index) < 6) && (0 < i16(p.counter))) {
            iVar4 = random.range(0,8);
            p.pose.scale[0] = (float)iVar4 * double(0.25f) + double(1.0f);
            p.pose.scale[1] = double(p.pose.scale[0]);
          }
          if (i16(p.animation.cursor.index) == 6) {
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[0]);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            for (local_34 = 0; local_34 < 5; local_34 = local_34 + 1) {
              events.sound(2);
              owner.launchParameters[0] = (float)(local_34 * -4 + 8);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x338,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            events.shake(4.0f);
            p.response = 0;
          }
          if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x338,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            events.shake(4.0f);
            p.response = 0;
          }
          if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x338,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            events.shake(4.0f);
            p.response = 0;
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x33a:
        if ((i16(owner.animation.cursor.action) == 0x260) ||
           (i16(owner.animation.cursor.action) == 0x262)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x69) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(145.0f);
        }
        if ((i16(owner.animation.cursor.action) == 0x260) ||
           (i16(owner.animation.cursor.action) == 0x262)) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.10000000149011612f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[2] = double(p.parameters[0]);
            p.counter = 0;
          }
          if (i16(p.animation.cursor.index) == 1) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.0025f);
          }
          if (1 < i16(p.animation.cursor.index)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.pose.scale[1] = double(p.pose.scale[1]) + double(3.0f);
          }
          if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.pose.scale[1] = double(p.pose.scale[1]) + double(3.0f);
          }
          if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.pose.scale[1] = double(p.pose.scale[1]) + double(3.0f);
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x33b:
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
          p.motion.velocityY = 6.0f;
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
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33c:
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
          p.motion.velocityY = 6.0f;
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
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33d:
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
          p.motion.velocityY = 6.0f;
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
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33e:
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
            p.counter = 100;
          }
          p.counter = i16(p.counter) + -1;
          if (i16(p.counter) < 0) {
            p.counter = 0;
          }
          p.polar(0.0f,(float)(int)i16(p.counter) * double(p.parameters[1]) * double(0.01f))
          ;
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
                if (u8(channel(24)) < 0x19) {
                  setChannel(24,u8(0));
                }
                else {
                  setChannel(24,u8(i8(channel(24)) + -0x19));
                }
              }
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
          else {
            p.frame(2);
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_38 = 0; local_38 < 3; local_38 = local_38 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x33f:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (3 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.hits = 0;
            p.pose.scale[0] = 0.5f;
            p.pose.scale[1] = 0.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.angle = 0;
          }
          if (i16(p.animation.cursor.index) < 3) {
            p.polar((float)((int)i16(state.orbitAngle) << 1) +
                         double(p.parameters[0]),(float)(int)i16(p.counter));
            p.counter = i16(p.counter) + 10;
            if (0x50 < i16(p.counter)) {
              p.counter = 0x50;
            }
            p.motion.x = double(p.motion.velocityX) + double(p.parameters[1]);
            p.motion.y = double(p.motion.velocityY) + double(p.parameters[2]);
            if ((((i16(owner.animation.cursor.action) == 0x263) ||
                 (i16(owner.animation.cursor.action) == 0x264)) ||
                (i16(owner.animation.cursor.action) == 0x265)) &&
               (i16(owner.animation.cursor.index) == 0)) {
              p.frame(3);
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              return;
            }
          }
          launchSpeed = 1;
          uVar3 = (u32)u8(p.motion.facing);
          fVar8 = double(p.motion.y);
          iVar4 = random.range(0,0x14);
          fVar7 = (float)iVar4 + double(float(double(fVar8)-10.0));
          fVar8 = double(p.motion.x);
          iVar4 = random.range(0,0x14);
          owned(0x33b,(float)iVar4 + double(float(double(fVar8)-10.0)),fVar7,uVar3,launchSpeed);
          if (i16(p.animation.cursor.elapsed) == 0) {
            uVar1 = random.range(0,0x14);
            p.timer = uVar1;
            p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
            p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              if (i16(p.angle) < 5) {
                p.frame(1);
                p.angle = i16(p.angle) + 1;
              }
              else {
                p.angle = 0;
              }
            }
            if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
              if (i16(p.angle) < 0xf) {
                p.angle = i16(p.angle) + 1;
                p.frame(2);
              }
              else {
                p.destroy();
              }
            }
          }
        }
        else {
          p.destroy();
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x340:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
                ((double(640.0f) < double(p.motion.y) || (double(p.motion.y) < -double(120.0f))))) {
          p.destroy();
        }
        else {
          if (((i16(owner.animation.cursor.action) != 0x2c0) ||
              ((i16(owner.animation.cursor.action) == 0x2c0 &&
               (9 < i16(owner.animation.cursor.index))))) &&
             (i16(p.animation.cursor.index) < 2)) {
            p.parameters[2] = double(p.parameters[2]) * double(0.9f);
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 3;
            p.response = 0;
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            p.counter = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (i16(p.animation.cursor.index) < 2) {
            p.parameters[0] = double(p.parameters[2]) * double(0.1f) + double(p.parameters[0])
            ;
          }
          p.polar(double(p.parameters[0]),double(p.parameters[1]));
          fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
          p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (1 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 5) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -5));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if (((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) < 0x32)) {
            p.frame(1);
            p.response = 0;
            p.counter = i16(p.counter) + 1;
          }
        }
        break;
      case 0x341:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
                ((double(640.0f) < double(p.motion.y) || (double(p.motion.y) < -double(120.0f))))) {
          p.destroy();
        }
        else {
          if (((i16(owner.animation.cursor.action) != 0x2c0) ||
              ((i16(owner.animation.cursor.action) == 0x2c0 &&
               (9 < i16(owner.animation.cursor.index))))) &&
             (i16(p.animation.cursor.index) < 2)) {
            p.parameters[2] = double(p.parameters[2]) * double(0.75f);
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 3;
            p.response = 0;
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            p.counter = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (i16(p.animation.cursor.index) < 2) {
            p.parameters[0] = double(p.parameters[2]) * double(0.1f) + double(p.parameters[0])
            ;
          }
          p.polar(double(p.parameters[0]),double(p.parameters[1]));
          fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
          p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (1 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 5) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -5));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if (((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) < 0x32)) {
            p.frame(1);
            p.response = 0;
            p.counter = i16(p.counter) + 1;
          }
        }
        break;
      case 0x342:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (0x25 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 2;
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
          if (i16(p.animation.cursor.index) < 0x26) {
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.1f) + double(p.parameters[1]));
          }
          if ((p.response == 4) && (0 < i16(p.hits))) {
            p.response = 0;
          }
          if ((p.response == 0) || (0x25 < i16(p.animation.cursor.index))) {
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
              if (0x25 < i16(p.animation.cursor.index)) {
                if (u8(channel(24)) < 0xf) {
                  p.destroy();
                  return;
                }
                setChannel(24,u8(i8(channel(24)) + -0xf));
                p.pose.scale[0] = double(p.pose.scale[0]) * double(0.9f);
                p.pose.scale[1] = double(p.pose.scale[0]);
              }
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
              if ((i16(p.animation.cursor.index) == 0x26) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.frame(0x15);
              }
            }
          }
          else {
            p.frame(0x26);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_3c = 0; local_3c < 3; local_3c = local_3c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x343:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.5f;
          p.pose.scale[1] = 1.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 40.0f;
          p.pose.rotation[1] = (float)(i8(p.motion.facing) * -0x28);
          iVar4 = random.range(0,0x167);
          p.pose.rotation[2] = (float)iVar4;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.05f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.05f);
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x344:
        if (((0x31 < i16(owner.animation.cursor.action)) &&
            (i16(owner.animation.cursor.action) < 0x96)) &&
           (i16(p.animation.cursor.index) < 3)) {
          p.frame(3);
        }
        if (((i16(owner.animation.cursor.action) == 0x2c2) &&
            (i16(owner.animation.cursor.index) == 0)) && (i16(p.animation.cursor.index) < 3))
        {
          p.frame(3);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.0f;
          p.pose.scale[1] = 0.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
          p.timer = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.timer = i16(p.timer) + -1;
        if (i16(p.animation.cursor.index) == 2) {
          p.motion.velocityX = 0.0f;
          if (double(other.motion.x) + double(40.0f) <
              double(p.motion.x)) {
            p.motion.velocityX = (float)(i8(p.motion.facing) * -2);
          }
          if (double(p.motion.x) <
              double(other.motion.x) - double(40.0f)) {
            p.motion.velocityX = (float)((int)i8(p.motion.facing) << 1);
          }
        }
        if ((i16(p.animation.cursor.index) == 2) && (i16(p.timer) < 0)) {
          iVar4 = random.range(0,0x50);
          owner.launchParameters[0] = (float)(-0x32 - iVar4);
          iVar4 = random.range(0,0x12);
          owner.launchParameters[1] = (float)(iVar4 + 0xf);
          uVar9 = 1;
          uVar3 = (u32)u8(p.motion.facing);
          launchSpeed = double(p.motion.y);
          iVar4 = random.range(0,200);
          owned(0x345,(float)((100 - iVar4) * (int)i8(p.motion.facing)) +
                             double(p.motion.x),launchSpeed,uVar3,uVar9);
          p.timer = 1;
        }
        if (i16(p.animation.cursor.index) == 3) {
          if (u8(channel(24)) < 0xe) {
            setChannel(24,u8(0));
          }
          else {
            setChannel(24,u8(i8(channel(24)) + -0xd));
          }
        }
        if (i16(p.animation.cursor.index) < 2) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.8f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
          events.sound(7);
        }
        break;
      case 0x345:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
          p.hits = 1;
          p.response = 0;
        }
        p.polar(double(p.parameters[0]),double(p.parameters[1]));
        if ((i16(p.animation.cursor.index) < 2) && (double(p.parameters[1]) == double(2.0f))) {
          p.frame(2);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        launchSpeed = 1;
        uVar3 = (u32)u8(p.motion.facing);
        fVar8 = double(p.motion.y);
        iVar4 = random.range(0,0x14);
        fVar7 = (float)iVar4 + double(float(double(fVar8)-10.0));
        fVar8 = double(p.motion.x);
        iVar4 = random.range(0,0x14);
        owned(0x325,(float)iVar4 + double(float(double(fVar8)-10.0)),fVar7,uVar3,launchSpeed);
        if (i16(p.animation.cursor.elapsed) == 0) {
          uVar1 = random.range(0,0x14);
          p.timer = uVar1;
          p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
          p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if (((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) < 0x1e)) {
          p.frame(2);
          p.counter = i16(p.counter) + 1;
        }
        break;
      case 0x346:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (3 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            if (i16(owner.animation.cursor.action) == 0x265) {
              p.hits = 1;
            }
            else {
              p.hits = 0;
            }
            p.pose.scale[0] = 0.5f;
            p.pose.scale[1] = 0.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.angle = 0;
          }
          if (i16(p.animation.cursor.index) < 3) {
            p.polar((float)((int)i16(state.orbitAngle) << 1) +
                         double(p.parameters[0]),(float)(int)i16(p.counter));
            p.counter = i16(p.counter) + 10;
            if (0x78 < i16(p.counter)) {
              p.counter = 0x78;
            }
            p.motion.x = double(p.motion.velocityX) + double(p.parameters[1]);
            p.motion.y = double(p.motion.velocityY) + double(p.parameters[2]);
            if ((((i16(p.hits) == 0) &&
                 (i16(owner.animation.cursor.action) != 0x263)) &&
                (i16(owner.animation.cursor.action) != 0x264)) ||
               ((((i16(owner.animation.cursor.action) == 0x263 ||
                  (i16(owner.animation.cursor.action) == 0x264)) ||
                 (i16(owner.animation.cursor.action) == 0x265)) &&
                (i16(owner.animation.cursor.index) == 0)))) {
              p.frame(3);
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              return;
            }
          }
          launchSpeed = 1;
          uVar3 = (u32)u8(p.motion.facing);
          fVar8 = double(p.motion.y);
          iVar4 = random.range(0,0x14);
          fVar7 = (float)iVar4 + double(float(double(fVar8)-10.0));
          fVar8 = double(p.motion.x);
          iVar4 = random.range(0,0x14);
          owned(0x33b,(float)iVar4 + double(float(double(fVar8)-10.0)),fVar7,uVar3,launchSpeed);
          if (i16(p.animation.cursor.elapsed) == 0) {
            uVar1 = random.range(0,0x14);
            p.timer = uVar1;
            p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
            p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              if (i16(p.angle) < 5) {
                p.frame(1);
                p.angle = i16(p.angle) + 1;
              }
              else {
                p.angle = 0;
              }
            }
            if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
              if (i16(p.angle) < 0x28) {
                p.angle = i16(p.angle) + 1;
                p.frame(2);
              }
              else {
                p.destroy();
              }
            }
          }
        }
        else {
          p.destroy();
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x347:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (3 < i16(p.animation.cursor.index))) {
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
            launchSpeed = 4.0f;
            iVar4 = random.range(0,0x3c);
            p.polar((float)(iVar4 + -0x1e),launchSpeed);
            p.hits = 1;
            p.response = 0;
          }
          if (p.response == 0) {
            if (double(other.motion.y) + double(100.0f) <
                double(p.motion.y)) {
              p.motion.velocityY = double(p.motion.velocityY) - double(0.02f);
            }
            if (double(p.motion.y) <=
                double(other.motion.y) + double(100.0f)) {
              p.motion.velocityY = double(p.motion.velocityY) + double(0.02f);
            }
            p.motion.velocityX = double(p.motion.velocityX) + double(0.05f);
            if (i16(p.counter) < 1) {
              p.response = 0;
              p.counter = 10;
            }
            p.counter = i16(p.counter) + -1;
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            if (i16(p.animation.cursor.elapsed) == 0) {
              launchSpeed = 1;
              uVar3 = (u32)u8(p.motion.facing);
              fVar8 = double(p.motion.y);
              iVar4 = random.range(0,0x14);
              fVar7 = (float)iVar4 + double(float(double(fVar8)-10.0));
              fVar8 = double(p.motion.x);
              iVar4 = random.range(0,0x14);
              owned(0x348,(float)iVar4 + double(float(double(fVar8)-10.0)),fVar7,uVar3,launchSpeed);
            }
            if (i16(p.animation.cursor.elapsed) == 0) {
              uVar1 = random.range(0,0x14);
              p.timer = uVar1;
              p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
              p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              if ((i16(p.angle) < 0x3c) && (0 < i16(p.hits))) {
                p.frame(1);
                p.angle = i16(p.angle) + 1;
              }
              else {
                p.destroy();
              }
            }
          }
          else {
            p.destroy();
          }
        }
        else {
          p.destroy();
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
      case 0x348:
        p.motion.facing = u8(owner.motion.facing);
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
        p.motion.velocityY = double(p.motion.velocityY) - double(0.1f);
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
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x349:
        if (i16(owner.animation.cursor.action) == 0x135) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x14) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(40.0f);
        }
        if (i16(owner.animation.cursor.action) == 0x135) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 2.0f;
            p.pose.scale[1] = 2.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.projection[2] = 0x4e2;
            p.pose.rotation[0] = -70.0f;
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x34a:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
        }
        p.polar(double(p.parameters[0]) * double(45.0f) +
                     (float)(int)i16(state.orbitAngle),140.0f);
        p.motion.x =
             double(p.motion.velocityX) +
             double(other.motion.x);
        p.motion.y =
             double(p.motion.velocityY) * double(0.5f) +
             double(other.motion.y) + double(100.0f);
        if (double(p.motion.velocityY) <= double(0.0f)) {
          p.depth = 5.0f;
        }
        else {
          p.depth = -5.0f;
        }
        if (i16(p.animation.cursor.index) < 0x15) {
          if (2 < i16(other.combat.health)) {
            other.combat.health =
                 i16(other.combat.health) + -1;
          }
          if ((0 < i16(other.combat.spellTimer)) &&
             (2 < i16(other.combat.maximumHealth))) {
            other.combat.maximumHealth =
                 i16(other.combat.maximumHealth) + -1;
          }
          if (u8(channel(24)) < 0xeb) {
            setChannel(24,u8(i8(channel(24)) + '\x14'));
          }
          else {
            setChannel(24,u8(0xff));
          }
        }
        if ((((((i16(other.combat.health) < 1) ||
               (i16(owner.combat.spellTimer) < 1)) ||
              (i16(owner.combat.health) < 1)) ||
             (i16(state.orbitRadius) == 0)) ||
            ((0x31 < i16(owner.animation.cursor.action) &&
             (i16(owner.animation.cursor.action) < 0x96)))) &&
           (i16(p.animation.cursor.index) < 0x15)) {
          p.frame(i16(p.animation.cursor.index) + 0x14);
          state.orbitRadius =
               i16(state.orbitRadius) + -1;
          if (i16(state.orbitRadius) < 0) {
            state.orbitRadius = 0;
          }
        }
        if (0x14 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.frame(0x15);
        }
        else if ((i16(p.animation.cursor.index) == 0x15) && (i16(p.animation.cursor.elapsed) == 0)) {
          if (double(p.parameters[0]) * double(5.0f) <= (float)(i16(p.counter) + -1)) {
            state.orbitRadius =
                 i16(state.orbitRadius) + -1;
            if (i16(state.orbitRadius) < 0) {
              state.orbitRadius = 0;
            }
          }
          else {
            p.frame(1);
            p.counter = i16(p.counter) + 1;
          }
        }
        break;
      case 0x34b:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
          p.hits = 1;
          p.response = 0;
          (aimRadians = atan((double(other.motion.y) + double(128.0f)) -
                       double(p.motion.y),(double(other.motion.x) -
                       double(p.motion.x)) * (float)(int)i8(p.motion.facing)));
          uVar1 = i16(i32(aimRadians/double(0.017453289f)));
          p.angle = uVar1;
          if (i16(p.angle) < -0x28) {
            p.angle = 0xffd8;
          }
          if (0x28 < i16(p.angle)) {
            p.angle = 0x28;
          }
        }
        if (i16(p.animation.cursor.index) < 3) {
          p.polar((float)(int)i16(p.angle),20.0f);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        if (i16(p.animation.cursor.index) < 3) {
          launchSpeed = 1;
          uVar3 = (u32)u8(p.motion.facing);
          fVar8 = double(p.motion.y);
          iVar4 = random.range(0,0x14);
          fVar7 = (float)iVar4 + double(float(double(fVar8)-10.0));
          fVar8 = double(p.motion.x);
          iVar4 = random.range(0,0x14);
          owned(0x329,(float)iVar4 + double(float(double(fVar8)-10.0)),fVar7,uVar3,launchSpeed);
        }
        if (p.response == 6) {
          p.hits = i16(p.hits) + 1;
          p.response = 0;
        }
        if ((i16(p.animation.cursor.index) == 2) && (p.response == 1)) {
          static_cast<void>(0xe);
          p.frame(3);
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          state.orbitRadius = 0;
        }
        else {
          if ((i16(p.animation.cursor.index) == 3) &&
             ((i16(p.animation.cursor.elapsed) == 0 && (p.response == 1)))) {
            for (local_40 = 0; local_40 < 8; local_40 = local_40 + 1) {
              owner.launchParameters[0] = (float)local_40;
              owned(0x34a,double(other.motion.x),double(other.motion.y) + double(100.0f),1,1);
              state.orbitRadius =
                   i16(state.orbitRadius) + 1;
            }
          }
          if (i16(p.animation.cursor.elapsed) == 0) {
            uVar1 = random.range(0,0x14);
            p.timer = uVar1;
            p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
            p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.5f);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(2);
          }
        }
        break;
      case 0x34c:
        if (i16(owner.animation.cursor.action) == 300) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if (i16(owner.animation.cursor.action) == 300) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((i16(owner.combat.freeze) == 0) &&
             (sVar2 = p.advance(), sVar2 == i16(p.animation.cursor.frameCount))) {
            p.destroy();
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x34d:
        if (i16(owner.animation.cursor.action) == 0x12e) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(owner.animation.cursor.action) == 0x12e) &&
           ((i16(owner.animation.cursor.action) != 0x12e ||
            (i16(owner.animation.cursor.index) != 0)))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((i16(owner.combat.freeze) == 0) &&
             (sVar2 = p.advance(), sVar2 == i16(p.animation.cursor.frameCount))) {
            p.destroy();
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x34e:
        if (i16(owner.animation.cursor.action) == 0x131) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if (i16(owner.animation.cursor.action) == 0x131) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
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
        if ((i16(owner.animation.cursor.action) == 0x12f) ||
           (i16(owner.animation.cursor.action) == 0x189)) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(owner.animation.cursor.action) == 0x12f) ||
           (i16(owner.animation.cursor.action) == 0x189)) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((i16(owner.combat.freeze) == 0) &&
             (sVar2 = p.advance(), sVar2 == i16(p.animation.cursor.frameCount))) {
            p.destroy();
          }
        }
        else {
          p.destroy();
        }
        break;
      case 899:
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
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.frame(0);
        }
        break;
      case 900:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
                ((double(640.0f) < double(p.motion.y) || (double(p.motion.y) < -double(120.0f))))) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 3;
            p.response = 0;
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          p.polar(double(p.parameters[0]),double(p.parameters[1]));
          fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
          p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
          if ((i16(p.counter) == 0) &&
             (p.parameters[1] = double(p.parameters[1]) - double(0.2f),
             double(p.parameters[1]) < double(1.0f))) {
            p.parameters[1] = 1.0f;
            p.counter = 1;
            p.frame(2);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (2 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 5) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -5));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.response = 0;
            p.frame(1);
          }
          if (((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) &&
             ((int)i16(p.counter) < state.difficulty * 2)) {
            p.frame(2);
            p.response = 0;
            p.counter = i16(p.counter) + 1;
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
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.timer = 0;
          }
          p.timer = i16(p.timer) + -1;
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.timer) < 0)) {
            iVar4 = random.range(0,0x28);
            owner.launchParameters[0] = (float)(-0x46 - iVar4);
            iVar4 = random.range(0,6);
            owner.launchParameters[1] = (float)(iVar4 + 0xf);
            uVar9 = 1;
            uVar3 = (u32)u8(p.motion.facing);
            launchSpeed = double(p.motion.y);
            iVar4 = random.range(0,0x14);
            owned(0x387,(float)((10 - iVar4) * (int)i8(p.motion.facing)) +
                               double(p.motion.x),launchSpeed,uVar3,uVar9);
            if ((state.difficulty == '\x03') && (iVar4 = random.range(0,100), iVar4 < 0x14)) {
              uVar9 = 1;
              uVar3 = (u32)u8(p.motion.facing);
              launchSpeed = double(p.motion.y);
              iVar4 = random.range(0,0x14);
              owned(0x388,(float)((10 - iVar4) * (int)i8(p.motion.facing)) +
                                 double(p.motion.x),launchSpeed,uVar3,uVar9);
            }
            p.timer = 3;
          }
          if (i16(p.animation.cursor.index) == 3) {
            if (u8(channel(24)) < 0xe) {
              setChannel(24,u8(0));
            }
            else {
              setChannel(24,u8(i8(channel(24)) + -0xd));
            }
          }
          if ((i16(p.animation.cursor.index) < 2) && (i16(p.animation.cursor.elapsed) < 10)) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.4f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            events.sound(7);
          }
          if (((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) < 0)) {
            p.frame(2);
            p.counter = i16(p.counter) + 1;
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
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.timer = 0;
            p.hits = 1;
            p.response = 0;
          }
          p.polar(double(p.parameters[0]),double(p.parameters[1]));
          if ((i16(p.animation.cursor.index) < 2) && (double(p.parameters[1]) == double(2.0f))) {
            p.frame(2);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          p.timer = i16(p.timer) + -1;
          if (i16(p.timer) < 1) {
            launchSpeed = 1;
            uVar3 = (u32)u8(p.motion.facing);
            fVar8 = double(p.motion.y);
            iVar4 = random.range(0,0x14);
            fVar7 = (float)iVar4 + double(float(double(fVar8)-10.0));
            fVar8 = double(p.motion.x);
            iVar4 = random.range(0,0x14);
            owned(0x325,(float)iVar4 + double(float(double(fVar8)-10.0)),fVar7,uVar3,launchSpeed);
            p.timer = 4;
          }
          if (i16(p.animation.cursor.elapsed) == 0) {
            uVar1 = random.range(0,0x14);
            p.timer = uVar1;
            p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(1.0f);
            p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(1.0f);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if (((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) < 0x1e)) {
            p.frame(2);
            p.counter = i16(p.counter) + 1;
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
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.hits = 1;
            p.response = 0;
            iVar4 = random.range(0,0x5a);
            p.parameters[0] = (float)(-0x1e - iVar4);
            iVar4 = random.range(0,9);
            p.parameters[1] = (float)(iVar4 + 8);
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            iVar4 = random.range(0,10);
            p.motion.velocityX = (float)(iVar4 + -3);
            iVar4 = random.range(0,10);
            p.motion.velocityY = (float)(iVar4 + 5);
          }
          if (i16(p.animation.cursor.index) == 2) {
            p.motion.velocityY = double(p.motion.velocityY) - double(0.4f);
            if (double(p.motion.velocityY) < -double(4.0f)) {
              p.motion.velocityY = -4.0f;
            }
            fVar8 = double(p.motion.y) + double(p.motion.velocityY);
            if (fVar8 < double(0.0f) != (fVar8 == double(0.0f))) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.frame(3);
            }
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          p.angle = i16(p.angle) + -1;
          if (i16(p.angle) < 1) {
            launchSpeed = 1;
            uVar3 = (u32)u8(p.motion.facing);
            fVar8 = double(p.motion.y);
            iVar4 = random.range(0,0x14);
            fVar7 = (float)iVar4 + double(float(double(fVar8)-10.0));
            fVar8 = double(p.motion.x);
            iVar4 = random.range(0,0x14);
            owned(0x325,(float)iVar4 + double(float(double(fVar8)-10.0)),fVar7,uVar3,launchSpeed);
            p.angle = 2;
          }
          if ((p.response == 0) || (i16(p.animation.cursor.index) != 2)) {
            if (i16(p.animation.cursor.elapsed) == 0) {
              uVar1 = random.range(0,0x14);
              p.timer = uVar1;
              p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.02f) + double(0.9f);
              p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.02f) + double(0.9f);
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if (((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.counter) < 0xf)) {
              p.counter = i16(p.counter) + 1;
              p.frame(1);
            }
            if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(2);
            }
          }
          else {
            p.frame(3);
          }
        }
        break;
      case 0x389:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.5f;
            p.pose.scale[1] = 0.5f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            static_cast<void>(6);
            if ((state.difficulty < '\x02') && (i16(p.counter) < 5)) {
              uVar9 = 1;
              uVar3 = (u32)u8(p.motion.facing);
              launchSpeed = 0;
              sVar2 = i16(p.counter);
              iVar4 = random.range(0,0x32);
              owned(0x386,(float)(sVar2 * 300 + iVar4),launchSpeed,uVar3,uVar9);
            }
            if ((state.difficulty == '\x02') && (i16(p.counter) < 5)) {
              uVar9 = 1;
              uVar3 = (u32)u8(p.motion.facing);
              launchSpeed = 0;
              sVar2 = i16(p.counter);
              iVar4 = random.range(0,0x32);
              owned(0x386,(float)(sVar2 * 0x10e + iVar4),launchSpeed,uVar3,uVar9);
            }
            if ((state.difficulty == '\x03') && (i16(p.counter) < 6)) {
              uVar9 = 1;
              uVar3 = (u32)u8(p.motion.facing);
              launchSpeed = 0;
              sVar2 = i16(p.counter);
              iVar4 = random.range(0,0x32);
              owned(0x386,(float)(sVar2 * 0xf0 + iVar4),launchSpeed,uVar3,uVar9);
            }
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if (((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) < 0xd)) {
            p.counter = i16(p.counter) + 1;
            p.frame(1);
          }
        }
        break;
      case 0x38a:
        if (i16(p.animation.cursor.index) < 3) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if (((i16(owner.animation.cursor.action) == 0x2f8) ||
            (i16(owner.animation.cursor.action) == 0x2f9)) ||
           (2 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,150.0f,0.0f);
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.05000000074505806f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
          }
          if ((i16(p.animation.cursor.index) == 0) &&
             (p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f),
             double(1.0f) < double(p.pose.scale[0]))) {
            p.pose.scale[0] = 1.0f;
          }
          if (i16(p.animation.cursor.index) == 0) {
            sceneFadeColor = 0xffffff;
            if (i16(p.counter) == 0) {
              sceneFadeAlpha = 0xff;
              p.counter = 1;
            }
            if (i16(p.counter) == 1) {
              if (sceneFadeAlpha < 0x10) {
                sceneFadeAlpha = 0;
                p.counter = 2;
              }
              else {
                sceneFadeAlpha = sceneFadeAlpha - 0xf;
              }
            }
          }
          if ((i16(p.animation.cursor.index) == 1) &&
             (p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f),
             double(1.0f) < double(p.pose.scale[1]))) {
            p.pose.scale[1] = 1.0f;
          }
          if (i16(p.animation.cursor.index) == 3) {
            if (u8(channel(24)) < 0x1a) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x19));
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(2);
            owned(0x38b,double(p.motion.x),double(p.motion.y),1,0);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            state.aimAngle = 1;
          }
          if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(2);
          }
        }
        else {
          p.frame(3);
        }
        break;
      case 0x38b:
        if (i16(p.animation.cursor.index) < 3) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,150.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if (i16(p.animation.cursor.index) == 0) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.01f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.01f);
          if (u8(channel(24)) < 0x1a) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x19));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x38c:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            uVar1 = i16(i32(p.parameters[0]));
            p.timer = uVar1;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 0x100;
            p.response = 0;
            p.polar((float)(int)i16(p.timer),(float)(int)i16(p.counter) * double(0.1f) + double(5.0f));
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          p.counter = i16(p.counter) + 1;
          p.polar((float)(int)i16(p.timer),(float)(int)i16(p.counter) * double(0.1f) + double(5.0f));
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if ((((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
              (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(160.0f))) {
            p.destroy();
          }
          else {
            if ((0x14 < u8(channel(24))) && (i16(p.animation.cursor.index) == 2)) {
              setChannel(24,u8(i8(channel(24)) + -0x14));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.frame(1);
              p.response = 0;
            }
          }
        }
        break;
      case 0x38d:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            uVar1 = i16(i32(p.parameters[0]));
            p.timer = uVar1;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.polar(double(p.parameters[0]),1.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (i16(p.animation.cursor.index) < 2) {
            p.parameters[0] = double(p.parameters[0]) + double(1.0f);
          }
          p.counter = i16(p.counter) + 1;
          p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.05f))
          ;
          if (i16(p.animation.cursor.index) == 1) {
            p.polar(double(p.parameters[0]),1.0f);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
          p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
          if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
             ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f))))) {
            p.destroy();
          }
          else if ((p.response == 0) || (2 < i16(p.animation.cursor.index))) {
            if ((0x14 < u8(channel(24))) && (i16(p.animation.cursor.index) == 3)) {
              setChannel(24,u8(i8(channel(24)) + -0x14));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.response = 0;
              p.counter = 0;
              fVar6 = (double)(aimRadians = atan((double(other.motion.y) + double(100.0f)) - double(p.motion.y),(double(other.motion.x) - double(p.motion.x)) *
                                            (float)(int)i8(p.motion.facing)));
              p.parameters[0] = (float)(fVar6 / (double)double(0.017453289f));
            }
          }
          else {
            p.frame(3);
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
            p.pose.rotation[2] = double(p.parameters[1]);
            if ((double(p.parameters[1]) <= double(0.0f)) || (double(180.0f) <= double(p.parameters[1]))) {
              p.pose.scale[0] = 6.0f;
            }
            else {
              fVar6 = (double)angle_sin(double(p.parameters[1]));
              p.pose.scale[0] =
                   (float)((((double)double(p.motion.y) / fVar6) / (double)double(256.0f)) *
                          (double)double(1.1f));
            }
            p.pose.scale[1] = 2.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
          }
          if ((((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 5)) &&
              (double(0.0f) < double(p.parameters[1]))) && (double(p.parameters[1]) < double(180.0f))) {
            uVar10 = 1;
            uVar9 = 1;
            launchSpeed = 0;
            fVar6 = (double)angle_tan(double(90.0f) - double(p.parameters[1]));
            owned(0x38f,(float)(fVar6 * (double)double(p.motion.y) + (double)double(640.0f)),launchSpeed,uVar9,uVar10);
          }
          p.pose.scale[1] = double(p.pose.scale[1]) * 0.85;
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
            p.counter = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.motion.velocityY = 0.0f;
            p.motion.velocityX = 0.0f;
            p.pose.scale[0] = 0.25f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[0] = 0.0f;
            p.pose.rotation[1] = 0.0f;
            p.pose.rotation[2] = 0.0f;
          }
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x390,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          if (i16(p.animation.cursor.index) < 3) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.07f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.07f);
          }
          if (i16(p.animation.cursor.index) == 4) {
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            if (u8(channel(24)) < 0xf) {
              setChannel(24,u8(0));
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0xf));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x390:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.25f;
            p.pose.scale[1] = 0.25f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.projection[2] = 0x4e2;
            p.pose.rotation[0] = -75.0f;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.15f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.15f);
          if (1 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0x15) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x14));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
            p.pose.rotation[2] = double(p.parameters[1]);
            if ((double(p.parameters[1]) <= double(0.0f)) || (double(180.0f) <= double(p.parameters[1]))) {
              p.pose.scale[0] = 6.0f;
            }
            else {
              fVar6 = (double)angle_sin(double(p.parameters[1]));
              p.pose.scale[0] =
                   (float)((((double)double(p.motion.y) / fVar6) / (double)double(256.0f)) *
                          (double)double(1.1f));
            }
            p.pose.scale[1] = 2.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 5)) &&
              (double(0.0f) < double(p.parameters[1]))) && (double(p.parameters[1]) < double(180.0f))) {
            uVar10 = 1;
            uVar9 = 1;
            launchSpeed = 0;
            fVar6 = (double)angle_tan(double(90.0f) - double(p.parameters[1]));
            owned(0x392,(float)(fVar6 * (double)double(p.motion.y) + (double)double(640.0f)),launchSpeed,uVar9,uVar10);
          }
          p.pose.scale[1] = double(p.pose.scale[1]) * 0.85;
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
            p.counter = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.motion.velocityY = 0.0f;
            p.motion.velocityX = 0.0f;
            p.pose.scale[0] = 0.25f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[0] = 0.0f;
            p.pose.rotation[1] = 0.0f;
            p.pose.rotation[2] = 0.0f;
          }
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x393,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          if (i16(p.animation.cursor.index) < 3) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.07f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.07f);
          }
          if (i16(p.animation.cursor.index) == 4) {
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            if (u8(channel(24)) < 0xf) {
              setChannel(24,u8(0));
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0xf));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
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
            p.pose.scale[0] = 0.25f;
            p.pose.scale[1] = 0.25f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.projection[2] = 0x4e2;
            p.pose.rotation[0] = -75.0f;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.15f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.15f);
          if (1 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0x15) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x14));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
          }
          p.counter = i16(p.counter) + 1;
          p.parameters[0] = double(p.parameters[0]) + double(p.parameters[2]);
          p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.2f) + double(p.parameters[1]));
          fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
          p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
              (double(640.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(120.0f))) {
            p.destroy();
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
        }
        break;
      case 0x395:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
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
          }
          p.counter = i16(p.counter) + 1;
          p.parameters[0] = double(p.parameters[0]) + double(p.parameters[2]);
          p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.2f) + double(p.parameters[1]));
          fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
          p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
             ((double(640.0f) < double(p.motion.y) || (double(p.motion.y) < -double(120.0f))))) {
            p.destroy();
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
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
          }
          p.counter = i16(p.counter) + 1;
          p.parameters[0] = double(p.parameters[0]) + double(p.parameters[2]);
          p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.2f) + double(p.parameters[1]));
          fVar6 = (double)(aimRadians = atan(double(p.motion.velocityY),double(p.motion.velocityX)));
          p.pose.rotation[2] = (float)(fVar6 / (double)double(0.017453289f));
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
              (double(640.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(120.0f))) {
            p.destroy();
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
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
            p.counter = 0;
            p.timer = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.hits = 1;
            p.response = 0;
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            if ((int)i16(p.counter) < 2 - state.difficulty) {
              p.counter = i16(p.counter) + 1;
            }
            else {
              launchSpeed = 1;
              iVar4 = random.range(0,100);
              iVar5 = (u32)(iVar4 < 0x32) * -2 + 1;
              iVar4 = random.range(0,0x78);
              fVar8 = (float)(iVar4 + 700);
              iVar4 = random.range(0,0x4b0);
              owned(0x388,(float)(iVar4 + 0x3c),fVar8,iVar5,launchSpeed);
              p.counter = 0;
            }
            p.frame(2);
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
          sVar2 = random.range(0,0x168);
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
          sVar2 = random.range(0,0x168);
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
          fVar8 = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[0] = fVar8 + fVar8;
          fVar8 = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[1] = fVar8 + fVar8;
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.frame(9);
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
