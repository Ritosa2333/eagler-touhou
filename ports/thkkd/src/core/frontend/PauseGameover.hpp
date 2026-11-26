// 完整5E3630状态机与Replay直接桥；已知错位跳转按原地址和值保留。
#pragma once
#include "core/frontend/PauseGameoverContext.hpp"
#include "core/frontend/PauseGameoverServices.hpp"
#include "core/frontend/PauseGameoverConstants.hpp"
#include <string>
namespace kaikeidou {
namespace pause_gameover_detail {
template<class Memory>inline std::int32_t input(Memory& memory,PauseGameoverServices& api,std::uint32_t address){return api.physical_input_count(memory.word(address));}
template<class Memory>inline void navigate(Memory& m,PauseGameoverServices& api,bool three,bool replay_variant){
 if(input(m,api,0xa733e4)==1){const auto first=three&&replay_variant?m.word(0xb43c00):0u;m.word(0xa87f50,m.word(0xa87f50)==first?(three?2u:1u):m.word(0xa87f50)-1u);m.word(0xa73238,1);}
 if(input(m,api,0xa733e8)==1){if(m.word(0xa87f50)==(three?2u:1u))m.word(0xa87f50,three&&replay_variant?m.word(0xb43c00):0u);else m.word(0xa87f50,m.word(0xa87f50)+1u);m.word(0xa73238,1);}
}
template<class Memory>inline void attempt(Memory& m,bool replay_variant,bool guard_normal,bool guard_extra){
 if(m.word(0xa7ea98)!=2)return;const bool extra=m.word(0xa7e610)==5;
 const auto address=extra?0xa81f84u+m.word(0xa74c0c)*0x3c0u+m.word(0xa7e614)*0x140u+m.word(0xa7eba4)*8u:0xa7f74cu+m.word(0xa74c0c)*0xf00u+m.word(0xa7e614)*0x500u+m.word(0xa7eba4)*8u;
 const auto value=m.word(address)+1u;
 if(!(replay_variant&&(extra?guard_extra:guard_normal)&&m.word(0xb4310c)==1))m.word(address,value);
}
template<class Memory>inline void title(Memory& m,PauseGameoverServices& api,bool replay_variant,bool metadata){
 if(replay_variant&&metadata)m.word(0xb43255,m.word(0xb43a04)==1?2u:1u);else m.word(0xa8ce58,10);
 api.return_title();api.initialize_title();
}
template<class Memory>inline void heading(Memory& m,PauseGameoverServices& api,std::uint32_t address){const auto font=m.word(0xa74bdc);const auto ink=api.color(255,255,255);if(address==0xb43a10){std::string value;for(auto at=address;;++at){const auto c=m.byte(at);if(!c)break;value.push_back(static_cast<char>(c));}api.draw_font_text(50,260,value,ink,font,0,0);}else api.draw_font_text(50,260,text(address),ink,font,0,0);}
template<class Memory>inline void item(Memory& m,PauseGameoverServices& api,std::uint32_t index,std::uint32_t address){const auto ink=m.word(0xa87f50)==index?api.color(255,255,140):api.color(130,130,130);const auto font=m.word(0xa74bdc);api.draw_font_text(100,static_cast<std::int32_t>(300+index*40),text(address),ink,font,0,0);}
template<class Memory>inline void shade(Memory& m,PauseGameoverServices& api,std::uint32_t alpha,bool purple_by_scene){api.blend(3,alpha);const auto ink=purple_by_scene&&m.word(0xa78e2c)!=3?api.color(255,0,255):api.color(0,255,255);api.rectangle(0,0,640,480,ink,1);api.blend(0,255);}
}
// Memory访问失败沿其typed fault传播，具体RuntimeAdapter封装成显式Outcome。
template<class Memory>inline std::uint32_t update_pause_gameover(Memory& m,PauseGameoverServices& api,bool replay_variant,PauseGameoverAbi& abi){
 using namespace pause_gameover_detail;const auto scene=m.word(0xa78e2c);
 if(scene==0){abi.eax=api.enter_scene_zero()?1u:0u;return abi.eax;}
 if(scene>6){abi.eax=0;return 0;}
 if(scene==1){
  switch(m.word(0xa87f48)){
  case 0:
   navigate(m,api,true,replay_variant);
   if((input(m,api,0xa733ec)==1&&m.word(0xa87f50)==0)||input(m,api,0xa733f0)==1||input(m,api,0xa733f8)==1){api.resume_background(0);api.resume_music(m.word(0xa78e28),0);m.word(0xa78e2c,0);}
   else if(input(m,api,0xa733ec)==1){if(m.word(0xa87f50)==1){m.word(0xa87f48,1);m.word(0xa87f4c,0);}else if(m.word(0xa87f50)==2){m.word(0xa87f48,2);m.word(0xa87f4c,0);}}break;
  case 1:m.word(0xa87f4c,m.word(0xa87f4c)+1u);if(m.word(0xa87f4c)==20){attempt(m,replay_variant,true,true);title(m,api,replay_variant,true);}break;
  case 2:m.word(0xa87f4c,m.word(0xa87f4c)+1u);if(m.word(0xa87f4c)==20){attempt(m,replay_variant,true,true);abi.eax=api.initialize_game(0);}break;
  default:break;
  }
  api.present_gameplay();heading(m,api,0x6c6d80);item(m,api,0,0x6c6d8c);item(m,api,1,0x6c6da0);item(m,api,2,0x6c6db0);abi.eax=1;return 1;
 }
 const bool three=scene==2;const bool purple=scene==3||scene==4;const bool challenge=scene==5||scene==6;
 switch(m.word(0xa87f48)){
 case 0:
  api.pause_background();api.present_gameplay();shade(m,api,m.word(0xa87f4c)<<1,purple);m.word(0xa87f4c,m.word(0xa87f4c)+1u);
  if(m.word(0xa87f4c)==90){m.word(0xa87f48,1);m.word(0xa87f4c,0);if(three){const auto address=0xa7ec0cu+m.word(0xa7e610)*36u+m.word(0xa74c0c)*12u+m.word(0xa7e614)*4u;m.word(address,m.word(0xa8cd78));api.store_scores_settings();}}
  abi.eax=1;return 1;
 case 1:
  navigate(m,api,three,replay_variant);
  if(input(m,api,0xa733ec)==1){
   if(three){
    if(m.word(0xa87f50)==0){
     abi.eax=api.initialize_game(1);
     if(replay_variant){
      if(m.word(0xb3e2ec)==6){++abi.edi;abi.eax+=0xb43a00u;abi.instruction=0xb42948;m.word(abi.eax,m.word(abi.eax)+abi.eax);abi.instruction=0xb4294a;m.byte(abi.eax,static_cast<std::uint8_t>(m.byte(abi.eax)+static_cast<std::uint8_t>(abi.eax)));abi.instruction=0;}
      else m.word(0xb43a00,1);
     }
     m.word(0xa8cd8c,m.word(0xa8cd8c)+1u);m.word(0xa8cd70,m.word(0xb3e2ec)!=0?400u:200u);
     if(m.word(0xb3e2ec)==6){m.word(0xa8cd7c,2);m.word(0xa8cd80,2);}else{m.word(0xa8cd7c,4);m.word(0xa8cd80,4);}
    }else if(m.word(0xa87f50)==1){m.word(0xa87f48,2);m.word(0xa87f4c,0);}else if(m.word(0xa87f50)==2){m.word(0xa87f48,3);m.word(0xa87f4c,0);}
   }else{if(m.word(0xa87f50)==0){m.word(0xa87f48,2);m.word(0xa87f4c,0);}else if(m.word(0xa87f50)==1){m.word(0xa87f48,3);m.word(0xa87f4c,0);}}
  }break;
 case 2:
  m.word(0xa87f4c,m.word(0xa87f4c)+1u);
  if(m.word(0xa87f4c)==20){attempt(m,replay_variant,false,challenge);if(challenge){abi.eax=api.initialize_game(0);m.word(0xa8cd8c,m.word(0xa8cd8c)+1u);}else title(m,api,replay_variant,true);}break;
 case 3:
  m.word(0xa87f4c,m.word(0xa87f4c)+1u);
  if(m.word(0xa87f4c)==20){attempt(m,replay_variant,false,false);if(challenge)title(m,api,replay_variant,false);else abi.eax=api.initialize_game(0);}break;
 default:break;
 }
 api.present_gameplay();shade(m,api,180,purple);
 if(three){heading(m,api,0x6c6dc4);item(m,api,0,0x6c6dd0);item(m,api,1,0x6c6de4);item(m,api,2,0x6c6df4);}
 else if(purple){heading(m,api,m.word(0xa78e2c)==3?0x6c6e08:replay_variant?0xb43a10:0x6c6e14);item(m,api,0,0x6c6e20);item(m,api,1,0x6c6e30);}
 else{heading(m,api,m.word(0xa78e2c)==3?0x6c6e44:0x6c6e50);item(m,api,0,0x6c6e58);item(m,api,1,0x6c6e64);}
 abi.eax=1;return 1;
}
}
