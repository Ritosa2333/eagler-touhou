#pragma once
#include <algorithm>
namespace th19 {
// EXE 0x5c5f90 (stage stride 0x80), 0x518488 and 0x51afe0.
struct GlobalRank {
 int value=0,cap=0,interval=1,clock=0,difficulty=0;bool versus=false;
 void begin(int d,bool vs,int storyStage){
  difficulty=std::clamp(d,0,3);versus=vs;clock=0;
  const int stage=vs?0:std::clamp(storyStage,1,5);
  constexpr int starts[]={0,0,30,50};value=starts[difficulty];
  constexpr int caps[3][4]={{10,60,80,100},{10,20,80,100},{20,50,80,100}};
  constexpr int periods[3][4]={{900,360,360,360},{1200,480,480,360},{600,480,480,360}};
  const int row=std::min(stage,2);cap=caps[row][difficulty];interval=periods[row][difficulty];
 }
 void tick(bool paused=false){
  // Native global attack-controller clock is advanced before the modulo test.
  ++clock;if(!paused&&clock%interval==0)value=std::clamp(value+1,0,cap);
  // Preserve the executable's actual branch (0x51b115..0x51b17c).
  if(versus&&clock>=10800){cap=difficulty==0?100:200;interval=240;}
 }
};
}
