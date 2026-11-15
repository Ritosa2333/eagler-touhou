#pragma once
#include <algorithm>
namespace th19 {
struct DeathRewards {int bullets=0,pure=0,charge=0;};
// 0x51d4e0. Inputs are native mode, elapsed battle frames, stage multiplier,
// and combo BEFORE 0x549780. No difficulty multiplier is applied here.
inline DeathRewards nativeDeathRewards(int kind,int inherited,int combo,int frame,int mode,int stage){
    const int band=frame<3600?0:frame<7200?1:frame<10800?2:3;
    constexpr int versusBullets[]={0,30,110,200},versusPure[]={0,20,40,100};
    constexpr int storyBullets[]={5,10,20,50},storyPure[]={1,2,3,5};
    const int factor=std::clamp(stage,0,999);
    const int b=mode==2?versusBullets[band]:storyBullets[band]*factor;
    const int p=mode==2?versusPure[band]:storyPure[band]*factor;
    DeathRewards r;
    if(kind==0){r.bullets=std::min(combo*3,60)+b+40;r.pure=combo>9?30:0;}
    else if((kind>=1&&kind<=4)||kind==6){
        r.charge=p+((kind==6?0:inherited)*5+5)*2;r.pure=p+5;
        r.bullets=std::min(combo*3,60)+b+20;
    }else if(kind==5){r.charge=50;r.pure=50;}
    return r;
}
}
