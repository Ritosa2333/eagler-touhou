#include "RemiliaObjects.hpp"
#include <cmath>
namespace th075 {void advance_remilia_object(RemiliaObject& p,FighterActor& owner,FighterActor& other,RemiliaState& state,ReplayStreams& random,FighterActionEvents& events){
i32 lastRoll=0;const auto nextRandom=[&](i32 low,i32 high){return lastRoll=random.range(low,high);};
const auto consumeSpirit=[&](i16 amount){if(owner.combat.spiritBreak<1){owner.combat.spirit=i16(u16(owner.combat.spirit)-u16(amount));if(owner.combat.spirit<1&&owner.combat.spiritBreak<1){events.system_sound(43);owner.combat.spirit=0;owner.combat.spiritBreak=1000;}}else owner.combat.spiritBreak=1000;};
const auto shared=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Shared,id,x,y,facing,layer,owner.launchParameters);};
double aimRadians=0;const auto atan=[](float y,float x){return std::atan2(double(y),double(x));};
const auto channel=[&](u32 shift){return p.pose.channel(shift);};const auto setChannel=[&](u32 shift,u8 value){p.pose.set_channel(shift,value);};
const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters);};
const auto ownedVisual=[&](i16 id,float x,float y,i8 facing,u8 layer,u8 visual,u8 order){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters,order,visual?&owner.effectVisual:nullptr);};

  u16 uVar1;
  i16 sVar2;
  int iVar3;
  u32 uVar4;
  double fVar5;
  float angleRadians;
  float fVar7;
  float launchSpeed;
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
        if ((p.response == 6) && (i16(p.animation.cursor.index) == 4)) {
          p.response = 0;
          p.hits = 1;
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
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
        if ((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(120.0f))) {
          p.destroy();
        }
        else {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(5.0f);
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if ((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 5)) {
            p.frame(5);
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.5f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.5f);
          }
          else {
            if (i16(p.animation.cursor.index) == 5) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.75f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.75f);
            }
            if (i16(p.animation.cursor.index) == 1) {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.05f);
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.05f);
              if (double(2.5f) < double(p.pose.scale[0])) {
                p.pose.scale[0] = 2.5f;
                p.pose.scale[1] = 2.5f;
              }
              if ((i16(owner.animation.cursor.action) == 400) &&
                 (4 < i16(owner.animation.cursor.index))) {
                p.frame(i16(p.counter) + 2);
                p.hits = i16(p.counter) + 1;
                p.motion.velocityX = (float)(int)i16(p.counter) * double(4.0f) + double(12.0f);
                return;
              }
              if (i16(owner.animation.cursor.action) != 400) {
                p.frame(5);
              }
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
              p.counter = i16(p.counter) + 1;
              if (2 < i16(p.counter)) {
                p.counter = 2;
              }
            }
            if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.response = 0;
              p.frame(2);
            }
            if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.response = 0;
              p.frame(3);
            }
            if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.response = 0;
              p.frame(4);
            }
          }
        }
        break;
      case 0x321:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 0;
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[1] = 60.0f;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.3f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
        if ((((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(160.0f))) {
          p.destroy();
        }
        else {
          if (u8(channel(24)) < 0x10) {
            setChannel(24,u8(0));
          }
          else {
            setChannel(24,u8(i8(channel(24)) + -0xf));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x322:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (1 < i16(p.animation.cursor.index))) {
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
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
          }
          if (((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (0 < i16(owner.combat.spirit))) {
            consumeSpirit(0xf);
            events.sound(0);
            owned(0x323,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
             ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f))))) {
            p.destroy();
          }
          else if ((i16(p.animation.cursor.index) < 2) &&
                  ((i16(owner.animation.cursor.action) != 600 ||
                   ((i16(owner.animation.cursor.action) == 600 &&
                    (9 < i16(owner.animation.cursor.index))))))) {
            p.frame(2);
          }
          else {
            if (i16(p.animation.cursor.index) == 2) {
              if (u8(channel(24)) < 0x10) {
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
            if ((((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) &&
                (i16(owner.animation.cursor.action) == 600)) &&
               ((i16(p.counter) < 3 &&
                (0 < i16(owner.combat.spirit))))) {
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
          for (local_8 = 0; local_8 < 2; local_8 = local_8 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x323:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (i16(p.hits) < 1)) {
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
            p.motion.velocityX = 15.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          if ((((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
              (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(160.0f))) {
            p.destroy();
          }
          else {
            if (i16(p.hits) == 0) {
              if (u8(channel(24)) < 0xb) {
                p.destroy();
              }
              else {
                setChannel(24,u8(i8(channel(24)) + -10));
              }
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
          for (local_c = 0; local_c < 2; local_c = local_c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
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
            uVar1 = i16(i32(p.parameters[0]));
            p.timer = uVar1;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            iVar3 = nextRandom(0x640,0x960);
            angleRadians = (float)iVar3 * double(0.01f);
            sVar2 = i16(p.timer);
            iVar3 = nextRandom(0,0x1e);
            p.polar((float)((sVar2 + 0xf) - iVar3),angleRadians);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          if ((((p.response == 1) || (p.response == 2)) ||
              (p.response == 4)) && (i16(p.animation.cursor.index) < 2)) {
            p.frame(2);
          }
          else {
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
          for (local_10 = 0; local_10 < 2; local_10 = local_10 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x325:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.response = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.5f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.hits = 5;
          p.motion.velocityX = 15.0f;
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.velocityX = double(p.motion.velocityX) - double(0.5f);
        if (double(p.motion.velocityX) < double(8.0f)) {
          p.motion.velocityX = 8.0f;
        }
        if (p.response == 4) {
          p.response = 0;
          p.hits = i16(p.hits) + -1;
        }
        if (p.response == 6) {
          p.response = 0;
          p.hits = i16(p.hits) + 1;
        }
        if (p.response != 0) {
          p.hits = 0;
        }
        if ((i16(p.animation.cursor.index) == 1) && (i16(p.hits) == 0)) {
          p.frame(2);
        }
        if (i16(p.animation.cursor.index) == 0) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
        }
        if (i16(p.animation.cursor.index) == 2) {
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
      case 0x326:
        if ((i16(owner.animation.cursor.action) == 700) &&
           (i16(p.animation.cursor.index) < 0x13)) {
          p.motion.x = double(owner.motion.x);
        }
        if ((i16(owner.animation.cursor.action) == 700) ||
           (0x12 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.hits = 0xc;
            p.response = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.timer = 0;
          }
          if ((p.response != 0) &&
             (p.timer = i16(p.timer) + 1,
             5 < i16(p.timer))) {
            p.timer = 0;
            p.response = 0;
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          else if (((i16(p.animation.cursor.index) == 0x13) && (i16(p.animation.cursor.elapsed) == 0)) &&
                  (i16(p.counter) < 3)) {
            p.counter = i16(p.counter) + 1;
            p.response = 0;
            p.frame(9);
          }
        }
        else {
          p.frame(0x13);
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
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
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
          p.hits = 5;
          p.response = 0;
          p.motion.velocityX = 45.0f;
          p.motion.velocityY = 0.0f;
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        if ((double(1800.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(680.0f))) {
          p.destroy();
        }
        else {
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.frame(1);
            p.response = 0;
          }
        }
        break;
      case 0x329:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = (float)(i8(p.motion.facing) * -0x23);
          p.pose.rotation[1] = 70.0f;
          iVar3 = nextRandom(0,0x167);
          p.pose.rotation[2] = (float)iVar3;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(1.05f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(1.05f);
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(6.0f);
        if (u8(channel(24)) < 0xb) {
          p.destroy();
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -10));
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x32a:
        if ((i16(owner.animation.cursor.action) == 0x2c1) &&
           (i16(p.animation.cursor.index) < 0x13)) {
          p.motion.x = double(owner.motion.x) + double(0.0f);
        }
        if ((i16(owner.animation.cursor.action) == 0x2c1) ||
           (0x12 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.hits = 0x14;
            p.response = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.5f;
            p.pose.scale[1] = 1.25f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.timer = 0;
          }
          if ((p.response != 0) &&
             (p.timer = i16(p.timer) + 1,
             5 < i16(p.timer))) {
            p.timer = 0;
            p.response = 0;
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          else if (((i16(p.animation.cursor.index) == 0x13) && (i16(p.animation.cursor.elapsed) == 0)) &&
                  (i16(p.counter) < 6)) {
            p.counter = i16(p.counter) + 1;
            p.response = 0;
            p.frame(9);
          }
        }
        else {
          p.frame(0x13);
        }
        break;
      case 0x32b:
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
          p.motion.velocityX = 45.0f;
          p.motion.velocityY = 0.0f;
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        if ((double(1800.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(680.0f))) {
          p.destroy();
        }
        else {
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.frame(1);
            p.response = 0;
          }
        }
        break;
      default:
        p.advance();
        break;
      case 0x32e:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = (float)(i8(p.motion.facing) * -0x1e);
          p.pose.rotation[1] = 70.0f;
          iVar3 = nextRandom(0,0x167);
          p.pose.rotation[2] = (float)iVar3;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(1.05f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(1.05f);
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(6.0f);
        if (u8(channel(24)) < 0xb) {
          p.destroy();
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -10));
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
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
            launchSpeed = 25.0f;
            angleRadians = double(p.parameters[0]);
            iVar3 = nextRandom(0,0x1e);
            p.polar(double(float(double(angleRadians)+15.0))-iVar3,launchSpeed);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          if ((((p.response == 1) || (p.response == 2)) ||
              (p.response == 4)) && (i16(p.animation.cursor.index) < 2)) {
            p.frame(2);
          }
          else {
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
          for (local_14 = 0; local_14 < 3; local_14 = local_14 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x331:
        if ((((double(1380.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(100.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(240.0f))) {
          p.destroy();
        }
        else if (p.response == 5) {
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
            p.counter = 10;
            p.timer = 0;
            p.cycle = 0;
            uVar1 = i16(i32(p.parameters[0]));
            p.angle = uVar1;
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter) * double(0.1f) + double(10.0f));
          }
          if (i16(p.animation.cursor.index) < 5) {
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
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.counter) * double(0.15f) + double(7.0f));
          }
          p.counter = i16(p.counter) + 2;
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
             ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f))))) {
            p.destroy();
          }
          else {
            if ((p.response != 0) && (p.response != 6)) {
              p.frame(5);
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.pose.scale[0] = 2.0f;
              p.pose.scale[1] = 2.0f;
            }
            if (i16(p.animation.cursor.index) == 5) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
              if (u8(channel(24)) < 0x19) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x19));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
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
          for (local_18 = 0; local_18 < 5; local_18 = local_18 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x333:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = (float)(i8(p.motion.facing) * 0x23);
          p.pose.rotation[1] = 70.0f;
          iVar3 = nextRandom(0,0x167);
          p.pose.rotation[2] = (float)iVar3;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) * double(1.05f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(1.05f);
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(6.0f);
        if (u8(channel(24)) < 0xb) {
          p.destroy();
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -10));
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x334:
        if (((i16(owner.animation.cursor.action) == 0x2bf) ||
            (i16(owner.animation.cursor.action) == 0x2c4)) ||
           (i16(owner.animation.cursor.action) == 0x2e5)) {
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
        if ((i16(owner.combat.freeze) == 0) &&
           (sVar2 = p.advance(), sVar2 == i16(p.animation.cursor.frameCount))) {
          p.destroy();
        }
        break;
      case 0x335:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            uVar1 = i16(i32(p.parameters[0]));
            p.timer = uVar1;
            p.angle = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 6;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          p.polar((float)(int)i16(p.timer),25.0f);
          fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
          p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          if ((p.response == 4) && (0 < i16(p.hits))) {
            p.response = 0;
          }
          if ((p.response != 0) && (p.response != 0)) {
            p.hits = 0;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if ((((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
              (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(160.0f))) {
            p.destroy();
          }
          else {
            if (p.response != 0) {
              if (u8(channel(24)) < 0x15) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x14));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.frame(4);
            }
            if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = (float)(int)i16(p.timer);
              owned(0x345,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
            if ((i16(p.animation.cursor.index) == 9) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = (float)(int)i16(p.timer);
              owned(0x345,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          for (local_1c = 0; local_1c < 10; local_1c = local_1c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x336:
        if (i16(owner.animation.cursor.action) == 0x1f6) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(owner.animation.cursor.action) == 0x1f6) &&
           ((i16(owner.animation.cursor.action) != 0x1f6 ||
            (i16(owner.animation.cursor.index) < 0x10)))) {
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
      case 0x337:
        if (i16(owner.animation.cursor.action) == 0x1f7) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(owner.animation.cursor.action) == 0x1f7) &&
           ((i16(owner.animation.cursor.action) != 0x1f7 ||
            (i16(owner.animation.cursor.index) < 0x10)))) {
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
      case 0x338:
        if (i16(owner.animation.cursor.action) == 0x1f8) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(owner.animation.cursor.action) == 0x1f8) &&
           ((i16(owner.animation.cursor.action) != 0x1f8 ||
            (i16(owner.animation.cursor.index) < 0x10)))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,80.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[2] = -(double(p.parameters[0]) + double(15.0f));
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
      case 0x339:
        if (i16(owner.animation.cursor.action) == 0x130) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if (i16(owner.animation.cursor.action) == 0x130) {
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
      case 0x33a:
        if ((i16(owner.animation.cursor.action) == 0x1fe) ||
           (i16(owner.animation.cursor.action) == 0x1ff)) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((((i16(owner.animation.cursor.action) != 0x1fe) &&
             (i16(owner.animation.cursor.action) != 0x1ff)) ||
            ((i16(owner.animation.cursor.action) == 0x1fe &&
             (7 < i16(owner.animation.cursor.index))))) ||
           ((i16(owner.animation.cursor.action) == 0x1ff &&
            (0xb < i16(owner.animation.cursor.index))))) {
          if (u8(channel(24)) < 0x1f) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x1e));
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          if (i16(owner.animation.cursor.action) == 0x1ff) {
            p.pose.scale[0] = 1.5f;
            p.pose.scale[1] = 1.5f;
            p.pose.rotation[2] = 20.0f;
          }
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if ((i16(owner.combat.freeze) == 0) &&
           (sVar2 = p.advance(), sVar2 == i16(p.animation.cursor.frameCount))) {
          p.frame(0);
        }
        break;
      case 0x33b:
        if (((i16(owner.animation.cursor.action) == 700) ||
            (i16(owner.animation.cursor.action) == 0x2c1)) &&
           (i16(p.animation.cursor.index) < 0x13)) {
          p.motion.x = double(owner.motion.x);
        }
        if (((i16(owner.animation.cursor.action) != 700) &&
            (i16(owner.animation.cursor.action) != 0x2c1)) ||
           ((i16(owner.animation.cursor.action) == 700 &&
            (7 < i16(owner.animation.cursor.index))))) {
          if (u8(channel(24)) < 0x15) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x14));
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
          p.frame(0);
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
          p.motion.velocityX = 45.0f;
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        if ((double(1640.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(360.0f))) {
          p.destroy();
        }
        else {
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.frame(0);
          }
        }
        break;
      case 0x33d:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 2.0f;
          p.pose.scale[1] = 2.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 45.0f;
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        if ((double(1640.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(360.0f))) {
          p.destroy();
        }
        else {
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.frame(0);
          }
        }
        break;
      case 0x33e:
        if (i16(owner.animation.cursor.action) == 0x136) {
          p.motion.x = double(owner.motion.x);
        }
        if (i16(owner.animation.cursor.action) == 0x136) {
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
      case 0x33f:
        if (i16(owner.animation.cursor.action) == 0x135) {
          p.motion.x = double(owner.motion.x);
        }
        if (i16(owner.animation.cursor.action) == 0x135) {
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
        }
        else {
          p.destroy();
        }
        break;
      case 0x340:
        if (((i16(owner.animation.cursor.action) == 700) ||
            (i16(owner.animation.cursor.action) == 0x2c1)) &&
           (i16(p.animation.cursor.index) < 0x13)) {
          p.motion.x = double(owner.motion.x);
        }
        if (((i16(owner.animation.cursor.action) != 700) &&
            (i16(owner.animation.cursor.action) != 0x2c1)) ||
           ((i16(owner.animation.cursor.action) == 0x2c1 &&
            (7 < i16(owner.animation.cursor.index))))) {
          if (u8(channel(24)) < 0x15) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x14));
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
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.frame(0);
        }
        break;
      case 0x341:
        if (i16(owner.animation.cursor.action) == 0x133) {
          p.motion.x = double(owner.motion.x);
        }
        if (i16(owner.animation.cursor.action) == 0x133) {
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
      case 0x342:
        if (i16(owner.animation.cursor.action) == 0x133) {
          p.motion.x = double(owner.motion.x);
        }
        if (i16(owner.animation.cursor.action) == 0x133) {
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
      case 0x343:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 0;
          p.response = 0;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          iVar3 = nextRandom(0,0x167);
          p.pose.rotation[2] = (float)iVar3;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if (p.response == 6) {
          p.response = 0;
          p.hits = i16(p.hits) + 1;
        }
        if (p.response == 4) {
          p.response = 0;
        }
        if ((((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
            (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(160.0f))) {
          p.destroy();
        }
        else {
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x344:
        if ((i16(owner.animation.cursor.action) == 0x2c5) &&
           (i16(p.animation.cursor.index) < 0x10)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x37) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(86.0f);
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.5f;
            p.pose.scale[1] = 3.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[2] = -120.0f;
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.frame(0);
          }
        }
        else {
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
          p.pose.rotation[1] = 60.0f;
          p.pose.rotation[0] =
               (float)-(int)i8(p.motion.facing) * double(p.parameters[0]);
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.counter = 10;
          p.response = 0;
        }
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
        p.pose.scale[0] =
             (float)(int)i16(p.counter) * double(0.05f) + double(0.05f) + double(p.pose.scale[0]);
        p.pose.scale[1] =
             (float)(int)i16(p.counter) * double(0.05f) + double(0.05f) + double(p.pose.scale[1]);
        if (0 < i16(p.counter)) {
          p.counter = i16(p.counter) + -1;
        }
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
      case 0x346:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            uVar1 = i16(i32(p.parameters[0]));
            p.timer = uVar1;
            p.angle = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 6;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          p.polar((float)(int)i16(p.timer),3.0f);
          fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
          p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          if ((p.response == 4) && (0 < i16(p.hits))) {
            p.response = 0;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if ((((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
              (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(160.0f))) {
            p.destroy();
          }
          else {
            if (i16(p.hits) == 0) {
              if (u8(channel(24)) < 0x15) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x14));
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.frame(4);
            }
            if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = (float)(int)i16(p.timer);
              owned(0x345,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              if (p.response == 2) {
                p.response = 0;
              }
              else {
                p.action(0x335);
              }
            }
            if ((i16(p.animation.cursor.index) == 9) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = (float)(int)i16(p.timer);
              owned(0x345,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              if (p.response == 2) {
                p.response = 0;
              }
              else {
                p.action(0x335);
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
          for (local_20 = 0; local_20 < 10; local_20 = local_20 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x347:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 3;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 18.0f;
            p.motion.velocityY = 0.0f;
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x343,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          p.motion.velocityX = double(p.motion.velocityX) - double(0.45f);
          if (double(p.motion.velocityX) < -double(18.0f)) {
            p.motion.velocityX = -18.0f;
          }
          if (p.response == 4) {
            p.response = 0;
          }
          if (i16(p.hits) == 0) {
            p.destroy();
          }
          else {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if ((((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
                (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(160.0f))) {
              p.destroy();
            }
            else {
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                if (i16(p.counter) < 7) {
                  p.frame(1);
                  p.response = 0;
                  p.counter = i16(p.counter) + 1;
                }
                else {
                  p.destroy();
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
          for (local_24 = 0; local_24 < 10; local_24 = local_24 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x352:
        p.motion.facing = u8(owner.motion.facing);
        if ((0x95 < i16(owner.animation.cursor.action)) &&
           (i16(owner.animation.cursor.action) < 0x9a)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x1e) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(94.0f);
        }
        if ((0x99 < i16(owner.animation.cursor.action)) &&
           (i16(owner.animation.cursor.action) < 0x9e)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x46) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(60.0f);
        }
        if (i16(owner.animation.cursor.action) == 0x9e) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x39) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
        }
        if (((i16(owner.animation.cursor.action) < 0x96) ||
            (0x9e < i16(owner.animation.cursor.action))) &&
           (i16(p.animation.cursor.index) == 0)) {
          p.frame(1);
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
      case 900:
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
            p.polar(double(p.parameters[0]),40.0f);
            fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          owner.launchParameters[0] = -double(p.parameters[0]);
          launchSpeed = 1;
          uVar4 = (u32)u8(p.motion.facing);
          angleRadians = double(p.motion.y);
          iVar3 = nextRandom(0,0x3c);
          angleRadians = double(float(double(angleRadians)+30.0))-iVar3;
          iVar3 = nextRandom(0,0x3c);
          owned(0x385,((float)iVar3 + double(p.motion.x)) - double(30.0f),angleRadians,uVar4,launchSpeed);
          if ((double(1800.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(680.0f))) {
            p.destroy();
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.frame(1);
              p.response = 0;
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
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            uVar1 = nextRandom(0,5);
            p.counter = uVar1;
            p.hits = 1;
            p.response = 0;
            iVar3 = nextRandom(5,10);
            fVar7 = (float)iVar3;
            angleRadians = double(p.parameters[0]);
            iVar3 = nextRandom(0,0x28);
            p.polar((-angleRadians + double(20.0f)) - (float)iVar3,fVar7);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if ((((double(1300.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(60.0f))) ||
              (double(960.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(160.0f))) {
            p.destroy();
          }
          else {
            if (i16(p.animation.cursor.index) == 0) {
              if ((float)(int)i16(p.counter) * double(0.1f) + double(0.5f) <= double(p.pose.scale[0]))
              {
                p.frame(1);
                return;
              }
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            if (i16(p.animation.cursor.index) == 2) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.9f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
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
            p.pose.scale[0] = 4.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[2] = -double(p.parameters[0]);
          }
          p.pose.scale[1] = double(p.pose.scale[1]) * 0.85;
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
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
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            uVar1 = nextRandom(0,5);
            p.counter = uVar1;
            p.hits = 1;
            p.response = 0;
            iVar3 = nextRandom(5,10);
            angleRadians = (float)iVar3;
            iVar3 = nextRandom(0,0x167);
            p.polar((float)iVar3,angleRadians);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if ((((double(1300.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(60.0f))) ||
              (double(960.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(160.0f))) {
            p.destroy();
          }
          else {
            if (i16(p.animation.cursor.index) == 0) {
              if ((float)(int)i16(p.counter) * double(0.1f) + double(0.5f) <= double(p.pose.scale[0]))
              {
                p.frame(1);
                return;
              }
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            if (i16(p.animation.cursor.index) == 2) {
              p.pose.scale[0] = double(p.pose.scale[0]) * double(0.9f);
              p.pose.scale[1] = double(p.pose.scale[0]);
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
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
            p.hits = 1;
            p.response = 0;
            p.polar(double(p.parameters[0]),40.0f);
            fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          owner.launchParameters[0] = -double(p.parameters[0]);
          launchSpeed = 1;
          uVar4 = (u32)u8(p.motion.facing);
          angleRadians = double(p.motion.y);
          iVar3 = nextRandom(0,0x3c);
          angleRadians = double(float(double(angleRadians)+30.0))-iVar3;
          iVar3 = nextRandom(0,0x3c);
          owned(0x385,((float)iVar3 + double(p.motion.x)) - double(30.0f),angleRadians,uVar4,launchSpeed);
          if ((double(1800.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(680.0f))) {
            p.destroy();
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.frame(1);
              p.response = 0;
            }
          }
        }
        break;
      case 0x389:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(owner.animation.cursor.action) == 0x2ef) &&
             (i16(p.animation.cursor.index) < 2)) {
            p.motion.x = double(owner.motion.x);
            p.motion.y = double(owner.motion.y) + double(250.0f);
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
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
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.05f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.05f);
            if (double(1.0f) < double(p.pose.scale[0])) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
            }
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(2.0f);
          if ((((double(1800.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(680.0f))) ||
              (double(1280.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(380.0f))) {
            p.destroy();
          }
          else {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.frame(2);
              p.response = 0;
            }
            else {
              if (i16(p.animation.cursor.elapsed) == 0) {
                p.response = 0;
              }
              if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
                events.sound(0x11);
                launchSpeed = 20.0f;
                fVar5 = (double)atan((double(other.motion.y) + double(100.0f)) -
                                              double(p.motion.y),(double(other.motion.x) - double(p.motion.x)) *
                                              (float)(int)i8(p.motion.facing));
                p.polar((float)(fVar5 / (double)double(0.017453289f)),launchSpeed);
              }
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
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
            if (0xf < i16(p.animation.cursor.elapsed)) {
              if (u8(channel(24)) < 0x10) {
                setChannel(24,u8(0));
              }
              else {
                setChannel(24,u8(i8(channel(24)) + -0xf));
              }
            }
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(2.0f);
          if ((((double(1800.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(680.0f))) ||
              (double(1280.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(380.0f))) {
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
      case 0x38b:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if (((i16(owner.animation.cursor.action) == 0x2ef) ||
              (i16(owner.animation.cursor.action) == 0x2f1)) &&
             (i16(p.animation.cursor.index) < 2)) {
            p.motion.x = double(owner.motion.x);
            p.motion.y = double(owner.motion.y) + double(250.0f);
          }
          if (((i16(owner.animation.cursor.action) == 0x2ef) ||
              (i16(owner.animation.cursor.action) == 0x2f1)) &&
             ((7 < i16(owner.animation.cursor.index) && (i16(p.animation.cursor.index) < 2)))
             ) {
            p.frame(2);
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
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
            }
            if (i16(p.animation.cursor.index) == 0) {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
              if (double(1.0f) < double(p.pose.scale[0])) {
                p.pose.scale[0] = 1.0f;
                p.pose.scale[1] = 1.0f;
              }
            }
            if (i16(p.animation.cursor.index) == 2) {
              p.pose.scale[0] = double(p.pose.scale[1]) * double(0.85f);
              p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.frame(2);
              p.response = 0;
            }
            else {
              if (i16(p.animation.cursor.elapsed) == 0) {
                p.response = 0;
              }
              if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
                uVar1 = nextRandom(0,0x28);
                p.timer = uVar1;
                p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.8f);
                p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.8f);
                p.frame(1);
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
          if ((i16(owner.animation.cursor.action) == 0x2f0) &&
             (i16(p.animation.cursor.index) < 2)) {
            p.motion.x = double(owner.motion.x);
            p.motion.y = 0.0f;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[0] = 75.0f;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if (i16(p.animation.cursor.index) == 1) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
            if (double(1.0f) < double(p.pose.scale[0])) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
            }
          }
          if (i16(p.animation.cursor.index) == 2) {
            if (u8(channel(24)) < 0x10) {
              setChannel(24,u8(0));
            }
            else {
              setChannel(24,u8(i8(channel(24)) + -0xf));
            }
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(2.0f);
          if ((((double(1800.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(680.0f))) ||
              (double(1280.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(380.0f))) {
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
      case 0x38d:
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
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if (i16(p.animation.cursor.index) == 0) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.5f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(2.0f);
          }
          if (0 < i16(p.animation.cursor.index)) {
            p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.2f);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x38e:
        if (i16(owner.animation.cursor.action) == 0x2e5) {
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
          p.timer = 0;
        }
        if (i16(owner.combat.freeze) == 0) {
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if (((i16(p.animation.cursor.index) == 0x26) && (i16(p.animation.cursor.elapsed) == 0)) &&
             ((int)i16(p.timer) < (3 - state.difficulty) * 2)) {
            p.timer = i16(p.timer) + 1;
            p.frame(0x25);
          }
        }
        break;
      case 0x38f:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(owner.animation.cursor.action) == 0x2f0) &&
             (i16(p.animation.cursor.index) < 2)) {
            p.motion.x = double(owner.motion.x);
            p.motion.y = 0.0f;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[0] = 75.0f;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if (i16(p.animation.cursor.index) == 1) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.1f);
            if (0xf < i16(p.animation.cursor.elapsed)) {
              if (u8(channel(24)) < 0x10) {
                setChannel(24,u8(0));
              }
              else {
                setChannel(24,u8(i8(channel(24)) + -0xf));
              }
            }
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(2.0f);
          if ((((double(1800.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(680.0f))) ||
              (double(1280.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(380.0f))) {
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
      case 0x390:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            sVar2 = nextRandom(0,state.difficulty * 3 + 0x28);
            p.counter = -0x14 - sVar2;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.pose.scale[0] = 0.25f;
            p.pose.scale[1] = 0.25f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.polar((float)(int)i16(p.counter),30.0f);
            fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          angleRadians = double(p.motion.y) + double(p.motion.velocityY);
          if (angleRadians < double(0.0f) == (angleRadians == double(0.0f))) {
            if ((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(120.0f))) {
              p.destroy();
            }
            else {
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
            }
          }
          else {
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.motion.y = 0.0f;
            events.shake(3.0f);
            p.action(0x38d);
            p.pose.rotation[2] = 0.0f;
            setChannel(24,u8(0));
            events.sound(0x16);
          }
        }
        break;
      case 0x391:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            uVar1 = i16(i32(p.parameters[0]));
            p.timer = uVar1;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.polar((float)(int)i16(p.timer),18.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
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
              p.destroy();
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
            uVar1 = i16(i32(p.parameters[0]));
            p.timer = uVar1;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            iVar3 = nextRandom(0,9);
            angleRadians = (float)(iVar3 + 6);
            sVar2 = i16(p.timer);
            iVar3 = nextRandom(0,0x14);
            p.polar((float)((sVar2 + 10) - iVar3),angleRadians);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((((p.response == 1) || (p.response == 2)) ||
              (p.response == 4)) && (i16(p.animation.cursor.index) < 2)) {
            p.frame(2);
          }
          else {
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
                p.destroy();
              }
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
            p.counter = 0;
            uVar1 = i16(i32(p.parameters[0]));
            p.timer = uVar1;
            p.angle = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.polar((float)(int)i16(p.timer),7.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.counter = i16(p.counter) + -1;
          if ((state.difficulty == '\x03') &&
             (p.timer = i16(p.timer) + 1,
             0xb4 < i16(p.timer))) {
            p.timer = i16(p.timer) + -0x168;
          }
          p.polar((float)(int)i16(p.timer),7.0f);
          if (i16(p.counter) < 1) {
            owner.launchParameters[0] = (float)(int)i16(p.timer);
            owned(0x394,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            p.counter = 10 - state.difficulty;
          }
          if ('\0' < state.difficulty) {
            if (((double(owner.combat.screenBounds[2]) < double(p.motion.y)) &&
                (i16(p.angle) < 1)) && (double(p.motion.velocityY) < double(0.0f))) {
              events.sound(0xb);
              p.timer = -i16(p.timer);
              p.angle = i16(p.angle) + 1;
              p.response = 0;
              p.hits = 1;
              p.parameters[1] = 4.0f;
            }
            if (((double(p.motion.y) < double(0.0f)) && (i16(p.angle) < 1)) &&
               (double(0.0f) < double(p.motion.velocityY))) {
              events.sound(0xb);
              p.timer = -i16(p.timer);
              p.angle = i16(p.angle) + 1;
              p.response = 0;
              p.hits = 1;
              p.parameters[1] = 4.0f;
            }
            if (((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
                (i16(p.angle) < 1)) &&
               (((i8(p.motion.facing) == -1 && (double(0.0f) < double(p.motion.velocityX))) ||
                ((i8(p.motion.facing) == '\x01' && (double(p.motion.velocityX) < double(0.0f))))))) {
              events.sound(0xb);
              if (0 < i16(p.timer)) {
                p.timer = 0xb4 - i16(p.timer);
              }
              if (i16(p.timer) < 0) {
                p.timer = -0xb4 - i16(p.timer);
              }
              p.angle = i16(p.angle) + 1;
              p.response = 0;
              p.hits = 1;
              p.parameters[1] = 4.0f;
            }
            if (((double(owner.combat.screenBounds[1]) < double(p.motion.x)) &&
                (i16(p.angle) < 1)) &&
               (((i8(p.motion.facing) == -1 && (double(p.motion.velocityX) < double(0.0f))) ||
                ((i8(p.motion.facing) == '\x01' && (double(0.0f) < double(p.motion.velocityX))))))) {
              events.sound(0xb);
              if (0 < i16(p.timer)) {
                p.timer = 0xb4 - i16(p.timer);
              }
              if (i16(p.timer) < 0) {
                p.timer = -0xb4 - i16(p.timer);
              }
              p.angle = i16(p.angle) + 1;
              p.response = 0;
              p.hits = 1;
              p.parameters[1] = 4.0f;
            }
          }
          if ((((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
              (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(160.0f))) {
            p.destroy();
          }
          else {
            if (i16(p.animation.cursor.index) == 2) {
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
            p.pose.scale[0] = 3.0f;
            p.pose.scale[1] = 3.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            nextRandom(0,0x28);
            uVar1 = i16(i32(double(lastRoll)+double(p.parameters[0])-double(20.0f)));
            p.timer = uVar1;
            p.polar((float)(int)i16(p.timer),(float)(int)i16(p.counter) * double(0.05f));
            fVar5 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
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
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
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
