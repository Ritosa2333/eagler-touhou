#pragma once
#include "CpuAvoidance.hpp"
#include "CpuCharge.hpp"
#include "CpuTarget.hpp"
namespace th19 {
// Stateful ordering from 0x4fa3e0. Geometry gathering and obstacle steering
// are caller-owned: this class does not fabricate missing world observations.
struct CpuControl {
 CpuCharge charge;
 int savedChoice=0,holdFrames=0,chargeTail=0,specialCounter=0;
 struct Frame {
  int energy=0,currentCharge=0;std::array<int,4> thresholds{};
  uint32_t blocked=0;int ignoreDangerPercent=0;
  bool storyController=false,allBlocked=false;
  int eligibleTargetCount=0;
  const CpuTarget::Pursuit* pursuit=nullptr;
  bool activeSpecialTarget=false;
  int resolvedDirection=0;bool resolvedSlow=false;int boundaryHold=0;
 };
 struct Output {uint8_t bits=0;int targetCooldown=0;};
 template<class Random> Output tick(const Frame&f,Random&&random){
  // 0x4fa477: emergency bit 4, before charge RNG/target/timer updates.
  if(f.allBlocked)return {4,0};
  bool charging=charge.baseIntent(f.energy,f.currentCharge,f.thresholds,random);
  // 0x4fa70e: story controller may release a charge above C2.
  if(f.storyController&&f.currentCharge>std::clamp(f.thresholds[1],0,2500)&&random()%40==0)charging=false;
  int desired=0,originalDirection=0;
  if(holdFrames<1){
   if(chargeTail>0){charging=true;--chargeTail;}
   if(f.pursuit){
    charging=charging||f.pursuit->charge;
    if(f.pursuit->chargeTailFrames)chargeTail=f.pursuit->chargeTailFrames;
    if(f.pursuit->incrementSpecialCounter)++specialCounter;
    originalDirection=f.pursuit->direction;
   }else originalDirection=f.resolvedDirection;
   desired=f.resolvedDirection+(f.resolvedSlow?9:0);
   if(f.boundaryHold>0)holdFrames=f.boundaryHold;
  }else{
   --holdFrames;desired=savedChoice;
   // Original comparison direction was zeroed at the start of this frame.
   if(f.activeSpecialTarget&&specialCounter<50){charging=true;chargeTail=15;++specialCounter;}
  }
  int draw=0;if((f.blocked&(1u<<desired))&&f.ignoreDangerPercent>0)draw=int(random()%100);
  auto choice=CpuAvoidance::choose(desired,f.blocked,f.ignoreDangerPercent,draw);
  if(choice.holdFrames)holdFrames=choice.holdFrames;
  savedChoice=choice.index;
  auto horizontal=[](int dir){return dir==3||dir==5||dir==7?-1:dir==4||dir==6||dir==8?1:0;};
  int cooldown=f.pursuit&&horizontal(choice.index%9)*horizontal(originalDirection)==-1?10:0;
  // 0x4fb1da reads eligible target count (+36ee94), NOT a shoot timer.
  return {CpuAvoidance::inputBits(choice.index,f.eligibleTargetCount!=0,charging),cooldown};
 }
};
}
