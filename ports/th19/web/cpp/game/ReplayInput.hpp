#pragma once
#include "Types.hpp"
#include <array>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cmath>
namespace th19 {
// Little-endian RLE input ABI. Menu, dialogue and pause ticks are recorded too.
struct ReplayTick {u32 ticks=1;std::array<u32,16> keys{};float x=0,y=0;u32 direct=0;};
static_assert(sizeof(ReplayTick)==80);
class ReplayInput {
public:
 static constexpr u32 frame_limit=216000;
 std::vector<ReplayTick> records;
 std::vector<u8> transfer;
 bool recording=false,playing=false,limited=false;
 u32 frames=0,position=0,index=0,run=0;
 void begin(){records.clear();recording=true;playing=limited=false;frames=position=index=run=0;}
 void capture(const bool* physical,const u8* injected,float x,float y,bool direct){
  if(!recording||playing)return;
  if(frames>=frame_limit){recording=false;limited=true;return;}
  ReplayTick tick;for(unsigned i=0;i<512;i++)if(physical[i]||injected[i])tick.keys[i/32]|=1u<<(i%32);
  tick.x=std::isfinite(x)?std::clamp(x,-512.f,512.f):0;tick.y=std::isfinite(y)?std::clamp(y,-512.f,512.f):0;tick.direct=direct;
  if(!records.empty()&&records.back().keys==tick.keys&&records.back().x==tick.x&&records.back().y==tick.y&&records.back().direct==tick.direct)++records.back().ticks;
  else records.push_back(tick);
  ++frames;
 }
 bool load(u32 bytes){
  if(!bytes||bytes>transfer.size()||bytes%sizeof(ReplayTick)||bytes>frame_limit*sizeof(ReplayTick))return false;
  std::vector<ReplayTick> parsed(bytes/sizeof(ReplayTick));std::memcpy(parsed.data(),transfer.data(),bytes);
  u32 total=0;for(const auto&t:parsed){if(!t.ticks||t.ticks>frame_limit-total||t.direct>1||!std::isfinite(t.x)||!std::isfinite(t.y)||std::abs(t.x)>512||std::abs(t.y)>512)return false;total+=t.ticks;}
  records=std::move(parsed);frames=total;position=index=run=0;recording=limited=false;playing=true;return true;
 }
 bool next(bool* keys,u8* injected,float&x,float&y,bool&direct){
  if(!playing||position>=frames||index>=records.size())return false;
  const auto&t=records[index];for(unsigned i=0;i<512;i++)keys[i]=injected[i]=!!(t.keys[i/32]&(1u<<(i%32)));
  x=t.x;y=t.y;direct=t.direct;
  ++position;if(++run==t.ticks){run=0;++index;}return true;
 }
};
}
