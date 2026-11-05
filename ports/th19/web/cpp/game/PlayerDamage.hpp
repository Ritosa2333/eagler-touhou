#pragma once
#include <algorithm>
#include <cmath>
namespace th19 {
// Native player state transitions: 0x545e10, 0x546320, 0x549830.
// Timer checks precede the native timer increment. Character death-prevention
// callbacks and rescue are explicit inputs, never inferred from charge level.
struct PlayerDamage {
 enum State {Respawning=0,Playing=1,Dead=2,Finished=3,Pending=4,Repelled=5};
 enum Event {None,Death,Respawn,Recovered,Defeat};
 State state=Playing;int frame=0;float angle=0;
 bool controllable()const{return state==Playing;}
 bool visible()const{return state!=Dead&&state!=Finished;}
 bool vulnerable(int invulnerable)const{return (state==Playing||state==Repelled||state==Respawning)&&invulnerable<=0;}
 void contact(bool barrier,float px,float py,float sx,float sy,int& invulnerable){
  state=barrier?Repelled:Pending;frame=0;angle=std::atan2(py-sy,px-sx);
  if(!barrier)invulnerable=6;
 }
 void rescue(){if(state==Pending){state=Playing;frame=60;}}
 Event die(int& lives,int& invulnerable){
  --lives;state=Dead;frame=0;invulnerable=180;return Death;
 }
 Event tick(int& lives,int& invulnerable,bool deathPrevented=false){
  if(state==Pending&&frame>=8&&!deathPrevented){
   return die(lives,invulnerable);
  }
  if(state==Dead&&frame==30){
   if(lives>0){state=Respawning;frame=0;return Respawn;}
   state=Finished;return Defeat;
  }
  if((state==Respawning&&frame>=60)||(state==Repelled&&frame>=40)){
   state=Playing;frame=0;return Recovered;
  }
  ++frame;return None;
 }
 float respawnY()const{return 480.f-float(frame*10240/60)/128.f;}
 float repelSpeed()const{return std::max(0.f,2.f-frame*2.f/40.f);}
};
// 0x4f43a0 copies 0x5c6794 + type*0x158 into Bullet+0x674.
inline float nativeBulletContactRadius(int type){
 static constexpr float radii[]={2.4f,2.4f,2.4f,2.0f,4.0f,4.0f,4.0f,4.0f,2.4f,2.4f,2.4f,2.8f,2.4f,2.4f,2.4f,2.4f,4.0f,4.0f,8.5f,8.5f,7.0f,6.0f,7.0f,7.0f,7.0f,6.0f,6.0f,4.0f,4.0f,10.0f,7.0f,4.0f,14.0f,12.0f,2.4f,3.2f,3.2f,4.0f,4.0f,4.0f,4.0f,4.0f,4.0f,5.0f,10.0f,10.0f,28.0f,28.0f};
 return type>=0&&type<48?radii[type]:0;
}
// 549570 -> 4d44e0: rotate into the rectangle, then cross strips and rounded corners.
inline bool nativeEnemyRectangleContact(float dx,float dy,float angle,float width,float height,float radius){
 if(width<=0||height<=0||radius<0)return false;
 const float c=std::cos(angle),s=std::sin(angle);
 const float x=std::abs(dx*c+dy*s),y=std::abs(dy*c-dx*s),hw=width*.5f,hh=height*.5f;
 if((x<=hw+radius&&y<=hh)||(x<=hw&&y<=hh+radius))return true;
 const float ex=x-hw,ey=y-hh;return ex*ex+ey*ey<radius*radius;
}
// 0x549e00 uses squared radii separately, with a strict boundary.
inline bool nativeCircleContact(float dx,float dy,float playerRadius,float otherRadius){
 return otherRadius>0&&dx*dx+dy*dy<playerRadius*playerRadius+otherRadius*otherRadius;
}
}
