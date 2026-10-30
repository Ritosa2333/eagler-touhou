#include "TitleCheat.hpp"
namespace th17 {
bool TitleCheat::update(u32 pressed,const TitleKeyboard& input,bool& unlocked,bool trophies){
 unlocked=false;if(!error.empty())return false;if(pressed&0x80103)sequence=idle=0;previous=current;
 if(input.format==1)current=input.keys;
 else if(input.format==2){constexpr u8 scans[]={30,48,46,32,18,33,34,35,23,36,37,38,50,49,24,25,16,19,31,20,22,47,17,45,21,44};current.fill(0);for(u32 i=0;i<26;i++)current[scans[i]]=input.keys[65+i];}
 else current.fill(0);
 if(input.format==1||input.format==2){
  std::array<u8,256> edge{};for(u32 i=0;i<256;i++)edge[i]=u8((previous[i]^current[i])&current[i]);
  constexpr u8 data_code[]={35,18,30,19,20,38,30,49,32};
  constexpr u8 trophy_code[]={50,30,31,20,18,19,31,32,19,18,30,50};
  const u8* code=trophies?trophy_code:data_code;const i32 size=trophies?12:9;
  if(sequence<0||sequence>size){error="Title unlock sequence outside original range";return false;}
  if(sequence<size){if(edge[code[sequence]]&0x80){sequence++;idle=0;}else{u8 any=0;for(u32 i=0;i<57;i++)any|=edge[i];if(any&0x80)sequence=0;}}
  else {if(!records.unlock_all()){error=records.error;return false;}unlocked=true;sequence=0;}
 }
 idle=wrapping_add(idle,1);if(idle>300)sequence=idle=0;return true;
}
}
