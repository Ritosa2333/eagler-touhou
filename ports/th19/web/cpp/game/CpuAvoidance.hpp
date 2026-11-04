#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <cmath>
namespace th19 {
// Original CPUInf: 0x4fb6f0, 0x4fa2a0, 0x4fa3e0.
// Mode 2 uses +38 as the chance to keep a hazardous chosen direction.
// Story mode uses a separate stage controller; it must not use versus defaults.
struct CpuDifficulty {
 int difficulty=0,mode=0,elapsed=0,ignoreDangerPercent=0;
 void reset(int d,int m){difficulty=std::clamp(d,0,3);mode=m;elapsed=0;
  constexpr int initial[]={40,10,0,0};ignoreDangerPercent=m==2?initial[difficulty]:0;}
 void tick(bool nativePause){if(mode!=2||nativePause)return;++elapsed;
  constexpr int interval[]={240,600,1200,1000000};
  if(elapsed>1799&&elapsed%interval[difficulty]==0&&ignoreDangerPercent<100)++ignoreDangerPercent;}
};
// 0x598f70, 18 rows of 0x48 bytes, only the first sixteen entries are read.
inline constexpr int cpuAvoidanceOrder[18][16]={
 {2,7,8,4,3,6,5,1,11,16,17,13,12,15,14,10},
 {0,4,3,7,8,6,5,2,9,13,12,16,17,15,14,11},
 {0,7,8,4,3,6,5,1,9,16,17,13,12,15,14,10},
 {7,0,2,5,8,6,4,1,16,9,11,14,17,15,13,10},
 {8,0,2,6,7,5,3,1,17,9,11,15,16,14,12,10},
 {3,7,0,2,8,4,6,1,12,16,9,11,17,13,15,10},
 {4,8,0,2,7,3,5,1,13,17,9,11,16,12,14,10},
 {3,8,5,2,0,4,6,1,12,17,14,11,9,13,15,10},
 {4,7,2,0,6,3,5,1,13,16,15,11,9,12,14,10},
 {2,7,8,4,3,6,5,1,11,16,17,13,12,15,14,10},
 {0,4,3,7,8,6,5,2,9,13,12,16,17,15,14,11},
 {0,7,8,4,3,6,5,1,9,16,17,13,12,15,14,10},
 {7,0,2,5,8,6,4,1,16,9,11,14,17,15,13,10},
 {8,0,2,6,7,5,3,1,17,9,11,15,16,14,12,10},
 {3,7,0,2,8,4,6,1,12,16,9,11,17,13,15,10},
 {4,8,0,2,7,3,5,1,13,17,9,11,16,12,14,10},
 {3,8,5,2,0,4,6,1,12,17,14,11,9,13,15,10},
 {4,7,2,0,6,3,5,1,13,16,15,11,9,12,14,10},
};
struct CpuAvoidance {
 struct Choice {int index=0,holdFrames=0;};
 struct Point {float x=0,y=0;};
 // 0x4f8f60 reads player +20e4..+20f0 (1/128 fixed point).
 static std::array<Point,18> positions(float x,float y,int fast,int slow,int fastDiagonal,int slowDiagonal){
  constexpr int dx[]={0,0,0,-1,1,-1,1,-1,1};
  constexpr int dy[]={0,-1,1,0,0,-1,-1,1,1};
  std::array<Point,18> out{};
  for(int i=0;i<18;++i){int dir=i%9;float step=(dir>=5?(i>=9?slowDiagonal:fastDiagonal):(i>=9?slow:fast))/128.f;
   out[i]={std::clamp(x+dx[dir]*step,-140.f,140.f),std::clamp(y+dy[dir]*step,32.f,432.f)};}
  return out;
 }
 struct Hazard {float x=0,y=0,width=0,height=0,angle=0,radius=0;bool circle=true;};
 // Native circle contact uses summed squares; the rectangle uses rounded corners.
 static bool blockedAt(Point p,const Hazard&h,float playerRadius){
  float x=p.x-h.x,y=p.y-h.y;
  if(h.circle)return x*x+y*y<h.radius*h.radius+playerRadius*playerRadius;
  float c=std::cos(h.angle),s=std::sin(h.angle),rx=std::abs(x*c+y*s),ry=std::abs(y*c-x*s);
  float hw=h.width*.5f,hh=h.height*.5f;
  if((rx<=hw+playerRadius&&ry<=hh)||(rx<=hw&&ry<=hh+playerRadius))return true;
  float cx=rx-hw,cy=ry-hh;return cx*cx+cy*cy<playerRadius*playerRadius;
 }
 template<class Hazards> static uint32_t blockedMask(const std::array<Point,18>&points,const Hazards&hazards,float playerRadius){
  uint32_t mask=0;for(const auto&h:hazards)for(int i=0;i<18;++i)if(blockedAt(points[i],h,playerRadius))mask|=1u<<i;return mask;
 }
 struct Perception {uint32_t blocked=0;int pass=0;bool allBlocked=false;};
 // 0x4fa455 retries with radius *10, *5, *1, stopping on the first safe candidate.
 template<class Hazards> static Perception perceive(const std::array<Point,18>&points,const Hazards&hazards,float playerRadius){
  constexpr float margins[]={10,5,1};Perception result;
  for(int pass=0;pass<3;++pass){result.blocked=blockedMask(points,hazards,playerRadius*margins[pass]);result.pass=pass;
   if(result.blocked!=0x3ffff)return result;}
  result.allBlocked=true;return result;
 }
 // No target/no collectible branch of 0x4fa3e0. Target pursuit is separate.
 static int idleDirection(float x,float y){
  int dir=y<320?2:y>416?1:0;
  if(x>80)return dir==2?7:dir==1?5:3;
  if(x< -80)return dir==2?8:dir==1?6:4;
  return dir;
 }
 // 0x4fb085..0x4fb16f. Caller supplies native RNG % 100 only when needed.
 static Choice choose(int preferred,uint32_t blocked,int ignoreDangerPercent,int random100){
  preferred=std::clamp(preferred,0,17);
  if(!(blocked&(1u<<preferred))||(ignoreDangerPercent>0&&random100<ignoreDangerPercent))return {preferred,0};
  for(int candidate:cpuAvoidanceOrder[preferred])if(!(blocked&(1u<<candidate)))return {candidate,8};
  return {preferred,0};
 }
 // 0x4fafc1..0x4fb075, applied after target/collectible steering,
 // before hazard avoidance. This is a steering margin, not movement clamping.
 static Choice boundaryDirection(int direction,float x,float y){
  int hold=0;if(x< -136){direction=4;hold=4;}else if(x>136){direction=3;hold=4;}
  if(y>400){
   switch(direction){case 0:case 2:direction=1;break;case 3:direction=5;break;
    case 7:direction=3;break;case 4:direction=6;break;case 8:direction=4;break;}
   hold=4;
  }
  return {direction,hold};
 }
 // Native bit layout returned by 0x4fa3e0, not SDL key codes.
 static uint8_t inputBits(int choice,bool shoot,bool charge){
  constexpr uint8_t direction[]={0,0x10,0x20,0x40,0x80,0x50,0x90,0x60,0xa0};
  choice=std::clamp(choice,0,17);
  return direction[choice%9]|(choice>=9?8:0)|(shoot?1:0)|(charge?2:0);
 }
};
}
