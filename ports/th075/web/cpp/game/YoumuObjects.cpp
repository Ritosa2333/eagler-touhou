#include "YoumuObjects.hpp"
#include <cmath>
namespace th075 {void advance_youmu_object(YoumuObject& p,FighterActor& owner,FighterActor& other,YoumuState& state,ReplayStreams& random,FighterActionEvents& events){
const auto consumeSpirit=[&](i16 amount){if(owner.combat.spiritBreak<1){owner.combat.spirit=i16(u16(owner.combat.spirit)-u16(amount));if(owner.combat.spirit<1&&owner.combat.spiritBreak<1){events.system_sound(43);owner.combat.spirit=0;owner.combat.spiritBreak=1000;}}else owner.combat.spiritBreak=1000;};
const auto shared=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Shared,id,x,y,facing,layer,owner.launchParameters);};
double aimRadians=0;const auto atan=[](float y,float x){return std::atan2(double(y),double(x));};
const auto channel=[&](u32 shift){return p.pose.channel(shift);};const auto setChannel=[&](u32 shift,u8 value){p.pose.set_channel(shift,value);};
const auto owned=[&](i16 id,float x,float y,i8 facing,u8 layer){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters);};
const auto ownedVisual=[&](i16 id,float x,float y,i8 facing,u8 layer,u8 visual,u8 order){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters,order,visual?&owner.effectVisual:nullptr);};

  u16 uVar1;
  i16 sVar2;
  const PhantomPose* historyPose=nullptr;
  u32 uVar4;
  int particles;
  double angleRadians;
  float fVar7;
  float fVar8;
  u32 uVar9;
  u32 uVar10;
  u32 uVar11;
  int local_200;
  int local_a8;
  int local_9c;
  int local_90;
  int local_c;
  
  if ((i16(other.combat.opponentStop) == 0) &&
     ((i16(owner.combat.opponentStop) == 0 || (i16(p.contactDelay) != 0)))) {
    if (i16(p.animation.cursor.action) < 800) {
      state.phantomAttacking = 1;
      if (i8(owner.combat.spellPhase) == '\x01') {
        owner.bodyPower = 0.5f;
        consumeSpirit(5);
        state.phantomTime =
             owner.combat.spirit;
      }
      if (i8(owner.combat.spellPhase) == '\x02') {
        owner.bodyPower = 0.4000000059604645f;
        consumeSpirit(7);
        state.phantomTime =
             owner.combat.spirit;
      }
      if ((i16(state.phantomTime) < 1) ||
         (i16(owner.combat.spellTimer) < 1)) {
        owner.bodyPower = 1.0f;
        state.phantomTime = 0;
        p.action(0x32a);
      }
      else {
        p.contactDelay = 0;
        p.begin_pose(0.0f,0.0f,0.0f);
        p.pose.scale[0] = 1.0f;
        p.pose.scale[1] = 1.0f;
        setChannel(24,u8(0x80));
        setChannel(16,u8(0xff));
        setChannel(8,u8(0xff));
        setChannel(0,u8(0xff));
        p.pose.flags = 0x82;
        if (i16(owner.combat.freeze) == 0) {
          if(state.history.empty())return;historyPose=&state.history.front();
          if (i16(p.animation.cursor.action) != historyPose->action) {
            p.hits = historyPose->hits;
          }
          sVar2 = historyPose->index;
          if (i16(p.animation.cursor.index) != sVar2) {
            p.response = 0;
          }
          p.animation.set(historyPose->action,historyPose->index,0);
          p.motion.x = historyPose->x;
          p.motion.y = historyPose->y;
          p.motion.facing = (i8)historyPose->facing;
        }
      }
    }
    else if (i16(p.freeze) == 0) {
      switch(p.animation.cursor.action) {
      case 800:
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
          }
          if ((i16(p.animation.cursor.index) == 1) && (i16(p.animation.cursor.elapsed) == 0)) {
            
            owner.launchParameters[0] = (float)(int)i16(p.counter);
            if (double(p.parameters[0]) == double(1.0f)) {
              owned(0x323,(float)((i16(p.counter) * 0x17 + -0x4b) *
                                        (int)i8(p.motion.facing)) + double(p.motion.x),(double(p.motion.y) + double(215.0f)) -
                            (float)(i16(p.counter) * 0x13),u8(p.motion.facing),1);
            }
            if (double(p.parameters[0]) == double(2.0f)) {
              owned(0x325,(float)(i8(p.motion.facing) * 100) +
                                 double(p.motion.x),(double(p.motion.y) + double(200.0f)) -
                           (float)(i16(p.counter) * 0x19),u8(p.motion.facing),1);
            }
            if (double(p.parameters[0]) == double(3.0f)) {
              owned(0x327,(float)((i16(p.counter) * 0xf + -10) *
                                        (int)i8(p.motion.facing)) + double(p.motion.x),double(p.motion.y) + double(20.0f) +
                            (float)(i16(p.counter) * 0x14),u8(p.motion.facing),1);
            }
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            if (i16(p.counter) < 10) {
              p.frame(1);
              p.counter = i16(p.counter) + 1;
            }
            else {
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
        }
        break;
      case 0x321:
        if ((i16(owner.animation.cursor.action) == 0x192) &&
           (i16(p.counter) == 0)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 99) + double(owner.motion.x)
          ;
          p.motion.y = double(owner.motion.y) + double(66.0f);
        }
        if (i16(owner.animation.cursor.action) != 0x192) {
          p.counter = 1;
        }
        if (p.response == 6) {
          p.response = 0;
          p.hits = 1;
        }
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
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
        if ((1 < i16(p.animation.cursor.index)) && (i8(channel(24)) != '\0')) {
          setChannel(24,u8(i8(channel(24)) + -3));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x322:
        if ((i16(owner.animation.cursor.action) == 0x194) &&
           (i16(p.counter) == 0)) {
          p.motion.x =
               (float)(i8(p.motion.facing) * 0x53) +
               double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(114.0f);
        }
        if (i16(owner.animation.cursor.action) != 0x194) {
          p.counter = 1;
        }
        if (p.response == 6) {
          p.response = 0;
          p.hits = 1;
        }
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
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.1f);
        p.pose.scale[1] = double(p.pose.scale[1]) * double(0.85f);
        if ((1 < i16(p.animation.cursor.index)) && (i8(channel(24)) != '\0')) {
          setChannel(24,u8(i8(channel(24)) + -3));
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x323:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            events.system_sound(0x2a);
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 2.5f;
            p.pose.scale[1] = 2.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (owner.launchParameters[0] = (float)(int)i16(p.counter),
             i16(p.timer) != 1)) {
            owned(0x324,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          p.pose.scale[0] = double(p.pose.scale[0]) * double(0.95f);
          p.pose.scale[1] = double(p.pose.scale[1]) * double(0.95f);
          if ((i16(p.animation.cursor.index) == 2) && (10 < u8(channel(24)))) {
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
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
      case 0x324:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            uVar1 = i16(i32(double(1000.0f)-double(p.parameters[0])*double(60.0f)));
            p.timer = uVar1;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                         double(p.motion.y),(double(other.motion.x) -
                         double(p.motion.x)) * (float)(int)i8(p.motion.facing));
            uVar1 = i32(aimRadians/double(0.017453289f));
            p.angle = uVar1;
            if (i16(p.angle) < -0x14) {
              p.angle = 10;
            }
            if (0x14 < i16(p.angle)) {
              p.angle = 0xfff6;
            }
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer) * double(0.01f));
            p.pose.scale[0] = 1.5f;
            p.pose.scale[1] = 1.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
          }
          if ((((p.response == 1) || (p.response == 2)) ||
              (p.response == 4)) && (i16(p.animation.cursor.index) < 2)) {
            p.frame(2);
          }
          else {
            if (i16(p.timer) < 800) {
              p.timer = 800;
            }
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer) * double(0.01f));
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x325:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            events.system_sound(0x2a);
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 2.5f;
            p.pose.scale[1] = 2.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (owner.launchParameters[0] = (float)(int)i16(p.counter),
             i16(p.timer) != 1)) {
            owned(0x326,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          p.pose.scale[0] = double(p.pose.scale[0]) * double(0.95f);
          p.pose.scale[1] = double(p.pose.scale[1]) * double(0.95f);
          if ((i16(p.animation.cursor.index) == 2) && (10 < u8(channel(24)))) {
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
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
      case 0x326:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            uVar1 = i16(i32(double(1000.0f)-double(p.parameters[0])*double(60.0f)));
            p.timer = uVar1;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                         double(p.motion.y),(double(other.motion.x) -
                         double(p.motion.x)) * (float)(int)i8(p.motion.facing));
            uVar1 = i32(aimRadians/double(0.017453289f));
            p.angle = uVar1;
            if (i16(p.angle) < -0x41) {
              p.angle = 0xffc9;
            }
            if (-0x19 < i16(p.angle)) {
              p.angle = 0xfff1;
            }
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer) * double(0.01f));
            p.pose.scale[0] = 1.5f;
            p.pose.scale[1] = 1.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
          }
          if ((((p.response == 1) || (p.response == 2)) ||
              (p.response == 4)) && (i16(p.animation.cursor.index) < 2)) {
            p.frame(2);
          }
          else {
            if (i16(p.timer) < 800) {
              p.timer = 800;
            }
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer) * double(0.01f));
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
               ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f))))) {
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x327:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            events.system_sound(0x2a);
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 2.5f;
            p.pose.scale[1] = 2.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if (((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (owner.launchParameters[0] = (float)(int)i16(p.counter),
             i16(p.timer) != 1)) {
            owned(0x328,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
          }
          p.pose.scale[0] = double(p.pose.scale[0]) * double(0.95f);
          p.pose.scale[1] = double(p.pose.scale[1]) * double(0.95f);
          if ((i16(p.animation.cursor.index) == 2) && (10 < u8(channel(24)))) {
            setChannel(24,u8(i8(channel(24)) + -10));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
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
      case 0x328:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            uVar1 = i16(i32(double(1000.0f)-double(p.parameters[0])*double(60.0f)));
            p.timer = uVar1;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            aimRadians = atan((double(other.motion.y) + double(100.0f)) -
                         double(p.motion.y),(double(other.motion.x) -
                         double(p.motion.x)) * (float)(int)i8(p.motion.facing));
            uVar1 = i32(aimRadians/double(0.017453289f));
            p.angle = uVar1;
            if (i16(p.angle) < 0x19) {
              p.angle = 0x23;
            }
            if (0x41 < i16(p.angle)) {
              p.angle = 0x37;
            }
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer) * double(0.01f));
            p.pose.scale[0] = 1.5f;
            p.pose.scale[1] = 1.5f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
          }
          if ((((p.response == 1) || (p.response == 2)) ||
              (p.response == 4)) && (i16(p.animation.cursor.index) < 2)) {
            p.frame(2);
          }
          else {
            if (i16(p.timer) < 800) {
              p.timer = 800;
            }
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer) * double(0.01f));
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
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x329:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.800000011920929f;
          p.pose.scale[1] = 0.5f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[2] = -double(p.parameters[0]);
        }
        if ((i16(p.animation.cursor.index) == 1) && (8 < i16(owner.animation.cursor.index)))
        {
          p.frame(2);
        }
        else {
          if (i16(p.animation.cursor.index) == 2) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
            p.pose.scale[1] = double(p.pose.scale[1]) * double(0.9f);
            if (u8(channel(24)) < 10) {
              setChannel(24,u8(0));
            }
            else {
              setChannel(24,u8(i8(channel(24)) + -9));
            }
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
        }
        break;
      case 0x32a:
        state.phantomAttacking = 0;
        if ((i16(owner.animation.cursor.action) == 0x2c0) ||
           (i16(owner.animation.cursor.action) == 0x2c1)) {
          p.contactDelay = 1;
        }
        else {
          p.contactDelay = 0;
        }
        if (i16(state.phantomTime) < 1) {
          p.motion.facing = u8(owner.motion.facing);
          if (((i16(owner.animation.cursor.action) == 0x31c) &&
              (i16(owner.animation.cursor.index) == 0x11)) &&
             (i8(owner.phaseVariants[owner.combat.spellPhase]) == '\x02')) {
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.action(0x38f);
          }
          else if (((i16(owner.animation.cursor.action) == 0x31c) &&
                   (i16(owner.animation.cursor.index) == 0x11)) &&
                  (i8(owner.phaseVariants[owner.combat.spellPhase]) == '\0')) {
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.parameters[0] = 0.0f;
            p.action(0x389);
            if (state.difficulty == '\x03') {
              owner.launchParameters[0] = 1.0f;
              owned(0x389,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            }
          }
          else if ((i16(owner.animation.cursor.action) == 0x19c) &&
                  (i16(owner.animation.cursor.index) == 3)) {
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.action(0x344);
            events.sound(0xc);
          }
          else if ((i16(owner.animation.cursor.action) == 0x19e) &&
                  (i16(owner.animation.cursor.index) == 3)) {
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.action(0x34e);
            events.sound(0xc);
          }
          else if ((i16(owner.animation.cursor.action) == 0x1a0) &&
                  (i16(owner.animation.cursor.index) == 3)) {
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.action(0x34f);
            events.sound(0xc);
          }
          else {
            if ((i16(p.animation.cursor.index) == 0) &&
               ((i16(p.animation.cursor.elapsed) == 0 && (i16(p.counter) == 0)))) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              setChannel(24,u8(0xff));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
              p.counter = 1;
            }
            if (i8(p.motion.facing) == '\x01') {
              if ((i16(owner.animation.cursor.action) == 0x2c0) ||
                 (i16(owner.animation.cursor.action) == 0x2c1)) {
                local_90 = 1;
              }
              else {
                local_90 = 0;
              }
              p.motion.velocityX =
                   (((double(owner.motion.x) -
                     (float)(u8)((i16(owner.animation.cursor.action) != 0x2c2) * 'd')) -
                    double(p.motion.x)) + (float)(local_90 * 100)) * double(0.05f);
            }
            if (i8(p.motion.facing) == -1) {
              if ((i16(owner.animation.cursor.action) == 0x2c0) ||
                 (i16(owner.animation.cursor.action) == 0x2c1)) {
                local_9c = 1;
              }
              else {
                local_9c = 0;
              }
              p.motion.velocityX =
                   -(((((float)(u8)((i16(owner.animation.cursor.action) != 0x2c2) * 'd')
                       + double(owner.motion.x)) - double(p.motion.x))
                     - (float)(local_9c * 100)) * double(0.05f));
            }
            if ((i16(owner.animation.cursor.action) == 0x2c0) ||
               (i16(owner.animation.cursor.action) == 0x2c1)) {
              local_a8 = 1;
            }
            else {
              local_a8 = 0;
            }
            p.motion.velocityY =
                 ((((double(owner.motion.y) + double(200.0f)) -
                   (float)(u8)((i16(owner.animation.cursor.action) == 0x2c2) * 'd')) -
                  double(p.motion.y)) - (float)(local_a8 * 100)) * double(0.05f);
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            if (i16(p.contactDelay) == 0) {
              uVar11 = 1;
              uVar4 = (u32)u8(p.motion.facing);
              fVar8 = double(p.motion.y);
              particles = random.range(0,0x14);
              fVar7 = (float)particles + double(float(double(fVar8)-10.0));
              fVar8 = double(p.motion.x);
              particles = random.range(0,0x14);
              owned(0x32b,(float)particles + double(float(double(fVar8)-10.0)),fVar7,uVar4,uVar11);
            }
            if (i16(p.contactDelay) != 0) {
              uVar9 = 1;
              uVar10 = 0;
              uVar11 = 1;
              uVar4 = (u32)u8(p.motion.facing);
              fVar8 = double(p.motion.y);
              particles = random.range(0,0x14);
              fVar7 = (float)particles + double(float(double(fVar8)-10.0));
              fVar8 = double(p.motion.x);
              particles = random.range(0,0x14);
              ownedVisual(0x32b,(float)particles + double(float(double(fVar8)-10.0)),fVar7,uVar4,uVar11,uVar10,uVar9);
            }
            if (i16(p.animation.cursor.elapsed) == 0) {
              uVar1 = random.range(0,0x14);
              p.timer = uVar1;
              p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
              p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.frame(0);
            }
          }
        }
        else {
          p.destroy();
          ownedVisual(0,double(owner.motion.x),double(owner.motion.y),u8(p.motion.facing),0,0,1);
        }
        break;
      case 0x32b:
        p.motion.facing = u8(owner.motion.facing);
        if ((i16(owner.animation.cursor.action) == 0x2c0) ||
           (i16(owner.animation.cursor.action) == 0x2c1)) {
          p.contactDelay = 1;
        }
        else {
          p.contactDelay = 0;
        }
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
      default:
        p.advance();
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
          state.summonWait = 0;
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
      case 0x334:
        if (i16(owner.animation.cursor.action) == 0x134) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x335:
        if (i16(owner.animation.cursor.action) == 0x1f8) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x336:
        if ((i16(owner.animation.cursor.action) == 0x1f6) ||
           (i16(owner.animation.cursor.action) == 0x201)) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x337:
        if (i16(owner.animation.cursor.action) == 0x1f7) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x338:
        if ((i16(owner.animation.cursor.action) == 500) ||
           (i16(owner.animation.cursor.action) == 0x1f5)) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x339:
        if (i16(owner.animation.cursor.action) == 0x135) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33a:
        if (i16(owner.animation.cursor.action) == 0x136) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33b:
        if (i16(owner.animation.cursor.action) == 0x1f5) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x33c:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(30.0f,60.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if ((i16(p.animation.cursor.index) < 0xe) && (5 < i16(p.animation.cursor.index))) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.004f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.004f);
        }
        if ((i16(p.animation.cursor.index) < 0xe) && (3 < i16(owner.animation.cursor.index)))
        {
          p.frame(0xe);
          p.begin_pose(0.0f,0.0f,0.0f);
        }
        else {
          if (0x1b < i16(p.animation.cursor.index)) {
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
          }
          if (i16(owner.animation.cursor.action) == 0x2be) {
            p.motion.x = double(owner.motion.x);
            p.motion.y = double(owner.motion.y);
            if (i16(owner.combat.freeze) == 0) {
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.destroy();
              }
              if (((i16(p.animation.cursor.index) == 0xe) && (i16(p.animation.cursor.elapsed) == 0)) &&
                 (i16(owner.animation.cursor.index) < 4)) {
                p.frame(6);
              }
              if ((i16(p.animation.cursor.index) == 0x1c) && (i16(p.animation.cursor.elapsed) == 0)) {
                p.begin_pose(55.0f,2.0f,0.0f);
              }
            }
          }
          else {
            p.destroy();
          }
        }
        break;
      case 0x33d:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(40.0f,75.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if ((i16(p.animation.cursor.index) < 0xe) && (5 < i16(p.animation.cursor.index))) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.004f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.004f);
        }
        if ((i16(p.animation.cursor.index) < 0xe) && (3 < i16(owner.animation.cursor.index)))
        {
          p.frame(0xe);
          p.begin_pose(0.0f,0.0f,0.0f);
        }
        else if (i16(owner.animation.cursor.action) == 0x2bf) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
          if (i16(owner.combat.freeze) == 0) {
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
            if (((i16(p.animation.cursor.index) == 0xe) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(owner.animation.cursor.index) < 4)) {
              p.frame(6);
            }
            if ((i16(p.animation.cursor.index) == 0x1c) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.begin_pose(55.0f,2.0f,0.0f);
            }
          }
        }
        else {
          p.destroy();
        }
        break;
      case 0x340:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (6 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(3.0f,137.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[0] = (float)(i8(p.motion.facing) * -0x1e);
            p.pose.rotation[1] = 60.0f;
            p.hits = 999;
            p.counter = 0;
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(10.0f);
          if (p.response == 5) {
            events.system_sound(0x25);
            owned(0x347,double(p.attackPower),double(p.attackModifier),u8(p.motion.facing),1);
            owned(0x347,double(p.attackPower),double(p.attackModifier),u8(p.motion.facing),1);
            owned(0x347,double(p.attackPower),double(p.attackModifier),u8(p.motion.facing),1);
            p.response = 0;
          }
          if (p.response != 0) {
            p.response = 0;
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if (((i16(p.animation.cursor.index) == 7) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) < 0x1e)) {
            p.counter = i16(p.counter) + 1;
            p.frame(6);
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
        }
        break;
      case 0x341:
        if (i16(owner.animation.cursor.action) == 0x186) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x342:
        state.phantomAttacking = 1;
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            if (i16(p.counter) == 0) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              setChannel(24,u8(0xff));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
              p.counter = 1;
              p.angle = 0;
              p.motion.velocityX = 7.0f;
              p.motion.velocityY = 8.0f;
            }
            owner.launchParameters[0] = 1000.0f;
            owner.launchParameters[1] = -45.0f;
            owned(0x343,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            consumeSpirit(0x14);
            events.system_sound(0x2a);
          }
          p.motion.velocityY = double(p.motion.velocityY) - double(0.4f);
          if (double(p.motion.velocityY) < -double(0.5f) != (double(p.motion.velocityY) == -double(0.5f))) {
            p.motion.velocityY = -0.5f;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          uVar11 = 1;
          uVar4 = (u32)u8(p.motion.facing);
          fVar8 = double(p.motion.y);
          particles = random.range(0,0x14);
          fVar7 = (float)particles + double(float(double(fVar8)-10.0));
          fVar8 = double(p.motion.x);
          particles = random.range(0,0x14);
          owned(0x32b,(float)particles + double(float(double(fVar8)-10.0)),fVar7,uVar4,uVar11);
          if (i16(p.animation.cursor.elapsed) == 0) {
            uVar1 = random.range(0,0x14);
            p.timer = uVar1;
            p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
            p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.frame(0);
            p.angle = i16(p.angle) + 1;
            if (0xf < i16(p.angle)) {
              p.counter = 0;
              p.timer = 0;
              p.angle = 0;
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
              p.action(0x32a);
            }
          }
        }
        else {
          p.counter = 0;
          p.timer = 0;
          p.angle = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.action(0x32a);
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
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
          }
          if ((((p.response == 1) || (p.response == 2)) ||
              ((p.response == 4 || (p.response == 6)))) &&
             (i16(p.animation.cursor.index) < 2)) {
            p.frame(2);
          }
          else {
            p.polar((float)(int)i16(p.angle),(float)(int)i16(p.timer) * double(0.01f));
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
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          p.destroy();
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
          owned(0x3e4,double(p.motion.x),double(p.motion.y),1,1);
        }
        break;
      case 0x344:
        state.phantomAttacking = 1;
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 1;
            p.timer = 0;
            p.angle = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            owner.launchParameters[0] = 1000.0f;
            owner.launchParameters[1] =
                 (float)(i16(p.angle) * 3 + -0x1e);
            consumeSpirit(0x14);
            owned(0x343,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            events.system_sound(0x2a);
            p.angle = i16(p.angle) + 1;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          uVar11 = 1;
          uVar4 = (u32)u8(p.motion.facing);
          fVar8 = double(p.motion.y);
          particles = random.range(0,0x14);
          fVar7 = (float)particles + double(float(double(fVar8)-10.0));
          fVar8 = double(p.motion.x);
          particles = random.range(0,0x14);
          owned(0x32b,(float)particles + double(float(double(fVar8)-10.0)),fVar7,uVar4,uVar11);
          if (i16(p.animation.cursor.elapsed) == 0) {
            uVar1 = random.range(0,0x14);
            p.timer = uVar1;
            p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
            p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
          }
          sVar2 = p.advance();
          if ((sVar2 == i16(p.animation.cursor.frameCount)) &&
             (p.frame(1), 5 < i16(p.angle))) {
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.action(0x32a);
          }
        }
        else {
          p.counter = 0;
          p.timer = 0;
          p.angle = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.action(0x32a);
        }
        break;
      case 0x345:
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
          p.timer = state.phantomTime;
        }
        fVar8 = (float)(int)i16(state.phantomTime) /
                (float)(int)i16(p.timer);
        p.pose.scale[0] = fVar8 + fVar8;
        fVar8 = (float)(int)i16(state.phantomTime) /
                (float)(int)i16(p.timer);
        p.pose.scale[1] = fVar8 + fVar8;
        if ((i16(p.animation.cursor.index) == 1) && (0 < i16(state.phantomTime)))
        {
          if (u8(channel(24)) < 0xf5) {
            setChannel(24,u8(i8(channel(24)) + '\n'));
          }
          else {
            setChannel(24,u8(0xff));
          }
        }
        if (i16(state.phantomTime) < 1) {
          p.destroy();
        }
        else {
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.frame(1);
          }
        }
        break;
      case 0x346:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.2999999523162842f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
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
            uVar1 = i16(i32(p.parameters[0]));
            p.timer = uVar1;
            uVar1 = i16(i32(p.parameters[1]));
            p.angle = uVar1;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            particles = random.range(0,10);
            fVar8 = (float)(particles + 10);
            particles = random.range(0,0x3c);
            p.polar((float)(0x1e - particles),fVar8);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
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
          p.frame(2);
        }
        break;
      case 0x348:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(50.0f,20.0f,0.0f);
          p.pose.scale[0] = 1.0099999904632568f;
          p.pose.scale[1] = 1.0099999904632568f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.001f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.0015f);
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
      case 0x349:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(55.0f,2.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.100000023841858f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.01f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.02f);
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
      case 0x34a:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[2] = double(p.parameters[0]);
          p.polar(double(p.parameters[0]),30.0f);
        }
        if (i16(p.animation.cursor.index) == 1) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
          p.pose.scale[1] = double(p.pose.scale[1]) - double(0.19f);
        }
        if (i16(p.animation.cursor.index) == 2) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.2f);
        }
        p.motion.x =
             (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
             double(p.motion.x);
        p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
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
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
          p.response = 0;
          p.hits = 1;
          uVar1 = random.range(0,100);
          p.counter = uVar1;
          p.frame(4);
          if (i16(p.counter) < 0x4b) {
            p.frame(3);
          }
          if (i16(p.counter) < 0x32) {
            p.frame(2);
          }
          if (i16(p.counter) < 0x19) {
            p.frame(1);
          }
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        if (i16(p.animation.cursor.elapsed) == 0) {
          p.destroy();
        }
        break;
      case 0x34c:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 0.800000011920929f;
          p.pose.scale[1] = 4.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[2] = 90.0f;
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x34d:
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) ||
           (6 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(3.0f,137.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.pose.rotation[0] = (float)(i8(p.motion.facing) * -0x1e);
            p.pose.rotation[1] = 60.0f;
            p.hits = 999;
            p.counter = 0;
          }
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(10.0f);
          if (p.response == 5) {
            events.system_sound(0x25);
            owned(0x347,double(p.attackPower),double(p.attackModifier),u8(p.motion.facing),1);
            owned(0x347,double(p.attackPower),double(p.attackModifier),u8(p.motion.facing),1);
            owned(0x347,double(p.attackPower),double(p.attackModifier),u8(p.motion.facing),1);
            p.response = 0;
          }
          if (p.response != 0) {
            p.response = 0;
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.destroy();
          }
          if (((i16(p.animation.cursor.index) == 7) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) < 0x28)) {
            p.counter = i16(p.counter) + 1;
            p.frame(6);
          }
        }
        else {
          p.destroy();
          if (0x19 < i16(owner.knockdownTime)) {
            owner.knockdownTime = 0;
            owned(0x3e3,double(p.motion.x),double(p.motion.y),1,1);
          }
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
          owned(0x3e4,double(p.motion.x),double(p.motion.y) + double(100.0f),1,1)
          ;
        }
        break;
      case 0x34e:
        state.phantomAttacking = 1;
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 1;
            p.timer = 0;
            p.angle = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            owner.launchParameters[0] = 1000.0f;
            owner.launchParameters[1] =
                 (float)(i16(p.angle) * 3 + 10);
            consumeSpirit(0x14);
            owned(0x343,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            events.system_sound(0x2a);
            p.angle = i16(p.angle) + 1;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          uVar11 = 1;
          uVar4 = (u32)u8(p.motion.facing);
          fVar8 = double(p.motion.y);
          particles = random.range(0,0x14);
          fVar7 = (float)particles + double(float(double(fVar8)-10.0));
          fVar8 = double(p.motion.x);
          particles = random.range(0,0x14);
          owned(0x32b,(float)particles + double(float(double(fVar8)-10.0)),fVar7,uVar4,uVar11);
          if (i16(p.animation.cursor.elapsed) == 0) {
            uVar1 = random.range(0,0x14);
            p.timer = uVar1;
            p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
            p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
          }
          sVar2 = p.advance();
          if ((sVar2 == i16(p.animation.cursor.frameCount)) &&
             (p.frame(1), 5 < i16(p.angle))) {
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.action(0x32a);
          }
        }
        else {
          p.counter = 0;
          p.timer = 0;
          p.angle = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.action(0x32a);
        }
        break;
      case 0x34f:
        state.phantomAttacking = 1;
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 1;
            p.timer = 0;
            p.angle = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            owner.launchParameters[0] = 1000.0f;
            owner.launchParameters[1] =
                 (float)(i16(p.angle) * 3 + -0x32);
            consumeSpirit(0x14);
            owned(0x343,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            events.system_sound(0x2a);
            p.angle = i16(p.angle) + 1;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          uVar11 = 1;
          uVar4 = (u32)u8(p.motion.facing);
          fVar8 = double(p.motion.y);
          particles = random.range(0,0x14);
          fVar7 = (float)particles + double(float(double(fVar8)-10.0));
          fVar8 = double(p.motion.x);
          particles = random.range(0,0x14);
          owned(0x32b,(float)particles + double(float(double(fVar8)-10.0)),fVar7,uVar4,uVar11);
          if (i16(p.animation.cursor.elapsed) == 0) {
            uVar1 = random.range(0,0x14);
            p.timer = uVar1;
            p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
            p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
          }
          sVar2 = p.advance();
          if ((sVar2 == i16(p.animation.cursor.frameCount)) &&
             (p.frame(1), 5 < i16(p.angle))) {
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.action(0x32a);
          }
        }
        else {
          p.counter = 0;
          p.timer = 0;
          p.angle = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.action(0x32a);
        }
        break;
      case 0x350:
        state.phantomAttacking = 1;
        if ((i16(owner.animation.cursor.action) < 0x32) ||
           (0x95 < i16(owner.animation.cursor.action))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 1;
            p.timer = 0;
            p.angle = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            owner.launchParameters[0] = 1000.0f;
            owner.launchParameters[1] =
                 (float)(i16(p.angle) * 3 + -0x1e);
            consumeSpirit(0x14);
            owned(0x343,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            events.system_sound(0x2a);
            p.angle = i16(p.angle) + 1;
          }
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          uVar11 = 1;
          uVar4 = (u32)u8(p.motion.facing);
          fVar8 = double(p.motion.y);
          particles = random.range(0,0x14);
          fVar7 = (float)particles + double(float(double(fVar8)-10.0));
          fVar8 = double(p.motion.x);
          particles = random.range(0,0x14);
          owned(0x32b,(float)particles + double(float(double(fVar8)-10.0)),fVar7,uVar4,uVar11);
          if (i16(p.animation.cursor.elapsed) == 0) {
            uVar1 = random.range(0,0x14);
            p.timer = uVar1;
            p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
            p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
          }
          sVar2 = p.advance();
          if ((sVar2 == i16(p.animation.cursor.frameCount)) &&
             (p.frame(1), 8 < i16(p.angle))) {
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.action(0x32a);
          }
        }
        else {
          p.counter = 0;
          p.timer = 0;
          p.angle = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.action(0x32a);
        }
        break;
      case 0x351:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.timer = 0x14;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((p.response != 0) || (0 < i16(p.counter))) {
            if (p.response == 6) {
              p.response = 0;
              p.hits = i16(p.hits) + 1;
            }
            else {
              if (u8(channel(24)) < 0x15) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x14));
            }
            p.response = 1;
          }
          p.timer = i16(p.timer) + -1;
          if (i16(p.timer) < 5) {
            p.timer = 5;
          }
          p.polar(20.0f,(float)(int)i16(p.timer));
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
              p.frame(1);
              p.counter = i16(p.counter) + 1;
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
        }
        break;
      case 0x352:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.timer = 0x14;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((p.response != 0) || (0 < i16(p.counter))) {
            if (p.response == 6) {
              p.response = 0;
              p.hits = i16(p.hits) + 1;
            }
            else {
              if (u8(channel(24)) < 0x15) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x14));
            }
            p.response = 1;
          }
          p.timer = i16(p.timer) + -1;
          if (i16(p.timer) < 5) {
            p.timer = 5;
          }
          p.polar(-30.0f,(float)(int)i16(p.timer));
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
              p.frame(1);
              p.counter = i16(p.counter) + 1;
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
        }
        break;
      case 0x353:
        if (p.response == 5) {
          p.destroy();
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) ||
                (1 < i16(p.animation.cursor.index))) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.timer = 0x14;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
          }
          if ((p.response != 0) || (0 < i16(p.counter))) {
            if (p.response == 6) {
              p.response = 0;
              p.hits = i16(p.hits) + 1;
            }
            else {
              if (u8(channel(24)) < 0x15) {
                p.destroy();
                return;
              }
              setChannel(24,u8(i8(channel(24)) + -0x14));
            }
            p.response = 1;
          }
          p.timer = i16(p.timer) + -1;
          if (i16(p.timer) < 5) {
            p.timer = 5;
          }
          p.polar(0.0f,(float)(int)i16(p.timer));
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
              p.frame(1);
              p.counter = i16(p.counter) + 1;
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
        }
        break;
      case 900:
        state.phantomAttacking = 1;
        if (i16(owner.combat.spellTimer) < 1) {
          p.action(0x32a);
          state.phantomAttacking = 0;
          if (i16(p.cycle) == 1) {
            p.destroy();
          }
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) || (state.difficulty != '\0')) {
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
            p.angle = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if (((i16(p.animation.cursor.index) < 2) || (9 < i16(p.animation.cursor.index))) ||
             (angleRadians = (double)std::abs(float(double(other.motion.x) - double(p.motion.x))),
             (double)double(320.0f) <= angleRadians)) {
            if ((0xf < i16(p.animation.cursor.index)) &&
               (p.motion.velocityX = double(p.motion.velocityX) - double(0.25f),
               double(p.motion.velocityX) < double(0.0f))) {
              p.motion.velocityX = 0.0f;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            if (double(p.motion.x) < double(30.0f)) {
              p.motion.velocityX = 0.0f;
              p.motion.x = 30.0f;
            }
            if (double(1250.0f) < double(p.motion.x)) {
              p.motion.velocityX = 0.0f;
              p.motion.x = 1250.0f;
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.action(0x387);
            }
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = 12.5f;
            }
            if (((i16(p.animation.cursor.index) == 10) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.counter) < 1)) {
              p.counter = i16(p.counter) + 1;
              p.frame(2);
            }
            if ((i16(p.animation.cursor.index) == 0xf) && (i16(p.animation.cursor.elapsed) == 0)) {
              shared(0x80,(float)(i8(p.motion.facing) * 100) +
                                double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              owned(0x336,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              events.sound(2);
            }
            if ((i16(p.animation.cursor.index) == 0x10) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = 17.5f;
            }
          }
          else {
            p.frame(10);
          }
        }
        else {
          p.action(0x394);
          p.counter = 0;
          p.timer = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
        }
        break;
      case 0x385:
        state.phantomAttacking = 1;
        if (i16(owner.combat.spellTimer) < 1) {
          p.action(0x32a);
          state.phantomAttacking = 0;
          if (i16(p.cycle) == 1) {
            p.destroy();
          }
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) || (state.difficulty != '\0')) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0x80));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 0;
            p.timer = 0;
            p.angle = 0;
          }
          if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.response = 0;
            p.hits = 1;
            p.motion.velocityX = 10.0f;
            p.motion.velocityY = 0.0f;
            owned(0x33b,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            events.sound(1);
          }
          if ((i16(p.animation.cursor.index) == 0xd) && (i16(p.animation.cursor.elapsed) == 0)) {
            owned(0x338,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            events.sound(1);
          }
          if ((double(0.0f) < double(p.motion.velocityX)) &&
             (p.motion.velocityX = double(p.motion.velocityX) - double(0.5f),
             double(p.motion.velocityX) < double(0.0f))) {
            p.motion.velocityX = 0.0f;
          }
          if (((double(p.motion.y) <= double(0.0f)) || (0x17 < i16(p.animation.cursor.index))) ||
             (p.motion.velocityY = double(p.motion.velocityY) - double(0.65f),
             fVar8 = double(p.motion.y) + double(p.motion.velocityY),
             fVar8 < double(0.0f) == (fVar8 == double(0.0f)))) {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            if (double(p.motion.x) < double(30.0f)) {
              p.motion.velocityX = 0.0f;
              p.motion.x = 30.0f;
            }
            if (double(1250.0f) < double(p.motion.x)) {
              p.motion.velocityX = 0.0f;
              p.motion.x = 1250.0f;
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.action(0x387);
            }
            if (((i16(p.animation.cursor.index) == 0xd) && (i16(p.animation.cursor.elapsed) == 0)) &&
               (i16(p.counter) < 1)) {
              p.counter = i16(p.counter) + 1;
              p.frame(2);
            }
            if ((i16(p.animation.cursor.index) == 0xf) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.motion.velocityX = 12.5f;
              p.motion.velocityY = 15.0f;
              p.response = 0;
              p.hits = 1;
            }
            if ((i16(p.animation.cursor.index) == 0x18) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(0x16);
            }
          }
          else {
            p.frame(0x18);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.motion.y = 0.0f;
          }
        }
        else {
          p.action(0x394);
          p.counter = 0;
          p.timer = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
        }
        break;
      case 0x386:
        state.phantomAttacking = 1;
        if (i16(owner.combat.spellTimer) < 1) {
          p.action(0x32a);
          state.phantomAttacking = 0;
          if (i16(p.cycle) == 1) {
            p.destroy();
          }
        }
        else if (((i16(owner.animation.cursor.action) < 0x32) ||
                 (0x95 < i16(owner.animation.cursor.action))) || (state.difficulty != '\0')) {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0x80));
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
            p.response = 0;
            p.hits = 1;
            p.motion.velocityX = 10.0f;
            p.motion.velocityY = 0.0f;
          }
          if (((double(p.motion.y) <= double(0.0f)) || (0x19 < i16(p.animation.cursor.index))) ||
             (p.motion.velocityY = double(p.motion.velocityY) - double(0.8f),
             fVar8 = double(p.motion.y) + double(p.motion.velocityY),
             fVar8 < double(0.0f) == (fVar8 == double(0.0f)))) {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            if (double(p.motion.x) < double(30.0f)) {
              p.motion.velocityX = 0.0f;
              p.motion.x = 30.0f;
            }
            if (double(1250.0f) < double(p.motion.x)) {
              p.motion.velocityX = 0.0f;
              p.motion.x = 1250.0f;
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.action(0x387);
            }
            if ((i16(p.animation.cursor.index) == 3) && (i16(p.animation.cursor.elapsed) == 0)) {
              shared(0x7f,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              particles = random.range(0,6);
              p.motion.velocityX = (float)particles + double(7.5f);
              p.motion.velocityY = 16.0f;
              p.response = 0;
              p.hits = 1;
            }
            if ((i16(p.animation.cursor.index) == 0x12) && (i16(p.animation.cursor.elapsed) == 0)) {
              owned(0x335,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
              events.sound(4);
            }
            if ((i16(p.animation.cursor.index) == 0x1a) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(0x18);
            }
          }
          else {
            p.frame(0x1a);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
            p.motion.y = 0.0f;
          }
        }
        else {
          p.action(0x394);
          p.counter = 0;
          p.timer = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
        }
        break;
      case 0x387:
        state.phantomAttacking = 1;
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) || (state.difficulty != '\0')) {
          if (i16(owner.combat.spellTimer) < 1) {
            p.action(0x32a);
            state.phantomAttacking = 0;
            if (i16(p.cycle) == 1) {
              p.destroy();
            }
          }
          else {
            if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              setChannel(24,u8(0x80));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
              sVar2 = random.range(0,0x1e);
              p.counter = sVar2 + 0x5a + state.difficulty * -0x1e;
              p.timer = 0;
              p.angle = 0;
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
            }
            p.counter = i16(p.counter) + -1;
            if (i16(p.counter) < 1) {
              particles = random.range(0,100);
              if (particles < 0x19) {
                p.action(0x388);
              }
              angleRadians = (double)std::abs(float(double(other.motion.x) - double(p.motion.x)));
              if ((angleRadians < (double)double(320.0f)) && (particles = random.range(0,100), particles < 0x32)) {
                p.action(0x385);
                return;
              }
              angleRadians = (double)std::abs(float(double(other.motion.x) - double(p.motion.x)));
              if (((double)double(320.0f) < angleRadians) && (particles = random.range(0,100), particles < 0x32)) {
                particles = random.range(0,100);
                if (particles < 0x32) {
                  p.action(900);
                  return;
                }
                p.action(0x386);
                return;
              }
              uVar1 = random.range(0,100);
              p.timer = uVar1;
              if (i16(p.timer) < 0x19) {
                p.action(900);
                return;
              }
              if (i16(p.timer) < 0x32) {
                p.action(0x385);
                return;
              }
              if (i16(p.timer) < 0x4b) {
                p.action(0x386);
                return;
              }
              if (i16(p.timer) < 999) {
                p.action(0x388);
                return;
              }
            }
            if (double(other.motion.x) <=
                double(p.motion.x)) {
              p.motion.facing = 0xff;
            }
            else {
              p.motion.facing = 1;
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.frame(1);
            }
          }
        }
        else {
          p.action(0x394);
          p.counter = 0;
          p.timer = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
        }
        break;
      case 0x388:
        state.phantomAttacking = 1;
        if (((i16(owner.animation.cursor.action) < 0x32) ||
            (0x95 < i16(owner.animation.cursor.action))) || (state.difficulty != '\0')) {
          if (i16(owner.combat.spellTimer) < 1) {
            p.action(0x32a);
            state.phantomAttacking = 0;
            if (i16(p.cycle) == 1) {
              p.destroy();
            }
          }
          else {
            if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.begin_pose(0.0f,0.0f,0.0f);
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
              setChannel(24,u8(0x80));
              setChannel(16,u8(0xff));
              setChannel(8,u8(0xff));
              setChannel(0,u8(0xff));
              sVar2 = random.range(0,0x78);
              p.counter = sVar2 + -0x3c;
              p.angle = 0;
              p.motion.velocityX = 0.0f;
              p.motion.velocityY = 0.0f;
            }
            if (i16(p.counter) < 0) {
              p.counter = i16(p.counter) + 1;
              p.motion.velocityX = 3.0f;
              if (double(p.motion.x) < double(30.0f)) {
                p.motion.velocityX = 0.0f;
                p.motion.x = 30.0f;
              }
              if (double(1250.0f) < double(p.motion.x)) {
                p.motion.velocityX = 0.0f;
                p.motion.x = 1250.0f;
              }
            }
            if (0 < i16(p.counter)) {
              p.counter = i16(p.counter) + -1;
              p.motion.velocityX = -3.0f;
              if (double(p.motion.x) < double(30.0f)) {
                p.motion.velocityX = 0.0f;
                p.motion.x = 30.0f;
              }
              if (double(1250.0f) < double(p.motion.x)) {
                p.motion.velocityX = 0.0f;
                p.motion.x = 1250.0f;
              }
            }
            if (i16(p.counter) == 0) {
              if (double(other.motion.x) <=
                  double(p.motion.x)) {
                p.motion.facing = 0xff;
              }
              else {
                p.motion.facing = 1;
              }
              p.action(0x387);
            }
            else {
              p.motion.x =
                   (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                   double(p.motion.x);
              p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
              sVar2 = p.advance();
              if (sVar2 == i16(p.animation.cursor.frameCount)) {
                p.frame(1);
              }
            }
          }
        }
        else {
          p.action(0x394);
          p.counter = 0;
          p.timer = 0;
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
        }
        break;
      case 0x389:
        state.phantomAttacking = 1;
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0x80));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.motion.velocityX = -3.0f;
          p.motion.velocityY = 0.0f;
          p.counter = 0;
          p.timer = 0;
          p.angle = 0;
          uVar1 = i16(i32(p.parameters[0]));
          p.cycle = uVar1;
        }
        if (i16(owner.combat.spellTimer) < 1) {
          p.action(0x32a);
          state.phantomAttacking = 0;
          if (i16(p.cycle) == 1) {
            p.destroy();
          }
        }
        else if (((double(p.motion.y) <= double(0.0f)) || (6 < i16(p.animation.cursor.index))) ||
                (p.motion.velocityY = double(p.motion.velocityY) - double(0.6f),
                fVar8 = double(p.motion.y) + double(p.motion.velocityY),
                fVar8 < double(0.0f) == (fVar8 == double(0.0f)))) {
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          if (double(p.motion.x) < double(30.0f)) {
            p.motion.velocityX = 0.0f;
            p.motion.x = 30.0f;
          }
          if (double(1250.0f) < double(p.motion.x)) {
            p.motion.velocityX = 0.0f;
            p.motion.x = 1250.0f;
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.action(0x387);
          }
          if ((i16(p.animation.cursor.index) == 7) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.frame(5);
          }
        }
        else {
          p.frame(7);
          p.motion.velocityX = 0.0f;
          p.motion.velocityY = 0.0f;
          p.motion.y = 0.0f;
        }
        break;
      case 0x38a:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(40.0f,75.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if ((i16(p.animation.cursor.index) < 0xe) && (5 < i16(p.animation.cursor.index))) {
          p.pose.scale[0] = double(p.pose.scale[0]) + double(0.004f);
          p.pose.scale[1] = double(p.pose.scale[1]) + double(0.004f);
        }
        if ((i16(p.animation.cursor.index) < 0xe) && (3 < i16(owner.animation.cursor.index)))
        {
          p.frame(0xe);
          p.begin_pose(0.0f,0.0f,0.0f);
        }
        else {
          if (699 < i16(owner.animation.cursor.action)) {
            p.motion.x = double(owner.motion.x);
            p.motion.y = double(owner.motion.y);
          }
          if (5 < i16(p.animation.cursor.index)) {
            if (u8(channel(24)) < 0x15) {
              p.destroy();
              return;
            }
            setChannel(24,u8(i8(channel(24)) + -0x14));
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.frame(6);
          }
        }
        break;
      case 0x38b:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(40.0f,75.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if (i16(owner.animation.cursor.action) < 0x2bd) {
          p.destroy();
        }
        else {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
          if ((i16(owner.combat.freeze) == 0) &&
             (sVar2 = p.advance(), sVar2 == i16(p.animation.cursor.frameCount))) {
            p.destroy();
          }
        }
        break;
      case 0x38c:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(40.0f,75.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
        }
        if (i16(owner.animation.cursor.action) < 0x2bd) {
          p.destroy();
        }
        else {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y);
          if ((i16(owner.combat.freeze) == 0) &&
             (sVar2 = p.advance(), sVar2 == i16(p.animation.cursor.frameCount))) {
            p.destroy();
          }
        }
        break;
      case 0x38d:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 2.5f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x38e:
        if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
          p.begin_pose(0.0f,0.0f,0.0f);
          p.pose.scale[0] = 1.0f;
          p.pose.scale[1] = 1.0f;
          setChannel(24,u8(0xff));
          setChannel(16,u8(0xff));
          setChannel(8,u8(0xff));
          setChannel(0,u8(0xff));
          p.pose.projection[2] = 0x4e2;
          p.pose.rotation[0] = 0.0f;
          p.pose.rotation[1] = 0.0f;
          p.pose.rotation[2] = 0.0f;
        }
        p.pose.scale[0] = double(p.pose.scale[0]) + double(0.025f);
        p.pose.scale[1] = double(p.pose.scale[1]) + double(0.025f);
        if (i16(owner.animation.cursor.action) == 0x2e5) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
          p.pose.rotation[2] = -45.0f;
        }
        if (i16(owner.animation.cursor.action) == 0x2e6) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(100.0f);
          p.pose.rotation[0] = -200.0f;
          p.pose.rotation[2] = 0.0f;
        }
        if (i16(owner.animation.cursor.action) == 0x2e7) {
          p.motion.x = double(owner.motion.x);
          p.motion.y = double(owner.motion.y) + double(50.0f);
          p.pose.rotation[0] = -100.0f;
          p.pose.rotation[2] = 0.0f;
        }
        sVar2 = p.advance();
        if (sVar2 == i16(p.animation.cursor.frameCount)) {
          p.destroy();
        }
        break;
      case 0x38f:
        state.phantomAttacking = 1;
        if (i16(owner.combat.spellTimer) < 1) {
          p.action(0x32a);
          state.phantomAttacking = 0;
        }
        else {
          p.motion.facing = u8(owner.motion.facing);
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
            p.timer = 0;
            p.angle = 0;
          }
          if (((i16(owner.animation.cursor.action) == 0x2f0) &&
              (i16(owner.animation.cursor.index) == 5)) &&
             ((0 < i16(other.combat.health) &&
              (p.angle = i16(p.angle) + -1,
              i16(p.angle) < 1)))) {
            events.sound(0);
            owned(0x390,double(p.motion.x),double(p.motion.y),u8(p.motion.facing),1);
            p.angle = state.difficulty * -2 + 0xe;
          }
          if (i8(p.motion.facing) == '\x01') {
            p.motion.velocityX =
                 (double(owner.motion.x) - double(p.motion.x)) * double(0.05f);
          }
          if (i8(p.motion.facing) == -1) {
            p.motion.velocityX =
                 -((double(owner.motion.x) - double(p.motion.x)) * double(0.05f)
                  );
          }
          if ((i16(owner.animation.cursor.action) == 0x2c0) ||
             (i16(owner.animation.cursor.action) == 0x2c1)) {
            local_200 = 1;
          }
          else {
            local_200 = 0;
          }
          p.motion.velocityY =
               (((float)(int)((u32)(i16(owner.animation.cursor.action) == 0x2c2) * -100 +
                             0x17c) - double(p.motion.y)) - (float)(local_200 * 100)) * double(0.05f)
          ;
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
          uVar11 = 1;
          uVar4 = (u32)u8(p.motion.facing);
          fVar8 = double(p.motion.y);
          particles = random.range(0,0x14);
          fVar7 = (float)particles + double(float(double(fVar8)-10.0));
          fVar8 = double(p.motion.x);
          particles = random.range(0,0x14);
          owned(0x32b,(float)particles + double(float(double(fVar8)-10.0)),fVar7,uVar4,uVar11);
          if (i16(p.animation.cursor.elapsed) == 0) {
            uVar1 = random.range(0,0x14);
            p.timer = uVar1;
            p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
            p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
          }
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.frame(0);
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
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            uVar11 = 0x40a00000;
            particles = random.range(0,0x78);
            p.polar((float)-particles,uVar11);
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
          }
          if (((((p.response == 6) || (p.response == 1)) ||
               (p.response == 2)) || (p.response == 4)) &&
             (i16(p.animation.cursor.index) < 3)) {
            p.frame(3);
          }
          else {
            if (double(p.pose.scale[0]) < double(1.0f)) {
              p.pose.scale[0] = double(p.pose.scale[0]) + double(0.05f);
              p.pose.scale[1] = double(p.pose.scale[1]) + double(0.05f);
              if (double(1.0f) < double(p.pose.scale[0])) {
                p.pose.scale[0] = 1.0f;
                p.pose.scale[1] = 1.0f;
              }
            }
            if (((i16(p.animation.cursor.index) == 2) &&
                (i16(owner.animation.cursor.action) == 0x2f0)) &&
               ((i16(owner.animation.cursor.index) == 8 &&
                (double(p.motion.y) < double(380.0f))))) {
              p.destroy();
              sVar2 = random.range(0,3);
              p.counter = sVar2 + 3;
              for (local_c = 0; local_c < i16(p.counter); local_c = local_c + 1) {
                uVar10 = 1;
                uVar11 = u8(owner.motion.facing);
                particles = random.range(0,0x3c);
                fVar8 = ((float)particles + double(p.motion.y)) - double(30.0f);
                particles = random.range(0,0x3c);
                owned(0x391,((float)particles + double(p.motion.x)) - double(30.0f),fVar8,uVar11,uVar10);
              }
            }
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if (((double(1400.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(160.0f))) ||
               ((double(1000.0f) < double(p.motion.y) || (double(p.motion.y) < -double(160.0f))))) {
              p.destroy();
            }
            else {
              if ((0x14 < u8(channel(24))) && (i16(p.animation.cursor.index) == 3)) {
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
      case 0x391:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            particles = random.range(0,3);
            p.pose.scale[0] = (float)particles * double(0.1f) + double(0.8f);
            p.pose.scale[1] = double(p.pose.scale[0]);
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            uVar1 = random.range(0,0x1e);
            p.counter = uVar1;
            p.timer = 1;
            uVar1 = random.range(0,5);
            p.angle = uVar1;
          }
          if (i16(p.animation.cursor.index) < 2) {
            p.timer = i16(p.timer) + 1;
            p.polar((float)(i16(p.counter) + -0xf),(float)(int)i16(p.timer) * double(0.1f) +
                         (float)(int)i16(p.angle) * double(0.2f));
            angleRadians = (double)atan(double(p.motion.velocityY),double(p.motion.velocityX));
            p.pose.rotation[2] = (float)(angleRadians / (double)double(0.017453289f));
          }
          if ((p.response == 0) || (1 < i16(p.animation.cursor.index))) {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
            if ((double(1400.0f) < double(p.motion.x)) ||
               (((double(p.motion.x) < -double(160.0f) || (double(1000.0f) < double(p.motion.y))) ||
                (double(p.motion.y) < -double(160.0f))))) {
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
          else {
            p.frame(2);
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
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
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.hits = 1;
            p.response = 0;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            particles = random.range(0,0x167);
            p.pose.rotation[2] = (float)particles;
            uVar1 = random.range(0,0x167);
            p.counter = uVar1;
            sVar2 = random.range(0,0x50);
            p.timer = sVar2 + 100;
          }
          if (i16(p.animation.cursor.index) == 0) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.015f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.015f);
          }
          p.polar((float)(int)i16(p.counter),(float)(int)i16(p.timer) * double(0.1f));
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(4.0f);
          p.motion.velocityX = double(p.motion.velocityX) + double(0.05f);
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          p.timer = i16(p.timer) + -5;
          if (i16(p.timer) < 0x14) {
            p.timer = 0x14;
          }
          if (u8(channel(24)) < 6) {
            p.destroy();
          }
          else {
            setChannel(24,u8(i8(channel(24)) + -5));
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
        break;
      case 0x393:
        if (i16(owner.combat.spellTimer) < 1) {
          p.destroy();
        }
        else {
          if ((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) {
            p.counter = 0;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 0.0f;
            p.pose.scale[1] = 0.0f;
            p.hits = 0xff;
            p.response = 0;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
          }
          if (i16(p.animation.cursor.index) == 0) {
            p.pose.scale[0] = double(p.pose.scale[0]) + double(0.05f);
            p.pose.scale[1] = double(p.pose.scale[1]) + double(0.05f);
            if (double(1.0f) < double(p.pose.scale[0])) {
              p.pose.scale[0] = 1.0f;
              p.pose.scale[1] = 1.0f;
            }
          }
          p.motion.velocityX = double(p.motion.velocityX) + double(0.2f);
          p.motion.x =
               (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
               double(p.motion.x);
          p.motion.y = double(p.motion.y) - double(p.motion.velocityY);
          if ((((double(1600.0f) < double(p.motion.x)) || (double(p.motion.x) < -double(360.0f))) ||
              (double(1000.0f) < double(p.motion.y))) || (double(p.motion.y) < -double(160.0f))) {
            p.destroy();
          }
          else {
            if ((i16(p.animation.cursor.index) == 2) && (i16(p.animation.cursor.elapsed) == 0)) {
              p.frame(1);
              p.response = 0;
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.destroy();
            }
          }
        }
        break;
      case 0x394:
        state.phantomAttacking = 1;
        if (i16(owner.combat.spellTimer) < 1) {
          p.action(0x32a);
          state.phantomAttacking = 0;
        }
        else {
          p.motion.facing = u8(owner.motion.facing);
          if (((i16(p.animation.cursor.index) == 0) && (i16(p.animation.cursor.elapsed) == 0)) &&
             (i16(p.counter) == 0)) {
            p.motion.velocityX = -10.0f;
            p.motion.velocityY = 10.0f;
            p.begin_pose(0.0f,0.0f,0.0f);
            p.pose.scale[0] = 1.0f;
            p.pose.scale[1] = 1.0f;
            setChannel(24,u8(0xff));
            setChannel(16,u8(0xff));
            setChannel(8,u8(0xff));
            setChannel(0,u8(0xff));
            p.counter = 1;
            p.timer = 0;
            p.angle = 0;
          }
          if (-double(2.0f) <= double(p.motion.velocityX)) {
            p.motion.velocityX = -2.0f;
          }
          else {
            p.motion.velocityX = double(p.motion.velocityX) + double(0.6f);
          }
          if (double(p.motion.velocityY) <= double(0.0f)) {
            p.motion.velocityY = 0.0f;
          }
          else {
            p.motion.velocityY = double(p.motion.velocityY) - double(0.6f);
          }
          if (double(p.motion.velocityX) == double(2.0f)) {
            p.timer = i16(p.timer) + 1;
          }
          if (double(p.motion.x) < double(30.0f)) {
            p.motion.velocityX = 0.0f;
            p.motion.x = 30.0f;
          }
          if (double(1250.0f) < double(p.motion.x)) {
            p.motion.velocityX = 0.0f;
            p.motion.x = 1250.0f;
          }
          if (((i16(p.timer) < 0x79) ||
              (i16(owner.animation.cursor.action) < 0x96)) &&
             (0x31 < i16(owner.animation.cursor.action))) {
            p.motion.x =
                 (float)(int)i8(p.motion.facing) * double(p.motion.velocityX) +
                 double(p.motion.x);
            p.motion.y = double(p.motion.y) + double(p.motion.velocityY);
            uVar11 = 1;
            uVar4 = (u32)u8(p.motion.facing);
            fVar8 = double(p.motion.y);
            particles = random.range(0,0x14);
            fVar7 = (float)particles + double(float(double(fVar8)-10.0));
            fVar8 = double(p.motion.x);
            particles = random.range(0,0x14);
            owned(0x32b,(float)particles + double(float(double(fVar8)-10.0)),fVar7,uVar4,uVar11);
            if (i16(p.animation.cursor.elapsed) == 0) {
              uVar1 = random.range(0,0x14);
              p.timer = uVar1;
              p.pose.scale[0] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
              p.pose.scale[1] = (float)(int)i16(p.timer) * double(0.01f) + double(0.9f);
            }
            sVar2 = p.advance();
            if (sVar2 == i16(p.animation.cursor.frameCount)) {
              p.frame(0);
            }
          }
          else {
            p.action(0x389);
            p.counter = 0;
            p.timer = 0;
            p.motion.velocityX = 0.0f;
            p.motion.velocityY = 0.0f;
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
               (float)(int)i16(state.farRangeFrames) / double(75.0f);
          p.pose.scale[1] = double(p.pose.scale[0]);
          if (i16(p.animation.cursor.index) == 0) {
            if (u8(channel(24)) < 0xe6) {
              setChannel(24,u8(i8(channel(24)) + '\x19'));
            }
            else {
              setChannel(24,u8(0xff));
            }
            if (0x95 < i16(state.farRangeFrames)) {
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
          fVar8 = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[0] = (double(owner.combat.spellTimer)/p.counter)*2.0+1.0;
          fVar8 = (float)(int)i16(owner.combat.spellTimer) /
                  (float)(int)i16(p.counter);
          p.pose.scale[1] = (double(owner.combat.spellTimer)/p.counter)*2.0+1.0;
          p.pose.rotation[2] = double(p.pose.rotation[2]) + double(1.0f);
          sVar2 = p.advance();
          if (sVar2 == i16(p.animation.cursor.frameCount)) {
            p.frame(9);
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
