#pragma once
#include <algorithm>
namespace th19 {
// EXE 5a85c0; 517790 rules initialization; 515540 result comparison.
struct VsMatch {
 int wins[2]{},round=0,remaining=0,limit=0,target=2;
 void begin(int timeRule,int roundRule){wins[0]=wins[1]=0;round=0;target=std::clamp(roundRule+1,1,3);static constexpr int times[]={0,10800,18000,25200,36000,54000,72000};limit=times[std::clamp(timeRule,0,6)];next();}
 void next(){++round;remaining=limit;}
 bool tick(){return limit>0&&remaining>0&&--remaining==0;}
 bool result(int winner){if(winner>=0&&winner<2)++wins[winner];return wins[0]>=target||wins[1]>=target;}
 static int timeout(int deaths0,int deaths1,int attacks0,int attacks1,int energy0,int energy1){
  if(deaths0!=deaths1)return deaths0<deaths1?0:1;
  if(attacks0!=attacks1)return attacks0>attacks1?0:1;
  if(energy0!=energy1)return energy0>energy1?0:1;
  return -1;
 }
};
}
