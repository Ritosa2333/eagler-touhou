#include "AliceObjects.hpp"
#include <cmath>
namespace th075 {
static void advance_alice_doll_hit(AliceObject& p,FighterActor& owner,FighterActor& other,AliceState& state,ReplayStreams& random,FighterActionEvents& events){

 double aimRadians=0;const auto atan=[](float y,float x){return std::atan2(double(y),double(x));};
 const auto channel=[&](u32 shift){return p.pose.channel(shift);};const auto setChannel=[&](u32 shift,u8 value){p.pose.set_channel(shift,value);};
 const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters);};

  u16 uVar1;
  i16 sVar2;
  double fVar3;
  int local_8;
  
  if (p.response == 5) {
    p.destroy();
  }
  else if (((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) || (1 < i16(p.animation.cursor.index))
          ) {
    if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
      uVar1 = i16(i32(p.parameters[0]));
      p.timer = uVar1;
      uVar1 = i16(i32(p.parameters[1]));
      p.angle = uVar1;
      p.begin_pose(0.0f,0.0f,0.0f);
      p.hits = 1;
      p.response = 0;
      p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer) * double(0.01f));
      p.pose.scale[0] = 1.5f;
      p.pose.scale[1] = 1.5f;
      setChannel(24,u8(0xff));
      setChannel(16,u8(0xff));
      setChannel(8,u8(0xff));
      setChannel(0,u8(0xff));
      fVar3 = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX))
      ;
      p.pose.rotation[2] = (float)(fVar3 / (double)double(0.017453289f));
    }
    if ((((p.response == 1) || (p.response == 2)) ||
        ((p.response == 4 || (p.response == 6)))) &&
       (i16(p.animation.cursor.index) < 2)) {
      p.frame(2);
      p.motion.velocityX = 0.0f;
      p.motion.velocityY = 0.0f;
    }
    else {
      if (i16(p.animation.cursor.index) < 2) {
        p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer) * double(0.01f));
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
      }
      if (i16(p.animation.cursor.index) == 2) {
        p.pose.scale[0] = double(p.pose.scale[0]) * double(0.85f);
        p.pose.scale[1] = double(p.pose.scale[0]);
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
    }
  }
  else {
    p.destroy();
    for (local_8 = 0; local_8 < 1; local_8 = local_8 + 1) {
      owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
    }
    if (0x19 < i16(owner.knockdownTime)) {
      owner.knockdownTime = 0;
      owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
    }
  }
  return;

}
void advance_alice_object(AliceObject& p,FighterActor& owner,FighterActor& other,AliceState& state,ReplayStreams& random,FighterActionEvents& events){

 double aimRadians=0;const auto atan=[](float y,float x){return std::atan2(double(y),double(x));};
 const auto channel=[&](u32 shift){return p.pose.channel(shift);};const auto setChannel=[&](u32 shift,u8 value){p.pose.set_channel(shift,value);};
 const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters);};

  float fVar1;
  u16 uVar2;
  i16 sVar3;
  int iVar4;
  double fVar5;
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
      uVar2=0;
      switch(p.animation.cursor.action) {
      case 800:
        advance_alice_doll_hit(p,owner,other,state,random,events);
        break;
      case 0x321:
        advance_alice_doll_hit(p,owner,other,state,random,events);
        break;
      case 0x322:
        advance_alice_doll_hit(p,owner,other,state,random,events);
        break;
      case 0x323:
        advance_alice_doll_hit(p,owner,other,state,random,events);
        break;
      case 0x324:
        advance_alice_doll_hit(p,owner,other,state,random,events);
        break;
      case 0x325:
        advance_alice_doll_hit(p,owner,other,state,random,events);
        break;
      case 0x326:
        advance_alice_doll_hit(p,owner,other,state,random,events);
        break;
      case 0x327:
        if (((i16(owner.animation.cursor.action) < 0x46) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (4 < i16(p.animation.cursor.index))) {
          if (p.response == 6) {
            p.response = 0;
            p.hits = 1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0xffe2;
            p.timer = 0x96;
            p.angle = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 4;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x371,double(p.motion.x),double(p.motion.y) + double(10.0f),u8(p.motion.facing),1);
            events.sound(2);
          }
          if (i16(p.animation.cursor.index) == 0) {
            p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer) * double(0.1f));
            p.timer = i16(p.timer) + -5;
            p.counter = i16(p.counter) + 3;
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(18.0f);
            if (i16(p.timer) < 1) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.frame(1);
              p.pose.rotation[2] = 0.0f;
              return;
            }
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if ((i16(p.animation.cursor.index) < 5) && (i16(p.hits) == 0)) {
            p.frame(i16(p.animation.cursor.index) + 4);
          }
          else {
            if (4 < i16(p.animation.cursor.index)) {
              p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
              if (u8(channel(24)) < 10) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -10));
            }
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(5);
            }
            else {
              if (i16(p.animation.cursor.elapsed) == 0) {
                p.response = 0;
              }
              if (((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) &&
                 (i16(p.angle) < 10)) {
                p.frame(1);
                p.angle = i16(p.angle) + 1;
              }
            }
          }
        }
        else {
          p.destroy();
          for (local_8 = 0; local_8 < 8; local_8 = local_8 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x328:
        if (((i16(owner.animation.cursor.action) < 0x46) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (4 < i16(p.animation.cursor.index))) {
          if (p.response == 6) {
            p.response = 0;
            p.hits = 1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0x46;
            p.timer = 0x96;
            p.angle = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 4;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x371,double(p.motion.x),double(p.motion.y) + double(10.0f),u8(p.motion.facing),1);
            events.sound(2);
          }
          if (i16(p.animation.cursor.index) == 0) {
            p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer) * double(0.1f));
            p.timer = i16(p.timer) + -5;
            p.counter = i16(p.counter) + -2;
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(18.0f);
            if (i16(p.timer) < 1) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.frame(1);
              p.pose.rotation[2] = 0.0f;
              return;
            }
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if ((i16(p.animation.cursor.index) < 5) && (i16(p.hits) == 0)) {
            p.frame(i16(p.animation.cursor.index) + 4);
          }
          else {
            if (4 < i16(p.animation.cursor.index)) {
              p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
              if (u8(channel(24)) < 10) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -10));
            }
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(5);
            }
            else {
              if (i16(p.animation.cursor.elapsed) == 0) {
                p.response = 0;
              }
              if (((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) &&
                 (i16(p.angle) < 10)) {
                p.frame(1);
                p.angle = i16(p.angle) + 1;
              }
            }
          }
        }
        else {
          p.destroy();
          for (local_c = 0; local_c < 8; local_c = local_c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x329:
        if (((i16(owner.animation.cursor.action) < 0x46) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (4 < i16(p.animation.cursor.index))) {
          if (p.response == 6) {
            p.response = 0;
            p.hits = 1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0xffe2;
            p.timer = 200;
            p.angle = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 4;
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x371,double(p.motion.x),double(p.motion.y) + double(10.0f),u8(p.motion.facing),1);
            events.sound(2);
          }
          if (i16(p.animation.cursor.index) == 0) {
            p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer) * double(0.1f));
            p.timer = i16(p.timer) + -5;
            p.counter = i16(p.counter) + 4;
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(18.0f);
            if (i16(p.timer) < 1) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.frame(1);
              p.pose.rotation[2] = 0.0f;
              return;
            }
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if ((i16(p.animation.cursor.index) < 5) && (i16(p.hits) == 0)) {
            p.frame(i16(p.animation.cursor.index) + 4);
          }
          else {
            if (4 < i16(p.animation.cursor.index)) {
              p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
              if (u8(channel(24)) < 10) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -10));
            }
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(5);
            }
            else {
              if (i16(p.animation.cursor.elapsed) == 0) {
                p.response = 0;
              }
              if (((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) &&
                 (i16(p.angle) < 10)) {
                p.frame(1);
                p.angle = i16(p.angle) + 1;
              }
            }
          }
        }
        else {
          p.destroy();
          for (local_10 = 0; local_10 < 8; local_10 = local_10 + 1) {
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
          if (p.response == 6) {
            p.response = 0;
            p.hits = 1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            if (double(p.parameters[0]) == double(0.0f)) {
              p.motion.velocityX = 5.0f;
              p.motion.velocityY = 6.0f;
            }
            if (double(p.parameters[0]) == double(1.0f)) {
              p.motion.velocityX = 3.0f;
              p.motion.velocityY = 8.0f;
            }
            if (double(p.parameters[0]) == double(2.0f)) {
              p.motion.velocityX = 1.0f;
              p.motion.velocityY = 10.0f;
            }
            if (double(p.parameters[0]) == double(3.0f)) {
              p.motion.velocityX = -5.0f;
              p.motion.velocityY = 6.0f;
            }
            if (double(p.parameters[0]) == double(4.0f)) {
              p.motion.velocityX = -3.0f;
              p.motion.velocityY = 8.0f;
            }
            if (double(p.parameters[0]) == double(5.0f)) {
              p.motion.velocityX = -1.0f;
              p.motion.velocityY = 10.0f;
            }
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
          if ((i16(p.animation.cursor.index) < 2) &&
             (p.pose.rotation[2] = double(p.pose.rotation[2]) + double(16.0f),
             fVar1 = double(p.motion.y) + double(p.motion.velocityY),
             fVar1 < double(0.0f) != (fVar1 == double(0.0f)))) {
            owned(0x32b,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            owned(0x32d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            events.sound(4);
            p.action(0x33b);
            p.motion.y = 0.0f;
            p.motion.velocityY = 0.0f;
            p.motion.velocityX = 0.0f;
            p.pose.rotation[2] = 0.0f;
          }
          else {
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          for (local_14 = 0; local_14 < 5; local_14 = local_14 + 1) {
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
          p.counter = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.motion.velocityY = 0.0f;
          p.motion.velocityX = 0.0f;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
          events.shake(5.0f);
        }
        p.pose.scale[0] =
             (float)(5 - i16(p.animation.cursor.elapsed)) * double(0.03f) * (float)(i16(p.animation.cursor.elapsed) < 5) + double(0.01f) + double(p.pose.scale[0]);
        p.pose.scale[1] =
             (float)(5 - i16(p.animation.cursor.elapsed)) * double(0.03f) * (float)(i16(p.animation.cursor.elapsed) < 5) + double(0.01f) + double(p.pose.scale[1]);
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 10) {
            setChannel(24,u8(0));
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -10));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x32c:
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.polar(double(p.parameters[0]),15.0f);
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
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if ((i16(p.animation.cursor.index) < 2) &&
             (p.pose.rotation[2] = double(p.pose.rotation[2]) + double(16.0f),
             fVar1 = double(p.motion.y) + double(p.motion.velocityY),
             fVar1 < double(0.0f) != (fVar1 == double(0.0f)))) {
            events.sound(4);
            owned(0x32b,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            owned(0x32d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            p.action(0x33b);
            p.motion.y = 0.0f;
            p.motion.velocityY = 0.0f;
            p.motion.velocityX = 0.0f;
            p.pose.rotation[2] = 0.0f;
          }
          else {
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          for (local_18 = 0; local_18 < 5; local_18 = local_18 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x32d:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.4000000059604645f;
          p.pose.scale[1] = 0.4000000059604645f;
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
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x32e:
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
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.07f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.07f);
        setChannel(24,u8(i8(channel(24)) + -10));
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      default:
        p.advance();
        break;
      case 0x330:
        if (((i16(owner.animation.cursor.action) < 0x46) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (10 < i16(p.animation.cursor.index))) {
          if (p.response == 6) {
            p.response = 0;
            p.hits = 1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = -4.0f;
            p.motion.velocityY = 6.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 0) {
            if (u8(channel(24)) < 0xec) {
              setChannel(24,u8(i8(channel(24)) + '\x14'));
            }
            else {
              setChannel(24,u8(0xff));
            }
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(18.0f);
            p.motion.velocityY = double(p.motion.velocityY) - double(0.3f);
          }
          if (i16(p.animation.cursor.index) == 0xb) {
            p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.pose.rotation[2] = 0.0f;
            }
            if (((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.angle) < 0)) {
              p.frame(2);
              p.angle = i16(p.angle) + 1;
            }
            if (((i16(p.animation.cursor.index) == 10) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.timer) < 6)) {
              p.timer = i16(p.timer) + 1;
              p.frame(9);
            }
            if ((i16(p.animation.cursor.index) == 9) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = 1200.0f;
              iVar4 = random.range(0,10);
              owner.launchParameters[1] = (float)(iVar4 + -5);
              owned(800,(float)(i8(p.motion.facing) * 10) + double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              events.sound(6);
            }
          }
        }
        else {
          p.destroy();
          for (local_1c = 0; local_1c < 8; local_1c = local_1c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x331:
        if (((i16(owner.animation.cursor.action) < 0x46) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (10 < i16(p.animation.cursor.index))) {
          if (p.response == 6) {
            p.response = 0;
            p.hits = 1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 3.0f;
            p.motion.velocityY = 16.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 0) {
            if (u8(channel(24)) < 0xec) {
              setChannel(24,u8(i8(channel(24)) + '\x14'));
            }
            else {
              setChannel(24,u8(0xff));
            }
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(18.0f);
            p.motion.velocityY = double(p.motion.velocityY) - double(0.8f);
          }
          if (i16(p.animation.cursor.index) == 0xb) {
            p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.pose.rotation[2] = 0.0f;
            }
            if (((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.angle) < 0)) {
              p.frame(2);
              p.angle = i16(p.angle) + 1;
            }
            if (((i16(p.animation.cursor.index) == 10) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.timer) < 6)) {
              p.timer = i16(p.timer) + 1;
              p.frame(9);
            }
            if ((i16(p.animation.cursor.index) == 9) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = 1200.0f;
              iVar4 = random.range(0,0xf);
              owner.launchParameters[1] = (float)(iVar4 + -0x2d);
              owned(0x321,(float)(i8(p.motion.facing) * 10) +
                                 double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              events.sound(6);
            }
          }
        }
        else {
          p.destroy();
          for (local_20 = 0; local_20 < 8; local_20 = local_20 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x332:
        if (((i16(owner.animation.cursor.action) < 0x46) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (10 < i16(p.animation.cursor.index))) {
          if (p.response == 6) {
            p.response = 0;
            p.hits = 1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 6.0f;
            p.motion.velocityY = 4.5f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 0) {
            if (u8(channel(24)) < 0xec) {
              setChannel(24,u8(i8(channel(24)) + '\x14'));
            }
            else {
              setChannel(24,u8(0xff));
            }
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(18.0f);
            p.motion.velocityY = double(p.motion.velocityY) - double(0.3f);
          }
          if (i16(p.animation.cursor.index) == 0xb) {
            p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.pose.rotation[2] = 0.0f;
              p.motion.facing = -i8(p.motion.facing);
            }
            if (((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.angle) < 0)) {
              p.frame(2);
              p.angle = i16(p.angle) + 1;
            }
            if (((i16(p.animation.cursor.index) == 10) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.timer) < 6)) {
              p.timer = i16(p.timer) + 1;
              p.frame(9);
            }
            if ((i16(p.animation.cursor.index) == 9) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = 1200.0f;
              iVar4 = random.range(0,10);
              owner.launchParameters[1] = (float)(iVar4 + -5);
              owned(0x322,(float)(i8(p.motion.facing) * 10) +
                                 double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              events.sound(6);
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
      case 0x335:
        if (p.response == 6) {
          p.response = 0;
          p.hits = 99;
        }
        if ((((i16(owner.animation.cursor.action) < 0x46) ||
             (0x95 < i16(owner.animation.cursor.action))) ||
            (i16(p.hits) < 1)) || (10 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.response = 0;
            p.hits = 99;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.polar(double(p.parameters[0]),14.0f);
          }
          if (i16(p.animation.cursor.index) == 0) {
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f);
            p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if ((i16(p.animation.cursor.index) == 10) &&
             (fVar1 = double(p.motion.y) - double(p.motion.velocityY),
             fVar1 < double(100.0f) != (fVar1 == double(100.0f)))) {
            p.motion.velocityX = 2.0f;
            p.motion.velocityY = 2.0f;
            p.frame(0xb);
          }
          if ((10 < i16(p.animation.cursor.index)) && (2 < i16(p.counter))) {
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(10);
            p.counter = i16(p.counter) + 1;
          }
          else {
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.pose.rotation[2] = 0.0f;
            }
            if (((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) &&
               ((float)(int)i16(p.angle) < double(p.parameters[1]) + double(0.0f))) {
              p.frame(2);
              p.angle = i16(p.angle) + 1;
            }
            if ((i16(p.animation.cursor.index) == 10) && (i16(p.animation.cursor.elapsed) == 0)) {
              events.sound(0x10);
              p.motion.velocityX = 18.0f;
              p.motion.velocityY = 18.0f;
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
      case 0x338:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.motion.velocityY = 0.0f;
          p.motion.velocityX = 0.0f;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
          events.shake(10.0f);
          owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
        }
        if (p.response == 6) {
          p.response = 0;
          p.hits = i16(p.hits) + 1;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.07f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.07f);
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0x14) {
            setChannel(24,u8(0));
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
          owned(0x339,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
        }
        break;
      case 0x339:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.motion.velocityY = 0.0f;
          p.motion.velocityX = 0.0f;
          p.pose.scale[0] = 1.2999999523162842f;
          p.pose.scale[1] = 1.2999999523162842f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.07f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.07f);
        if (1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0x28) {
            setChannel(24,u8(0));
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x28));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
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
          p.pose.projection[2] = 0x4e2;
          p.pose.rotation[0] = -75.0f;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.3f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
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
      case 0x33b:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          launchSpeed = 18.0f;
          iVar4 = random.range(0,0x2d);
          p.polar(-(((float)iVar4 - double(22.5f)) + double(90.0f)),launchSpeed);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.motion.velocityY = double(p.motion.velocityY) - double(0.85f);
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f);
        if (-double(480.0f) <= double(p.motion.y)) {
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x33c:
        if (i16(owner.animation.cursor.action) == 0x2be) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x41) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(110.0f);
        }
        if (((i16(owner.animation.cursor.action) == 0x2be) &&
            ((i16(owner.animation.cursor.index) < 10 ||
             (i16(owner.animation.cursor.action) != 0x2be)))) ||
           (i16(p.counter) != 0)) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (i16(p.counter) == 0) {
            if (u8(channel(24)) < 0xe2) {
              setChannel(24,u8(i8(channel(24)) + '\x1e'));
            }
            else {
              setChannel(24,u8(0xff));
            }
          }
          if (i16(p.counter) == 1) {
            p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(1);
          }
        }
        else {
          p.counter = 1;
        }
        break;
      case 0x33d:
        if (i16(owner.animation.cursor.action) == 0x2c1) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x41) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(110.0f);
        }
        if (((i16(owner.animation.cursor.action) == 0x2c1) &&
            ((i16(owner.animation.cursor.index) < 10 ||
             (i16(owner.animation.cursor.action) != 0x2be)))) ||
           (i16(p.counter) != 0)) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (i16(p.counter) == 0) {
            if (u8(channel(24)) < 0xe2) {
              setChannel(24,u8(i8(channel(24)) + '\x1e'));
            }
            else {
              setChannel(24,u8(0xff));
            }
          }
          if (i16(p.counter) == 1) {
            p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(1);
          }
        }
        else {
          p.counter = 1;
        }
        break;
      case 0x33e:
        if (i16(owner.animation.cursor.action) == 0x2be) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x8c) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(110.0f);
        }
        if ((i16(owner.animation.cursor.action) == 0x2be) ||
           (6 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.hits = 1;
            p.response = 0;
            if (i16(p.counter) != 1) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.pose.scale[0] = 1.5f;
              p.pose.scale[1] = 1.5f;
              p.counter = 0;
              setChannel(24,u8(0xff));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
              p.hits = 10;
            }
          }
          if (i16(p.animation.cursor.index) == 6) {
            iVar4 = random.range(0,4);
            p.pose.scale[1] = (float)(iVar4 + -2) * double(0.05f) + double(1.5f);
          }
          if (i16(p.animation.cursor.index) == 7) {
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.7f);
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if (((i16(p.animation.cursor.index) == 7) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) < 10)) {
            p.frame(6);
            p.response = 0;
            p.counter = i16(p.counter) + 1;
          }
        }
        else {
          p.frame(7);
        }
        break;
      case 0x33f:
        if (i16(owner.animation.cursor.action) == 0x2c1) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0xb4) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(110.0f);
        }
        if ((i16(owner.animation.cursor.action) == 0x2c1) ||
           (0x10 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.response = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 2.0f;
            p.pose.scale[1] = 1.5f;
            p.counter = 0;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.hits = 0x19;
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if (i16(p.animation.cursor.elapsed) == 0) {
            p.response = 0;
          }
          if (((i16(p.animation.cursor.index) == 0xd) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) < 2)) {
            p.frame(6);
            p.counter = i16(p.counter) + 1;
          }
        }
        else {
          p.frame(0x11);
        }
        break;
      case 0x340:
        if (p.response == 6) {
          p.response = 0;
          p.hits = 1;
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          fVar5 = (double)std::abs(float(((float)(i8(owner.motion.facing) * 0x5a)
                                        + double(owner.motion.x)) -
                                        double(other.motion.x)));
          p.motion.velocityX = (float)(fVar5 * (double)double(0.04f));
          if (double(p.motion.velocityX) < double(4.0f)) {
            p.motion.velocityX = 4.0f;
          }
          p.motion.velocityY = 20.0f;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.motion.velocityY = double(p.motion.velocityY) - double(2.0f);
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        if (i16(p.animation.cursor.index) < 2) {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(16.0f);
          fVar1 = double(p.motion.y) + double(p.motion.velocityY);
          if (fVar1 < double(0.0f) != (fVar1 == double(0.0f))) {
            owned(0x338,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            owned(0x33a,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            events.sound(8);
            p.action(0x33b);
            p.motion.y = 0.0f;
            p.motion.velocityY = 0.0f;
            p.motion.velocityX = 0.0f;
            p.pose.rotation[2] = 0.0f;
            return;
          }
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
        }
        break;
      case 0x341:
        if (p.response == 6) {
          p.response = 0;
          p.hits = 1;
        }
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          fVar5 = (double)std::abs(float(((float)(i8(owner.motion.facing) * 0x5a)
                                        + double(owner.motion.x)) -
                                        double(other.motion.x)));
          p.motion.velocityX = (float)(fVar5 * (double)double(0.04f));
          if (double(p.motion.velocityX) < double(4.0f)) {
            p.motion.velocityX = 4.0f;
          }
          p.motion.velocityY = 20.0f;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.motion.velocityY = double(p.motion.velocityY) - double(2.0f);
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        if (i16(p.animation.cursor.index) < 2) {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(16.0f);
          fVar1 = double(p.motion.y) + double(p.motion.velocityY);
          if (fVar1 < double(0.0f) != (fVar1 == double(0.0f))) {
            owned(0x342,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            owned(0x344,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            events.sound(10);
            p.action(0x33b);
            p.motion.y = 0.0f;
            p.motion.velocityY = 0.0f;
            p.motion.velocityX = 0.0f;
            p.pose.rotation[2] = 0.0f;
            return;
          }
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
        }
        break;
      case 0x342:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.motion.velocityY = 0.0f;
          p.motion.velocityX = 0.0f;
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
          events.shake(10.0f);
        }
        if (p.response == 6) {
          p.response = 0;
          p.hits = i16(p.hits) + 1;
        }
        if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
          owned(0x343,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x343:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.hits = 1;
          p.response = 0;
          p.motion.velocityY = 0.0f;
          p.motion.velocityX = 0.0f;
          p.pose.scale[0] = 1.7000000476837158f;
          p.pose.scale[1] = 1.7000000476837158f;
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
        if (-1 < i16(p.animation.cursor.index)) {
          if (u8(channel(24)) < 0x14) {
            setChannel(24,u8(0));
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
      case 0x344:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.projection[2] = 0x4e2;
          p.pose.rotation[0] = -75.0f;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.3f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
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
      case 0x345:
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
        }
        if (i16(p.animation.cursor.index) < 6) {
          if ((((i16(p.animation.cursor.index) == 1) &&
               (p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f),
               i16(state.dollPhase) == 1)) &&
              (i16(p.animation.cursor.index) == 1)) &&
             (i16(state.dollPhase) == 1)) {
            p.frame(2);
            p.hits = 99;
            p.pose.rotation[2] = 0.0f;
            p.response = 0;
          }
          else {
            if (i16(state.dollPhase) == 2) {
              if (i16(p.animation.cursor.index) < 6) {
                p.animation.cursor.index = i16(p.animation.cursor.index) + 4;
              }
              p.timer = 0x19;
              p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer));
            }
            if ((i16(p.animation.cursor.index) < 6) &&
               (i16(state.dollPhase) < 2)) {
              uVar2 = i16(i32(double(state.dollRadius-180)+double(p.parameters[0])));
              p.counter = uVar2;
              if (0xb4 < i16(p.counter)) {
                p.counter = i16(p.counter) + -0x168;
              }
              if (i16(p.counter) < -0xb4) {
                p.counter = i16(p.counter) + 0x168;
              }
              p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer));
              iVar4 = i32(double(state.dollCenterX)+double(p.motion.facing)*double(p.motion.velocityX));
              p.motion.x = (float)iVar4;
              iVar4 = i32(double(state.dollCenterY)+double(p.motion.velocityY));
              p.motion.y = (float)iVar4;
              if (i16(p.timer) < 0x8c) {
                p.timer = i16(p.timer) + 7;
              }
              if (0x8c < i16(p.timer)) {
                p.timer = 0x8c;
              }
            }
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(6);
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
            if (((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(state.dollPhase) == 1)) {
              p.frame(2);
              p.response = 0;
            }
          }
        }
        else {
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (((double(1600.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(400.0f))) ||
             ((double(p.motion.y) < -double(400.0f) || (double(1280.0f) < double(p.motion.y))))) {
            p.destroy();
          }
          else {
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(6);
            }
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
          state.dollRadius = 0;
          p.motion.velocityX = 25.0f;
        }
        if (i16(p.animation.cursor.index) < 2) {
          state.dollRadius =
               i16(state.dollRadius) + -3;
          uVar2 = i16(i32(p.motion.x));
          state.dollCenterX = uVar2;
          uVar2 = i16(i32(p.motion.y));
          state.dollCenterY = uVar2;
          p.motion.velocityX = double(p.motion.velocityX) - double(1.0f);
          if (double(p.motion.velocityX) < double(0.0f)) {
            p.motion.velocityX = 0.0f;
          }
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
          state.dollPhase = 0;
        }
        if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
          state.dollPhase = 1;
        }
        if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
          state.dollPhase = 2;
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
          p.counter = 0;
          p.timer = 0;
        }
        if (i16(p.animation.cursor.index) < 6) {
          if ((((i16(p.animation.cursor.index) == 1) &&
               (p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f),
               i16(state.dollPhase) == 1)) &&
              (i16(p.animation.cursor.index) == 1)) &&
             (i16(state.dollPhase) == 1)) {
            p.frame(2);
            p.hits = 99;
            p.pose.rotation[2] = 0.0f;
            p.response = 0;
          }
          else {
            if (i16(state.dollPhase) == 2) {
              if (i16(p.animation.cursor.index) < 6) {
                p.animation.cursor.index = i16(p.animation.cursor.index) + 4;
              }
              p.timer = 0x19;
              p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer));
            }
            if ((i16(p.animation.cursor.index) < 6) &&
               (i16(state.dollPhase) < 2)) {
              uVar2 = i16(i32(double(state.dollRadius-180)+double(p.parameters[0])));
              p.counter = uVar2;
              if (0xb4 < i16(p.counter)) {
                p.counter = i16(p.counter) + -0x168;
              }
              if (i16(p.counter) < -0xb4) {
                p.counter = i16(p.counter) + 0x168;
              }
              p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer));
              iVar4 = i32(double(state.dollCenterX)+double(p.motion.facing)*double(p.motion.velocityX));
              p.motion.x = (float)iVar4;
              iVar4 = i32(double(state.dollCenterY)+double(p.motion.velocityY));
              p.motion.y = (float)iVar4;
              if (i16(p.timer) < 0x78) {
                p.timer = i16(p.timer) + 6;
              }
              if (0x78 < i16(p.timer)) {
                p.timer = 0x78;
              }
            }
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(6);
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
            if (((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(state.dollPhase) == 1)) {
              p.frame(2);
              p.response = 0;
            }
          }
        }
        else {
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (((double(1600.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(400.0f))) ||
             ((double(p.motion.y) < -double(400.0f) || (double(1280.0f) < double(p.motion.y))))) {
            p.destroy();
          }
          else {
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(6);
            }
          }
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
          p.counter = 0;
          p.timer = 0;
        }
        if (i16(p.animation.cursor.index) < 6) {
          if ((((i16(p.animation.cursor.index) == 1) &&
               (p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f),
               i16(state.dollPhase) == 1)) &&
              (i16(p.animation.cursor.index) == 1)) &&
             (i16(state.dollPhase) == 1)) {
            p.frame(2);
            p.hits = 99;
            p.pose.rotation[2] = 0.0f;
            p.response = 0;
          }
          else {
            if (i16(state.dollPhase) == 2) {
              if (i16(p.animation.cursor.index) < 6) {
                p.animation.cursor.index = i16(p.animation.cursor.index) + 4;
              }
              p.timer = 0x19;
              p.polar((float)(0xb4 - i16(p.counter)),(float)(int)i16(p.timer));
            }
            if ((i16(p.animation.cursor.index) < 6) &&
               (i16(state.dollPhase) < 2)) {
              uVar2 = i16(i32(double(state.dollRadius-180)+double(p.parameters[0])));
              p.counter = uVar2;
              if (0xb4 < i16(p.counter)) {
                p.counter = i16(p.counter) + -0x168;
              }
              if (i16(p.counter) < -0xb4) {
                p.counter = i16(p.counter) + 0x168;
              }
              p.polar((float)-(int)i16(p.counter),(float)(int)i16(p.timer));
              iVar4 = i32(double(state.dollCenterX)+double(p.motion.facing)*double(p.motion.velocityX));
              p.motion.x = (float)iVar4;
              iVar4 = i32(double(state.dollCenterY)+double(p.motion.velocityY));
              p.motion.y = (float)iVar4;
              if (i16(p.timer) < 200) {
                p.timer = i16(p.timer) + 10;
              }
              if (200 < i16(p.timer)) {
                p.timer = 200;
              }
            }
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(6);
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
            if (((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(state.dollPhase) == 1)) {
              p.frame(2);
              p.response = 0;
            }
          }
        }
        else {
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (((double(1600.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(400.0f))) ||
             ((double(p.motion.y) < -double(400.0f) || (double(1280.0f) < double(p.motion.y))))) {
            p.destroy();
          }
          else {
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(6);
            }
          }
        }
        break;
      case 0x349:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          state.dollRadius = 0;
          p.motion.velocityX = 25.0f;
        }
        if (i16(p.animation.cursor.index) < 2) {
          state.dollRadius =
               i16(state.dollRadius) + -3;
          uVar2 = i16(i32(p.motion.x));
          state.dollCenterX = uVar2;
          uVar2 = i16(i32(p.motion.y));
          state.dollCenterY = uVar2;
          p.motion.velocityX = double(p.motion.velocityX) - double(1.0f);
          if (double(p.motion.velocityX) < double(0.0f)) {
            p.motion.velocityX = 0.0f;
          }
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
          state.dollPhase = 0;
        }
        if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
          state.dollPhase = 1;
        }
        if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
          state.dollPhase = 2;
        }
        break;
      case 0x34a:
        if (((i16(owner.animation.cursor.action) < 0x46) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (10 < i16(p.animation.cursor.index))) {
          if (p.response == 6) {
            p.response = 0;
            p.hits = 1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 3.0f;
            p.motion.velocityY = -3.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 0) {
            if (u8(channel(24)) < 0xec) {
              setChannel(24,u8(i8(channel(24)) + '\x14'));
            }
            else {
              setChannel(24,u8(0xff));
            }
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(18.0f);
            p.motion.velocityY = double(p.motion.velocityY) - double(0.3f);
            fVar1 = double(p.motion.y) + double(p.motion.velocityY);
            if (fVar1 < double(20.0f) != (fVar1 == double(20.0f))) {
              p.motion.y = 20.0f;
              p.motion.velocityY = 0.0f;
              p.motion.velocityX = 0.0f;
              p.frame(1);
              p.pose.rotation[2] = 0.0f;
              setChannel(24,u8(0xff));
              return;
            }
          }
          if (i16(p.animation.cursor.index) == 0xb) {
            p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.pose.rotation[2] = 0.0f;
            }
            if (((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.angle) < 1)) {
              p.frame(2);
              p.angle = i16(p.angle) + 1;
            }
            if (((i16(p.animation.cursor.index) == 10) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.timer) < 6)) {
              p.timer = i16(p.timer) + 1;
              p.frame(9);
            }
            if ((i16(p.animation.cursor.index) == 9) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = 1200.0f;
              iVar4 = random.range(0,0x3c);
              owner.launchParameters[1] = (float)iVar4;
              owned(0x323,(float)(i8(p.motion.facing) * 10) +
                                 double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              events.sound(6);
            }
          }
        }
        else {
          p.destroy();
          for (local_2c = 0; local_2c < 8; local_2c = local_2c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x34b:
        if (((i16(owner.animation.cursor.action) < 0x46) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (10 < i16(p.animation.cursor.index))) {
          if (p.response == 6) {
            p.response = 0;
            p.hits = 1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.polar(double(p.parameters[1]),double(p.parameters[0]));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 0) {
            if (u8(channel(24)) < 0xec) {
              setChannel(24,u8(i8(channel(24)) + '\x14'));
            }
            else {
              setChannel(24,u8(0xff));
            }
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(18.0f);
            p.motion.velocityY = double(p.motion.velocityY) - double(0.5f);
          }
          if (i16(p.animation.cursor.index) == 0xb) {
            p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.pose.rotation[2] = 0.0f;
            }
            if (((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.angle) < 0)) {
              p.frame(2);
              p.angle = i16(p.angle) + 1;
            }
            if (((i16(p.animation.cursor.index) == 10) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.timer) < 6)) {
              p.timer = i16(p.timer) + 1;
              p.frame(9);
            }
            if ((i16(p.animation.cursor.index) == 9) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = 1200.0f;
              iVar4 = random.range(0,0xf);
              owner.launchParameters[1] = (float)(iVar4 + -0x3c);
              owned(0x324,(float)(i8(p.motion.facing) * 10) +
                                 double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              events.sound(6);
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
      case 0x34c:
        if (((i16(owner.animation.cursor.action) < 0x46) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (4 < i16(p.animation.cursor.index))) {
          if (((((i16(owner.animation.cursor.action) == 0x198) &&
                (i16(owner.animation.cursor.index) < 7)) ||
               ((i16(owner.animation.cursor.action) == 0x19c &&
                (i16(owner.animation.cursor.index) < 5)))) ||
              ((i16(owner.animation.cursor.action) == 0x19a &&
               (i16(owner.animation.cursor.index) < 7)))) ||
             ((i16(owner.animation.cursor.action) == 0x19d &&
              (i16(owner.animation.cursor.index) < 5)))) {
            p.motion.x =
                 (float)(i8(p.motion.facing) * 0x3c) +
                 double(owner.motion.x);
            p.motion.y = double(owner.motion.y) + double(120.0f);
            if (i16(owner.animation.cursor.action) == 0x19a) {
              p.motion.y = double(owner.motion.y) + double(60.0f);
            }
          }
          else if (i16(p.animation.cursor.index) < 5) {
            p.frame(5);
            return;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.timer = 0;
          }
          if (i16(p.animation.cursor.index) == 5) {
            p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          else {
            if (((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.timer) < 8)) {
              p.timer = i16(p.timer) + 1;
              p.frame(3);
            }
            if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
              if (i16(owner.animation.cursor.action) == 0x198) {
                owner.launchParameters[0] = 1000.0f;
                iVar4 = random.range(0,10);
                owner.launchParameters[1] = (float)(iVar4 + -5);
                owned(0x322,(float)(i8(p.motion.facing) * 10) +
                                   double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
                events.sound(6);
              }
              if (i16(owner.animation.cursor.action) == 0x19a) {
                owner.launchParameters[0] = 1000.0f;
                iVar4 = random.range(0,10);
                owner.launchParameters[1] = (float)(iVar4 + 0x1e);
                owned(0x323,(float)(i8(p.motion.facing) * 10) +
                                   double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
                events.sound(6);
              }
              if (i16(owner.animation.cursor.action) == 0x19c) {
                owner.launchParameters[0] = 1000.0f;
                iVar4 = random.range(0,10);
                owner.launchParameters[1] = (float)(iVar4 + -5);
                owned(0x324,(float)(i8(p.motion.facing) * 10) +
                                   double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
                events.sound(6);
              }
              if (i16(owner.animation.cursor.action) == 0x19d) {
                owner.launchParameters[0] = 1000.0f;
                iVar4 = random.range(0,10);
                owner.launchParameters[1] = (float)(-0x1e - iVar4);
                owned(0x325,(float)(i8(p.motion.facing) * 10) +
                                   double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
                events.sound(6);
              }
            }
          }
        }
        else {
          p.destroy();
          for (local_34 = 0; local_34 < 8; local_34 = local_34 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x34d:
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if (p.response == 6) {
            p.response = 0;
            p.hits = 1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            p.motion.velocityX = double(p.parameters[0]);
            p.motion.velocityY = double(p.parameters[1]);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          p.motion.velocityY = double(p.motion.velocityY) - double(0.8f);
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if ((i16(p.animation.cursor.index) < 2) &&
             (p.pose.rotation[2] = double(p.pose.rotation[2]) + double(16.0f),
             fVar1 = double(p.motion.y) + double(p.motion.velocityY),
             fVar1 < double(0.0f) != (fVar1 == double(0.0f)))) {
            events.sound(4);
            owned(0x32b,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            owned(0x32d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            p.action(0x33b);
            p.motion.y = 0.0f;
            p.motion.velocityY = 0.0f;
            p.motion.velocityX = 0.0f;
            p.pose.rotation[2] = 0.0f;
          }
          else {
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(1);
            }
          }
        }
        else {
          p.destroy();
          for (local_38 = 0; local_38 < 5; local_38 = local_38 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x34e:
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if (p.response == 6) {
            p.response = 0;
            p.hits = 1;
          }
          if (2 < i16(p.counter)) {
            p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.motion.velocityX = 20.0f;
            p.motion.velocityY = 0.0f;
            p.counter = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          p.motion.velocityX = double(p.motion.velocityX) - double(1.0f);
          if (double(p.motion.velocityX) < double(2.0f)) {
            p.motion.velocityX = 2.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(1);
            p.counter = i16(p.counter) + 1;
          }
        }
        else {
          p.destroy();
          for (local_3c = 0; local_3c < 4; local_3c = local_3c + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x34f:
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if (p.response == 6) {
            p.response = 0;
            p.hits = 1;
          }
          if (1 < i16(p.counter)) {
            p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.motion.velocityX = 20.0f;
            p.motion.velocityY = 0.0f;
            p.counter = 0;
            p.response = 0;
            p.hits = 1;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          p.motion.velocityX = double(p.motion.velocityX) - double(1.0f);
          if (double(p.motion.velocityX) < double(2.0f)) {
            p.motion.velocityX = 2.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        else {
          p.destroy();
          for (local_40 = 0; local_40 < 4; local_40 = local_40 + 1) {
            owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          }
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
        }
        break;
      case 0x370:
        if (i16(owner.animation.cursor.action) == 0x133) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x104) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(96.0f);
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
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.3f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.8f);
        if (u8(channel(24)) < 0x15) {
          p.destroy();
        }
        else {
          setChannel(24,u8(i8(channel(24)) + -0x14));
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x371:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.8500000238418579f;
          p.pose.scale[1] = 0.8500000238418579f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.projection[2] = 0x4e2;
          p.pose.rotation[0] = -65.0f;
        }
        p.pose.rotation[2] = double(p.pose.rotation[2]) - double(22.5f);
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
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = double(p.parameters[0]) * double(0.1f);
            p.motion.velocityY = double(p.parameters[1]) * double(0.1f);
          }
          p.motion.velocityY = double(p.motion.velocityY) - double(0.6f);
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) < 2) {
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(16.0f);
            fVar1 = double(p.motion.y) + double(p.motion.velocityY);
            if (fVar1 < double(0.0f) != (fVar1 == double(0.0f))) {
              owned(0x388,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0);
              events.sound(8);
              p.action(0x385);
              if ('\x01' < state.difficulty) {
                p.action(0x38e);
              }
              p.motion.y = 0.0f;
              p.motion.velocityY = 0.0f;
              p.motion.velocityX = 0.0f;
              p.pose.rotation[2] = 0.0f;
              return;
            }
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.frame(1);
          }
        }
        break;
      case 0x385:
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
            p.pose.scale[0] = 0.75f;
            p.pose.scale[1] = 0.75f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[0] = 0.0f;
            p.pose.rotation[1] = 0.0f;
            p.pose.rotation[2] = 0.0f;
            events.shake(5.0f);
            owned(0x387,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.04f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.04f);
          if (1 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0x28) {
              setChannel(24,u8(0));
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x28));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x386,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
        }
        break;
      case 0x386:
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
            p.pose.scale[0] = 0.75f;
            p.pose.scale[1] = 0.75f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[0] = 0.0f;
            p.pose.rotation[1] = 0.0f;
            p.pose.rotation[2] = 0.0f;
          }
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.04f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.04f);
          if (u8(channel(24)) < 0x14) {
            setChannel(24,u8(0));
            p.destroy();
          }
          else {
            setChannel(24,u8(i8(channel(24)) + -0x14));
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
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
            p.pose.scale[0] = 0.75f;
            p.pose.scale[1] = 0.75f;
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
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            launchSpeed = 18.0f;
            iVar4 = random.range(0,0x2d);
            p.polar(-(((float)iVar4 - double(22.5f)) + double(90.0f)),launchSpeed);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          p.motion.velocityY = double(p.motion.velocityY) - double(0.85f);
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f);
          if (-double(480.0f) <= double(p.motion.y)) {
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
          else {
            p.destroy();
          }
        }
        break;
      case 0x389:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if (p.response == 6) {
            p.response = 0;
            p.hits = 1;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0xffe2;
            p.timer = 0x96;
            p.angle = 0;
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
          if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x371,double(p.motion.x),double(p.motion.y) + double(10.0f),u8(p.motion.facing),1);
            events.sound(2);
          }
          if (i16(p.animation.cursor.index) == 0) {
            if (u8(channel(24)) < 0xe1) {
              setChannel(24,u8(i8(channel(24)) + '\x1e'));
            }
            else {
              setChannel(24,u8(0xff));
            }
          }
          if ((i16(p.animation.cursor.index) < 8) && (i16(p.hits) == 0)) {
            p.frame(i16(p.animation.cursor.index) + 4);
          }
          else {
            if (7 < i16(p.animation.cursor.index)) {
              p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
              if (u8(channel(24)) < 10) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -10));
            }
            sVar3 = p.advance();
            if (sVar3 == i16(p.animation.cursor.frameCount)) {
              p.frame(8);
            }
            else {
              if (i16(p.animation.cursor.elapsed) == 0) {
                p.response = 0;
              }
              if (((i16(p.animation.cursor.index) == 8) && (i16(p.animation.cursor.elapsed) == 0)) &&
                 (i16(p.angle) < 5)) {
                p.frame(4);
                p.angle = i16(p.angle) + 1;
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
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 10;
            p.timer = 0;
            p.cycle = 0;
            p.angle = 0;
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x389,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          if (0 < i16(p.animation.cursor.index)) {
            aimRadians = atan((double(other.motion.y) -
                         double(p.motion.y)) + double(100.0f),(double(other.motion.x) -
                         double(p.motion.x)) * (float)(int)i8(p.motion.facing));
            uVar2 = i16(i32(aimRadians/double(0.017453289f)));
            p.timer = uVar2;
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
          p.polar((float)(int)i16(p.angle),4.0f);
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            if ((int)i16(p.counter) < state.difficulty * 5 + 10) {
              p.counter = i16(p.counter) + 1;
              p.frame(1);
            }
            else {
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
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.response = 0;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = double(p.parameters[0]) * double(0.1f);
            p.motion.velocityY = double(p.parameters[1]) * double(0.1f);
            uVar2 = random.range(0xfffffe20,0x3c0);
            p.counter = uVar2;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (i16(p.animation.cursor.index) == 0) {
            if (u8(channel(24)) < 0xec) {
              setChannel(24,u8(i8(channel(24)) + '\x14'));
            }
            else {
              setChannel(24,u8(0xff));
            }
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(18.0f);
            p.motion.velocityY = double(p.motion.velocityY) - double(0.6f);
          }
          if (i16(p.animation.cursor.index) == 0xc) {
            p.pose.scale[0] = double(p.pose.scale[0]) * 0.85;
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.3f);
            if (u8(channel(24)) < 10) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.pose.rotation[2] = 0.0f;
            }
            if (((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.angle) < 0)) {
              p.frame(2);
              p.angle = i16(p.angle) + 1;
            }
            if ((i16(p.animation.cursor.index) == 8) && (i16(p.animation.cursor.elapsed) == 0)) {
              owner.launchParameters[0] = (float)(int)i16(p.counter);
              owner.launchParameters[1] = double(p.motion.x);
              owner.launchParameters[2] = double(p.motion.y);
              owned(0x38c,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              events.sound(0xf);
            }
            if (((i16(p.animation.cursor.index) == 10) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.timer) < 6)) {
              p.timer = i16(p.timer) + 1;
              p.frame(9);
            }
            if ((i16(p.animation.cursor.index) == 0xb) && (i16(p.animation.cursor.elapsed) == 0)) {
              owned(0x38d,(float)((int)i16(p.counter) *
                                        (int)i8(p.motion.facing)) + double(p.motion.x),0.0f,u8(p.motion.facing),1);
              events.sound(8);
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
            fVar5 = (double)std::sqrt(double(float(double(p.parameters[2]) * double(p.parameters[2]) +
                                          double(p.parameters[0]) * double(p.parameters[0]))));
            p.pose.scale[0] = (float)((fVar5 / (double)double(256.0f)) * (double)double(1.1f));
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            fVar5 = (double)atan(double(p.parameters[2]),double(p.parameters[0]));
            p.pose.rotation[2] = (float)(fVar5 / (double)double(0.017453289f));
          }
          p.pose.scale[1] = double(p.pose.scale[1]) * 0.85;
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x38d:
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
          events.shake(10.0f);
        }
        if (p.response == 6) {
          p.response = 0;
          p.hits = i16(p.hits) + 1;
        }
        if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
          owned(0x343,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
        }
        if (i16(p.animation.cursor.index) < 3) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.07f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.07f);
        }
        if (i16(p.animation.cursor.index) == 4) {
          p.pose.scale[0] = double(p.pose.scale[0]) * double(0.8f);
          if (u8(channel(24)) < 0x14) {
            setChannel(24,u8(0));
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -0x14));
          setChannel(16,u8(i8(channel(16)) + -0x14));
          setChannel(8,u8(i8(channel(8)) + -0x14));
        }
        sVar3 = p.advance();
        if (sVar3 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if (((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) < 5)) {
          p.counter = i16(p.counter) + 1;
          p.frame(3);
        }
        break;
      case 0x38e:
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
            p.pose.scale[0] = 0.75f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[0] = 0.0f;
            p.pose.rotation[1] = 0.0f;
            p.pose.rotation[2] = 0.0f;
            events.shake(5.0f);
            owned(0x387,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          if (p.response == 6) {
            p.response = 0;
            p.hits = i16(p.hits) + 1;
          }
          sVar3 = p.advance();
          if (sVar3 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
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
          sVar3 = random.range(0,0x168);
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
          aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                       double(p.motion.y),double(other.motion.x) -
                       double(p.motion.x));
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
          sVar3 = random.range(0,0x168);
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
          aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                       double(p.motion.y),double(other.motion.x) -
                       double(p.motion.x));
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
      }
    }
    else {
      p.freeze = i16(p.freeze) + -1;
    }
  }
  return;

}
}
