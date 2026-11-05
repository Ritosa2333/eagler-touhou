#include "PatchouliObjects.hpp"
#include <cmath>
namespace th075 {void advance_patchouli_object(PatchouliObject& p,FighterActor& owner,FighterActor& other,PatchouliState& state,ReplayStreams& random,FighterActionEvents& events){
const auto consumeSpirit=[&](i16 amount){if(owner.combat.spiritBreak<1){owner.combat.spirit=i16(u16(owner.combat.spirit)-u16(amount));if(owner.combat.spirit<1&&owner.combat.spiritBreak<1){events.system_sound(43);owner.combat.spirit=0;owner.combat.spiritBreak=1000;}}else owner.combat.spiritBreak=1000;};
const auto shared=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Shared,id,x,y,facing,layer,owner.launchParameters);};
double aimRadians=0;const auto atan=[](float y,float x){return std::atan2(double(y),double(x));};
const auto channel=[&](u32 shift){return p.pose.channel(shift);};const auto setChannel=[&](u32 shift,u8 value){p.pose.set_channel(shift,value);};
const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters);};
const auto ownedVisual=[&](i16 id,float x,float y,i8 facing,u8 layer,u8 visual,u8 order){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters,order,visual?&owner.effectVisual:nullptr);};

  u16 uVar1;
  i16 sVar2;
  u32 uVar3;
  int roll;
  double fVar5;
  float angleRadians;
  float fVar7;
  u32 uVar8;
  u32 uVar9;
  int local_ac;
  int local_a8;
  int local_a4;
  int local_a0;
  int local_9c;
  int local_98;
  int local_94;
  int local_90;
  int local_8c;
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
        if ((i16(owner.animation.cursor.action) == 700) &&
           ((i16(owner.animation.cursor.action) != 700 ||
            (i16(owner.animation.cursor.index) < 0xd)))) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x42) +
               double(owner.motion.x);
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
            p.motion.velocityY = 10.0f;
          }
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          p.motion.velocityY = double(p.motion.velocityY) - double(1.0f);
          if (double(p.motion.velocityY) < double(0.0f)) {
            p.motion.velocityY = 0.0f;
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.frame(1);
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x321:
        if (i16(owner.animation.cursor.action) == 700) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.counter = 0;
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if (i16(p.animation.cursor.index) < 6) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
            if (double(1.0f) < double(p.pose.scale[0])) {
              p.pose.scale[0] = 2.0f;
            }
            if (double(1.0f) < double(p.pose.scale[1])) {
              p.pose.scale[1] = 2.0f;
            }
          }
          if ((i16(p.animation.cursor.index) < 6) ||
             (angleRadians = double(p.motion.y) + double(p.motion.velocityY),
             angleRadians < double(0.0f) == (angleRadians == double(0.0f)))) {
            if ((i16(owner.animation.cursor.index) == 9) &&
               (i16(p.animation.cursor.index) < 6)) {
              p.frame(i16(p.animation.cursor.index) + 5);
              p.motion.velocityX =
                   ((double(other.motion.x) -
                    double(p.motion.x)) * (float)(int)i8(p.motion.facing)) / double(12.0f);
              if (double(p.motion.velocityX) < double(6.0f)) {
                p.motion.velocityX = 6.0f;
              }
              p.motion.velocityY = -24.0f;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.frame(6);
            }
            else if ((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
          }
          else {
            p.motion.y = 0.0f;
            p.motion.velocityY = 0.0f;
            p.motion.velocityX = 0.0f;
            events.sound(0xb);
            p.action(0x322);
            owned(0x347,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            shared(0x7f,double(p.motion.x),0.0f,u8(p.motion.facing),1);
            for (local_8 = 0; local_8 < 0x14; local_8 = local_8 + 1) {
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            events.shake(10.0f);
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x322:
        if (p.response == 5) {
          p.destroy();
        }
        else {
          if (p.response == 6) {
            p.response = 0;
            p.hits = 1;
          }
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
            p.pose.rotation[2] = 15.0f;
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x323:
        if (p.response == 5) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.counter = 0;
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.1f) + double(40.0f));
            p.pose.scale[0] = 0.10000000149011612f;
            p.pose.scale[1] = 0.10000000149011612f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          p.counter = i16(p.counter) + 1;
          if ((i16(p.animation.cursor.index) < 2) &&
             (p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.1f) + double(25.0f)),
             double(p.pose.scale[0]) < double(1.0f))) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
          }
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
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
                if (u8(channel(24)) < 0x19) {
                  setChannel(24,u8(0));
                  p.destroy();
                  return;
                }
                setChannel(24,u8(i8(channel(24)) + -0x19));
                setChannel(16,u8(i8(channel(16)) + -0x19));
                setChannel(8,u8(i8(channel(8)) + -0x19));
                p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
                p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
              }
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
          else {
            p.frame(2);
            p.pose.scale[0] = 4.0f;
            p.pose.scale[1] = 4.0f;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
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
            p.hits = 1;
            p.response = 0;
            p.counter = 0;
            p.polar(double(p.parameters[0]),15.0f);
            p.pose.scale[0] = 0.10000000149011612f;
            p.pose.scale[1] = 0.10000000149011612f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          p.counter = i16(p.counter) + 1;
          if ((i16(p.animation.cursor.index) < 2) &&
             (p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.1f) + double(30.0f)),
             double(p.pose.scale[0]) < double(1.0f))) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
          }
          if ((p.response == 0) || (i16(p.animation.cursor.index) != 1)) {
            angleRadians = double(p.motion.y) - double(p.motion.velocityY);
            if (angleRadians < double(0.0f) == (angleRadians == double(0.0f))) {
              p.motion.x =
                   (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                   double(p.motion.x);
              p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
              if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f)))
                  || (double(640.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(120.0f)))
              {
                p.destroy();
              }
              else {
                if (1 < i16(p.animation.cursor.index)) {
                  if (u8(channel(24)) < 0x19) {
                    setChannel(24,u8(0));
                    p.destroy();
                    return;
                  }
                  setChannel(24,u8(i8(channel(24)) + -0x19));
                  setChannel(16,u8(i8(channel(16)) + -0x19));
                  setChannel(8,u8(i8(channel(8)) + -0x19));
                  p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
                  p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
                }
                sVar2 = p.advance();
                if (sVar2 == i16(p.animation.cursor.frameCount)) {
                  p.destroy();
                }
              }
            }
            else {
              p.action(0x325);
              p.motion.y = 0.0f;
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
            }
          }
          else {
            p.frame(2);
            p.pose.scale[0] = 4.0f;
            p.pose.scale[1] = 4.0f;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
        }
        else {
          p.destroy();
          for (local_c = 0; local_c < 3; local_c = local_c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x325:
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
            p.hits = 1;
            p.response = 0;
            events.sound(0x17);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        else {
          p.destroy();
          for (local_10 = 0; local_10 < 3; local_10 = local_10 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      default:
        p.advance();
        break;
      case 0x328:
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((double(1380.0f) < double(p.motion.x)) ||
             (((double(p.motion.x) < -double(100.0f) || (double(1000.0f) < double(p.motion.y))) ||
              (double(p.motion.y) < -double(240.0f))))) {
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
              p.polar(double(p.parameters[0]),13.0f);
              p.pose.rotation[2] = double(p.parameters[0]);
              p.counter = 0;
              p.hits = 1;
              p.response = 0;
            }
            if (p.response == 6) {
              p.response = 0;
              p.hits = i16(p.hits) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            uVar8 = 0;
            uVar3 = (u32)u8(p.motion.facing);
            roll = random.range(0,0x3c);
            angleRadians = ((float)roll + double(p.motion.y)) - double(30.0f);
            roll = random.range(0,0x3c);
            owned(0x32b,((float)roll + double(p.motion.x)) - double(30.0f),angleRadians,uVar3,uVar8)
            ;
            if ((i16(p.hits) == 0) ||
               (angleRadians = double(p.motion.y) + double(p.motion.velocityY),
               angleRadians < double(0.0f) != (angleRadians == double(0.0f)))) {
              p.destroy();
              events.shake(5.0f);
              events.sound(1);
              owned(0x339,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            else {
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.frame(1);
              }
            }
          }
        }
        else {
          p.destroy();
          for (local_14 = 0; local_14 < 10; local_14 = local_14 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x329:
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((double(1380.0f) < double(p.motion.x)) ||
             (((double(p.motion.x) < -double(100.0f) || (double(1000.0f) < double(p.motion.y))) ||
              (double(p.motion.y) < -double(240.0f))))) {
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
              p.motion.velocityX = 20.0f;
              p.counter = 0;
              p.hits = 1;
              p.response = 0;
            }
            if (p.response == 6) {
              p.response = 0;
              p.hits = i16(p.hits) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            uVar8 = 0;
            uVar3 = (u32)u8(p.motion.facing);
            roll = random.range(0,0x3c);
            angleRadians = ((float)roll + double(p.motion.y)) - double(30.0f);
            roll = random.range(0,0x3c);
            owned(0x32b,((float)roll + double(p.motion.x)) - double(30.0f),angleRadians,uVar3,uVar8)
            ;
            if (i16(p.hits) == 0) {
              p.destroy();
              events.shake(5.0f);
              events.sound(1);
              owned(0x339,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            else {
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.frame(1);
              }
            }
          }
        }
        else {
          p.destroy();
          for (local_18 = 0; local_18 < 10; local_18 = local_18 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x32a:
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((double(1380.0f) < double(p.motion.x)) ||
             (((double(p.motion.x) < -double(100.0f) || (double(1000.0f) < double(p.motion.y))) ||
              (double(p.motion.y) < -double(240.0f))))) {
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
              p.motion.velocityX = 10.0f;
              p.counter = 0;
              p.hits = 1;
              p.response = 0;
            }
            if (p.response == 6) {
              p.response = 0;
              p.hits = i16(p.hits) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            uVar8 = 0;
            uVar3 = (u32)u8(p.motion.facing);
            roll = random.range(0,0x3c);
            angleRadians = ((float)roll + double(p.motion.y)) - double(30.0f);
            roll = random.range(0,0x3c);
            owned(0x32b,((float)roll + double(p.motion.x)) - double(30.0f),angleRadians,uVar3,uVar8)
            ;
            if (i16(p.hits) == 0) {
              p.destroy();
              events.shake(5.0f);
              events.sound(1);
              owned(0x339,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            else {
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.frame(1);
              }
            }
          }
        }
        else {
          p.destroy();
          for (local_1c = 0; local_1c < 10; local_1c = local_1c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x32b:
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
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x32c:
        p.motion.facing = u8(owner.motion.facing);
        if (i16(owner.animation.cursor.action) == 0x131) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
          if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
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
      case 0x32d:
        if (i16(owner.animation.cursor.action) == 0x12d) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.5f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x32e:
        p.motion.facing = u8(owner.motion.facing);
        if (i16(owner.animation.cursor.action) == 0x131) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
          if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
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
        if (i16(owner.animation.cursor.action) == 0x133) {
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
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x330:
        if (((i16(owner.animation.cursor.action) < 0x46) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (2 < i16(p.animation.cursor.index))) {
          if (p.response == 6) {
            p.hits = i16(p.hits) + 1;
            p.response = 0;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,60.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.hits = 1;
            p.response = 0;
            p.motion.velocityY = 26.5f;
            events.sound(3);
            owned(0x345,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 1) {
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(30.0f);
            p.motion.velocityY = double(p.motion.velocityY) - double(1.0f);
            if (double(p.motion.velocityY) < double(0.0f)) {
              p.motion.velocityY = 0.0f;
            }
          }
          if (i16(p.animation.cursor.index) == 2) {
            p.motion.velocityY = double(p.motion.velocityY) - double(0.5f);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            angleRadians = double(p.motion.y) + double(p.motion.velocityY);
            if ((angleRadians < double(0.0f) != (angleRadians == double(0.0f))) ||
               ((p.response != 0 && (p.response != 6)))) {
              p.motion.velocityY = 0.0f;
              p.frame(3);
              events.sound(5);
              return;
            }
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            events.sound(4);
            p.motion.velocityX = -20.0f;
            p.pose.rotation[2] = 0.0f;
          }
        }
        else {
          p.destroy();
          for (local_20 = 0; local_20 < 5; local_20 = local_20 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
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
          state.blockEffectSent = 0;
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
      case 0x332:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x333:
        if (((i16(owner.animation.cursor.action) < 0x46) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (0x16 < i16(p.animation.cursor.index))) {
          if (p.response == 5) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.pose.scale[0] = 0.10000000149011612f;
              p.pose.scale[1] = 0.10000000149011612f;
              setChannel(24,u8(0));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
              p.counter = 0;
              p.timer = 0;
              p.angle = 0xaa;
              p.hits = 0;
              p.response = 0;
            }
            p.polar(30.0f,(float)(int)i16(p.angle) * double(0.1f));
            p.angle = i16(p.angle) + -6;
            if (i16(p.angle) < 0x14) {
              p.angle = 0x14;
            }
            if (i16(p.counter) == 0) {
              if (double(1.0f) <= double(p.pose.scale[0])) {
                p.pose.scale[0] = 1.0f;
                p.pose.scale[1] = 1.0f;
                setChannel(24,u8(0xff));
              }
              else {
                p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
                p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
                setChannel(24,u8(i8(channel(24)) + '\x19'));
              }
              if ((i8(channel(24)) == -1) && (i16(p.counter) == 0)) {
                p.counter = i16(p.counter) + 1;
                p.hits = 10;
              }
            }
            if (((p.response == 6) || (p.response == 4)) &&
               (0 < i16(p.hits))) {
              p.response = 0;
            }
            if (p.response != 0) {
              p.counter = i16(p.counter) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 0x17)) &&
               (0 < i16(p.counter))) {
              p.frame(0x17);
            }
            else {
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
              else if (((i16(p.animation.cursor.index) == 0x17) && (i16(p.animation.cursor.elapsed) == 0)) &&
                      (i16(p.timer) < 0)) {
                p.frame(1);
                p.timer = i16(p.timer) + 1;
              }
            }
          }
        }
        else {
          p.destroy();
          for (local_24 = 0; local_24 < 8; local_24 = local_24 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x334:
        if (((i16(owner.animation.cursor.action) < 0x46) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (0x16 < i16(p.animation.cursor.index))) {
          if (p.response == 5) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.pose.scale[0] = 0.10000000149011612f;
              p.pose.scale[1] = 0.10000000149011612f;
              setChannel(24,u8(0));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
              p.counter = 0;
              p.timer = 0;
              p.angle = 0xaa;
              p.hits = 0;
              p.response = 0;
            }
            p.polar(0.0f,(float)(int)i16(p.angle) * double(0.1f));
            p.angle = i16(p.angle) + -6;
            if (i16(p.angle) < 0x14) {
              p.angle = 0x14;
            }
            if (i16(p.counter) == 0) {
              if (double(1.0f) <= double(p.pose.scale[0])) {
                p.pose.scale[0] = 1.0f;
                p.pose.scale[1] = 1.0f;
                setChannel(24,u8(0xff));
              }
              else {
                p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
                p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
                setChannel(24,u8(i8(channel(24)) + '\x19'));
              }
              if ((i8(channel(24)) == -1) && (i16(p.counter) == 0)) {
                p.counter = i16(p.counter) + 1;
                p.hits = 10;
              }
            }
            if (((p.response == 6) || (p.response == 4)) &&
               (0 < i16(p.hits))) {
              p.response = 0;
            }
            if (p.response != 0) {
              p.counter = i16(p.counter) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 0x17)) &&
               (0 < i16(p.counter))) {
              p.frame(0x17);
            }
            else {
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
              else if (((i16(p.animation.cursor.index) == 0x17) && (i16(p.animation.cursor.elapsed) == 0)) &&
                      (i16(p.timer) < 0)) {
                p.frame(1);
                p.timer = i16(p.timer) + 1;
              }
            }
          }
        }
        else {
          p.destroy();
          for (local_28 = 0; local_28 < 8; local_28 = local_28 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x335:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (6 < i16(p.animation.cursor.index))) {
          if (p.response == 5) {
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
              p.motion.velocityX = 10.0f;
              p.motion.velocityY = 5.0f;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
        }
        else {
          p.destroy();
          for (local_2c = 0; local_2c < 4; local_2c = local_2c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x336:
        if (((i16(owner.animation.cursor.action) < 0x46) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (0x16 < i16(p.animation.cursor.index))) {
          if (p.response == 5) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.pose.scale[0] = 0.10000000149011612f;
              p.pose.scale[1] = 0.10000000149011612f;
              setChannel(24,u8(0));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
              p.counter = 0;
              p.timer = 0;
              p.angle = 0xaa;
              p.hits = 0;
              p.response = 0;
            }
            p.polar(-30.0f,(float)(int)i16(p.angle) * double(0.1f));
            p.angle = i16(p.angle) + -6;
            if (i16(p.angle) < 0x14) {
              p.angle = 0x14;
            }
            if (i16(p.counter) == 0) {
              if (double(1.0f) <= double(p.pose.scale[0])) {
                p.pose.scale[0] = 1.0f;
                p.pose.scale[1] = 1.0f;
                setChannel(24,u8(0xff));
              }
              else {
                p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
                p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
                setChannel(24,u8(i8(channel(24)) + '\x19'));
              }
              if ((i8(channel(24)) == -1) && (i16(p.counter) == 0)) {
                p.counter = i16(p.counter) + 1;
                p.hits = 10;
              }
            }
            if (((p.response == 6) || (p.response == 4)) &&
               (0 < i16(p.hits))) {
              p.response = 0;
            }
            if (p.response != 0) {
              p.counter = i16(p.counter) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 0x17)) &&
               (0 < i16(p.counter))) {
              p.frame(0x17);
            }
            else {
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
              else if (((i16(p.animation.cursor.index) == 0x17) && (i16(p.animation.cursor.elapsed) == 0)) &&
                      (i16(p.timer) < 0)) {
                p.frame(1);
                p.timer = i16(p.timer) + 1;
              }
            }
          }
        }
        else {
          p.destroy();
          for (local_30 = 0; local_30 < 8; local_30 = local_30 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x337:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x338:
        if ((i16(owner.animation.cursor.action) < 0x46) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(owner.animation.cursor.action) == 0x25c) &&
             (i16(owner.animation.cursor.index) == 0)) {
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
              p.hits = 5;
              p.response = 0;
            }
            if (i16(p.animation.cursor.index) == 1) {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.15f);
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.15f);
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              owned(0x337,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              if ((i16(p.counter) < 5) && (0 < i16(p.hits))) {
                p.response = 0;
                p.counter = i16(p.counter) + 1;
                p.frame(2);
              }
              else {
                p.destroy();
              }
            }
            else if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              events.sound(8);
            }
          }
        }
        else {
          p.destroy();
          for (local_34 = 0; local_34 < 0xd; local_34 = local_34 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x339:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.projection[2] = 0x4e2;
          p.pose.rotation[0] = -30.0f;
          p.pose.rotation[1] = (float)(i8(p.motion.facing) * -0x2d);
          p.counter = 0;
        }
        p.pose.scale[0] =
             (double(0.3f) - (float)(int)i16(p.counter) * double(0.03f) *
                    (float)(i16(p.counter) < 10)) + double(0.02f) + double(p.pose.scale[0]);
        p.pose.scale[1] =
             (double(0.3f) - (float)(int)i16(p.counter) * double(0.03f) *
                    (float)(i16(p.counter) < 10)) + double(0.02f) + double(p.pose.scale[1]);
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
        break;
      case 0x33a:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          sVar2 = random.range(0,0xd);
          p.counter = sVar2 + 0xc;
          uVar1 = random.range(0,0x167);
          p.timer = uVar1;
          roll = random.range(0,0x1e);
          p.pose.scale[0] = (float)(roll + 0x14) * double(0.1f);
          p.pose.scale[1] = double(p.pose.scale[0]) * double(0.5f);
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.counter = i16(p.counter) + -1;
        if (i16(p.counter) < 4) {
          p.counter = 4;
        }
        p.polar((float)(int)i16(p.timer),(float)(int)i16(p.counter));
        fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
        p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.92f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.92f);
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(640.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(120.0f))) {
          p.destroy();
        }
        else {
          if (1 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 10) {
              setChannel(24,u8(0));
            }
            else {
              setChannel(24,u8(i8(channel(24)) + -10));
            }
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x33b:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if (p.response == 5) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.hits = 3;
              p.response = 0;
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              setChannel(24,u8(0xff));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
              p.counter = 10;
              p.timer = 0;
              p.cycle = 0;
              uVar1 = i16(i32(p.parameters[0]));
              p.angle = uVar1;
            }
            if (i16(p.animation.cursor.index) < 2) {
              aimRadians = atan((double(other.motion.y) -
                           double(p.motion.y)) + double(100.0f),(double(other.motion.x) -
                           double(p.motion.x)) * (float)(int)i8(p.motion.facing));
              uVar1 = i32(aimRadians/double(0.017453289f));
              p.timer = uVar1;
              if (i16(p.angle) + 10 < (int)i16(p.timer)) {
                p.angle = i16(p.angle) + 1;
              }
              if ((int)i16(p.timer) < i16(p.angle) + -10) {
                p.angle = i16(p.angle) + -1;
              }
              if (0x5a < i16(p.angle)) {
                p.angle = 0x5a;
              }
              if (i16(p.angle) < -0x5a) {
                p.angle = 0xffa6;
              }
            }
            if (i16(p.animation.cursor.index) < 2) {
              p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter) * double(0.1f) + double(15.0f));
              owned(0x33c,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              roll = random.range(0,4);
              p.pose.scale[0] = (float)(roll + -2) * 0.05 + double(1.0f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            p.counter = i16(p.counter) + 2;
            if ((0 < i16(p.animation.cursor.index)) && (i16(p.timer) < 0x3c)) {
              if (double(other.motion.y) + double(100.0f) <
                  double(p.motion.y)) {
                p.parameters[0] = double(p.parameters[0]) + double(2.0f);
              }
              if (double(p.motion.y) <
                  double(other.motion.y) + double(100.0f)) {
                p.parameters[0] = double(p.parameters[0]) - double(2.0f);
              }
              p.timer = i16(p.timer) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((p.response != 0) && (p.response != 6)) &&
               (i16(p.animation.cursor.index) == 1)) {
              if ((i16(p.hits) == 0) || (p.response != 4)) {
                p.motion.velocityX = 0.0f;
                p.motion.velocityY = 0.0f;
                p.frame(2);
                p.pose.scale[0] = 2.5f;
                p.pose.scale[1] = 2.5f;
                return;
              }
              if (((0 < i16(p.hits)) && (p.response == 4)) &&
                 (0 < i16(p.hits))) {
                p.response = 0;
              }
            }
            if (1 < i16(p.animation.cursor.index)) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
              if (u8(channel(24)) < 0x19) {
                setChannel(24,u8(0));
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x19));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          for (local_38 = 0; local_38 < 3; local_38 = local_38 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x33c:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
        if (u8(channel(24)) < 0x14) {
          setChannel(24,u8(0));
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33d:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if (p.response == 5) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.hits = 3;
              p.response = 0;
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              setChannel(24,u8(0xff));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
              p.counter = 10;
              p.timer = 0;
              p.cycle = 0;
              uVar1 = i16(i32(p.parameters[0]));
              p.angle = uVar1;
            }
            if (i16(p.animation.cursor.index) < 2) {
              aimRadians = atan((double(other.motion.y) -
                           double(p.motion.y)) + double(100.0f),(double(other.motion.x) -
                           double(p.motion.x)) * (float)(int)i8(p.motion.facing));
              uVar1 = i32(aimRadians/double(0.017453289f));
              p.timer = uVar1;
              if (i16(p.angle) + 10 < (int)i16(p.timer)) {
                p.angle = i16(p.angle) + 1;
              }
              if ((int)i16(p.timer) < i16(p.angle) + -10) {
                p.angle = i16(p.angle) + -1;
              }
              if (0x5a < i16(p.angle)) {
                p.angle = 0x5a;
              }
              if (i16(p.angle) < -0x5a) {
                p.angle = 0xffa6;
              }
            }
            if (i16(p.animation.cursor.index) < 2) {
              p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter) * double(0.1f) + double(15.0f));
              owned(0x33e,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              roll = random.range(0,4);
              p.pose.scale[0] = (float)(roll + -2) * 0.05 + double(1.0f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            p.counter = i16(p.counter) + 2;
            if ((0 < i16(p.animation.cursor.index)) && (i16(p.timer) < 0x3c)) {
              if (double(other.motion.y) + double(100.0f) <
                  double(p.motion.y)) {
                p.parameters[0] = double(p.parameters[0]) + double(2.0f);
              }
              if (double(p.motion.y) <
                  double(other.motion.y) + double(100.0f)) {
                p.parameters[0] = double(p.parameters[0]) - double(2.0f);
              }
              p.timer = i16(p.timer) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((p.response != 0) && (p.response != 6)) &&
               (i16(p.animation.cursor.index) == 1)) {
              if ((i16(p.hits) == 0) || (p.response != 4)) {
                p.motion.velocityX = 0.0f;
                p.motion.velocityY = 0.0f;
                p.frame(2);
                p.pose.scale[0] = 2.5f;
                p.pose.scale[1] = 2.5f;
                return;
              }
              if (((0 < i16(p.hits)) && (p.response == 4)) &&
                 (0 < i16(p.hits))) {
                p.response = 0;
              }
            }
            if (1 < i16(p.animation.cursor.index)) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
              if (u8(channel(24)) < 0x19) {
                setChannel(24,u8(0));
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x19));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          for (local_3c = 0; local_3c < 3; local_3c = local_3c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x33e:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
        if (u8(channel(24)) < 0x14) {
          setChannel(24,u8(0));
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33f:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if (p.response == 5) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.hits = 3;
              p.response = 0;
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              setChannel(24,u8(0xff));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
              p.counter = 10;
              p.timer = 0;
              p.cycle = 0;
              uVar1 = i16(i32(p.parameters[0]));
              p.angle = uVar1;
            }
            if (i16(p.animation.cursor.index) < 2) {
              aimRadians = atan((double(other.motion.y) -
                           double(p.motion.y)) + double(100.0f),(double(other.motion.x) -
                           double(p.motion.x)) * (float)(int)i8(p.motion.facing));
              uVar1 = i32(aimRadians/double(0.017453289f));
              p.timer = uVar1;
              if (i16(p.angle) + 10 < (int)i16(p.timer)) {
                p.angle = i16(p.angle) + 1;
              }
              if ((int)i16(p.timer) < i16(p.angle) + -10) {
                p.angle = i16(p.angle) + -1;
              }
              if (0x5a < i16(p.angle)) {
                p.angle = 0x5a;
              }
              if (i16(p.angle) < -0x5a) {
                p.angle = 0xffa6;
              }
            }
            if (i16(p.animation.cursor.index) < 2) {
              p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter) * double(0.1f) + double(15.0f));
              owned(0x340,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              roll = random.range(0,4);
              p.pose.scale[0] = (float)(roll + -2) * 0.05 + double(1.0f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            p.counter = i16(p.counter) + 2;
            if ((0 < i16(p.animation.cursor.index)) && (i16(p.timer) < 0x3c)) {
              if (double(other.motion.y) + double(100.0f) <
                  double(p.motion.y)) {
                p.parameters[0] = double(p.parameters[0]) + double(2.0f);
              }
              if (double(p.motion.y) <
                  double(other.motion.y) + double(100.0f)) {
                p.parameters[0] = double(p.parameters[0]) - double(2.0f);
              }
              p.timer = i16(p.timer) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((p.response != 0) && (p.response != 6)) &&
               (i16(p.animation.cursor.index) == 1)) {
              if ((i16(p.hits) == 0) || (p.response != 4)) {
                p.motion.velocityX = 0.0f;
                p.motion.velocityY = 0.0f;
                p.frame(2);
                p.pose.scale[0] = 2.5f;
                p.pose.scale[1] = 2.5f;
                return;
              }
              if (((0 < i16(p.hits)) && (p.response == 4)) &&
                 (0 < i16(p.hits))) {
                p.response = 0;
              }
            }
            if (1 < i16(p.animation.cursor.index)) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
              if (u8(channel(24)) < 0x19) {
                setChannel(24,u8(0));
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x19));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          for (local_40 = 0; local_40 < 3; local_40 = local_40 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
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
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
        if (u8(channel(24)) < 0x14) {
          setChannel(24,u8(0));
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x341:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if (p.response == 5) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.hits = 3;
              p.response = 0;
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              setChannel(24,u8(0xff));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
              p.counter = 10;
              p.timer = 0;
              p.cycle = 0;
              uVar1 = i16(i32(p.parameters[0]));
              p.angle = uVar1;
            }
            if (i16(p.animation.cursor.index) < 2) {
              aimRadians = atan((double(other.motion.y) -
                           double(p.motion.y)) + double(100.0f),(double(other.motion.x) -
                           double(p.motion.x)) * (float)(int)i8(p.motion.facing));
              uVar1 = i32(aimRadians/double(0.017453289f));
              p.timer = uVar1;
              if (i16(p.angle) + 10 < (int)i16(p.timer)) {
                p.angle = i16(p.angle) + 1;
              }
              if ((int)i16(p.timer) < i16(p.angle) + -10) {
                p.angle = i16(p.angle) + -1;
              }
              if (0x5a < i16(p.angle)) {
                p.angle = 0x5a;
              }
              if (i16(p.angle) < -0x5a) {
                p.angle = 0xffa6;
              }
            }
            if (i16(p.animation.cursor.index) < 2) {
              p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter) * double(0.1f) + double(15.0f));
              owned(0x342,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              roll = random.range(0,4);
              p.pose.scale[0] = (float)(roll + -2) * 0.05 + double(1.0f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            p.counter = i16(p.counter) + 2;
            if ((0 < i16(p.animation.cursor.index)) && (i16(p.timer) < 0x3c)) {
              if (double(other.motion.y) + double(100.0f) <
                  double(p.motion.y)) {
                p.parameters[0] = double(p.parameters[0]) + double(2.0f);
              }
              if (double(p.motion.y) <
                  double(other.motion.y) + double(100.0f)) {
                p.parameters[0] = double(p.parameters[0]) - double(2.0f);
              }
              p.timer = i16(p.timer) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((p.response != 0) && (p.response != 6)) &&
               (i16(p.animation.cursor.index) == 1)) {
              if ((i16(p.hits) == 0) || (p.response != 4)) {
                p.motion.velocityX = 0.0f;
                p.motion.velocityY = 0.0f;
                p.frame(2);
                p.pose.scale[0] = 2.5f;
                p.pose.scale[1] = 2.5f;
                return;
              }
              if (((0 < i16(p.hits)) && (p.response == 4)) &&
                 (0 < i16(p.hits))) {
                p.response = 0;
              }
            }
            if (1 < i16(p.animation.cursor.index)) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
              if (u8(channel(24)) < 0x19) {
                setChannel(24,u8(0));
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x19));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          for (local_44 = 0; local_44 < 3; local_44 = local_44 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
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
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
        if (u8(channel(24)) < 0x14) {
          setChannel(24,u8(0));
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x343:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if (p.response == 5) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.hits = 3;
              p.response = 0;
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              setChannel(24,u8(0xff));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
              p.counter = 10;
              p.timer = 0;
              p.cycle = 0;
              uVar1 = i16(i32(p.parameters[0]));
              p.angle = uVar1;
            }
            if (i16(p.animation.cursor.index) < 2) {
              aimRadians = atan((double(other.motion.y) -
                           double(p.motion.y)) + double(100.0f),(double(other.motion.x) -
                           double(p.motion.x)) * (float)(int)i8(p.motion.facing));
              uVar1 = i32(aimRadians/double(0.017453289f));
              p.timer = uVar1;
              if (i16(p.angle) + 10 < (int)i16(p.timer)) {
                p.angle = i16(p.angle) + 1;
              }
              if ((int)i16(p.timer) < i16(p.angle) + -10) {
                p.angle = i16(p.angle) + -1;
              }
              if (0x5a < i16(p.angle)) {
                p.angle = 0x5a;
              }
              if (i16(p.angle) < -0x5a) {
                p.angle = 0xffa6;
              }
            }
            if (i16(p.animation.cursor.index) < 2) {
              p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter) * double(0.1f) + double(15.0f));
              owned(0x344,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              roll = random.range(0,4);
              p.pose.scale[0] = (float)(roll + -2) * 0.05 + double(1.0f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            p.counter = i16(p.counter) + 2;
            if ((0 < i16(p.animation.cursor.index)) && (i16(p.timer) < 0x3c)) {
              if (double(other.motion.y) + double(100.0f) <
                  double(p.motion.y)) {
                p.parameters[0] = double(p.parameters[0]) + double(2.0f);
              }
              if (double(p.motion.y) <
                  double(other.motion.y) + double(100.0f)) {
                p.parameters[0] = double(p.parameters[0]) - double(2.0f);
              }
              p.timer = i16(p.timer) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((p.response != 0) && (p.response != 6)) &&
               (i16(p.animation.cursor.index) == 1)) {
              if ((i16(p.hits) == 0) || (p.response != 4)) {
                p.motion.velocityX = 0.0f;
                p.motion.velocityY = 0.0f;
                p.frame(2);
                p.pose.scale[0] = 2.5f;
                p.pose.scale[1] = 2.5f;
                return;
              }
              if (((0 < i16(p.hits)) && (p.response == 4)) &&
                 (0 < i16(p.hits))) {
                p.response = 0;
              }
            }
            if (1 < i16(p.animation.cursor.index)) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
              if (u8(channel(24)) < 0x19) {
                setChannel(24,u8(0));
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x19));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          for (local_48 = 0; local_48 < 3; local_48 = local_48 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
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
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
        if (u8(channel(24)) < 0x14) {
          setChannel(24,u8(0));
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -0x14));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x345:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 2.0f;
          p.pose.scale[1] = 2.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x346:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (4 < i16(p.animation.cursor.index))) {
          if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.response = 0;
            p.hits = 1;
            p.counter = 0xf;
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 1) {
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter));
            p.counter = i16(p.counter) + -1;
            if (i16(p.counter) < 0) {
              p.counter = 0;
            }
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(45.0f);
          }
          if (i16(p.animation.cursor.index) == 2) {
            p.counter = i16(p.counter) + 1;
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.98f);
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter));
            fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
            fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            owner.effectVisual.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          if (i16(p.animation.cursor.index) == 3) {
            p.counter = i16(p.counter) + 1;
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.98f);
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter));
            fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
            if (u8(channel(24)) < 0xb) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          if (((p.response == 2) || (p.response == 4)) &&
             (i16(p.animation.cursor.index) == 2)) {
            p.frame(3);
          }
          else if ((((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f)))
                   || (double(1000.0f) < double(p.motion.y))) ||
                  (double(p.motion.y) < -double(160.0f))) {
            p.destroy();
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              aimRadians = atan((double(other.motion.y) + double(128.0f)) -
                           double(p.motion.y),(double(other.motion.x) -
                           double(p.motion.x)) * (float)(int)i8(p.motion.facing));
              uVar1 = i32(aimRadians/double(0.017453289f));
              p.angle = uVar1;
            }
            if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.destroy();
            }
          }
        }
        else {
          p.destroy();
          for (local_4c = 0; local_4c < 3; local_4c = local_4c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x347:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.projection[2] = 0x4e2;
          p.pose.rotation[0] = -30.0f;
          p.pose.rotation[1] = (float)(i8(p.motion.facing) * 0x19);
          p.counter = 0;
        }
        p.pose.scale[0] =
             (double(0.6f) - (float)(int)i16(p.counter) * double(0.06f) *
                    (float)(i16(p.counter) < 10)) + double(0.02f) + double(p.pose.scale[0]);
        p.pose.scale[1] =
             (double(0.6f) - (float)(int)i16(p.counter) * double(0.06f) *
                    (float)(i16(p.counter) < 10)) + double(0.02f) + double(p.pose.scale[1]);
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
        break;
      case 0x348:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
        }
        p.polar(double(p.parameters[0]) * double(120.0f) +
                     (float)(int)i16(state.spinAngle),(float)(int)i16(p.counter));
        if (i16(p.counter) < 0x78) {
          p.counter = i16(p.counter) + 0x14;
        }
        if (0x78 < i16(p.counter)) {
          p.counter = 0x78;
        }
        p.motion.x =
             double(p.motion.velocityX) + double(owner.motion.x);
        p.motion.y =
             double(p.motion.velocityY) + double(owner.motion.y) + double(100.0f);
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(10.0f);
        if ((i16(p.animation.cursor.index) == 1) &&
           ((i16(owner.animation.cursor.action) != 0x2be ||
            ((i16(owner.animation.cursor.action) == 0x2be &&
             (8 < i16(owner.animation.cursor.index))))))) {
          p.frame(2);
        }
        if (1 < i16(p.animation.cursor.index)) {
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
      case 0x349:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
        }
        p.motion.x = double(owner.motion.x);
        p.motion.y = double(owner.motion.y) + double(80.0f);
        p.pose.rotation[2] = double(p.pose.rotation[2]) - double(5.0f);
        if ((i16(p.animation.cursor.index) == 1) &&
           ((i16(owner.animation.cursor.action) != 0x2be ||
            ((i16(owner.animation.cursor.action) == 0x2be &&
             (8 < i16(owner.animation.cursor.index))))))) {
          p.frame(2);
        }
        if (1 < i16(p.animation.cursor.index)) {
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
      case 0x34a:
        p.motion.x = double(owner.motion.x);
        p.motion.y = 0.0f;
        p.contactDelay = 1;
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 2.5f;
          p.pose.scale[1] = 2.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.projection[2] = 0x4e2;
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
          p.pose.rotation[0] = -80.0f;
          events.sound(0x10);
        }
        if ((i16(p.animation.cursor.index) == 1) &&
           ((i16(owner.animation.cursor.action) != 0x2c0 ||
            ((i16(owner.animation.cursor.index) == 0x2c0 &&
             (6 < i16(owner.animation.cursor.index))))))) {
          p.frame(2);
        }
        else {
          if (i16(p.animation.cursor.index) == 2) {
            if (u8(channel(24)) < 0x10) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0xf));
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x34b:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 99;
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
        }
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
        if (double(p.motion.y) <= double(1280.0f)) {
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 0xf) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.motion.velocityY = -40.0f;
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x34c:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          if (i16(p.counter) != 0x8c) {
            p.counter = 0;
          }
        }
        p.polar(double(p.parameters[0]) * double(72.0f) +
                     (float)(int)i16(state.orbitAngle),(float)(int)i16(p.counter));
        p.motion.x =
             double(p.motion.velocityX) + double(owner.motion.x);
        p.motion.y =
             double(p.motion.velocityY) * double(0.25f) + double(owner.motion.y) + double(200.0f)
        ;
        if (i16(p.counter) < 0x8c) {
          p.counter = i16(p.counter) + 5;
        }
        else {
          p.counter = 0x8c;
        }
        if ((((i16(other.combat.health) < 1) ||
             (i16(owner.combat.health) < 1)) ||
            (i16(owner.combat.spellTimer) < 1)) && (i16(p.animation.cursor.index) < 2))
        {
          p.frame(2);
        }
        if (((i16(owner.animation.cursor.action) == 0x2c1) &&
            (i16(owner.animation.cursor.index) == 2)) && (i16(p.animation.cursor.index) < 2))
        {
          p.frame(2);
        }
        if ((i16(p.animation.cursor.index) == 0) &&
           ((((i16(owner.animation.cursor.action) == 400 &&
              (i16(owner.animation.cursor.index) == 4)) ||
             (((i16(owner.animation.cursor.action) == 600 &&
               (i16(owner.animation.cursor.index) == 7)) ||
              ((i16(owner.animation.cursor.action) == 0x259 &&
               (i16(owner.animation.cursor.index) == 7)))))) ||
            ((i16(owner.animation.cursor.action) == 0x25a &&
             (i16(owner.animation.cursor.index) == 3)))))) {
          for (local_50 = 0; local_50 < 8; local_50 = local_50 + 1) {
            
            owner.launchParameters[0] = (float)(local_50 * 0x2d);
            owned(0x360,double(p.motion.x),double(p.motion.y),u8(owner.motion.facing),1);
          }
          p.frame(2);
          events.sound(4);
        }
        if (((i16(p.animation.cursor.index) == 1) &&
            (i16(owner.animation.cursor.action) == 0x2c1)) &&
           (i16(owner.animation.cursor.index) == 3)) {
          p.frame(0);
          events.sound(0x14);
        }
        else {
          if (1 < i16(p.animation.cursor.index)) {
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
          else if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(0);
            owner.combat.spellExtent =
                 double(owner.combat.spellExtent) - double(25.0f);
            events.sound(0x14);
          }
        }
        break;
      case 0x34d:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          if (i16(p.counter) != 0x8c) {
            p.counter = 0;
          }
        }
        p.polar(double(p.parameters[0]) * double(72.0f) +
                     (float)(int)i16(state.orbitAngle),(float)(int)i16(p.counter));
        p.motion.x =
             double(p.motion.velocityX) + double(owner.motion.x);
        p.motion.y =
             double(p.motion.velocityY) * double(0.25f) + double(owner.motion.y) + double(200.0f)
        ;
        if (i16(p.counter) < 0x8c) {
          p.counter = i16(p.counter) + 5;
        }
        else {
          p.counter = 0x8c;
        }
        if ((((i16(other.combat.health) < 1) ||
             (i16(owner.combat.health) < 1)) ||
            (i16(owner.combat.spellTimer) < 1)) && (i16(p.animation.cursor.index) < 2))
        {
          p.frame(2);
        }
        if (((i16(owner.animation.cursor.action) == 0x2c1) &&
            (i16(owner.animation.cursor.index) == 2)) && (i16(p.animation.cursor.index) < 2))
        {
          p.frame(2);
        }
        if ((i16(p.animation.cursor.index) == 0) &&
           (((i16(owner.animation.cursor.action) == 0x191 &&
             (i16(owner.animation.cursor.index) == 4)) ||
            ((i16(owner.animation.cursor.action) == 0x25c &&
             (i16(owner.animation.cursor.index) == 6)))))) {
          for (local_54 = 0; local_54 < 8; local_54 = local_54 + 1) {
            
            owner.launchParameters[0] = (float)(local_54 * 0x2d);
            owned(0x361,double(p.motion.x),double(p.motion.y),u8(owner.motion.facing),1);
          }
          p.frame(2);
          events.sound(4);
        }
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        if (((i16(p.animation.cursor.index) == 1) &&
            (i16(owner.animation.cursor.action) == 0x2c1)) &&
           (i16(owner.animation.cursor.index) == 3)) {
          p.frame(0);
          events.sound(0x14);
        }
        else {
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          else if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(0);
            owner.combat.spellExtent =
                 double(owner.combat.spellExtent) - double(25.0f);
            events.sound(0x14);
          }
        }
        break;
      case 0x34e:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          if (i16(p.counter) != 0x8c) {
            p.counter = 0;
          }
        }
        p.polar(double(p.parameters[0]) * double(72.0f) +
                     (float)(int)i16(state.orbitAngle),(float)(int)i16(p.counter));
        p.motion.x =
             double(p.motion.velocityX) + double(owner.motion.x);
        p.motion.y =
             double(p.motion.velocityY) * double(0.25f) + double(owner.motion.y) + double(200.0f)
        ;
        if (i16(p.counter) < 0x8c) {
          p.counter = i16(p.counter) + 5;
        }
        else {
          p.counter = 0x8c;
        }
        if ((((i16(other.combat.health) < 1) ||
             (i16(owner.combat.health) < 1)) ||
            (i16(owner.combat.spellTimer) < 1)) && (i16(p.animation.cursor.index) < 2))
        {
          p.frame(2);
        }
        if (((i16(owner.animation.cursor.action) == 0x2c1) &&
            (i16(owner.animation.cursor.index) == 2)) && (i16(p.animation.cursor.index) < 2))
        {
          p.frame(2);
        }
        if ((i16(p.animation.cursor.index) == 0) &&
           ((((i16(owner.animation.cursor.action) == 0x195 &&
              (i16(owner.animation.cursor.index) == 4)) ||
             ((i16(owner.animation.cursor.action) == 500 &&
              (i16(owner.animation.cursor.index) == 0xe)))) ||
            ((i16(owner.animation.cursor.action) == 0x1f5 &&
             (i16(owner.animation.cursor.index) == 0xe)))))) {
          for (local_58 = 0; local_58 < 8; local_58 = local_58 + 1) {
            
            owner.launchParameters[0] = (float)(local_58 * 0x2d);
            owned(0x362,double(p.motion.x),double(p.motion.y),u8(owner.motion.facing),1);
          }
          p.frame(2);
          events.sound(4);
        }
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        if (((i16(p.animation.cursor.index) == 1) &&
            (i16(owner.animation.cursor.action) == 0x2c1)) &&
           (i16(owner.animation.cursor.index) == 3)) {
          p.frame(0);
          events.sound(0x14);
        }
        else {
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          else if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(0);
            owner.combat.spellExtent =
                 double(owner.combat.spellExtent) - double(25.0f);
            events.sound(0x14);
          }
        }
        break;
      case 0x34f:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          if (i16(p.counter) != 0x8c) {
            p.counter = 0;
          }
        }
        p.polar(double(p.parameters[0]) * double(72.0f) +
                     (float)(int)i16(state.orbitAngle),(float)(int)i16(p.counter));
        p.motion.x =
             double(p.motion.velocityX) + double(owner.motion.x);
        p.motion.y =
             double(p.motion.velocityY) * double(0.25f) + double(owner.motion.y) + double(200.0f)
        ;
        if (i16(p.counter) < 0x8c) {
          p.counter = i16(p.counter) + 5;
        }
        else {
          p.counter = 0x8c;
        }
        if ((((i16(other.combat.health) < 1) ||
             (i16(owner.combat.health) < 1)) ||
            (i16(owner.combat.spellTimer) < 1)) && (i16(p.animation.cursor.index) < 2))
        {
          p.frame(2);
        }
        if (((i16(owner.animation.cursor.action) == 0x2c1) &&
            (i16(owner.animation.cursor.index) == 2)) && (i16(p.animation.cursor.index) < 2))
        {
          p.frame(2);
        }
        if ((i16(p.animation.cursor.index) == 0) &&
           ((((i16(owner.animation.cursor.action) == 0x194 &&
              (i16(owner.animation.cursor.index) == 4)) ||
             ((i16(owner.animation.cursor.action) == 0x263 &&
              (i16(owner.animation.cursor.index) == 5)))) ||
            ((i16(owner.animation.cursor.action) == 0x264 &&
             (i16(owner.animation.cursor.index) == 5)))))) {
          for (local_5c = 0; local_5c < 8; local_5c = local_5c + 1) {
            
            owner.launchParameters[0] = (float)(local_5c * 0x2d);
            owned(0x363,double(p.motion.x),double(p.motion.y),u8(owner.motion.facing),1);
          }
          p.frame(2);
          events.sound(4);
        }
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        if (((i16(p.animation.cursor.index) == 1) &&
            (i16(owner.animation.cursor.action) == 0x2c1)) &&
           (i16(owner.animation.cursor.index) == 3)) {
          p.frame(0);
          events.sound(0x14);
        }
        else {
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          else if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(0);
            owner.combat.spellExtent =
                 double(owner.combat.spellExtent) - double(25.0f);
            events.sound(0x14);
          }
        }
        break;
      case 0x350:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          if (i16(p.counter) != 0x8c) {
            p.counter = 0;
          }
        }
        p.polar(double(p.parameters[0]) * double(72.0f) +
                     (float)(int)i16(state.orbitAngle),(float)(int)i16(p.counter));
        p.motion.x =
             double(p.motion.velocityX) + double(owner.motion.x);
        p.motion.y =
             double(p.motion.velocityY) * double(0.25f) + double(owner.motion.y) + double(200.0f)
        ;
        if (i16(p.counter) < 0x8c) {
          p.counter = i16(p.counter) + 5;
        }
        else {
          p.counter = 0x8c;
        }
        if ((((i16(other.combat.health) < 1) ||
             (i16(owner.combat.health) < 1)) ||
            (i16(owner.combat.spellTimer) < 1)) && (i16(p.animation.cursor.index) < 2))
        {
          p.frame(2);
        }
        if (((i16(owner.animation.cursor.action) == 0x2c1) &&
            (i16(owner.animation.cursor.index) == 2)) && (i16(p.animation.cursor.index) < 2))
        {
          p.frame(2);
        }
        if ((i16(p.animation.cursor.index) == 0) &&
           ((((i16(owner.animation.cursor.action) == 0x192 &&
              (i16(owner.animation.cursor.index) == 4)) ||
             (((i16(owner.animation.cursor.action) == 0x260 &&
               (i16(owner.animation.cursor.index) == 4)) ||
              ((i16(owner.animation.cursor.action) == 0x261 &&
               (i16(owner.animation.cursor.index) == 4)))))) ||
            ((i16(owner.animation.cursor.action) == 0x262 &&
             (i16(owner.animation.cursor.index) == 4)))))) {
          for (local_60 = 0; local_60 < 8; local_60 = local_60 + 1) {
            
            owner.launchParameters[0] = (float)(local_60 * 0x2d);
            owned(0x364,double(p.motion.x),double(p.motion.y),u8(owner.motion.facing),1);
          }
          p.frame(2);
          events.sound(4);
        }
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0xb) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        if (((i16(p.animation.cursor.index) == 1) &&
            (i16(owner.animation.cursor.action) == 0x2c1)) &&
           (i16(owner.animation.cursor.index) == 3)) {
          p.frame(0);
          events.sound(0x14);
        }
        else {
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          else if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(0);
            owner.combat.spellExtent =
                 double(owner.combat.spellExtent) - double(25.0f);
            events.sound(0x14);
          }
        }
        break;
      case 0x351:
        p.motion.x =
             (float)(i8(owner.motion.facing) * 0x14) +
             double(owner.motion.x);
        p.timer = i16(p.timer) + 1;
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 99;
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
          p.timer = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = -1.0f;
        }
        if ((6 < i16(owner.animation.cursor.index)) && (i16(p.animation.cursor.index) < 6)) {
          p.frame(i16(p.animation.cursor.index) + 5);
          events.sound(0xf);
        }
        if ((5 < i16(p.animation.cursor.index)) && (i16(p.animation.cursor.index) < 0xb)) {
          p.motion.velocityY = 0.0f;
          if (double(40.0f) <= double(p.pose.scale[0])) {
            p.frame(i16(p.animation.cursor.index) + 5);
            return;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) + double(2.0f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(2.0f);
        }
        if ((10 < i16(p.animation.cursor.index)) && (i16(p.animation.cursor.index) < 0x10)) {
          owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
        }
        if (0xf < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 6) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -5));
        }
        if (i16(p.timer) == 0x23) {
          ownedVisual(0x352,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1,0,1);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.frame(0x10);
        }
        else {
          if (i16(p.animation.cursor.elapsed) == 0) {
            p.response = 0;
          }
          if ((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(1);
          }
          if ((i16(p.animation.cursor.index) == 0xb) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(6);
          }
          if ((((i16(p.animation.cursor.index) == 0x10) && (i16(p.animation.cursor.elapsed) == 0)) &&
              (i16(owner.animation.cursor.action) == 0x2bf)) &&
             ((6 < i16(owner.animation.cursor.index) &&
              (i16(owner.animation.cursor.index) < 9)))) {
            p.frame(0xb);
          }
        }
        break;
      case 0x352:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(1.0f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(1.0f);
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
        break;
      case 0x353:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x354:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (0x16 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.10000000149011612f;
            p.pose.scale[1] = 0.10000000149011612f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 18.0f;
            p.motion.velocityY = 0.0f;
            p.counter = 0;
            p.timer = 0;
            p.hits = 0;
            p.response = 0;
          }
          p.motion.velocityX = double(p.motion.velocityX) - double(0.4f);
          p.motion.velocityY = double(p.motion.velocityY) + double(0.15f);
          if (double(p.motion.velocityX) < double(2.0f)) {
            p.motion.velocityX = 2.0f;
          }
          if (i16(p.counter) == 0) {
            if (double(1.0f) <= double(p.pose.scale[0])) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              setChannel(24,u8(0xff));
            }
            else {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
              setChannel(24,u8(i8(channel(24)) + '\x19'));
            }
            if ((i8(channel(24)) == -1) && (i16(p.counter) == 0)) {
              p.counter = i16(p.counter) + 1;
              p.hits = 5;
            }
          }
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          if ((p.response == 4) && (0 < i16(p.hits))) {
            p.response = 0;
          }
          if (p.response != 0) {
            p.counter = i16(p.counter) + 1;
            p.hits = 0;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 0x17)) &&
             (0 < i16(p.counter))) {
            p.frame(0x17);
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            else if (((i16(p.animation.cursor.index) == 0x17) && (i16(p.animation.cursor.elapsed) == 0)) &&
                    (i16(p.timer) < 0)) {
              p.frame(1);
              p.timer = i16(p.timer) + 1;
            }
          }
        }
        else {
          p.destroy();
          for (local_64 = 0; local_64 < 8; local_64 = local_64 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x355:
        if ((i16(owner.animation.cursor.action) < 0x46) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 10.0f;
            p.motion.velocityY = 6.0f;
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.hits = 0;
            p.response = 0;
          }
          if (double(1.3f) <= double(p.pose.scale[0])) {
            if (i16(p.counter) == 0) {
              p.hits = 10;
              p.counter = i16(p.counter) + 1;
            }
          }
          else {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
          }
          if ((double(0.5f) < double(p.motion.velocityX)) &&
             (p.motion.velocityX = double(p.motion.velocityX) - double(0.5f),
             double(p.motion.velocityX) < double(0.5f))) {
            p.motion.velocityX = 0.5f;
          }
          if ((i16(p.angle) == 0) &&
             (p.motion.velocityY = double(p.motion.velocityY) - double(0.15f),
             double(p.motion.velocityY) < -double(4.5f))) {
            p.angle = 1;
          }
          if ((i16(p.angle) == 1) &&
             (p.motion.velocityY = double(p.motion.velocityY) + double(0.15f),
             double(4.5f) < double(p.motion.velocityY))) {
            p.angle = 0;
          }
          if ((p.response == 4) && (0 < i16(p.hits))) {
            p.response = 0;
          }
          if ((p.response != 0) && (i16(p.animation.cursor.index) < 0x1f)) {
            p.frame(0x1f);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.angle = 2;
          }
          if (i16(p.animation.cursor.index) == 0x1f) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
            setChannel(24,u8(i8(channel(24)) + -0x19));
          }
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 0x1f) && (i16(p.animation.cursor.elapsed) == 0)) {
            if (i16(p.timer) < 6) {
              p.timer = i16(p.timer) + 1;
              p.frame(1);
            }
            else {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.angle = 2;
            }
          }
        }
        else {
          p.destroy();
          for (local_68 = 0; local_68 < 10; local_68 = local_68 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x356:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (0x16 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.10000000149011612f;
            p.pose.scale[1] = 0.10000000149011612f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 18.0f;
            p.motion.velocityY = -7.5f;
            p.counter = 0;
            p.timer = 0;
            p.hits = 0;
            p.response = 0;
          }
          p.motion.velocityX = double(p.motion.velocityX) - double(0.4f);
          p.motion.velocityY = double(p.motion.velocityY) + double(0.25f);
          if (double(p.motion.velocityX) < double(2.0f)) {
            p.motion.velocityX = 2.0f;
          }
          if (i16(p.counter) == 0) {
            if (double(1.0f) <= double(p.pose.scale[0])) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              setChannel(24,u8(0xff));
            }
            else {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
              setChannel(24,u8(i8(channel(24)) + '\x19'));
            }
            if ((i8(channel(24)) == -1) && (i16(p.counter) == 0)) {
              p.counter = i16(p.counter) + 1;
              p.hits = 5;
            }
          }
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          if ((p.response == 4) && (0 < i16(p.hits))) {
            p.response = 0;
          }
          if (p.response != 0) {
            p.counter = i16(p.counter) + 1;
            p.hits = 0;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 0x17)) &&
             (0 < i16(p.counter))) {
            p.frame(0x17);
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            else if (((i16(p.animation.cursor.index) == 0x17) && (i16(p.animation.cursor.elapsed) == 0)) &&
                    (i16(p.timer) < 0)) {
              p.frame(1);
              p.timer = i16(p.timer) + 1;
            }
          }
        }
        else {
          p.destroy();
          for (local_6c = 0; local_6c < 8; local_6c = local_6c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x357:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (0x16 < i16(p.animation.cursor.index))) {
          if ((i16(owner.animation.cursor.action) == 0x189) &&
             (i16(p.animation.cursor.index) < 0x17)) {
            p.motion.x =
                 (float)(i8(p.motion.facing) * 0x78) +
                 double(owner.motion.x);
            p.motion.y = 120.0f;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.10000000149011612f;
            p.pose.scale[1] = 0.10000000149011612f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.timer = 0;
          }
          if (i16(p.counter) == 0) {
            if (double(1.0f) <= double(p.pose.scale[0])) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              setChannel(24,u8(0xff));
            }
            else {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
              setChannel(24,u8(i8(channel(24)) + '\x19'));
            }
            if ((i8(channel(24)) == -1) && (i16(p.counter) == 0)) {
              p.counter = i16(p.counter) + 1;
              p.hits = 1;
            }
          }
          if ((i16(owner.combat.freeze) == 0) &&
             (sVar2 = p.advance(), sVar2 == i16(p.animation.cursor.frameCount))) {
            p.destroy();
          }
        }
        else {
          p.frame(0x17);
        }
        break;
      case 0x358:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (0x16 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.motion.velocityX = 8.0f;
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            events.sound(2);
            owned(0x359,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          if ((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(120.0f))) {
            p.destroy();
          }
          else {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              if (i16(p.counter) < 5) {
                p.counter = i16(p.counter) + 1;
                p.frame(1);
              }
              else {
                p.destroy();
              }
            }
          }
        }
        else {
          p.destroy();
          for (local_70 = 0; local_70 < 10; local_70 = local_70 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x359:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.hits = 1;
          p.response = 0;
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x35a:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.5f;
          p.pose.scale[1] = 1.5f;
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x35b:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (0x16 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.10000000149011612f;
            p.pose.scale[1] = 0.10000000149011612f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 22.0f;
            p.counter = 0;
            p.timer = 0;
            p.hits = 0;
            p.response = 0;
          }
          p.motion.velocityX = double(p.motion.velocityX) - double(0.4f);
          if (double(p.motion.velocityX) < double(1.0f)) {
            p.motion.velocityX = 1.0f;
          }
          if (i16(p.counter) == 0) {
            if (double(1.0f) <= double(p.pose.scale[0])) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              setChannel(24,u8(0xff));
            }
            else {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
              setChannel(24,u8(i8(channel(24)) + '\x19'));
            }
            if ((i8(channel(24)) == -1) && (i16(p.counter) == 0)) {
              p.counter = i16(p.counter) + 1;
              p.hits = 5;
            }
          }
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          if ((p.response == 4) && (0 < i16(p.hits))) {
            p.response = 0;
          }
          if (p.response != 0) {
            p.hits = 0;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          if (((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 0x17)) &&
             (0 < i16(p.counter))) {
            p.frame(0x17);
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            else if (((i16(p.animation.cursor.index) == 0x17) && (i16(p.animation.cursor.elapsed) == 0)) &&
                    (i16(p.timer) < 1)) {
              p.frame(1);
              p.timer = i16(p.timer) + 1;
            }
          }
        }
        else {
          p.destroy();
          for (local_74 = 0; local_74 < 8; local_74 = local_74 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x35c:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 99;
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.counter = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
        if (0 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0x10) {
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
      case 0x35d:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
        }
        if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
          consumeSpirit(0x14);
          uVar9 = 1;
          uVar3 = (u32)u8(p.motion.facing);
          uVar8 = 0;
          angleRadians = double(p.motion.x);
          roll = random.range(0,200);
          owned(0x330,double(float(double(angleRadians)+100.0))-roll,uVar8,uVar3,uVar9);
          p.counter = i16(p.counter) + 1;
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          if (i16(p.counter) < 5) {
            p.frame(1);
          }
          else {
            p.destroy();
          }
        }
        break;
      case 0x35e:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if ((i16(owner.animation.cursor.action) < 0x32) ||
                (0x95 < i16(owner.animation.cursor.action))) {
          if (p.response == 5) {
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
              p.counter = 0x3c;
              p.timer = 0;
              p.cycle = 0;
              uVar1 = i16(i32(p.parameters[0]));
              p.angle = uVar1;
              fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
              p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
              p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter) * double(0.1f));
            }
            if (0 < i16(p.animation.cursor.index)) {
              if (i16(p.hits) == 0) {
                p.destroy();
                events.sound(1);
                owned(0x35f,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
                return;
              }
              p.counter = i16(p.counter) + 2;
              fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
              p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
              aimRadians = atan((double(other.motion.y) -
                           double(p.motion.y)) + double(100.0f),(double(other.motion.x) -
                           double(p.motion.x)) * (float)(int)i8(p.motion.facing));
              uVar1 = i32(aimRadians/double(0.017453289f));
              p.timer = uVar1;
              if (i16(p.timer) < i16(p.angle)) {
                if ((int)i16(p.angle) - (int)i16(p.timer) < 0xb5) {
                  p.angle = i16(p.angle) + -2;
                  if (i16(p.angle) < -0xb4) {
                    p.angle = i16(p.angle) + 0x168;
                  }
                }
                else {
                  p.angle = i16(p.angle) + 2;
                  if (0xb4 < i16(p.angle)) {
                    p.angle = i16(p.angle) + -0x168;
                  }
                }
              }
              if (i16(p.angle) <= i16(p.timer)) {
                if ((int)i16(p.angle) - (int)i16(p.timer) < -0xb4) {
                  p.angle = i16(p.angle) + -2;
                  if (i16(p.angle) < -0xb4) {
                    p.angle = i16(p.angle) + 0x168;
                  }
                }
                else {
                  p.angle = i16(p.angle) + 2;
                  if (0xb4 < i16(p.angle)) {
                    p.angle = i16(p.angle) + -0x168;
                  }
                }
              }
            }
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter) * double(0.1f));
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          for (local_78 = 0; local_78 < 2; local_78 = local_78 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x35f:
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x360:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (1 < i16(p.animation.cursor.index))) {
          if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.response = 0;
            p.hits = 1;
            p.counter = 0xf;
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 0) {
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter));
            p.counter = i16(p.counter) + -1;
            if (i16(p.counter) < 0) {
              p.counter = 0;
            }
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(30.0f);
          }
          if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
             ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f))))) {
            p.destroy();
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              owner.launchParameters[0] = double(p.parameters[0]);
              owned(0x365,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              p.destroy();
            }
          }
        }
        else {
          p.destroy();
          for (local_7c = 0; local_7c < 2; local_7c = local_7c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x361:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (1 < i16(p.animation.cursor.index))) {
          if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.response = 0;
            p.hits = 1;
            p.counter = 0xf;
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 0) {
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter));
            p.counter = i16(p.counter) + -1;
            if (i16(p.counter) < 0) {
              p.counter = 0;
            }
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(30.0f);
          }
          if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
             ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f))))) {
            p.destroy();
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              owner.launchParameters[0] = double(p.parameters[0]);
              owned(0x366,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              p.destroy();
            }
          }
        }
        else {
          p.destroy();
          for (local_80 = 0; local_80 < 2; local_80 = local_80 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x362:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (1 < i16(p.animation.cursor.index))) {
          if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.response = 0;
            p.hits = 1;
            p.counter = 0xf;
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 0) {
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter));
            p.counter = i16(p.counter) + -1;
            if (i16(p.counter) < 0) {
              p.counter = 0;
            }
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(30.0f);
          }
          if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
             ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f))))) {
            p.destroy();
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              owner.launchParameters[0] = double(p.parameters[0]);
              owned(0x367,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              p.destroy();
            }
          }
        }
        else {
          p.destroy();
          for (local_84 = 0; local_84 < 2; local_84 = local_84 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x363:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (1 < i16(p.animation.cursor.index))) {
          if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.response = 0;
            p.hits = 1;
            p.counter = 0xf;
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 0) {
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter));
            p.counter = i16(p.counter) + -1;
            if (i16(p.counter) < 0) {
              p.counter = 0;
            }
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(30.0f);
          }
          if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
             ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f))))) {
            p.destroy();
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              owner.launchParameters[0] = double(p.parameters[0]);
              owned(0x368,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              p.destroy();
            }
          }
        }
        else {
          p.destroy();
          for (local_88 = 0; local_88 < 2; local_88 = local_88 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x364:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (1 < i16(p.animation.cursor.index))) {
          if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.response = 0;
            p.hits = 1;
            p.counter = 0xf;
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 0) {
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter));
            p.counter = i16(p.counter) + -1;
            if (i16(p.counter) < 0) {
              p.counter = 0;
            }
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(30.0f);
          }
          if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
             ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f))))) {
            p.destroy();
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              owner.launchParameters[0] = double(p.parameters[0]);
              owned(0x369,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              p.destroy();
            }
          }
        }
        else {
          p.destroy();
          for (local_8c = 0; local_8c < 2; local_8c = local_8c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x365:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if (p.response == 5) {
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
              p.counter = 0x3c;
              p.timer = 0;
              p.cycle = 0;
              p.angle = 0;
            }
            if (0 < i16(p.animation.cursor.index)) {
              p.counter = i16(p.counter) + 2;
              aimRadians = atan((double(other.motion.y) -
                           double(p.motion.y)) + double(100.0f),(double(other.motion.x) -
                           double(p.motion.x)) * (float)(int)i8(p.motion.facing));
              uVar1 = i32(aimRadians/double(0.017453289f));
              p.timer = uVar1;
              if (i16(p.timer) < i16(p.angle)) {
                if ((int)i16(p.angle) - (int)i16(p.timer) < 0xb5) {
                  p.angle = i16(p.angle) + -2;
                  if (i16(p.angle) < -0xb4) {
                    p.angle = i16(p.angle) + 0x168;
                  }
                }
                else {
                  p.angle = i16(p.angle) + 2;
                  if (0xb4 < i16(p.angle)) {
                    p.angle = i16(p.angle) + -0x168;
                  }
                }
              }
              if (i16(p.angle) <= i16(p.timer)) {
                if ((int)i16(p.angle) - (int)i16(p.timer) < -0xb4) {
                  p.angle = i16(p.angle) + -2;
                  if (i16(p.angle) < -0xb4) {
                    p.angle = i16(p.angle) + 0x168;
                  }
                }
                else {
                  p.angle = i16(p.angle) + 2;
                  if (0xb4 < i16(p.angle)) {
                    p.angle = i16(p.angle) + -0x168;
                  }
                }
              }
            }
            if (i16(p.animation.cursor.index) < 2) {
              p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter) * 0.05 + double(3.0f));
              owned(0x33c,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              roll = random.range(0,4);
              p.pose.scale[0] = (float)(roll + -2) * 0.05 + double(1.0f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            p.counter = i16(p.counter) + 2;
            if ((0 < i16(p.animation.cursor.index)) && (i16(p.timer) < 0x3c)) {
              if (double(other.motion.y) + double(100.0f) <
                  double(p.motion.y)) {
                p.parameters[0] = double(p.parameters[0]) + double(2.0f);
              }
              if (double(p.motion.y) <
                  double(other.motion.y) + double(100.0f)) {
                p.parameters[0] = double(p.parameters[0]) - double(2.0f);
              }
              p.timer = i16(p.timer) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((p.response != 0) && (p.response != 6)) &&
               (i16(p.animation.cursor.index) == 1)) {
              if ((i16(p.hits) == 0) || (p.response != 4)) {
                p.motion.velocityX = 0.0f;
                p.motion.velocityY = 0.0f;
                p.frame(2);
                p.pose.scale[0] = 2.5f;
                p.pose.scale[1] = 2.5f;
                return;
              }
              if (((0 < i16(p.hits)) && (p.response == 4)) &&
                 (0 < i16(p.hits))) {
                p.response = 0;
              }
            }
            if (1 < i16(p.animation.cursor.index)) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
              if (u8(channel(24)) < 0x19) {
                setChannel(24,u8(0));
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x19));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          for (local_90 = 0; local_90 < 2; local_90 = local_90 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x366:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if (p.response == 5) {
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
              p.counter = 0x3c;
              p.timer = 0;
              p.cycle = 0;
              p.angle = 0;
            }
            if (0 < i16(p.animation.cursor.index)) {
              p.counter = i16(p.counter) + 2;
              aimRadians = atan((double(other.motion.y) -
                           double(p.motion.y)) + double(100.0f),(double(other.motion.x) -
                           double(p.motion.x)) * (float)(int)i8(p.motion.facing));
              uVar1 = i32(aimRadians/double(0.017453289f));
              p.timer = uVar1;
              if (i16(p.timer) < i16(p.angle)) {
                if ((int)i16(p.angle) - (int)i16(p.timer) < 0xb5) {
                  p.angle = i16(p.angle) + -2;
                  if (i16(p.angle) < -0xb4) {
                    p.angle = i16(p.angle) + 0x168;
                  }
                }
                else {
                  p.angle = i16(p.angle) + 2;
                  if (0xb4 < i16(p.angle)) {
                    p.angle = i16(p.angle) + -0x168;
                  }
                }
              }
              if (i16(p.angle) <= i16(p.timer)) {
                if ((int)i16(p.angle) - (int)i16(p.timer) < -0xb4) {
                  p.angle = i16(p.angle) + -2;
                  if (i16(p.angle) < -0xb4) {
                    p.angle = i16(p.angle) + 0x168;
                  }
                }
                else {
                  p.angle = i16(p.angle) + 2;
                  if (0xb4 < i16(p.angle)) {
                    p.angle = i16(p.angle) + -0x168;
                  }
                }
              }
            }
            if (i16(p.animation.cursor.index) < 2) {
              p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter) * 0.05 + double(3.0f));
              owned(0x33e,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              roll = random.range(0,4);
              p.pose.scale[0] = (float)(roll + -2) * 0.05 + double(1.0f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            p.counter = i16(p.counter) + 2;
            if ((0 < i16(p.animation.cursor.index)) && (i16(p.timer) < 0x3c)) {
              if (double(other.motion.y) + double(100.0f) <
                  double(p.motion.y)) {
                p.parameters[0] = double(p.parameters[0]) + double(2.0f);
              }
              if (double(p.motion.y) <
                  double(other.motion.y) + double(100.0f)) {
                p.parameters[0] = double(p.parameters[0]) - double(2.0f);
              }
              p.timer = i16(p.timer) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((p.response != 0) && (p.response != 6)) &&
               (i16(p.animation.cursor.index) == 1)) {
              if ((i16(p.hits) == 0) || (p.response != 4)) {
                p.motion.velocityX = 0.0f;
                p.motion.velocityY = 0.0f;
                p.frame(2);
                p.pose.scale[0] = 2.5f;
                p.pose.scale[1] = 2.5f;
                return;
              }
              if (((0 < i16(p.hits)) && (p.response == 4)) &&
                 (0 < i16(p.hits))) {
                p.response = 0;
              }
            }
            if (1 < i16(p.animation.cursor.index)) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
              if (u8(channel(24)) < 0x19) {
                setChannel(24,u8(0));
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x19));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          for (local_94 = 0; local_94 < 2; local_94 = local_94 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x367:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if (p.response == 5) {
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
              p.counter = 0x3c;
              p.timer = 0;
              p.cycle = 0;
              p.angle = 0;
            }
            if (0 < i16(p.animation.cursor.index)) {
              p.counter = i16(p.counter) + 2;
              aimRadians = atan((double(other.motion.y) -
                           double(p.motion.y)) + double(100.0f),(double(other.motion.x) -
                           double(p.motion.x)) * (float)(int)i8(p.motion.facing));
              uVar1 = i32(aimRadians/double(0.017453289f));
              p.timer = uVar1;
              if (i16(p.timer) < i16(p.angle)) {
                if ((int)i16(p.angle) - (int)i16(p.timer) < 0xb5) {
                  p.angle = i16(p.angle) + -2;
                  if (i16(p.angle) < -0xb4) {
                    p.angle = i16(p.angle) + 0x168;
                  }
                }
                else {
                  p.angle = i16(p.angle) + 2;
                  if (0xb4 < i16(p.angle)) {
                    p.angle = i16(p.angle) + -0x168;
                  }
                }
              }
              if (i16(p.angle) <= i16(p.timer)) {
                if ((int)i16(p.angle) - (int)i16(p.timer) < -0xb4) {
                  p.angle = i16(p.angle) + -2;
                  if (i16(p.angle) < -0xb4) {
                    p.angle = i16(p.angle) + 0x168;
                  }
                }
                else {
                  p.angle = i16(p.angle) + 2;
                  if (0xb4 < i16(p.angle)) {
                    p.angle = i16(p.angle) + -0x168;
                  }
                }
              }
            }
            if (i16(p.animation.cursor.index) < 2) {
              p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter) * 0.05 + double(3.0f));
              owned(0x340,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              roll = random.range(0,4);
              p.pose.scale[0] = (float)(roll + -2) * 0.05 + double(1.0f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            p.counter = i16(p.counter) + 2;
            if ((0 < i16(p.animation.cursor.index)) && (i16(p.timer) < 0x3c)) {
              if (double(other.motion.y) + double(100.0f) <
                  double(p.motion.y)) {
                p.parameters[0] = double(p.parameters[0]) + double(2.0f);
              }
              if (double(p.motion.y) <
                  double(other.motion.y) + double(100.0f)) {
                p.parameters[0] = double(p.parameters[0]) - double(2.0f);
              }
              p.timer = i16(p.timer) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((p.response != 0) && (p.response != 6)) &&
               (i16(p.animation.cursor.index) == 1)) {
              if ((i16(p.hits) == 0) || (p.response != 4)) {
                p.motion.velocityX = 0.0f;
                p.motion.velocityY = 0.0f;
                p.frame(2);
                p.pose.scale[0] = 2.5f;
                p.pose.scale[1] = 2.5f;
                return;
              }
              if (((0 < i16(p.hits)) && (p.response == 4)) &&
                 (0 < i16(p.hits))) {
                p.response = 0;
              }
            }
            if (1 < i16(p.animation.cursor.index)) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
              if (u8(channel(24)) < 0x19) {
                setChannel(24,u8(0));
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x19));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          for (local_98 = 0; local_98 < 2; local_98 = local_98 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x368:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if (p.response == 5) {
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
              p.counter = 0x3c;
              p.timer = 0;
              p.cycle = 0;
              p.angle = 0;
            }
            if (0 < i16(p.animation.cursor.index)) {
              p.counter = i16(p.counter) + 2;
              aimRadians = atan((double(other.motion.y) -
                           double(p.motion.y)) + double(100.0f),(double(other.motion.x) -
                           double(p.motion.x)) * (float)(int)i8(p.motion.facing));
              uVar1 = i32(aimRadians/double(0.017453289f));
              p.timer = uVar1;
              if (i16(p.timer) < i16(p.angle)) {
                if ((int)i16(p.angle) - (int)i16(p.timer) < 0xb5) {
                  p.angle = i16(p.angle) + -2;
                  if (i16(p.angle) < -0xb4) {
                    p.angle = i16(p.angle) + 0x168;
                  }
                }
                else {
                  p.angle = i16(p.angle) + 2;
                  if (0xb4 < i16(p.angle)) {
                    p.angle = i16(p.angle) + -0x168;
                  }
                }
              }
              if (i16(p.angle) <= i16(p.timer)) {
                if ((int)i16(p.angle) - (int)i16(p.timer) < -0xb4) {
                  p.angle = i16(p.angle) + -2;
                  if (i16(p.angle) < -0xb4) {
                    p.angle = i16(p.angle) + 0x168;
                  }
                }
                else {
                  p.angle = i16(p.angle) + 2;
                  if (0xb4 < i16(p.angle)) {
                    p.angle = i16(p.angle) + -0x168;
                  }
                }
              }
            }
            if (i16(p.animation.cursor.index) < 2) {
              p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter) * 0.05 + double(3.0f));
              owned(0x342,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              roll = random.range(0,4);
              p.pose.scale[0] = (float)(roll + -2) * 0.05 + double(1.0f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            p.counter = i16(p.counter) + 2;
            if ((0 < i16(p.animation.cursor.index)) && (i16(p.timer) < 0x3c)) {
              if (double(other.motion.y) + double(100.0f) <
                  double(p.motion.y)) {
                p.parameters[0] = double(p.parameters[0]) + double(2.0f);
              }
              if (double(p.motion.y) <
                  double(other.motion.y) + double(100.0f)) {
                p.parameters[0] = double(p.parameters[0]) - double(2.0f);
              }
              p.timer = i16(p.timer) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((p.response != 0) && (p.response != 6)) &&
               (i16(p.animation.cursor.index) == 1)) {
              if ((i16(p.hits) == 0) || (p.response != 4)) {
                p.motion.velocityX = 0.0f;
                p.motion.velocityY = 0.0f;
                p.frame(2);
                p.pose.scale[0] = 2.5f;
                p.pose.scale[1] = 2.5f;
                return;
              }
              if (((0 < i16(p.hits)) && (p.response == 4)) &&
                 (0 < i16(p.hits))) {
                p.response = 0;
              }
            }
            if (1 < i16(p.animation.cursor.index)) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
              if (u8(channel(24)) < 0x19) {
                setChannel(24,u8(0));
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x19));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          for (local_9c = 0; local_9c < 2; local_9c = local_9c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x369:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if (p.response == 5) {
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
              p.counter = 0x3c;
              p.timer = 0;
              p.cycle = 0;
              p.angle = 0;
            }
            if (0 < i16(p.animation.cursor.index)) {
              p.counter = i16(p.counter) + 2;
              aimRadians = atan((double(other.motion.y) -
                           double(p.motion.y)) + double(100.0f),(double(other.motion.x) -
                           double(p.motion.x)) * (float)(int)i8(p.motion.facing));
              uVar1 = i32(aimRadians/double(0.017453289f));
              p.timer = uVar1;
              if (i16(p.timer) < i16(p.angle)) {
                if ((int)i16(p.angle) - (int)i16(p.timer) < 0xb5) {
                  p.angle = i16(p.angle) + -2;
                  if (i16(p.angle) < -0xb4) {
                    p.angle = i16(p.angle) + 0x168;
                  }
                }
                else {
                  p.angle = i16(p.angle) + 2;
                  if (0xb4 < i16(p.angle)) {
                    p.angle = i16(p.angle) + -0x168;
                  }
                }
              }
              if (i16(p.angle) <= i16(p.timer)) {
                if ((int)i16(p.angle) - (int)i16(p.timer) < -0xb4) {
                  p.angle = i16(p.angle) + -2;
                  if (i16(p.angle) < -0xb4) {
                    p.angle = i16(p.angle) + 0x168;
                  }
                }
                else {
                  p.angle = i16(p.angle) + 2;
                  if (0xb4 < i16(p.angle)) {
                    p.angle = i16(p.angle) + -0x168;
                  }
                }
              }
            }
            if (i16(p.animation.cursor.index) < 2) {
              p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter) * 0.05 + double(3.0f));
              owned(0x344,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              roll = random.range(0,4);
              p.pose.scale[0] = (float)(roll + -2) * 0.05 + double(1.0f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            p.counter = i16(p.counter) + 2;
            if ((0 < i16(p.animation.cursor.index)) && (i16(p.timer) < 0x3c)) {
              if (double(other.motion.y) + double(100.0f) <
                  double(p.motion.y)) {
                p.parameters[0] = double(p.parameters[0]) + double(2.0f);
              }
              if (double(p.motion.y) <
                  double(other.motion.y) + double(100.0f)) {
                p.parameters[0] = double(p.parameters[0]) - double(2.0f);
              }
              p.timer = i16(p.timer) + 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((p.response != 0) && (p.response != 6)) &&
               (i16(p.animation.cursor.index) == 1)) {
              if ((i16(p.hits) == 0) || (p.response != 4)) {
                p.motion.velocityX = 0.0f;
                p.motion.velocityY = 0.0f;
                p.frame(2);
                p.pose.scale[0] = 2.5f;
                p.pose.scale[1] = 2.5f;
                return;
              }
              if (((0 < i16(p.hits)) && (p.response == 4)) &&
                 (0 < i16(p.hits))) {
                p.response = 0;
              }
            }
            if (1 < i16(p.animation.cursor.index)) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
              if (u8(channel(24)) < 0x19) {
                setChannel(24,u8(0));
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x19));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          for (local_a0 = 0; local_a0 < 2; local_a0 = local_a0 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 900:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.counter = 0;
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if (i16(p.animation.cursor.index) < 6) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
            if (double(2.0f) < double(p.pose.scale[0])) {
              p.pose.scale[0] = 2.0f;
            }
            if (double(2.0f) < double(p.pose.scale[1])) {
              p.pose.scale[1] = 2.0f;
            }
          }
          if ((i16(p.animation.cursor.index) < 6) ||
             (angleRadians = double(p.motion.y) + double(p.motion.velocityY),
             angleRadians < double(0.0f) == (angleRadians == double(0.0f)))) {
            if ((i16(owner.animation.cursor.index) == 9) &&
               (i16(p.animation.cursor.index) < 6)) {
              p.frame(i16(p.animation.cursor.index) + 5);
              p.motion.velocityX =
                   ((double(other.motion.x) -
                    double(p.motion.x)) * (float)(int)i8(p.motion.facing)) / double(24.0f);
              if (double(p.motion.velocityX) < double(6.0f)) {
                p.motion.velocityX = 6.0f;
              }
              p.motion.velocityY = -12.0f;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.frame(6);
            }
            else if ((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
          }
          else {
            p.motion.y = 0.0f;
            p.motion.velocityY = 0.0f;
            p.motion.velocityX = 0.0f;
            events.sound(0xb);
            p.action(0x385);
            if ('\0' < state.difficulty) {
              owner.launchParameters[0] = 3.0f;
              owned(0x38e,double(p.motion.x),double(p.motion.y),1,1);
              owned(0x38e,double(p.motion.x),double(p.motion.y),0xffffffff,1);
            }
            if ('\x01' < state.difficulty) {
              owner.launchParameters[0] = 6.0f;
              owned(0x38e,double(p.motion.x),double(p.motion.y),1,1);
              owned(0x38e,double(p.motion.x),double(p.motion.y),0xffffffff,1);
            }
            owned(0x347,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            for (local_a4 = 0; local_a4 < 0xf; local_a4 = local_a4 + 1) {
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            events.shake(5.0f);
          }
        }
        break;
      case 0x385:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if (p.response == 6) {
            p.hits = i16(p.hits) + 1;
            p.response = 0;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.counter = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 2.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[2] = 15.0f;
          }
          if (i16(p.animation.cursor.index) == 2) {
            roll = random.range(0,6);
            p.pose.scale[0] = (float)roll * double(0.1f) + double(0.7f);
            p.pose.scale[1] = double(p.pose.scale[0]) + double(1.0f);
          }
          if (2 < i16(p.animation.cursor.index)) {
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.9f);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
            if (i16(p.counter) < 9) {
              p.counter = i16(p.counter) + 1;
              p.frame(2);
            }
            else {
              p.pose.scale[0] = 1.7999999523162842f;
              p.pose.scale[1] = 1.0f;
            }
          }
        }
        break;
      case 0x386:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if (i16(owner.combat.spellTimer) < 1) {
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
            p.motion.velocityX = 3.0f;
            p.motion.velocityY = 0.0f;
          }
          if (i16(p.animation.cursor.index) == 1) {
            roll = random.range(0,4);
            p.pose.scale[0] = (float)roll * double(0.1f) + 0.8;
            p.pose.scale[1] = double(p.pose.scale[0]);
            fVar5 = (double)std::abs(float(double(p.motion.x) -
                                          double(other.motion.x)));
            if (fVar5 < (double)double(20.0f)) {
              p.motion.velocityX = 0.0f;
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              events.shake(5.0f);
              p.frame(2);
              return;
            }
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            events.shake(10.0f);
            for (local_a8 = 0; local_a8 < 0xc; local_a8 = local_a8 + 1) {
              uVar8 = 1;
              uVar3 = (u32)u8(p.motion.facing);
              roll = random.range(0,0x140);
              fVar7 = (float)roll;
              angleRadians = double(p.motion.x);
              roll = random.range(0,0x50);
              owned(0x387,double(float(double(angleRadians)+60.0))-roll,fVar7,uVar3,uVar8);
            }
            p.destroy();
          }
          else if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(1);
          }
        }
        break;
      case 0x387:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if (i16(owner.combat.spellTimer) < 1) {
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
            roll = random.range(0,0x167);
            p.pose.rotation[2] = (float)roll;
            roll = random.range(0,0x10);
            p.motion.velocityX = (float)(roll + -8);
            roll = random.range(0,6);
            p.motion.velocityY = (float)(roll + 0xc);
          }
          p.motion.velocityY = double(p.motion.velocityY) - double(0.45f);
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(20.0f);
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if ((i16(p.animation.cursor.index) == 1) &&
             (angleRadians = double(p.motion.y) + double(p.motion.velocityY),
             angleRadians < double(0.0f) != (angleRadians == double(0.0f)))) {
            p.frame(2);
            p.motion.velocityY = -double(p.motion.velocityY);
            p.motion.y = 0.0f;
          }
          if (i16(p.animation.cursor.index) == 2) {
            if (u8(channel(24)) < 0x10) {
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
      case 0x388:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if (p.response == 6) {
            p.hits = i16(p.hits) + 1;
            p.response = 0;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,60.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.hits = 1;
            p.response = 0;
            p.motion.velocityY = 22.5f;
            events.sound(3);
            owned(0x345,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 1) {
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(30.0f);
            p.motion.velocityY = double(p.motion.velocityY) - double(1.0f);
            if (double(p.motion.velocityY) < double(0.0f)) {
              p.motion.velocityY = 0.0f;
            }
          }
          if (i16(p.animation.cursor.index) == 2) {
            p.motion.velocityY = double(p.motion.velocityY) - double(0.5f);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            angleRadians = double(p.motion.y) + double(p.motion.velocityY);
            if ((angleRadians < double(0.0f) != (angleRadians == double(0.0f))) ||
               ((p.response != 0 && (p.response != 6)))) {
              p.motion.velocityY = 0.0f;
              p.frame(3);
              events.sound(5);
              return;
            }
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            events.sound(4);
            p.motion.velocityX = -10.0f;
            p.pose.rotation[2] = 0.0f;
          }
        }
        break;
      case 0x389:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if (p.response == 5) {
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
            sVar2 = random.range(0,300);
            p.counter = sVar2 + 200;
            sVar2 = random.range(0,0x32);
            p.timer = sVar2 + 0x2d;
            sVar2 = random.range(0,0xf);
            p.angle = sVar2 + 5;
            p.polar((float)(int)i16(p.timer),(float)(int)i16(p.counter) * double(0.1f));
          }
          if (i16(p.animation.cursor.index) < 2) {
            p.polar((float)(int)i16(p.timer),(float)(int)i16(p.counter) * double(0.1f));
            owned(0x33c,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            roll = random.range(0,4);
            p.pose.scale[0] = (float)(roll + -2) * 0.05 + double(1.0f);
            p.pose.scale[1] = double(p.pose.scale[0]);
            p.counter = i16(p.counter) + -0xf;
            if (i16(p.counter) < i16(p.angle)) {
              p.counter = p.angle;
            }
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (((p.response == 0) || (p.response == 6)) ||
             (i16(p.animation.cursor.index) != 1)) {
            if (1 < i16(p.animation.cursor.index)) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.93f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.93f);
              if (u8(channel(24)) < 0x19) {
                setChannel(24,u8(0));
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x19));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
          else {
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.frame(2);
            p.pose.scale[0] = 2.5f;
            p.pose.scale[1] = 2.5f;
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
            p.counter = 0;
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if (i16(p.animation.cursor.index) < 6) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
            if (double(1.0f) < double(p.pose.scale[0])) {
              p.pose.scale[0] = 2.0f;
            }
            if (double(1.0f) < double(p.pose.scale[1])) {
              p.pose.scale[1] = 2.0f;
            }
          }
          if ((i16(p.animation.cursor.index) < 6) ||
             (angleRadians = double(p.motion.y) + double(p.motion.velocityY),
             angleRadians < double(0.0f) == (angleRadians == double(0.0f)))) {
            if ((i16(owner.animation.cursor.index) == 9) &&
               (i16(p.animation.cursor.index) < 6)) {
              p.frame(i16(p.animation.cursor.index) + 5);
              roll = random.range(0,0x1e);
              p.motion.velocityX = (float)(roll + 10);
              p.motion.velocityY = -20.0f;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.frame(6);
            }
            else if ((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
          }
          else {
            p.motion.y = 0.0f;
            p.motion.velocityY = 0.0f;
            p.motion.velocityX = 0.0f;
            events.sound(0xb);
            p.action(0x385);
            owned(0x347,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            for (local_ac = 0; local_ac < 0xf; local_ac = local_ac + 1) {
              owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            for (local_ac = 0; local_ac < 0xf; local_ac = local_ac + 1) {
              owned(0x389,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            events.shake(5.0f);
          }
        }
        break;
      case 0x38b:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.10000000149011612f;
            p.pose.scale[1] = 0.10000000149011612f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0x80));
            p.motion.velocityX = 20.0f;
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.hits = 0;
            p.response = 0;
          }
          if ((i16(p.angle) == 0) &&
             (p.motion.velocityX = double(p.motion.velocityX) - double(0.8f),
             double(p.motion.velocityX) < double(1.0f))) {
            p.motion.velocityX = 1.0f;
            p.angle = 1;
          }
          if ((0 < i16(p.angle)) && (i16(p.angle) < 0x1e)) {
            p.angle = i16(p.angle) + 1;
          }
          if (0x1d < i16(p.angle)) {
            p.motion.velocityX = double(p.motion.velocityX) + double(0.5f);
          }
          if (i16(p.counter) == 0) {
            if (double(1.0f) <= double(p.pose.scale[0])) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              setChannel(24,u8(0xff));
            }
            else {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
              setChannel(24,u8(i8(channel(24)) + '\x19'));
            }
            if ((i8(channel(24)) == -1) && (i16(p.counter) == 0)) {
              p.counter = i16(p.counter) + 1;
              p.hits = 1;
            }
          }
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          if ((p.response != 0) &&
             (p.counter = i16(p.counter) + 1,
             5 < i16(p.counter))) {
            p.counter = 0;
            p.response = 0;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          if (((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 0x17)) &&
             (0 < i16(p.counter))) {
            p.frame(0x17);
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            else if ((i16(p.animation.cursor.index) == 0x17) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
          }
        }
        break;
      case 0x38c:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 6.0f;
        }
        p.motion.velocityX = double(p.motion.velocityX) + double(0.8f);
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x38e:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.motion.velocityX = double(p.parameters[0]);
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x38f,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          if ((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(120.0f))) {
            p.destroy();
          }
          else {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            sVar2 = p.advance();
            if ((sVar2 == i16(p.animation.cursor.frameCount)) &&
               ((int)i16(p.counter) < state.difficulty * 8 + 10)) {
              p.counter = i16(p.counter) + 1;
              p.frame(1);
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
            p.counter = 0;
            p.hits = 1;
            p.response = 0;
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
            p.hits = 1;
            p.response = 0;
            p.counter = 0;
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.1f) + double(25.0f));
            p.pose.scale[0] = 0.10000000149011612f;
            p.pose.scale[1] = 0.10000000149011612f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          p.counter = i16(p.counter) + 1;
          if ((i16(p.animation.cursor.index) < 2) &&
             (p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.1f) + double(25.0f)),
             double(p.pose.scale[0]) < double(1.0f))) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
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
                if (u8(channel(24)) < 0x19) {
                  setChannel(24,u8(0));
                  p.destroy();
                  return;
                }
                setChannel(24,u8(i8(channel(24)) + -0x19));
                setChannel(16,u8(i8(channel(16)) + -0x19));
                setChannel(8,u8(i8(channel(8)) + -0x19));
                p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
                p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
              }
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
          else {
            p.frame(2);
            p.pose.scale[0] = 4.0f;
            p.pose.scale[1] = 4.0f;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
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
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 3.0f;
            roll = random.range(0,9);
            p.motion.velocityY = double(4.5f) - (float)roll;
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.hits = 0;
            p.response = 0;
          }
          if (double(1.0f) <= double(p.pose.scale[0])) {
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
          }
          else {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
          }
          if ((i16(p.angle) == 0) &&
             (p.motion.velocityY = double(p.motion.velocityY) - double(0.15f),
             double(p.motion.velocityY) < -double(4.5f))) {
            p.angle = 1;
          }
          if ((i16(p.angle) == 1) &&
             (p.motion.velocityY = double(p.motion.velocityY) + double(0.15f),
             double(4.5f) < double(p.motion.velocityY))) {
            p.angle = 0;
          }
          if ((double(p.pose.scale[0]) == double(1.0f)) && ('\x01' < state.difficulty)) {
            if ((double(0.0f) < double(p.motion.velocityX)) && (double(1280.0f) < double(p.motion.x))) {
              p.motion.velocityX = -double(p.motion.velocityX);
            }
            if ((double(p.motion.velocityX) < double(0.0f)) && (double(p.motion.x) < double(0.0f))) {
              p.motion.velocityX = -double(p.motion.velocityX);
            }
          }
          if ((((i16(p.timer) == 0) && (double(p.pose.scale[0]) == double(1.0f))) &&
              (double(other.motion.y) <
               double(p.motion.y))) &&
             (fVar5 = (double)std::abs(float(double(p.motion.x) -
                                            double(other.motion.x))), fVar5 < (double)double(10.0f))) {
            p.timer = 1;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = -30.0f;
            p.pose.scale[0] = 0.10000000149011612f;
            p.pose.scale[1] = 1.0f;
          }
          if ((i16(p.timer) == 1) &&
             (angleRadians = double(p.motion.y) + double(p.motion.velocityY),
             angleRadians < double(0.0f) != (angleRadians == double(0.0f)))) {
            p.destroy();
            owned(0x392,double(p.motion.x),0.0f,u8(p.motion.facing),1);
          }
          else if ((double(1460.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(180.0f))) {
            p.destroy();
          }
          else {
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 0x1f) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
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
            else if (i16(p.timer) == 0) {
              uVar9 = 1;
              uVar8 = 1;
              roll = random.range(0,0x3c);
              owned(0x391,0.0f,(float)(roll + 0x17c),uVar8,uVar9);
              p.timer = 1;
              p.counter = 0;
            }
            else {
              uVar9 = 1;
              uVar8 = 0xffffffff;
              roll = random.range(0,0x78);
              owned(0x391,1280.0f,(float)(roll + 0xf0),uVar8,uVar9);
              p.timer = 0;
              p.counter = 0;
            }
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
          aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                       double(p.motion.y),double(other.motion.x) -
                       double(p.motion.x));
          uVar1 = i32(aimRadians/double(0.017453289f));
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
          aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                       double(p.motion.y),double(other.motion.x) -
                       double(p.motion.x));
          uVar1 = i32(aimRadians/double(0.017453289f));
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
          angleRadians = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[0] = angleRadians + angleRadians;
          angleRadians = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[1] = angleRadians + angleRadians;
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
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

}}
