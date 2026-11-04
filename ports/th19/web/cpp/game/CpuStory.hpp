#pragma once
#include <algorithm>
#include <array>
namespace th19 {
// CPUStage1..6StoryInf (4f8720,4f83e0,4f8160,4f7e80,4f7b90,4f77a0).
// +8 is attack credit, NOT elapsed frames: its writer is 40b500.
struct CpuStory {
 struct Event {int tier=0,rank=0,extraIncrement=0;bool finalBoss=false;};
 struct Step {int credit,tier,rank,extra;bool waitBoss,finalBoss;};
 int stage=1,difficulty=0,state=0,credit=0;
 void reset(int s,int d){stage=std::clamp(s,1,6);difficulty=std::clamp(d,0,3);state=credit=0;}
 void add(int n,bool bossPresent){if(!bossPresent)credit=std::clamp(credit+n,0,10000);}
 Event tick(bool bossPresent){
  static constexpr Step steps[6][9]={
   {{300,3,0,0,0,0},{300,3,0,0,0,0},{400,4,1,0,1,0},{200,4,0,0,1,1}},
   {{300,3,0,0,0,0},{300,3,0,1,0,0},{500,4,2,0,1,0},{400,4,0,0,1,1}},
   {{300,3,0,0,0,0},{300,3,0,1,0,0},{500,4,2,0,1,0},{300,3,0,1,0,0},{600,4,0,0,1,1}},
   {{300,3,0,0,0,0},{350,3,0,1,0,0},{500,4,1,0,1,0},{300,3,0,1,0,0},{500,4,3,0,1,0},{300,4,0,0,1,1}},
   {{300,3,0,0,0,0},{300,4,1,0,0,0},{150,3,0,2,1,0},{300,3,0,1,1,0},{300,4,4,0,0,0},{300,3,0,1,1,0},{100,4,0,0,1,1}},
   {{300,3,0,0,0,0},{200,4,0,0,0,0},{200,4,2,0,0,0},{200,3,0,2,1,0},{200,3,0,1,1,0},{400,3,0,1,0,0},{200,4,3,0,0,0},{300,4,-1,0,1,0},{100,4,0,0,1,1}}
  };
  static constexpr int finalRanks[6][4]={{2,3,4,6},{3,4,5,7},{4,5,6,8},{4,5,6,8},{5,6,7,8},{14,15,16,17}};
  if(state>=9)return {};
  const auto&s=steps[stage-1][state];
  if(!s.tier||credit<s.credit||(s.waitBoss&&bossPresent))return {};
  ++state;credit=0;
  return {s.tier,s.finalBoss?finalRanks[stage-1][difficulty]:s.rank<0?10+difficulty:s.rank,s.extra,s.finalBoss};
 }
};
}
