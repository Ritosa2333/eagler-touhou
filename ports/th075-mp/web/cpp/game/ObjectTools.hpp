#pragma once
#include "FighterObject.hpp"
#include <cmath>
namespace th075::object {
inline i16 add(i16 n,i32 amount){return i16(u16(u32(i32(n))+u32(amount)));}
inline bool start(const FighterObject& p){return p.animation.cursor.index==0&&p.animation.cursor.elapsed==0;}
inline bool at(const FighterObject& p,i16 i){return p.animation.cursor.index==i&&p.animation.cursor.elapsed==0;}
inline bool end(FighterObject& p){const auto result=p.advance();return result==p.animation.cursor.frameCount;}
inline void scale(FighterObject& p,float n){p.pose.scale[0]=p.pose.scale[1]=n;}
inline u8 alpha(const FighterObject& p){return u8(p.pose.color>>24);}
inline void alpha(FighterObject& p,u8 n){p.pose.color=(p.pose.color&0xffffff)|(u32(n)<<24);}
inline void fade(FighterObject& p,u8 amount){alpha(p,alpha(p)<amount?0:u8(alpha(p)-amount));}
inline void move(FighterObject& p,i32 vertical=-1){p.motion.x=float(double(p.motion.x)+i32(p.motion.facing)*double(p.motion.velocityX));p.motion.y=float(double(p.motion.y)+vertical*double(p.motion.velocityY));}
inline bool outside(const FighterObject& p,float left=-100,float right=1380,float bottom=-240,float top=1000){return p.motion.x<left||p.motion.x>right||p.motion.y<bottom||p.motion.y>top;}
inline float degrees(float y,float x){return float(std::atan2(double(y),double(x))/double(0.017453289f));}
inline i16 aim(const FighterObject& p,const FighterActor& other,bool relative=true,bool addFirst=true){const float y=addFirst?float((double(other.motion.y)+100)-p.motion.y):float((double(other.motion.y)-p.motion.y)+100);const float x=float((double(other.motion.x)-p.motion.x)*(relative?i32(p.motion.facing):1));return i16(u16(i32(std::atan2(double(y),double(x))/double(0.017453289f))));}
inline void spawn(FighterActor& owner,FighterActionEvents& events,i16 id,float x,float y,i8 facing,u8 layer=1){events.effect(FighterEffectSpace::Owned,id,x,y,facing,layer,owner.launchParameters);}
inline void spawn(FighterActor& owner,FighterActionEvents& events,const FighterObject& p,i16 id){spawn(owner,events,id,p.motion.x,p.motion.y,p.motion.facing);}
inline bool cancel_on_hurt(FighterObject& p,FighterActor& owner,FighterActionEvents& events,i16 limit,i32 count,bool rewardFirst=false){
    if(owner.animation.cursor.action<50||owner.animation.cursor.action>149||p.animation.cursor.index>limit)return false;
    p.destroy();const auto reward=[&](){if(owner.knockdownTime>25){owner.knockdownTime=0;spawn(owner,events,995,p.motion.x,p.motion.y,1);}};
    if(rewardFirst)reward();for(i32 i=0;i<count;i++)spawn(owner,events,996,p.motion.x,p.motion.y,1);if(!rewardFirst)reward();return true;
}
}
