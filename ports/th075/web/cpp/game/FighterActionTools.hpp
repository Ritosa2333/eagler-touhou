#pragma once
#include "FighterActionEvents.hpp"
namespace th075::action {
inline i16 add(i16 v,i32 n){return i16(u16(u32(i32(v))+u32(n)));}
inline bool start(const FighterActor& a){return a.animation.cursor.index==0&&a.animation.cursor.elapsed==0;}
inline bool at(const FighterActor& a,i16 f){return a.animation.cursor.index==f&&a.animation.cursor.elapsed==0;}
inline void frame(FighterActor& a,i16 f){a.animation.set(a.animation.cursor.action,f,0);}
inline bool end(FighterActor& a){const i16 result=a.animation.advance();return result==a.animation.cursor.frameCount;}
inline void emit(FighterActor& a,FighterActionEvents& events,FighterEffectSpace space,i16 id,float x,float y,u8 layer=1){events.effect(space,id,x,y,a.motion.facing,layer,a.launchParameters);}
inline float x(const FighterActor& a,i32 offset){return float(double(a.motion.x)+i32(a.motion.facing)*offset);}
inline float y(const FighterActor& a,float offset){return float(double(a.motion.y)+offset);}
inline void attack_start(FighterActor& a,i16 hits){if(start(a)){if(a.carryMomentum==0)a.motion.clear();a.remainingHits=hits;a.response=0;}}
inline void carry_brake(FighterActor& a){if(i8(a.carryMomentum)>0){a.motion.velocityX-=.6f;if(a.motion.velocityX<0){a.motion.velocityX=0;a.carryMomentum=0;}}}
inline void clear_flight(FighterActor& a){a.motion.velocityX=a.motion.velocityY=a.motion.accelerationY=0;}
inline bool air_land(FighterActor& a){a.motion.velocityY-=a.motion.accelerationY;if(double(a.motion.y)+a.motion.velocityY<=0){a.select_action(10);a.motion.y=a.motion.velocityY=a.motion.accelerationY=0;return true;}return false;}
}
