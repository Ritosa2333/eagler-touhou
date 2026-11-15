#pragma once
#include <algorithm>
namespace th19 {
// Player+0x18, native 0x544be0 / 0x544c90 / 0x544dd0.
// 0x546320 calls 0x544dd0 every player tick. Restoration is timed by
// the configured recovery, or immediate on C3/C4 (0x40b420).
struct AutoBarrier {
 int state=1,elapsed=0,recovery=600,increase=300,invulnerable=0;
 void reset(int base=600,int extra=300){recovery=std::clamp(base,0,60000);increase=std::clamp(extra,0,60000);state=recovery?1:3;elapsed=invulnerable=0;}
 bool ready()const{return state==1;}
 bool hit(bool playerAlive=true,bool messageActive=false){
  if(!playerAlive||messageActive||invulnerable>0||!ready())return false;
  state=2;elapsed=0;invulnerable+=100;return true;
 }
 bool restore_from_charge(int tier){
  if((tier!=3&&tier!=4)||state!=2)return false;
  state=1;elapsed=0;recovery+=increase;return true;
 }
 bool tick(bool timedRecovery=true){
  if(invulnerable>0)--invulnerable;

  // 544dd0 checks elapsed before advancing; increases the next wait only
  // after restoration. 40b420 applies the same increase on charge recovery.
  if(timedRecovery&&state==2&&elapsed>=recovery){state=1;elapsed=0;recovery+=increase;return true;}
  ++elapsed;return false;
 }
};
}
