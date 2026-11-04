#pragma once
#include "BulletChanges.hpp"
#include <functional>
namespace th19 {
// LaserStraightInf 0x52f080/0x52e6c0; distinct from ordinary BulletChanges.
inline bool supportedStraightLaserChange(const BulletChange&c){
 if(c.flags==0x40)return c.integers[0]>=0&&c.integers[1]>=0&&c.integers[1]<=31;
 if(c.flags==0||c.flags==0x100||c.flags==0x80||c.flags==0x40000001u)return true;
 return (c.flags==4||c.flags==0x200000)&&c.integers[0]>0&&std::isfinite(c.floats[0])&&std::isfinite(c.floats[1])&&c.floats[1]<999990.f;
}
struct StraightLaserChanges {
 struct Action{bool active=false;int elapsed=0,duration=0;float scalar=0,ax=0,ay=0;};
 std::array<Action,3> actions{};int cursor=0;bool initialized=false;float vx=0,vy=0;
 bool reflecting=false;int reflect_slot=-1,reflect_count=0,reflect_edges=0;float reflect_speed=0;
 int tick(std::array<BulletChange,24>&records,float&speed,float&angle,int&bounds,int&lock,const std::function<bool()>&reflectStep={}){
  if(!initialized){vx=std::cos(angle)*speed;vy=std::sin(angle)*speed;initialized=true;}
  int sounds=0;
  for(int guard=0;guard<25;++guard){
   while(cursor<24){const auto&c=records[cursor];if(!c.flags)break;
    bool busy=reflecting;for(const auto&a:actions)busy|=a.active;if(!c.concurrent&&busy)break;
    const int index=cursor++;
    if(c.flags==0x40){if(c.integers[0]>0){reflecting=true;reflect_slot=index;reflect_count=c.integers[0]-1;reflect_edges=c.integers[1];reflect_speed=c.floats[0]<0?speed:c.floats[0];records[index].integers[0]--;}continue;}
    if(c.flags==0x100){bounds=c.integers[0];continue;}
    if(c.flags==0x80){lock=c.integers[0];continue;}
    const int slot=c.flags==4?0:c.flags==0x200000?1:2;
    auto&a=actions[slot];a={};a.duration=c.integers[0];a.active=a.duration>0;
    if(slot!=2){a.scalar=c.flags==4?c.floats[0]:(c.floats[0]-speed)/a.duration;
     const float dir=c.floats[1]<=-999990.f?angle:c.floats[1];a.ax=std::cos(dir)*a.scalar;a.ay=std::sin(dir)*a.scalar;
     if(index>0)++sounds;}
   }
   bool completed=false;
   for(int slot=0;slot<3;++slot){
    // 52e0c0: reflection follows acceleration and precedes the wait timer.
    // A completed reflection repeats this entire pass before position integrates.
    if(slot==2&&reflecting&&reflectStep)completed|=reflectStep();
    auto&a=actions[slot];if(!a.active)continue;
    if(a.elapsed>=a.duration){a.active=false;completed=true;continue;}
    if(slot<2){speed+=a.scalar;vx+=a.ax;vy+=a.ay;
     // Unlike bullets, native laser scalar speed is NOT recomputed from vector length.
     if(std::abs(vx)>.0001f||std::abs(vy)>.0001f)angle=std::atan2(vy,vx);}
    ++a.elapsed;
   }
   if(lock!=0)--lock;
   if(!completed)break;
  }
  return sounds;
 }
};
}
