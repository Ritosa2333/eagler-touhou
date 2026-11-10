#pragma once
#include <array>
#include <algorithm>
#include <string>
namespace th19 {
// Enemy+0x63b8: eight 0x88-byte records. 0x50d080 / 0x508630 / 0x508850.
struct BossPhaseTriggers {
 struct Entry {int hp=-1,timeout=-1;std::string defeated,timed_out;};
 std::array<Entry,8> entries{};
 int clock=0;
 // 0x508630: first eligible timeout, integer seconds and hundredths at 60 Hz.
 int remaining_frames()const{
  for(const auto&e:entries)if(e.hp>=0&&e.timeout>0)return std::max(0,e.timeout-clock);
  return -1;
 }
 struct Result {int slot=-1,hp=0;bool timeout=false;std::string sub;explicit operator bool()const{return slot>=0;}};
 Result check(int hp,bool timeout)const{
  for(int i=0;i<8;++i){const auto&e=entries[i];if(e.hp<0||(timeout&&e.timeout<1))continue;
   // Only the first eligible slot is considered, even if a later slot is due.
   if(timeout?clock<e.timeout:hp>e.hp)return {};
   return {i,e.hp,timeout,timeout?e.timed_out:e.defeated};
  }return {};
 }
 void consume(const Result&r){entries[r.slot].hp=-1;clock=0;}
};
}
