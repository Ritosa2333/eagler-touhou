#pragma once
#include "FighterActionEvents.hpp"
namespace th075 {
// Battle objects own their animation and collision state; their creator is
// supplied explicitly by the character owner, without native pointer layouts.
struct FighterObject {
    virtual ~FighterObject()=default;
    FighterMotion motion;SpritePose pose;FighterAnimation animation;
    i32 response=0;i16 hits=0,freeze=0,contactDelay=0;
    std::array<i16,3> recoveryTimers{};
    float depth=0;
    float attackPower=0,attackModifier=0,impulseX=0,impulseY=0,bodyPower=1,sourcePower=1,sourceModifier=1;
    CollisionSnapshot collision;u8 layer=1,order=0,group=0;
    std::array<float,4> parameters{};bool alive=true;
    void save_collision(){collision.response=response;collision.frame=animation.frame;const i16 action=animation.cursor.action;collision.sequence=animation.pattern&&action>=0&&u32(action)<animation.pattern->sequences.size()?&animation.pattern->sequences[action]:nullptr;collision.x=motion.x;collision.y=motion.y;collision.facing=motion.facing;collision.action=action;}
    void destroy(){alive=false;}
    i16 advance(){const bool rebound=i32(animation.cursor.elapsed)+1>=animation.cursor.duration;const i16 result=animation.advance();if(rebound)alive=animation.frame!=nullptr;return result;}
    void frame(i16 value){animation.set(animation.cursor.action,value,0);alive=animation.frame!=nullptr;}
    void action(i16 value){animation.reset(value);alive=animation.frame!=nullptr;}
    void begin_pose(float x=0,float y=0,float z=0){pose={};pose.active=true;pose.offset={x,y,z};}
    void polar(float degrees,float speed){motion.velocityX=float(double(angle_cos(degrees))*speed);motion.velocityY=float(-double(angle_sin(degrees))*speed);}
};
}
