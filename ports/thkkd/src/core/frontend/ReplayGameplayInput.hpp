// 原6604C0输入前缀与Memory移动尾部；保留服务之间的即时字段写入。
#pragma once
#include "core/player/PlayerMovement.hpp"
#include "core/player/PlayerShotFireContext.hpp"
#include "core/shared/DialogueContext.hpp"
namespace kaikeidou {
struct ReplayGameplayInputTransfer {
 std::uint32_t entry=0xb42382,target=0x924682,source=0xb423a4,binding=0,caller_return=0,inherited_eax=0;
};
struct ReplayGameplayInputOutcome {
 MovementInput input;
 bool dead_return=false,played=false,eof_metadata=false;
};
template<class Memory>void bind_replay_gameplay_input(Memory& memory,PlayerShotFireContext& fire,DialogueContext& dialogue){
 memory.bind(0xb43108,fire.replay_buttons);memory.bind(0xb43108,dialogue.replay_buttons);
}
namespace replay_gameplay_input_detail {
inline bool positive(std::uint32_t value){return value!=0&&!(value>>31);}
inline bool above_39(std::uint32_t value){return value>39&&!(value>>31);}
inline double real(std::uint64_t value){double out;std::memcpy(&out,&value,8);return out;}
template<class Memory>std::uint32_t counter(Memory& memory,PlayerShotFireInput& input,std::uint32_t binding,bool replay,std::uint32_t return_site,std::uint32_t inherited_eax){
 if(replay&&binding>=16)throw ReplayGameplayInputTransfer{0xb42382,0x924682,0xb423a4,binding,return_site,inherited_eax};
 memory.reconcile();const auto raw=input.read_counter(binding);memory.reconcile();
 if(replay)return raw!=0?1u:0u;return binding<16?raw:0xffffffffu;
}
template<class Memory>void movement_tail(Memory& memory,const MovementInput& input,unsigned precision){
 const auto& keys=input.directions;const int horizontal=keys[0]?-1:keys[1]?1:0,vertical=keys[2]?-1:keys[3]?1:0;
 const auto speed=positive(input.slow)?(horizontal&&vertical?0xa8ce30u:0xa8ce28u):(horizontal&&vertical?0xa8ce20u:0xa8ce18u);
 const auto move=[&](std::uint32_t address,int direction){const auto position=memory.quad(address);auto delta=memory.quad(speed);if(direction<0)delta^=std::uint64_t(1)<<63;memory.quad(address,binary_float::to_binary64(binary_float::add(binary_float::binary64(position),binary_float::binary64(delta),precision)));};
 if(horizontal)move(0xa8cdf8,horizontal);if(vertical)move(0xa8ce00,vertical);
 if(real(memory.quad(0xa8cdf8))<8.)memory.quad(0xa8cdf8,0x4020000000000000ull);
 else if(real(memory.quad(0xa8cdf8))>376.)memory.quad(0xa8cdf8,0x4077800000000000ull);
 if(real(memory.quad(0xa8ce00))<16.)memory.quad(0xa8ce00,0x4030000000000000ull);
 else if(real(memory.quad(0xa8ce00))>432.&&(memory.word(0xa8cd68)!=2||above_39(memory.word(0xa8cd6c))))memory.quad(0xa8ce00,0x407b000000000000ull);
}
}
template<class Memory>ReplayGameplayInputOutcome update_replay_gameplay_movement(Memory& memory,PlayerShotFireInput& physical,bool replay_variant,unsigned precision){
 using namespace replay_gameplay_input_detail;memory.reconcile();ReplayGameplayInputOutcome result;auto& keys=result.input.directions;
 const auto left_binding=memory.word(0xa733dc);keys[0]=counter(memory,physical,left_binding,replay_variant,0x6604d1,left_binding);
 const auto right_binding=memory.word(0xa733e0);keys[1]=counter(memory,physical,right_binding,replay_variant,0x6604e3,keys[0]);
 const auto up_binding=memory.word(0xa733e4);keys[2]=counter(memory,physical,up_binding,replay_variant,0x6604f5,keys[1]);
 const auto down_binding=memory.word(0xa733e8);keys[3]=counter(memory,physical,down_binding,replay_variant,0x660506,down_binding);
 if(replay_variant){
  if(memory.word(0xb4310c)==0){memory.word(0xb43104,0);auto value=memory.word(0xb43104);value+=keys[3];value+=keys[0]*2u;value+=keys[1]*4u;value+=keys[2]*8u;memory.word(0xb43108,value);}
  else{
   result.played=true;const auto cursor=memory.word(0xb43110);
   if(cursor==memory.word(0xb43a08)){memory.word(0xa78e2c,12);memory.word(0xb43a10,0xc5b7a5b2);result.eof_metadata=true;}
   const auto current=memory.word(0xb43110),buffer=memory.word(0xb43100);const auto value=(right_binding&0xffffff00u)|memory.byte(buffer+current);memory.word(0xb43108,value);
   keys[3]=value&1u;keys[0]=(value>>1)&1u;keys[1]=(value>>2)&1u;keys[2]=(value>>3)&1u;
  }
 }
 if(memory.word(0xa8cd68)==1){result.dead_return=true;return result;}
 if(keys[0]&&keys[1])keys[0]=keys[1]=0;if(keys[2]&&keys[3])keys[2]=keys[3]=0;
 if(positive(keys[0]))memory.word(0xa8cdf0,memory.word(0xa8cdf0)+8u);else if(positive(keys[1]))memory.word(0xa8cdf0,memory.word(0xa8cdf0)+4u);
 const auto slow_binding=memory.word(0xa733f4);result.input.slow=counter(memory,physical,slow_binding,replay_variant,0x660585,slow_binding);
 if(replay_variant){if(memory.word(0xb4310c)==0){const auto value=memory.word(0xb43108);memory.word(0xb43108,value|result.input.slow*128u);}else result.input.slow=(memory.word(0xb43108)>>7)&1u;}
 movement_tail(memory,result.input,precision);return result;
}
}
