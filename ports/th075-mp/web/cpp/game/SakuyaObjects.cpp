#include "SakuyaObjects.hpp"
#include <cmath>
namespace th075 {void advance_sakuya_object(SakuyaObject& p,FighterActor& owner,FighterActor& other,SakuyaState& state,ReplayStreams& random,FighterActionEvents& events){
double aimRadians=0;const auto atan=[](float y,float x){return std::atan2(double(y),double(x));};
const auto channel=[&](u32 shift){return p.pose.channel(shift);};const auto setChannel=[&](u32 shift,u8 value){p.pose.set_channel(shift,value);};
const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters);};
const auto ownedVisual=[&](i16 id,float x,float y,i8 facing,u8 layer,u8 visual){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters,0,visual?&owner.effectVisual:nullptr);};

  float position;
  u16 wordValue;
  i16 advancedFrame;
  int roll;
  int particles;
  double angleRadians;
  float direction;
  
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
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.counter = i16(p.counter) + 1;
          if (i16(p.animation.cursor.index) < 4) {
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.5f) + double(p.parameters[1]));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            owner.effectVisual.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            owner.effectVisual.mode = 0;
            owner.effectVisual.projection[0] = 0;
            owner.effectVisual.projection[1] = 0;
            owner.effectVisual.projection[2] = 0;
            owner.effectVisual.scale[0] = 1.0f;
            owner.effectVisual.scale[1] = 1.0f;
            owner.effectVisual.alpha = 0xff;
            owner.effectVisual.red = 0xff;
            owner.effectVisual.green = 0xff;
            owner.effectVisual.blue = 0xff;
            owner.effectVisual.rotation[0] = 0.0f;
            owner.effectVisual.rotation[1] = 0.0f;
            ownedVisual(0x32d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0,1);
          }
          if (((p.response == 2) || (p.response == 4)) &&
             (i16(p.animation.cursor.index) == 0)) {
            p.frame(6);
          }
          else if ((((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f)))
                   || (double(1000.0f) < double(p.motion.y))) ||
                  (double(p.motion.y) < -double(160.0f))) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = -double(p.motion.velocityX) * double(0.25f);
              p.motion.velocityY = -14.0f;
            }
            if (i16(p.animation.cursor.index) == 6) {
              p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
              p.pose.rotation[2] = double(p.pose.rotation[2]) + double(40.0f);
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 4) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(0);
            }
            if ((i16(p.animation.cursor.index) == 7) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(6);
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
      case 0x321:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          particles = random.range(0,7);
          p.pose.scale[0] = (float)particles * double(0.1f) + double(1.0f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          particles = random.range(0,0x32);
          roll = random.range(0,100);
          p.pose.rotation[0] = (float)(int)(particles + 0x1e + (u32)(roll < 0x32) * 0xb4);
          particles = random.range(0,0x3c);
          p.pose.rotation[2] = (float)particles;
          particles = random.range(0,0x5a);
          p.pose.rotation[1] = (float)((particles + -0x19) * (int)i8(p.motion.facing));
          wordValue = random.range(0,2);
          p.timer = wordValue;
        }
        p.motion.x =
             (float)(int)i16(p.timer) * double(6.0f) * (float)(int)i8(p.motion.facing) +
             double(p.motion.x);
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.02f);
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x322:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.response = 0;
          particles = random.range(0,9);
          p.pose.scale[0] = (float)particles * double(0.1f) + double(1.7f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          particles = random.range(0,0x32);
          roll = random.range(0,100);
          p.pose.rotation[0] = (float)(int)(particles + 0x1e + (u32)(roll < 0x32) * 0xb4);
          particles = random.range(0,0x3c);
          p.pose.rotation[2] = (float)particles;
          particles = random.range(0,0x5a);
          p.pose.rotation[1] = (float)((particles + -0x19) * (int)i8(p.motion.facing));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.02f);
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.5f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.02f);
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x323:
        if (p.response == 5) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) == 0)) {
            p.hits = 1;
            p.response = 0;
            aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                         double(p.motion.y),(double(other.motion.x) -
                         double(p.motion.x)) * (float)(int)i8(p.motion.facing));
            wordValue = i32(aimRadians/double(0.017453289f));
            p.angle = wordValue;
            p.counter = i16(p.counter) + 1;
          }
          if (i16(p.animation.cursor.index) < 2) {
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f);
            p.polar(double(p.parameters[0]),4.5f);
          }
          if (1 < i16(p.animation.cursor.index)) {
            p.timer = i16(p.timer) + 1;
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.5f);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (((double(1600.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(400.0f))) ||
             ((double(1400.0f) < double(p.motion.y) || (double(p.motion.y) < -double(400.0f))))) {
            p.destroy();
          }
          else {
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.frame(2);
            }
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              setChannel(24,u8(0xff));
            }
          }
        }
        break;
      case 0x324:
        if ((i16(owner.animation.cursor.action) == 0x133) ||
           (i16(owner.animation.cursor.action) == 0x186)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x3c) +
               double(owner.motion.x);
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
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x325:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x46) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (0 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.response = 0;
            p.hits = 1;
            p.timer = 0;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.counter = i16(p.counter) + 1;
          if (10 < i16(p.counter)) {
            p.counter = 10;
          }
          if (i16(p.animation.cursor.index) == 0) {
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            owner.launchParameters[0] = double(p.pose.rotation[2]);
            owner.launchParameters[1] = (float)(int)i16(p.counter);
            owned(0x32c,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            owner.effectVisual.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            owner.effectVisual.mode = 0;
            owner.effectVisual.projection[0] = 0;
            owner.effectVisual.projection[1] = 0;
            owner.effectVisual.projection[2] = 0;
            owner.effectVisual.scale[0] = 1.0f;
            owner.effectVisual.scale[1] = 1.0f;
            owner.effectVisual.alpha = 0xff;
            owner.effectVisual.red = 0xff;
            owner.effectVisual.green = 0xff;
            owner.effectVisual.blue = 0xff;
            owner.effectVisual.rotation[0] = 0.0f;
            owner.effectVisual.rotation[1] = 0.0f;
            ownedVisual(0x32d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0,1);
            if (((double(owner.combat.screenBounds[2]) < double(p.motion.y)) &&
                (i16(p.timer) < 2)) && (double(p.motion.velocityY) < double(0.0f))) {
              events.sound(0xb);
              p.parameters[0] = -double(p.parameters[0]);
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
            if (((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
                (i16(p.timer) < 2)) &&
               (((i8(p.motion.facing) == -1 && (double(0.0f) < double(p.motion.velocityX))) ||
                ((i8(p.motion.facing) == '\x01' && (double(p.motion.velocityX) < double(0.0f))))))) {
              events.sound(0xb);
              if (double(0.0f) < double(p.parameters[0])) {
                p.parameters[0] = double(180.0f) - double(p.parameters[0]);
              }
              if (double(p.parameters[0]) < double(0.0f)) {
                p.parameters[0] = -double(180.0f) - double(p.parameters[0]);
              }
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
            if (((double(owner.combat.screenBounds[1]) < double(p.motion.x)) &&
                (i16(p.timer) < 2)) &&
               (((i8(p.motion.facing) == -1 && (double(p.motion.velocityX) < double(0.0f))) ||
                ((i8(p.motion.facing) == '\x01' && (double(0.0f) < double(p.motion.velocityX))))))) {
              events.sound(0xb);
              if (double(0.0f) < double(p.parameters[0])) {
                p.parameters[0] = double(180.0f) - double(p.parameters[0]);
              }
              if (double(p.parameters[0]) < double(0.0f)) {
                p.parameters[0] = -double(180.0f) - double(p.parameters[0]);
              }
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
          }
          if (((p.response == 2) || (p.response == 4)) &&
             (i16(p.animation.cursor.index) == 0)) {
            p.frame(1);
          }
          else if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f)))
                  || ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f)))
                     )) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = -double(p.motion.velocityX) * double(0.25f);
              p.motion.velocityY = -14.0f;
            }
            if (i16(p.animation.cursor.index) == 1) {
              p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
              p.pose.rotation[2] = double(p.pose.rotation[2]) + double(40.0f);
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x326:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (0 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.response = 0;
            p.hits = 1;
            p.timer = 0;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.counter = i16(p.counter) + 1;
          if (10 < i16(p.counter)) {
            p.counter = 10;
          }
          if (i16(p.animation.cursor.index) == 0) {
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            owner.launchParameters[0] = double(p.pose.rotation[2]);
            owner.launchParameters[1] = (float)(int)i16(p.counter);
            owned(0x32c,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            owner.effectVisual.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            owner.effectVisual.mode = 0;
            owner.effectVisual.projection[0] = 0;
            owner.effectVisual.projection[1] = 0;
            owner.effectVisual.projection[2] = 0;
            owner.effectVisual.scale[0] = 1.0f;
            owner.effectVisual.scale[1] = 1.0f;
            owner.effectVisual.alpha = 0xff;
            owner.effectVisual.red = 0xff;
            owner.effectVisual.green = 0xff;
            owner.effectVisual.blue = 0xff;
            owner.effectVisual.rotation[0] = 0.0f;
            owner.effectVisual.rotation[1] = 0.0f;
            ownedVisual(0x32d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0,1);
            if (((double(owner.combat.screenBounds[2]) < double(p.motion.y)) &&
                (i16(p.timer) < 1)) && (double(p.motion.velocityY) < double(0.0f))) {
              events.sound(0xb);
              p.parameters[0] = -double(p.parameters[0]);
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
            if (((double(p.motion.y) < double(0.0f)) && (i16(p.timer) < 1)) &&
               (double(0.0f) < double(p.motion.velocityY))) {
              events.sound(0xb);
              p.parameters[0] = -double(p.parameters[0]);
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
            if (((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
                (i16(p.timer) < 1)) &&
               (((i8(p.motion.facing) == -1 && (double(0.0f) < double(p.motion.velocityX))) ||
                ((i8(p.motion.facing) == '\x01' && (double(p.motion.velocityX) < double(0.0f))))))) {
              events.sound(0xb);
              if (double(0.0f) <= double(p.parameters[0])) {
                p.parameters[0] = double(180.0f) - double(p.parameters[0]);
              }
              if (double(p.parameters[0]) < double(0.0f)) {
                p.parameters[0] = -double(180.0f) - double(p.parameters[0]);
              }
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
            if (((double(owner.combat.screenBounds[1]) < double(p.motion.x)) &&
                (i16(p.timer) < 1)) &&
               (((i8(p.motion.facing) == -1 && (double(p.motion.velocityX) < double(0.0f))) ||
                ((i8(p.motion.facing) == '\x01' && (double(0.0f) < double(p.motion.velocityX))))))) {
              events.sound(0xb);
              if (double(0.0f) <= double(p.parameters[0])) {
                p.parameters[0] = double(180.0f) - double(p.parameters[0]);
              }
              if (double(p.parameters[0]) < double(0.0f)) {
                p.parameters[0] = -double(180.0f) - double(p.parameters[0]);
              }
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
          }
          if (((p.response == 2) || (p.response == 4)) &&
             (i16(p.animation.cursor.index) == 0)) {
            p.frame(1);
          }
          else if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f)))
                  || ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f)))
                     )) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = -double(p.motion.velocityX) * double(0.25f);
              p.motion.velocityY = -14.0f;
            }
            if (i16(p.animation.cursor.index) == 1) {
              p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
              p.pose.rotation[2] = double(p.pose.rotation[2]) + double(40.0f);
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x327:
        if (p.response == 5) {
          p.destroy();
        }
        else if ((((i16(owner.animation.cursor.action) < 0x46) ||
                  (1 < i16(p.animation.cursor.index))) &&
                 ((i16(owner.animation.cursor.action) < 0x3c ||
                  (i16(p.animation.cursor.index) != 2)))) ||
                (0x95 < i16(owner.animation.cursor.action))) {
          if ((((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
              (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(160.0f))) {
            p.destroy();
          }
          else if (((i16(owner.animation.cursor.action) < 400) ||
                   (0x195 < i16(owner.animation.cursor.action))) ||
                  ((i16(owner.animation.cursor.elapsed) != 0 ||
                   (i16(p.animation.cursor.index) != 0)))) {
            if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              setChannel(24,u8(0x80));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
              p.response = 0;
              p.hits = 1;
              p.counter = 0;
              p.timer = 0;
            }
            if (i16(p.animation.cursor.index) == 1) {
              p.pose.rotation[2] = double(p.pose.rotation[2]) + double(40.0f);
            }
            if (i16(p.animation.cursor.index) == 2) {
              p.counter = i16(p.counter) + 1;
              owner.launchParameters[0] = double(p.pose.rotation[2]);
              owner.launchParameters[1] = (float)(int)i16(p.counter);
              owned(0x32c,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
              owner.effectVisual.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
              owner.effectVisual.mode = 0;
              owner.effectVisual.projection[0] = 0;
              owner.effectVisual.projection[1] = 0;
              owner.effectVisual.projection[2] = 0;
              owner.effectVisual.scale[0] = 1.0f;
              owner.effectVisual.scale[1] = 1.0f;
              owner.effectVisual.alpha = 0xff;
              owner.effectVisual.red = 0xff;
              owner.effectVisual.green = 0xff;
              owner.effectVisual.blue = 0xff;
              owner.effectVisual.rotation[0] = 0.0f;
              owner.effectVisual.rotation[1] = 0.0f;
              ownedVisual(0x32d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0,1);
            }
            if ((i16(p.animation.cursor.index) == 0) || (i16(p.animation.cursor.index) == 2)) {
              p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.1f) + double(p.parameters[1]));
              angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
              p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((p.response == 2) || (p.response == 4)) &&
               (i16(p.animation.cursor.index) < 3)) {
              p.frame(3);
            }
            else {
              if (i16(p.animation.cursor.index) == 3) {
                p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
                p.pose.rotation[2] = double(p.pose.rotation[2]) + double(40.0f);
              }
              advancedFrame = p.advance();
              if (advancedFrame == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
              if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.hits = 1;
                p.response = 0;
                angleRadians = (double)atan((double(other.motion.y) + double(100.0f)) -
                                              double(p.motion.y),(double(other.motion.x) - double(p.motion.x)) *
                                              (float)(int)i8(p.motion.facing));
                p.parameters[0] = (float)(angleRadians / (double)double(0.017453289f));
                p.parameters[1] = 17.5f;
                p.polar(double(p.parameters[0]),double(p.parameters[1]));
                angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
                p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
              }
            }
          }
          else {
            setChannel(24,u8(0xff));
            p.frame(1);
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x328:
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
            p.response = 0;
            p.hits = 1;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if (0 < i16(p.animation.cursor.index)) {
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.5f) + double(p.parameters[1]));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.counter = i16(p.counter) + 1;
          if (((double(2000.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(1000.0f))) ||
             ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f))))) {
            p.destroy();
          }
          else {
            if (i16(p.hits) == 0) {
              if (u8(channel(24)) < 0xb) {
                setChannel(24,u8(0));
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -10));
            }
            if (i16(p.animation.cursor.index) == 1) {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.3f);
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x329:
        if ((((i16(owner.animation.cursor.action) == 500) ||
             (i16(owner.animation.cursor.action) == 0x1f5)) ||
            (i16(owner.animation.cursor.action) == 0x1f6)) ||
           ((i16(owner.animation.cursor.action) == 0x1f7 ||
            (1 < i16(p.animation.cursor.index))))) {
          if ((((i16(owner.animation.cursor.action) == 500) ||
               (i16(owner.animation.cursor.action) == 0x1f5)) &&
              (i16(owner.animation.cursor.index) < 10)) ||
             (((i16(owner.animation.cursor.action) == 0x1f6 ||
               (i16(owner.animation.cursor.action) == 0x1f7)) &&
              (i16(owner.animation.cursor.index) < 4)))) {
            p.motion.x = double(owner.motion.x);
            p.motion.y = double(owner.motion.y) + double(132.0f);
          }
          else if (i16(p.animation.cursor.index) < 2) {
            p.frame(2);
            return;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = double(p.parameters[0]) * double(0.1f);
            p.pose.scale[1] = double(p.parameters[0]) * double(0.1f);
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.projection[2] = 0x4e2;
            p.pose.rotation[0] = 0.0f;
            p.pose.rotation[1] =
                 (float)(int)i8(p.motion.facing) * double(p.parameters[1]);
            p.pose.rotation[2] = 0.0f;
            p.pose.rotation[0] = double(p.parameters[2]);
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(10.0f);
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
        break;
      case 0x32a:
        if (((i16(owner.animation.cursor.action) == 500) ||
            (i16(owner.animation.cursor.action) == 0x1f5)) ||
           ((i16(owner.animation.cursor.action) == 0x1f6 ||
            ((i16(owner.animation.cursor.action) == 0x1f7 ||
             (1 < i16(p.animation.cursor.index))))))) {
          if ((((i16(owner.animation.cursor.action) == 500) ||
               (i16(owner.animation.cursor.action) == 0x1f5)) &&
              (i16(owner.animation.cursor.index) < 10)) ||
             (((i16(owner.animation.cursor.action) == 0x1f6 ||
               (i16(owner.animation.cursor.action) == 0x1f7)) &&
              (i16(owner.animation.cursor.index) < 4)))) {
            p.motion.x = double(owner.motion.x);
            p.motion.y = double(owner.motion.y) + double(132.0f);
          }
          else if (i16(p.animation.cursor.index) < 2) {
            p.frame(2);
            return;
          }
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = double(p.parameters[0]) * double(0.1f);
            p.pose.scale[1] = double(p.parameters[0]) * double(0.1f);
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.projection[2] = 0x4e2;
            p.pose.rotation[0] = 0.0f;
            p.pose.rotation[1] =
                 (float)(int)i8(p.motion.facing) * double(p.parameters[1]);
            p.pose.rotation[2] = 0.0f;
            p.pose.rotation[0] = double(p.parameters[2]);
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(10.0f);
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
        break;
      case 0x32b:
        if (p.response == 5) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.hits = 0;
            p.response = 0;
          }
          if ((1 < i16(p.animation.cursor.index)) && (i16(p.animation.cursor.index) < 6)) {
            p.timer = i16(p.timer) + 1;
            p.polar(double(p.parameters[0]),(float)(int)i16(p.timer));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.5f);
          }
          if ((5 < i16(p.animation.cursor.index)) && (i16(p.animation.cursor.index) < 10)) {
            p.timer = i16(p.timer) + 1;
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.5f);
          }
          if (i16(p.animation.cursor.index) < 2) {
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f);
            p.polar(double(p.parameters[0]),4.5f);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(6);
            p.counter = i16(p.counter) + 1;
            if (0x28 < i16(p.counter)) {
              p.destroy();
            }
          }
          else {
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              setChannel(24,u8(0xff));
            }
            if (((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.counter) < 0x14)) {
              p.counter = i16(p.counter) + 1;
              p.frame(2);
            }
            else if ((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) {
              state.knivesActive = 0;
              p.pose.scale[0] = 1.0f;
              p.counter = 0;
              p.timer = 0;
              p.hits = 1;
              p.response = 0;
              aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                           double(p.motion.y),(double(other.motion.x) -
                           double(p.motion.x)) * (float)(int)i8(p.motion.facing));
              wordValue = i32(aimRadians/double(0.017453289f));
              p.angle = wordValue;
            }
          }
        }
        break;
      case 0x32c:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = double(p.parameters[1]) * double(0.2f) + double(0.25f);
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.response = 0;
          p.hits = 1;
          p.pose.rotation[2] = double(p.parameters[0]);
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x32d:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x32e:
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.2999999523162842f;
          p.pose.scale[1] = 1.2999999523162842f;
          setChannel(24,u8(0));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if (0 < i16(state.clockReserve)) {
          if (u8(channel(24)) < 0xf5) {
            setChannel(24,u8(i8(channel(24)) + '\n'));
          }
          else {
            setChannel(24,u8(0xff));
          }
        }
        if (i16(state.clockReserve) < 1) {
          if (u8(channel(24)) < 6) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -5));
        }
        p.pose.rotation[2] = double(p.pose.rotation[2]) + double(0.25f);
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
        }
        break;
      case 0x32f:
        p.motion.x = double(owner.motion.x);
        p.motion.y = double(owner.motion.y) + double(100.0f);
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 2.0f;
          p.pose.scale[1] = 2.0f;
          setChannel(24,u8(0));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.timer = state.clockReserve;
        }
        position = (float)(int)i16(state.clockReserve) /
                (float)(int)i16(p.timer);
        p.pose.scale[0] = position + position;
        position = (float)(int)i16(state.clockReserve) /
                (float)(int)i16(p.timer);
        p.pose.scale[1] = position + position;
        if ((i16(p.animation.cursor.index) == 1) && (0 < i16(state.clockReserve)))
        {
          if (0xf4 < u8(channel(24))) {
            setChannel(24,u8(0xff));
            p.frame(2);
            return;
          }
          setChannel(24,u8(i8(channel(24)) + '\n'));
        }
        if ((i16(p.animation.cursor.index) == 2) && (i16(state.clockReserve) < 1))
        {
          p.frame(3);
        }
        else if (i16(p.animation.cursor.index) == 3) {
          p.destroy();
        }
        else {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(3);
          }
          if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(2);
          }
        }
        break;
      case 0x330:
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.5f;
          p.pose.scale[1] = 1.5f;
          setChannel(24,u8(0));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if (0 < i16(state.timeReserve)) {
          if (u8(channel(24)) < 0xf5) {
            setChannel(24,u8(i8(channel(24)) + '\n'));
          }
          else {
            setChannel(24,u8(0xff));
          }
        }
        if (i16(state.timeReserve) < 1) {
          if (u8(channel(24)) < 6) {
            p.destroy();
            return;
          }
          setChannel(24,u8(i8(channel(24)) + -5));
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.frame(1);
        }
        break;
      case 0x331:
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
          p.counter = 1;
        }
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
          state.blockEffectSent = 0;
        }
        break;
      case 0x332:
        p.motion.x = double(owner.motion.x);
        p.motion.y = double(owner.motion.y) + double(100.0f);
        if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
           (i16(p.counter) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 2.0f;
          p.pose.scale[1] = 2.0f;
          setChannel(24,u8(0));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.timer = state.timeReserve;
        }
        position = (float)(int)i16(state.timeReserve) /
                (float)(int)i16(p.timer);
        p.pose.scale[0] = position + position;
        position = (float)(int)i16(state.timeReserve) /
                (float)(int)i16(p.timer);
        p.pose.scale[1] = position + position;
        if ((i16(p.animation.cursor.index) == 1) && (0 < i16(state.timeReserve)))
        {
          if (0xf4 < u8(channel(24))) {
            setChannel(24,u8(0xff));
            p.frame(2);
            return;
          }
          setChannel(24,u8(i8(channel(24)) + '\n'));
        }
        if ((i16(p.animation.cursor.index) == 2) && (i16(state.timeReserve) < 1))
        {
          p.frame(3);
        }
        else if (i16(p.animation.cursor.index) == 3) {
          p.destroy();
        }
        else {
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.frame(3);
          }
          if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(2);
          }
        }
        break;
      case 0x333:
        if (i16(owner.animation.cursor.action) == 0x12e) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if (i16(owner.animation.cursor.action) == 0x12e) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x334:
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
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x335:
        if ((i16(owner.animation.cursor.action) == 0x194) ||
           (i16(owner.animation.cursor.action) == 0x195)) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if ((i16(owner.animation.cursor.action) == 0x194) ||
           (i16(owner.animation.cursor.action) == 0x195)) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x336:
        if (i16(owner.animation.cursor.action) == 0x134) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        if (i16(owner.animation.cursor.action) == 0x134) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x337:
        if (i16(owner.animation.cursor.action) == 0x136) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
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
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            p.destroy();
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
          particles = random.range(0,0x5a);
          p.pose.rotation[2] = (float)(particles + -0x2d);
          wordValue = random.range(0,10);
          p.counter = wordValue;
          wordValue = random.range(0,10);
          p.timer = wordValue;
          wordValue = random.range(0,10);
          p.angle = wordValue;
          wordValue = random.range(0,5);
          p.cycle = wordValue;
          wordValue = random.range(0,5);
          p.variant = wordValue;
          particles = random.range(0,6);
          p.motion.velocityX = (float)(particles + -3);
          particles = random.range(0,2);
          p.motion.velocityY = (float)-particles;
        }
        p.motion.velocityY = double(p.motion.velocityY) - double(0.2f);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        if (i16(p.counter) < 5) {
          p.pose.rotation[2] =
               (float)(int)i16(p.cycle) + double(p.pose.rotation[2]);
        }
        if (4 < i16(p.counter)) {
          p.pose.rotation[2] =
               double(p.pose.rotation[2]) - (float)(int)i16(p.cycle);
        }
        if (i16(p.timer) < 5) {
          p.pose.rotation[0] =
               (float)(int)i16(p.variant) + double(p.pose.rotation[0]);
        }
        if (4 < i16(p.timer)) {
          p.pose.rotation[0] =
               double(p.pose.rotation[0]) - (float)(int)i16(p.variant);
        }
        if (i16(p.angle) < 5) {
          p.pose.rotation[1] =
               (float)(int)i16(p.variant) + double(p.pose.rotation[1]);
        }
        if (4 < i16(p.angle)) {
          p.pose.rotation[1] =
               double(p.pose.rotation[1]) - (float)(int)i16(p.variant);
        }
        setChannel(24,u8(i8(channel(24)) + -8));
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x339:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          particles = random.range(0,0x5a);
          p.pose.rotation[2] = (float)(particles + -0x2d);
          wordValue = random.range(0,10);
          p.counter = wordValue;
          wordValue = random.range(0,10);
          p.timer = wordValue;
          wordValue = random.range(0,10);
          p.angle = wordValue;
          wordValue = random.range(0,5);
          p.cycle = wordValue;
          wordValue = random.range(0,5);
          p.variant = wordValue;
          particles = random.range(0,6);
          p.motion.velocityX = (float)(particles + -3);
          particles = random.range(0,2);
          p.motion.velocityY = (float)-particles;
        }
        p.motion.velocityY = double(p.motion.velocityY) - double(0.2f);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        if (i16(p.counter) < 5) {
          p.pose.rotation[2] =
               (float)(int)i16(p.cycle) + double(p.pose.rotation[2]);
        }
        if (4 < i16(p.counter)) {
          p.pose.rotation[2] =
               double(p.pose.rotation[2]) - (float)(int)i16(p.cycle);
        }
        if (i16(p.timer) < 5) {
          p.pose.rotation[0] =
               (float)(int)i16(p.variant) + double(p.pose.rotation[0]);
        }
        if (4 < i16(p.timer)) {
          p.pose.rotation[0] =
               double(p.pose.rotation[0]) - (float)(int)i16(p.variant);
        }
        if (i16(p.angle) < 5) {
          p.pose.rotation[1] =
               (float)(int)i16(p.variant) + double(p.pose.rotation[1]);
        }
        if (4 < i16(p.angle)) {
          p.pose.rotation[1] =
               double(p.pose.rotation[1]) - (float)(int)i16(p.variant);
        }
        setChannel(24,u8(i8(channel(24)) + -8));
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
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
          particles = random.range(0,0x5a);
          p.pose.rotation[2] = (float)(particles + -0x2d);
          wordValue = random.range(0,10);
          p.counter = wordValue;
          wordValue = random.range(0,10);
          p.timer = wordValue;
          wordValue = random.range(0,10);
          p.angle = wordValue;
          wordValue = random.range(0,5);
          p.cycle = wordValue;
          wordValue = random.range(0,5);
          p.variant = wordValue;
          particles = random.range(0,6);
          p.motion.velocityX = (float)(particles + -3);
          particles = random.range(0,2);
          p.motion.velocityY = (float)-particles;
        }
        p.motion.velocityY = double(p.motion.velocityY) - double(0.2f);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        if (i16(p.counter) < 5) {
          p.pose.rotation[2] =
               (float)(int)i16(p.cycle) + double(p.pose.rotation[2]);
        }
        if (4 < i16(p.counter)) {
          p.pose.rotation[2] =
               double(p.pose.rotation[2]) - (float)(int)i16(p.cycle);
        }
        if (i16(p.timer) < 5) {
          p.pose.rotation[0] =
               (float)(int)i16(p.variant) + double(p.pose.rotation[0]);
        }
        if (4 < i16(p.timer)) {
          p.pose.rotation[0] =
               double(p.pose.rotation[0]) - (float)(int)i16(p.variant);
        }
        if (i16(p.angle) < 5) {
          p.pose.rotation[1] =
               (float)(int)i16(p.variant) + double(p.pose.rotation[1]);
        }
        if (4 < i16(p.angle)) {
          p.pose.rotation[1] =
               double(p.pose.rotation[1]) - (float)(int)i16(p.variant);
        }
        setChannel(24,u8(i8(channel(24)) + -8));
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
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
          particles = random.range(0,0x5a);
          p.pose.rotation[2] = (float)(particles + -0x2d);
          wordValue = random.range(0,10);
          p.counter = wordValue;
          wordValue = random.range(0,10);
          p.timer = wordValue;
          wordValue = random.range(0,10);
          p.angle = wordValue;
          wordValue = random.range(0,5);
          p.cycle = wordValue;
          wordValue = random.range(0,5);
          p.variant = wordValue;
          particles = random.range(0,6);
          p.motion.velocityX = (float)(particles + -3);
          particles = random.range(0,2);
          p.motion.velocityY = (float)-particles;
        }
        p.motion.velocityY = double(p.motion.velocityY) - double(0.2f);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        if (i16(p.counter) < 5) {
          p.pose.rotation[2] =
               (float)(int)i16(p.cycle) + double(p.pose.rotation[2]);
        }
        if (4 < i16(p.counter)) {
          p.pose.rotation[2] =
               double(p.pose.rotation[2]) - (float)(int)i16(p.cycle);
        }
        if (i16(p.timer) < 5) {
          p.pose.rotation[0] =
               (float)(int)i16(p.variant) + double(p.pose.rotation[0]);
        }
        if (4 < i16(p.timer)) {
          p.pose.rotation[0] =
               double(p.pose.rotation[0]) - (float)(int)i16(p.variant);
        }
        if (i16(p.angle) < 5) {
          p.pose.rotation[1] =
               (float)(int)i16(p.variant) + double(p.pose.rotation[1]);
        }
        if (4 < i16(p.angle)) {
          p.pose.rotation[1] =
               double(p.pose.rotation[1]) - (float)(int)i16(p.variant);
        }
        setChannel(24,u8(i8(channel(24)) + -8));
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
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
          particles = random.range(0,0x5a);
          p.pose.rotation[2] = (float)(particles + -0x2d);
          wordValue = random.range(0,10);
          p.counter = wordValue;
          wordValue = random.range(0,10);
          p.timer = wordValue;
          wordValue = random.range(0,10);
          p.angle = wordValue;
          wordValue = random.range(0,5);
          p.cycle = wordValue;
          wordValue = random.range(0,5);
          p.variant = wordValue;
          particles = random.range(0,6);
          p.motion.velocityX = (float)(particles + -3);
          particles = random.range(0,2);
          p.motion.velocityY = (float)-particles;
        }
        p.motion.velocityY = double(p.motion.velocityY) - double(0.2f);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        if (i16(p.counter) < 5) {
          p.pose.rotation[2] =
               (float)(int)i16(p.cycle) + double(p.pose.rotation[2]);
        }
        if (4 < i16(p.counter)) {
          p.pose.rotation[2] =
               double(p.pose.rotation[2]) - (float)(int)i16(p.cycle);
        }
        if (i16(p.timer) < 5) {
          p.pose.rotation[0] =
               (float)(int)i16(p.variant) + double(p.pose.rotation[0]);
        }
        if (4 < i16(p.timer)) {
          p.pose.rotation[0] =
               double(p.pose.rotation[0]) - (float)(int)i16(p.variant);
        }
        if (i16(p.angle) < 5) {
          p.pose.rotation[1] =
               (float)(int)i16(p.variant) + double(p.pose.rotation[1]);
        }
        if (4 < i16(p.angle)) {
          p.pose.rotation[1] =
               double(p.pose.rotation[1]) - (float)(int)i16(p.variant);
        }
        setChannel(24,u8(i8(channel(24)) + -8));
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33d:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          particles = random.range(0,0x5a);
          p.pose.rotation[2] = (float)(particles + -0x2d);
          wordValue = random.range(0,10);
          p.counter = wordValue;
          wordValue = random.range(0,10);
          p.timer = wordValue;
          wordValue = random.range(0,10);
          p.angle = wordValue;
          wordValue = random.range(0,5);
          p.cycle = wordValue;
          wordValue = random.range(0,5);
          p.variant = wordValue;
          particles = random.range(0,10);
          p.motion.velocityX = (float)(particles + -5);
          particles = random.range(0,2);
          p.motion.velocityY = (float)-particles;
        }
        p.motion.velocityY = double(p.motion.velocityY) - double(0.2f);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        if (i16(p.counter) < 5) {
          p.pose.rotation[2] =
               (float)(int)i16(p.cycle) + double(p.pose.rotation[2]);
        }
        if (4 < i16(p.counter)) {
          p.pose.rotation[2] =
               double(p.pose.rotation[2]) - (float)(int)i16(p.cycle);
        }
        if (i16(p.timer) < 5) {
          p.pose.rotation[0] =
               (float)(int)i16(p.variant) + double(p.pose.rotation[0]);
        }
        if (4 < i16(p.timer)) {
          p.pose.rotation[0] =
               double(p.pose.rotation[0]) - (float)(int)i16(p.variant);
        }
        if (i16(p.angle) < 5) {
          p.pose.rotation[1] =
               (float)(int)i16(p.variant) + double(p.pose.rotation[1]);
        }
        if (4 < i16(p.angle)) {
          p.pose.rotation[1] =
               double(p.pose.rotation[1]) - (float)(int)i16(p.variant);
        }
        setChannel(24,u8(i8(channel(24)) + -8));
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33e:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          particles = random.range(0,0xf);
          p.pose.scale[0] = (float)particles * double(0.1f) + double(0.5f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          particles = random.range(0,2);
          p.motion.velocityY = (float)-particles;
          p.motion.velocityX = 0.0f;
        }
        p.motion.velocityY = double(p.motion.velocityY) + double(0.4f);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        setChannel(24,u8(i8(channel(24)) + -8));
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33f:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           ((4 < i16(p.animation.cursor.index) ||
            (i16(owner.animation.cursor.action) == 0x2be)))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.response = 0;
            p.hits = 1;
            direction = 20.0f;
            particles = random.range(0,0x5a);
            p.polar((float)(particles + -0x2d),direction);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            if (i16(p.counter) < 4) {
              p.counter = i16(p.counter) + 1;
              p.frame(1);
            }
            else {
              p.destroy();
            }
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x340:
        if ((((i16(owner.animation.cursor.action) < 0x32) ||
             (0x95 < i16(owner.animation.cursor.action))) ||
            (4 < i16(p.animation.cursor.index))) ||
           (i16(owner.animation.cursor.action) == 0x2bf)) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.response = 0;
            p.hits = 1;
            direction = 30.0f;
            particles = random.range(0,0x50);
            p.polar((float)(particles + -0x28),direction);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            if (i16(p.counter) < 9) {
              p.counter = i16(p.counter) + 1;
              p.frame(1);
            }
            else {
              p.destroy();
            }
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x341:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.counter = 0;
          p.timer = 0x3c;
          advancedFrame = random.range(0,0x28);
          p.angle = 0x14 - advancedFrame;
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 2.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
        }
        if (0 < i16(p.animation.cursor.index)) {
          p.timer = i16(p.timer) + 1;
          p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer));
          angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
          p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.5f);
          p.pose.scale[1] = double(p.pose.scale[1]) * double(0.98f);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x342:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (4 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.response = 0;
            p.hits = 1;
            p.angle = 0;
            p.timer = 0;
            p.counter = 0;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.counter = i16(p.counter) + 1;
          if (10 < i16(p.counter)) {
            p.counter = 10;
          }
          if ((i16(p.animation.cursor.index) < 5) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            owner.effectVisual.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            owner.effectVisual.mode = 0;
            owner.effectVisual.projection[0] = 0;
            owner.effectVisual.projection[1] = 0;
            owner.effectVisual.projection[2] = 0;
            owner.effectVisual.scale[0] = 1.0f;
            owner.effectVisual.scale[1] = 1.0f;
            owner.effectVisual.alpha = 0xff;
            owner.effectVisual.red = 0xff;
            owner.effectVisual.green = 0xff;
            owner.effectVisual.blue = 0xff;
            owner.effectVisual.rotation[0] = 0.0f;
            owner.effectVisual.rotation[1] = 0.0f;
            ownedVisual(0x343,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0,1);
            p.angle = i16(p.angle) + 1;
            p.timer = i16(p.timer) + 1;
            p.parameters[1] = double(p.parameters[1]) + double(1.0f);
            if (double(15.0f) < double(p.parameters[1])) {
              p.parameters[1] = 15.0f;
            }
            if ((5 < i16(p.timer)) && (0 < i16(p.angle))) {
              p.timer = 0;
              particles = random.range(0,0xb4);
              p.parameters[0] = (float)(particles + -0x5a);
            }
          }
          if (((p.response == 2) || (p.response == 4)) &&
             (i16(p.animation.cursor.index) < 5)) {
            p.frame(5);
          }
          if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
             ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f))))) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = -double(p.motion.velocityX) * double(0.25f);
              p.motion.velocityY = -14.0f;
            }
            if (i16(p.animation.cursor.index) == 5) {
              p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
              p.pose.rotation[2] = double(p.pose.rotation[2]) + double(40.0f);
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x343:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        setChannel(24,u8(i8(channel(24)) + -0xc));
        setChannel(16,u8(i8(channel(16)) + -0xc));
        setChannel(8,u8(i8(channel(8)) + -0xc));
        advancedFrame = p.advance();
        if (advancedFrame == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x344:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (5 < i16(p.animation.cursor.index))) {
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
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.counter = i16(p.counter) + 1;
          if (i16(p.animation.cursor.index) < 6) {
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.5f) + double(p.parameters[1]));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            if (0 < i16(p.animation.cursor.index)) {
              angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
              owner.effectVisual.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
              owner.effectVisual.mode = 0;
              owner.effectVisual.projection[0] = 0;
              owner.effectVisual.projection[1] = 0;
              owner.effectVisual.projection[2] = 0;
              owner.effectVisual.scale[0] = 1.0f;
              owner.effectVisual.scale[1] = 1.0f;
              owner.effectVisual.alpha = 0xff;
              owner.effectVisual.red = 0xff;
              owner.effectVisual.green = 0xff;
              owner.effectVisual.blue = 0xff;
              owner.effectVisual.rotation[0] = 0.0f;
              owner.effectVisual.rotation[1] = 0.0f;
              ownedVisual(0x343,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0,1);
            }
          }
          if (((p.response == 2) || (p.response == 4)) &&
             (i16(p.animation.cursor.index) < 6)) {
            p.frame(7);
            p.motion.velocityX = -double(p.motion.velocityX) * double(0.25f);
            p.motion.velocityY = -14.0f;
          }
          else if ((((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f)))
                   || (double(1000.0f) < double(p.motion.y))) ||
                  (double(p.motion.y) < -double(160.0f))) {
            p.destroy();
          }
          else {
            if (i16(p.animation.cursor.index) == 7) {
              p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
              p.pose.rotation[2] = double(p.pose.rotation[2]) + double(40.0f);
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.contactDelay = 0;
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.response = 0;
              p.hits = 1;
            }
            if ((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(2);
            }
            if ((i16(p.animation.cursor.index) == 7) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(6);
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
      case 0x345:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (4 < i16(p.animation.cursor.index))) {
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
            p.timer = 0;
            p.counter = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.counter = i16(p.counter) + 1;
          if (i16(p.animation.cursor.index) < 5) {
            p.polar(double(p.parameters[0]),(float)(int)i16(p.counter) * double(0.5f) + double(p.parameters[1]));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            if (((double(owner.combat.screenBounds[2]) < double(p.motion.y)) &&
                (i16(p.timer) < 2)) && (double(p.motion.velocityY) < double(0.0f))) {
              events.sound(0xb);
              p.parameters[0] = -double(p.parameters[0]);
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
            if (((double(p.motion.y) < double(0.0f)) && (i16(p.timer) < 2)) &&
               (double(0.0f) < double(p.motion.velocityY))) {
              events.sound(0xb);
              p.parameters[0] = -double(p.parameters[0]);
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
            if (((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
                (i16(p.timer) < 2)) &&
               (((i8(p.motion.facing) == -1 && (double(0.0f) < double(p.motion.velocityX))) ||
                ((i8(p.motion.facing) == '\x01' && (double(p.motion.velocityX) < double(0.0f))))))) {
              events.sound(0xb);
              if (double(0.0f) < double(p.parameters[0])) {
                p.parameters[0] = double(180.0f) - double(p.parameters[0]);
              }
              if (double(p.parameters[0]) < double(0.0f)) {
                p.parameters[0] = -double(180.0f) - double(p.parameters[0]);
              }
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
            if (((double(owner.combat.screenBounds[1]) < double(p.motion.x)) &&
                (i16(p.timer) < 2)) &&
               (((i8(p.motion.facing) == -1 && (double(p.motion.velocityX) < double(0.0f))) ||
                ((i8(p.motion.facing) == '\x01' && (double(0.0f) < double(p.motion.velocityX))))))) {
              events.sound(0xb);
              if (double(0.0f) < double(p.parameters[0])) {
                p.parameters[0] = double(180.0f) - double(p.parameters[0]);
              }
              if (double(p.parameters[0]) < double(0.0f)) {
                p.parameters[0] = -double(180.0f) - double(p.parameters[0]);
              }
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
            if (0 < i16(p.animation.cursor.index)) {
              angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
              owner.effectVisual.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
              owner.effectVisual.mode = 0;
              owner.effectVisual.projection[0] = 0;
              owner.effectVisual.projection[1] = 0;
              owner.effectVisual.projection[2] = 0;
              owner.effectVisual.scale[0] = 1.0f;
              owner.effectVisual.scale[1] = 1.0f;
              owner.effectVisual.alpha = 0xff;
              owner.effectVisual.red = 0xff;
              owner.effectVisual.green = 0xff;
              owner.effectVisual.blue = 0xff;
              owner.effectVisual.rotation[0] = 0.0f;
              owner.effectVisual.rotation[1] = 0.0f;
              ownedVisual(0x343,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0,1);
            }
          }
          if (((p.response == 2) || (p.response == 4)) &&
             (i16(p.animation.cursor.index) == 0)) {
            p.frame(6);
          }
          else if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f)))
                  || ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f)))
                     )) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 7) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = -double(p.motion.velocityX) * double(0.25f);
              p.motion.velocityY = -14.0f;
            }
            if (i16(p.animation.cursor.index) == 7) {
              p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
              p.pose.rotation[2] = double(p.pose.rotation[2]) + double(40.0f);
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.contactDelay = 0;
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.response = 0;
              p.hits = 1;
            }
            if ((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(2);
            }
            if ((i16(p.animation.cursor.index) == 8) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(7);
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
      case 0x346:
        if (p.response == 5) {
          p.destroy();
        }
        else if ((((i16(owner.animation.cursor.action) < 0x46) ||
                  (1 < i16(p.animation.cursor.index))) &&
                 ((i16(owner.animation.cursor.action) < 0x3c ||
                  (i16(p.animation.cursor.index) != 2)))) ||
                (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0x80));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.response = 0;
            p.hits = 1;
            p.counter = 0;
            p.timer = 0;
          }
          if ((((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
              (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(160.0f))) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) != 2) && (i16(p.animation.cursor.index) != 4)) {
              p.polar(double(p.parameters[0]),double(p.parameters[1]));
              p.pose.rotation[2] = -double(p.parameters[0]);
            }
            if (i16(p.animation.cursor.index) == 2) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (i16(p.animation.cursor.index) == 3) {
              owner.launchParameters[0] = double(p.pose.rotation[2]);
              owner.launchParameters[1] = (float)(int)i16(p.counter);
              owned(0x32c,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0);
              angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
              owner.effectVisual.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
              owner.effectVisual.mode = 0;
              owner.effectVisual.projection[0] = 0;
              owner.effectVisual.projection[1] = 0;
              owner.effectVisual.projection[2] = 0;
              owner.effectVisual.scale[0] = 1.0f;
              owner.effectVisual.scale[1] = 1.0f;
              owner.effectVisual.alpha = 0xff;
              owner.effectVisual.red = 0xff;
              owner.effectVisual.green = 0xff;
              owner.effectVisual.blue = 0xff;
              owner.effectVisual.rotation[0] = 0.0f;
              owner.effectVisual.rotation[1] = 0.0f;
              ownedVisual(0x32d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0,1);
            }
            if ((((p.response == 2) || (p.response == 4)) ||
                (p.response == 6)) && (i16(p.animation.cursor.index) < 4)) {
              p.frame(4);
              particles = random.range(0,5);
              p.motion.velocityX = (float)(7 - particles);
              particles = random.range(0,7);
              p.motion.velocityY = (float)(7 - particles);
            }
            else {
              if (i16(p.animation.cursor.index) == 4) {
                p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
                p.pose.rotation[2] = double(p.pose.rotation[2]) + double(40.0f);
              }
              advancedFrame = p.advance();
              if (advancedFrame == i16(p.animation.cursor.frameCount)) {
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x347:
        if (p.response == 5) {
          p.destroy();
        }
        else if ((((i16(owner.animation.cursor.action) < 0x46) ||
                  (1 < i16(p.animation.cursor.index))) &&
                 ((i16(owner.animation.cursor.action) < 0x3c ||
                  (i16(p.animation.cursor.index) != 2)))) ||
                (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0x80));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.response = 0;
            p.hits = 1;
            p.counter = 0;
            p.timer = 0;
          }
          if ((((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
              (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(160.0f))) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) != 2) && (i16(p.animation.cursor.index) != 4)) {
              p.polar(double(p.parameters[0]),double(p.parameters[1]));
              p.pose.rotation[2] = -double(p.parameters[0]);
            }
            if (i16(p.animation.cursor.index) == 2) {
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (i16(p.animation.cursor.index) == 3) {
              owner.launchParameters[0] = double(p.pose.rotation[2]);
              owner.launchParameters[1] = (float)(int)i16(p.counter);
              owned(0x32c,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0);
              angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
              owner.effectVisual.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
              owner.effectVisual.mode = 0;
              owner.effectVisual.projection[0] = 0;
              owner.effectVisual.projection[1] = 0;
              owner.effectVisual.projection[2] = 0;
              owner.effectVisual.scale[0] = 1.0f;
              owner.effectVisual.scale[1] = 1.0f;
              owner.effectVisual.alpha = 0xff;
              owner.effectVisual.red = 0xff;
              owner.effectVisual.green = 0xff;
              owner.effectVisual.blue = 0xff;
              owner.effectVisual.rotation[0] = 0.0f;
              owner.effectVisual.rotation[1] = 0.0f;
              ownedVisual(0x32d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0,1);
            }
            if ((((p.response == 2) || (p.response == 4)) ||
                (p.response == 6)) && (i16(p.animation.cursor.index) < 4)) {
              p.frame(4);
              particles = random.range(0,5);
              p.motion.velocityX = (float)(7 - particles);
              particles = random.range(0,7);
              p.motion.velocityY = (float)(7 - particles);
            }
            else {
              if (i16(p.animation.cursor.index) == 4) {
                p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
                p.pose.rotation[2] = double(p.pose.rotation[2]) + double(40.0f);
              }
              advancedFrame = p.advance();
              if (advancedFrame == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
              if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.motion.facing = -i8(p.motion.facing);
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
      default:
        p.advance();
        break;
      case 0x385:
        if (i16(owner.combat.spellTimer) == 0) {
          p.destroy();
        }
        else {
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
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.counter = i16(p.counter) + 1;
          if (i16(p.animation.cursor.index) < 5) {
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            owner.effectVisual.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            owner.effectVisual.mode = 0;
            owner.effectVisual.projection[0] = 0;
            owner.effectVisual.projection[1] = 0;
            owner.effectVisual.projection[2] = 0;
            owner.effectVisual.scale[0] = 1.0f;
            owner.effectVisual.scale[1] = 1.0f;
            owner.effectVisual.alpha = 0xff;
            owner.effectVisual.red = 0xff;
            owner.effectVisual.green = 0xff;
            owner.effectVisual.blue = 0xff;
            owner.effectVisual.rotation[0] = 0.0f;
            owner.effectVisual.rotation[1] = 0.0f;
          }
          if (p.response == 6) {
            p.hits = 1;
          }
          if (((p.response == 2) || (p.response == 4)) &&
             (i16(p.animation.cursor.index) < 5)) {
            p.frame(6);
          }
          else if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f)))
                  || ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f)))
                     )) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = -double(p.motion.velocityX) * double(0.25f);
              p.motion.velocityY = -14.0f;
            }
            if (i16(p.animation.cursor.index) == 6) {
              p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
              p.pose.rotation[2] = double(p.pose.rotation[2]) + double(40.0f);
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
            if ((i16(p.animation.cursor.index) == 7) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(6);
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
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) == 0)) {
            p.hits = 1;
            p.response = 0;
            aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                         double(p.motion.y),(double(other.motion.x) -
                         double(p.motion.x)) * (float)(int)i8(p.motion.facing));
            wordValue = i32(aimRadians/double(0.017453289f));
            p.angle = wordValue;
            p.counter = i16(p.counter) + 1;
          }
          if (i16(p.animation.cursor.index) < 2) {
            p.pose.rotation[2] = double(p.pose.rotation[2]) + double(15.0f);
            p.polar(double(p.parameters[0]),4.5f);
          }
          if (1 < i16(p.animation.cursor.index)) {
            p.timer = i16(p.timer) + 1;
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.5f);
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if (((double(1600.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(400.0f))) ||
             ((double(1400.0f) < double(p.motion.y) || (double(p.motion.y) < -double(400.0f))))) {
            p.destroy();
          }
          else {
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.frame(2);
            }
            if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
              setChannel(24,u8(0xff));
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
            p.parameters[1] = 15.0f;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.response = 0;
            p.hits = 1;
            p.timer = 0;
          }
          if (p.response == 0) {
            setChannel(24,u8(0xff));
          }
          else {
            setChannel(24,u8(0x80));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.counter = i16(p.counter) + 1;
          if (5 < i16(p.counter)) {
            p.counter = 5;
          }
          if (i16(p.animation.cursor.index) == 1) {
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            if (p.response == 0) {
              owner.launchParameters[0] = double(p.pose.rotation[2]);
              owner.launchParameters[1] = (float)(int)i16(p.counter);
              owned(0x32c,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
              owner.effectVisual.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
              owner.effectVisual.mode = 0;
              owner.effectVisual.projection[0] = 0;
              owner.effectVisual.projection[1] = 0;
              owner.effectVisual.projection[2] = 0;
              owner.effectVisual.scale[0] = 1.0f;
              owner.effectVisual.scale[1] = 1.0f;
              owner.effectVisual.alpha = 0xff;
              owner.effectVisual.red = 0xff;
              owner.effectVisual.green = 0xff;
              owner.effectVisual.blue = 0xff;
              owner.effectVisual.rotation[0] = 0.0f;
              owner.effectVisual.rotation[1] = 0.0f;
              ownedVisual(0x32d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0,1);
            }
            if (((double(owner.combat.screenBounds[2]) < double(p.motion.y)) &&
                (i16(p.timer) < 1)) && (double(p.motion.velocityY) < double(0.0f))) {
              events.sound(0xb);
              p.parameters[0] = -double(p.parameters[0]);
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
              p.parameters[1] = 4.0f;
            }
            if (((double(p.motion.y) < double(0.0f)) && (i16(p.timer) < 1)) &&
               (double(0.0f) < double(p.motion.velocityY))) {
              events.sound(0xb);
              p.parameters[0] = -double(p.parameters[0]);
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
              p.parameters[1] = 4.0f;
            }
            if (((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
                (i16(p.timer) < 1)) &&
               (((i8(p.motion.facing) == -1 && (double(0.0f) < double(p.motion.velocityX))) ||
                ((i8(p.motion.facing) == '\x01' && (double(p.motion.velocityX) < double(0.0f))))))) {
              events.sound(0xb);
              if (double(0.0f) < double(p.parameters[0])) {
                p.parameters[0] = double(180.0f) - double(p.parameters[0]);
              }
              if (double(p.parameters[0]) < double(0.0f)) {
                p.parameters[0] = -double(180.0f) - double(p.parameters[0]);
              }
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
              p.parameters[1] = 4.0f;
            }
            if (((double(owner.combat.screenBounds[1]) < double(p.motion.x)) &&
                (i16(p.timer) < 1)) &&
               (((i8(p.motion.facing) == -1 && (double(p.motion.velocityX) < double(0.0f))) ||
                ((i8(p.motion.facing) == '\x01' && (double(0.0f) < double(p.motion.velocityX))))))) {
              events.sound(0xb);
              if (double(0.0f) < double(p.parameters[0])) {
                p.parameters[0] = double(180.0f) - double(p.parameters[0]);
              }
              if (double(p.parameters[0]) < double(0.0f)) {
                p.parameters[0] = -double(180.0f) - double(p.parameters[0]);
              }
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
              p.parameters[1] = 4.0f;
            }
          }
          if (((p.response == 2) || (p.response == 4)) &&
             (i16(p.animation.cursor.index) == 1)) {
            p.frame(2);
          }
          else if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f)))
                  || ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f)))
                     )) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = -double(p.motion.velocityX) * double(0.25f);
              p.motion.velocityY = -14.0f;
            }
            if (i16(p.animation.cursor.index) == 2) {
              p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
              p.pose.rotation[2] = double(p.pose.rotation[2]) + double(40.0f);
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
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
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.response = 0;
            p.hits = 1;
            p.timer = 0;
          }
          if (p.response == 0) {
            setChannel(24,u8(0xff));
          }
          else {
            setChannel(24,u8(0x80));
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.counter = i16(p.counter) + 1;
          if (5 < i16(p.counter)) {
            p.counter = 5;
          }
          if (i16(p.animation.cursor.index) == 1) {
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            if (p.response == 0) {
              owner.launchParameters[0] = double(p.pose.rotation[2]);
              owner.launchParameters[1] = (float)(int)i16(p.counter);
              owned(0x32c,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
              owner.effectVisual.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
              owner.effectVisual.mode = 0;
              owner.effectVisual.projection[0] = 0;
              owner.effectVisual.projection[1] = 0;
              owner.effectVisual.projection[2] = 0;
              owner.effectVisual.scale[0] = 1.0f;
              owner.effectVisual.scale[1] = 1.0f;
              owner.effectVisual.alpha = 0xff;
              owner.effectVisual.red = 0xff;
              owner.effectVisual.green = 0xff;
              owner.effectVisual.blue = 0xff;
              owner.effectVisual.rotation[0] = 0.0f;
              owner.effectVisual.rotation[1] = 0.0f;
              ownedVisual(0x32d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0,1);
            }
            if (((double(owner.combat.screenBounds[2]) < double(p.motion.y)) &&
                (i16(p.timer) < 2)) && (double(p.motion.velocityY) < double(0.0f))) {
              events.sound(0xb);
              p.parameters[0] = -double(p.parameters[0]);
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
            if (((double(p.motion.y) < double(0.0f)) && (i16(p.timer) < 2)) &&
               (double(0.0f) < double(p.motion.velocityY))) {
              events.sound(0xb);
              p.parameters[0] = -double(p.parameters[0]);
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
            if (((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
                (i16(p.timer) < 2)) &&
               (((i8(p.motion.facing) == -1 && (double(0.0f) < double(p.motion.velocityX))) ||
                ((i8(p.motion.facing) == '\x01' && (double(p.motion.velocityX) < double(0.0f))))))) {
              events.sound(0xb);
              if (double(0.0f) < double(p.parameters[0])) {
                p.parameters[0] = double(180.0f) - double(p.parameters[0]);
              }
              if (double(p.parameters[0]) < double(0.0f)) {
                p.parameters[0] = -double(180.0f) - double(p.parameters[0]);
              }
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
            if (((double(owner.combat.screenBounds[1]) < double(p.motion.x)) &&
                (i16(p.timer) < 2)) &&
               (((i8(p.motion.facing) == -1 && (double(p.motion.velocityX) < double(0.0f))) ||
                ((i8(p.motion.facing) == '\x01' && (double(0.0f) < double(p.motion.velocityX))))))) {
              events.sound(0xb);
              if (double(0.0f) < double(p.parameters[0])) {
                p.parameters[0] = double(180.0f) - double(p.parameters[0]);
              }
              if (double(p.parameters[0]) < double(0.0f)) {
                p.parameters[0] = -double(180.0f) - double(p.parameters[0]);
              }
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
          }
          if (((p.response == 2) || (p.response == 4)) &&
             (i16(p.animation.cursor.index) == 1)) {
            p.frame(2);
          }
          else if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f)))
                  || ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f)))
                     )) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = -double(p.motion.velocityX) * double(0.25f);
              p.motion.velocityY = -14.0f;
            }
            if (i16(p.animation.cursor.index) == 2) {
              p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
              p.pose.rotation[2] = double(p.pose.rotation[2]) + double(40.0f);
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
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
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.contactDelay = 0;
            p.response = 0;
            p.hits = 1;
            p.angle = 5;
            advancedFrame = random.range(0,0x1e);
            p.timer = advancedFrame + -0xf;
            wordValue = random.range(0,0x32);
            p.counter = wordValue;
            p.polar((float)(int)i16(p.timer),(float)(int)i16(p.angle) * double(0.01f));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          p.polar((float)(int)i16(p.timer),(float)(int)i16(p.angle) * double(0.01f));
          p.angle = i16(p.angle) + 5 + i16(p.counter);
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if ((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 5)) {
            p.frame(6);
          }
          else if ((double(1320.0f) < double(p.motion.x)) ||
                  (((double(p.motion.x) < -double(40.0f) || (double(1000.0f) < double(p.motion.y))) ||
                   (double(p.motion.y) < -double(160.0f))))) {
            p.destroy();
          }
          else {
            if (i16(p.animation.cursor.index) == 6) {
              if (u8(channel(24)) < 0x15) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x14));
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
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
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.response = 0;
            p.hits = 1;
            wordValue = random.range(0,0x32);
            p.counter = wordValue;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          p.motion.velocityX =
               (float)(int)i16(p.counter) * 0.01 + double(0.05f) + double(p.motion.velocityX);
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if ((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 5)) {
            p.frame(6);
          }
          else if ((double(1320.0f) < double(p.motion.x)) ||
                  (((double(p.motion.x) < -double(40.0f) || (double(1000.0f) < double(p.motion.y))) ||
                   (double(p.motion.y) < -double(160.0f))))) {
            p.destroy();
          }
          else {
            if (i16(p.animation.cursor.index) == 6) {
              if (u8(channel(24)) < 0x15) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x14));
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
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
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.response = 0;
            p.hits = 1;
            wordValue = random.range(0,0x32);
            p.counter = wordValue;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          p.motion.velocityX =
               (float)(int)i16(p.counter) * 0.01 + double(0.05f) + double(p.motion.velocityX);
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if ((i16(p.hits) == 0) && (i16(p.animation.cursor.index) < 5)) {
            p.frame(6);
          }
          else if ((double(1320.0f) < double(p.motion.x)) ||
                  (((double(p.motion.x) < -double(40.0f) || (double(1000.0f) < double(p.motion.y))) ||
                   (double(p.motion.y) < -double(160.0f))))) {
            p.destroy();
          }
          else {
            if (i16(p.animation.cursor.index) == 6) {
              if (u8(channel(24)) < 0x15) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x14));
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
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
            p.response = 0;
            p.hits = 1;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.counter = i16(p.counter) + 1;
          if (i16(p.animation.cursor.index) < 5) {
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            owner.effectVisual.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            owner.effectVisual.mode = 0;
            owner.effectVisual.projection[0] = 0;
            owner.effectVisual.projection[1] = 0;
            owner.effectVisual.projection[2] = 0;
            owner.effectVisual.scale[0] = 1.0f;
            owner.effectVisual.scale[1] = 1.0f;
            owner.effectVisual.alpha = 0xff;
            owner.effectVisual.red = 0xff;
            owner.effectVisual.green = 0xff;
            owner.effectVisual.blue = 0xff;
            owner.effectVisual.rotation[0] = 0.0f;
            owner.effectVisual.rotation[1] = 0.0f;
            ownedVisual(0x32d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0,1);
          }
          if (((p.response == 2) || (p.response == 4)) &&
             (i16(p.animation.cursor.index) < 5)) {
            p.frame(6);
          }
          else if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f)))
                  || ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f)))
                     )) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = -double(p.motion.velocityX) * double(0.25f);
              p.motion.velocityY = -14.0f;
            }
            if (i16(p.animation.cursor.index) == 6) {
              p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
              p.pose.rotation[2] = double(p.pose.rotation[2]) + double(40.0f);
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
            if ((i16(p.animation.cursor.index) == 7) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(6);
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
            p.counter = 0;
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            owner.launchParameters[0] = double(p.parameters[0]);
            owner.launchParameters[1] = 10.0f;
            owned(0x38d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
            if ((int)i16(p.counter) < state.difficulty * 2 + 1) {
              p.frame(1);
              p.counter = i16(p.counter) + 1;
            }
            else {
              p.destroy();
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
            p.response = 0;
            p.hits = 1;
            p.timer = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.counter = i16(p.counter) + 1;
          if (i16(p.animation.cursor.index) < 5) {
            if (((double(owner.combat.screenBounds[2]) < double(p.motion.y)) &&
                ((int)i16(p.timer) < (int)state.difficulty)) &&
               (double(p.motion.velocityY) < double(0.0f))) {
              events.sound(0xb);
              p.parameters[0] = -double(p.parameters[0]);
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
            if (((double(p.motion.x) < double(owner.combat.screenBounds[0])) &&
                ((int)i16(p.timer) < (int)state.difficulty)) &&
               (((i8(p.motion.facing) == -1 && (double(0.0f) < double(p.motion.velocityX))) ||
                ((i8(p.motion.facing) == '\x01' && (double(p.motion.velocityX) < double(0.0f))))))) {
              events.sound(0xb);
              if (double(0.0f) < double(p.parameters[0])) {
                p.parameters[0] = double(180.0f) - double(p.parameters[0]);
              }
              if (double(p.parameters[0]) < double(0.0f)) {
                p.parameters[0] = -double(180.0f) - double(p.parameters[0]);
              }
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
            if (((double(owner.combat.screenBounds[1]) < double(p.motion.x)) &&
                ((int)i16(p.timer) < (int)state.difficulty)) &&
               (((i8(p.motion.facing) == -1 && (double(p.motion.velocityX) < double(0.0f))) ||
                ((i8(p.motion.facing) == '\x01' && (double(0.0f) < double(p.motion.velocityX))))))) {
              events.sound(0xb);
              if (double(0.0f) < double(p.parameters[0])) {
                p.parameters[0] = double(180.0f) - double(p.parameters[0]);
              }
              if (double(p.parameters[0]) < double(0.0f)) {
                p.parameters[0] = -double(180.0f) - double(p.parameters[0]);
              }
              p.timer = i16(p.timer) + 1;
              p.response = 0;
              p.hits = 1;
            }
            p.polar(double(p.parameters[0]),double(p.parameters[1]));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            owner.effectVisual.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
            owner.effectVisual.mode = 0;
            owner.effectVisual.projection[0] = 0;
            owner.effectVisual.projection[1] = 0;
            owner.effectVisual.projection[2] = 0;
            owner.effectVisual.scale[0] = 1.0f;
            owner.effectVisual.scale[1] = 1.0f;
            owner.effectVisual.alpha = 0xff;
            owner.effectVisual.red = 0xff;
            owner.effectVisual.green = 0xff;
            owner.effectVisual.blue = 0xff;
            owner.effectVisual.rotation[0] = 0.0f;
            owner.effectVisual.rotation[1] = 0.0f;
            ownedVisual(0x32d,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),0,1);
            owner.launchParameters[0] = double(p.pose.rotation[2]);
            owner.launchParameters[1] = double(p.parameters[1]);
            owned(0x32c,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          if ((((p.response == 2) || (p.response == 4)) ||
              (p.response == 6)) && (i16(p.animation.cursor.index) < 5)) {
            p.frame(6);
          }
          else if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f)))
                  || ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f)))
                     )) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 6) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = -double(p.motion.velocityX) * double(0.25f);
              p.motion.velocityY = -14.0f;
            }
            if (i16(p.animation.cursor.index) == 6) {
              p.motion.velocityY = double(p.motion.velocityY) + double(0.5f);
              p.pose.rotation[2] = double(p.pose.rotation[2]) + double(40.0f);
            }
            advancedFrame = p.advance();
            if (advancedFrame == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if ((i16(p.animation.cursor.index) == 5) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
            }
            if ((i16(p.animation.cursor.index) == 7) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(6);
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
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
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
               (float)(int)i16(state.farRangeFrames) / double(60.0f);
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
          advancedFrame = p.advance();
          if (advancedFrame == i16(p.animation.cursor.frameCount)) {
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
        p.motion.y = double(owner.motion.y) * double(0.0f) + double(20.0f);
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
            p.frame(0x28);
          }
          if ((i16(p.animation.cursor.index) == 0x28) && (i16(p.animation.cursor.elapsed) == 0)) {
            setChannel(24,u8(0xff));
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
