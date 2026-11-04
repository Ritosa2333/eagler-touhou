#pragma once
#include "Types.hpp"
#include <vector>
#include <cstring>
#include <cmath>
#include <algorithm>
namespace th19 {
// 0x54dc40, confirmed against 0x54dd96..0x54de82 (turn factor 0x5aa1e8).
inline void advanceHomingShot(int age,bool target,float targetAngle,float& angle,float& speed){
 if(!target){speed=std::min(16.f,speed+.1f);return;}
 if(age>=60){speed+=.2f;return;}
 const float delta=std::remainder(targetAngle-angle,6.28318530718f);
 if(std::abs(delta)>=3.14159265359f/4)speed=std::max(4.f,speed-.2f);
 else if(std::abs(delta)<3.14159265359f/12)speed=std::min(16.f,speed+.2f);
 angle=std::remainder(angle+delta*.08f,6.28318530718f);
}
// Original v6 SHT: table at 0xf0, relative to byte 400; records 0x78 bytes.
// Native 0x5478f0 relocates table; 0x54f990 schedules; 0x54e960 creates ANM.
struct ShotDefinition {
 int interval=0,delay=0,script=-1,option=0,type=0,damage=0,sound=-1,hit_script=-1;
 int secondary_interval=0,secondary_delay=0,lifetime=0,laser_duration=0,transition_frame=0;
 int init=0,update=0,hit=0,laser_id=0;
 float x=0,y=0,angle=0,speed=0,angular_velocity=0,acceleration=0,hit_width=0,hit_height=0,hit_growth=0;
 bool due(int frame,int secondary_frame)const {
  if(interval==0)return true;
  return secondary_interval?secondary_frame%secondary_interval==secondary_delay:frame%interval==delay;
 }
};
// Native 0x54b380: Reimu's charge talismans brake, hover, then accelerate.
inline void advanceChargeTalisman(int age,float& speed){
 if(age>=120)speed+=.2f;
 else if(speed>0)speed=std::max(.2f,speed-.95f);
}
// 0x54aae0 sets motion flag 0x20 at the bite transition. 0x4d4dc0
// and 0x4d4a00 then skip velocity integration; only the callback's
// explicit four-unit movement on the next ten ticks remains.
inline bool enokoChargeFrozen(int age,int transition){return age>=transition;}
inline float enokoChargeStep(int age,int transition){return age>transition&&age<=transition+10?4.f:0.f;}
// Native update callbacks 0x54c7a0 (3) / 0x54a960 (14).
inline void advanceSpecialShot(int callback,float& angle,float& speed){
 if(callback==3)speed+=.3f;
 else if(callback==12)speed+=.1f;
 else if(callback==14){speed+=.1f;angle+=(-1.57079632679f-angle)*.05f;}
}
// Original initialization callbacks 20 (0x54acb0) and 22 (0x54aa00).
// The caller supplies a unit draw; the application's RNG stream is not a native replay.
inline float initializeSpecialShot(int callback,float angle,float unit){
 if(callback==14)angle=-1.57079632679f+(unit*2-1)*3.14159265359f*.2f;
 // 0x54b270, ability group28: upward spread of +/- pi/18.
 if(callback==16)angle=-1.57079632679f+(unit*2-1)*3.14159265359f/18;
 if(callback==20)angle+=(unit*2-1)*3.14159265359f/18;
 if(callback==22)angle=-1.57079632679f+(unit*2-1)*3.14159265359f*.25f;
 return std::remainder(angle,6.28318530718f);
}
struct PlayerShotTable {
 std::vector<std::vector<ShotDefinition>> groups;
 int option_script=-1,option_power_script=-1;
 struct Position {float x=0,y=0;};
 Position option_positions[2][10]{};
 Position option_position(int power,int index,bool focused)const {
  static constexpr int offsets[]={0,0,1,3,6};
  if(power<1||power>4||index<0||index>=power)return {};
  return option_positions[focused?1:0][offsets[power]+index];
 }
 bool load(const std::vector<u8>& b){
  groups.clear();if(b.size()<400)return false;
  auto u16at=[&](size_t p){u16 v;std::memcpy(&v,b.data()+p,2);return v;};
  auto i32at=[&](size_t p){i32 v;std::memcpy(&v,b.data()+p,4);return v;};
  auto f=[&](size_t p){float v;std::memcpy(&v,b.data()+p,4);return v;};
  int count=u16at(2);if(u16at(0)!=6||count>40)return false;
  std::vector<std::vector<ShotDefinition>> parsed(count);
  for(int g=0;g<count;++g){
   int offset=i32at(0xf0+4*g);if(offset<0)continue;
   size_t p=size_t(offset)+400;
   for(;;p+=120){
    if(p>=b.size())return false;
    if(static_cast<signed char>(b[p])<0)break;
    if(b.size()-p<120)return false;
    ShotDefinition d;d.interval=b[p];d.delay=b[p+1];d.damage=u16at(p+2);
    d.x=f(p+4);d.y=f(p+8);d.hit_width=f(p+0xc);d.hit_height=f(p+0x10);d.angle=f(p+0x14);d.speed=f(p+0x1c);
    d.hit_growth=f(p+0x68);d.angular_velocity=f(p+0x18);d.acceleration=f(p+0x20);
    d.secondary_interval=b[p+0x32];d.secondary_delay=b[p+0x33];d.lifetime=i32at(p+0x34);
    d.laser_duration=i32at(p+0x3c);d.transition_frame=i32at(p+0x40);
    d.option=b[p+0x28]&15;d.type=b[p+0x29];d.script=static_cast<short>(u16at(p+0x2a));
    d.hit_script=static_cast<short>(u16at(p+0x2e));d.sound=static_cast<short>(u16at(p+0x2c));d.laser_id=i32at(p+0x38);
    d.init=i32at(p+0x48);d.update=i32at(p+0x4c);d.hit=i32at(p+0x54);
    if(!std::isfinite(d.x)||!std::isfinite(d.y)||!std::isfinite(d.angle)||!std::isfinite(d.speed))return false;
    parsed[g].push_back(d);
   }
  }
  option_script=i32at(0x3c);option_power_script=i32at(0x40);
  for(int mode=0;mode<2;++mode)for(int i=0;i<10;++i){
   auto&p=option_positions[mode][i];p={f(0x50+mode*0x50+i*8),f(0x54+mode*0x50+i*8)};
   if(!std::isfinite(p.x)||!std::isfinite(p.y))return false;
  }
  groups=std::move(parsed);return true;
 }
};
}
