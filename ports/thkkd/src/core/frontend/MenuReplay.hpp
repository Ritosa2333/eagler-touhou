// Replay列表、命名与保存询问；各分支读取原共享地址并保留回调后的重读。
#pragma once
#include "core/frontend/MenuReplayFormatWrapper.hpp"
namespace kaikeidou {
namespace menu_replay_detail {
inline std::int64_t signed32(std::uint32_t n){return n>>31?std::int64_t(n)-0x100000000ll:std::int64_t(n);}
}
template<class Memory>class MenuReplay:public MenuReplayRuntime<Memory> {
 using Base=MenuReplayRuntime<Memory>;
public:
 using Base::Base;using Base::memory;using Base::registers;using Base::call;using Base::input;using Base::volume;
 MenuReplaySaveOutcome last_save{};
 std::uint32_t selection_color(std::uint32_t current,std::uint32_t selected){return current==selected?call(MenuReplayApi::color,{255,255,255}):call(MenuReplayApi::color,{128,128,128});}
 void formatted(std::uint32_t x,std::uint32_t y,std::uint32_t ink,std::uint32_t format,const std::vector<std::uint32_t>& varargs={}){
  std::vector<std::uint32_t> args{x,y,ink,format};args.insert(args.end(),varargs.begin(),varargs.end());call(MenuReplayApi::formatted_text,args);
 }
 void select_replay(){
  registers.eax=memory.word(0xa87f40);--registers.eax;if(menu_replay_detail::signed32(registers.eax)<0)registers.eax=12;
  registers.eax=memory.word(0xb43505+registers.eax*4u);if(memory.byte(registers.eax)=='-')return;
  memory.word(0xa8ce58,99);memory.word(0xb4310c,1);registers.eax=memory.word(0xa87f40);memory.word(0xb43940,registers.eax);
 }
 void legacy_list_cancel(){memory.word(0xb43056,0xf1cfbcc2);memory.word(0xb4303e,0);memory.word(0xb4305a,0);volume();memory.word(0xa87f48,1);memory.word(0xa87f4c,0);memory.word(0xa73234,1);}
 void list(){
  call(MenuReplayApi::set_font_size,{21});
  for(std::uint32_t row=0;row<13;++row){const auto ink=selection_color(memory.word(0xa87f40),row);registers.ecx=row;const auto y=row*28u+69u;
   format_menu_replay_wrapped(*this,53,y,ink,0xb4300c,{});format_menu_replay_wrapped(*this,96,y,ink,0x6c7ab0,{row});
   auto text=memory.word(0xb43501+registers.ecx*4u);format_menu_replay_wrapped(*this,128,y,ink,text,{});
   text=memory.word(0xb43535+registers.ecx*4u);format_menu_replay_wrapped(*this,320,y,ink,text,{});
   text=memory.word(0xb43569+registers.ecx*4u);format_menu_replay_wrapped(*this,432,y,ink,text,{});
  }
  call(MenuReplayApi::set_font_size,{16});const auto phase=memory.word(0xa87f48);
  if(phase==0){
   if(input(0xa733e4)==1){auto selected=memory.word(0xa87f40);memory.word(0xa87f40,selected==0?12:selected-1u);memory.word(0xa73238,1);}
   if(input(0xa733e8)==1){auto selected=memory.word(0xa87f40);memory.word(0xa87f40,selected==12?0:selected+1u);memory.word(0xa73238,1);}
   if(input(0xa733f8)==1){
    if(memory.word(0xb4303e)!=1)this->free_buffer();memory.word(0xb43056,0xf1cfbcc2);memory.word(0xb4303e,0);memory.word(0xb43255,0);memory.word(0xb4305a,0);
    volume();memory.word(0xa87f48,1);memory.word(0xa87f4c,0);memory.word(0xa73234,1);return;
   }
   if(input(0xa733ec)==1){if(memory.word(0xb4303e)!=1)select_replay();else{memory.word(0xb41215,2);registers.eax=memory.word(0xa87f40);memory.word(0xa87f40,0);memory.word(0xb43042,registers.eax);}}
  }else if(phase==1){memory.word(0xa87f4c,memory.word(0xa87f4c)+1u);if(memory.word(0xa87f4c)==20){memory.word(0xb41215,0);memory.word(0xa87f40,0);call(MenuReplayApi::return_title,{});}}
 }
 void name_action(){
  registers.eax=memory.word(0xa87f40);registers.ecx=memory.word(0xb43046);
  if(registers.eax==56){registers.eax=(registers.eax&0xffffff00u)|95u;memory.byte(0xb43015+registers.ecx,static_cast<std::uint8_t>(registers.eax));}
  else if(menu_replay_detail::signed32(registers.eax)<84){
   if(menu_replay_detail::signed32(registers.eax)<26)registers.eax+=65u;else if(menu_replay_detail::signed32(registers.eax)<52)registers.eax+=71u;else registers.eax-=19u;
   memory.byte(0xb43015+registers.ecx,static_cast<std::uint8_t>(registers.eax));
  }else if(registers.eax==84){backspace();return;}
  else{
   if(memory.byte(0xb4304a)==7)++registers.ecx;memory.byte(0xb43015+registers.ecx,0);last_save=this->save();return;
  }
  memory.word(0xb4304a,registers.ecx);++registers.ecx;if(registers.ecx==8)registers.ecx=7;memory.word(0xb43046,registers.ecx);
 }
 void backspace(){registers.ecx=memory.word(0xb4304a);memory.byte(0xb43015+registers.ecx,'-');memory.word(0xb43046,registers.ecx);--registers.ecx;if(registers.ecx==0xffffffffu)registers.ecx=0;memory.word(0xb4304a,registers.ecx);}
 void name(){
  call(MenuReplayApi::set_font_size,{21});
  for(std::uint32_t index=0;index<84;++index){const auto ink=selection_color(memory.word(0xa87f40),index);const auto column=index/13u;const auto x=index*28u+53u-column*364u;const auto y=column*28u+240u;
   const auto character=index==56?95u:index<26?index+65u:index<52?index+71u:index-19u;formatted(x,y,ink,0xb43024,{character});
  }
  auto ink=selection_color(memory.word(0xa87f40),84);formatted(288,408,ink,0xb43029);ink=selection_color(memory.word(0xa87f40),85);formatted(357,408,ink,0xb4302c);
  formatted(45,121,0xffff8080,0xb4300c);const auto slot=memory.word(0xb43042);formatted(96,121,0xffff8080,0x6c7ab0,{slot});
  auto text=memory.word(0xb43259);formatted(320,121,0xffff8080,text);text=memory.word(0xb4325d);formatted(432,121,0xffff8080,text);formatted(128,121,0xffff8080,0xb43015);
  call(MenuReplayApi::set_font_size,{16});const auto phase=memory.word(0xa87f48);
  if(phase==1){memory.word(0xa87f4c,memory.word(0xa87f4c)+1u);if(memory.word(0xa87f4c)==20){memory.word(0xb41215,0);memory.word(0xa87f40,0);call(MenuReplayApi::return_title,{});}return;}
  if(phase!=0)return;
  if(input(0xa733dc)==1){const auto selected=memory.word(0xa87f40);memory.word(0xa87f40,selected==0?85:selected-1u);memory.word(0xa73238,1);}
  if(input(0xa733e0)==1){const auto selected=memory.word(0xa87f40);memory.word(0xa87f40,selected==85?0:selected+1u);memory.word(0xa73238,1);}
  if(input(0xa733f0)==1){backspace();return;}
  if(input(0xa733f8)==1){volume();memory.word(0xb41215,1);memory.word(0xa87f40,0);return;}
  if(input(0xa733ec)==1){name_action();return;}
  if(input(0xa733e4)==1){auto value=memory.word(0xa87f40);const auto selected=menu_replay_detail::signed32(value);
   if(selected<13){if(selected>=10)value=85;else if(selected>=6)value=84;else value+=78u;}else value-=13u;
   memory.word(0xa87f40,value);memory.word(0xa73238,1);
  }
  if(input(0xa733e8)==1){auto value=memory.word(0xa87f40);const auto selected=menu_replay_detail::signed32(value);
   if(selected>=72){if(selected<=74)value=84;else if(selected<=77)value=85;else value-=78u;}else value+=13u;
   memory.word(0xa87f40,value);memory.word(0xa73238,1);
  }
 }
 void reject_save(){
  call(MenuReplayApi::return_title,{});memory.word(0xb43056,0xf1cfbcc2);registers.eax=0;memory.word(0xb4305a,0);memory.word(0xb4303e,0);memory.word(0xb41215,0);this->free_buffer();memory.word(0xb43255,0);
 }
 void accept_save(){
  registers.edx=memory.word(0xa7e614);registers.eax=memory.word(0xa74c0c);registers.eax=registers.eax*3u+registers.edx;registers.eax=memory.word(0xb43178+registers.eax*4u);memory.word(0xb43259,registers.eax);
  if(memory.word(0xb3e2ec)==6)memory.word(0xb4325d,0xb431a5);
  else if(memory.word(0xb3e2ec)==7){memory.word(0xb4325d,0xb4319c);registers.eax=memory.word(0xa77390)+49u;memory.word(0xb431a1,registers.eax);}
  else{
   registers.eax=memory.word(0xa7e610);
   if(memory.word(0xb43255)==0){registers.eax=memory.word(0xb431d2+registers.eax*4u);memory.word(0xb4325d,registers.eax);}
   else{
    registers.ecx=memory.word(0xb43218+registers.eax*4u);memory.word(0xb4325d,registers.ecx);registers.edx=memory.word(0xb3e2ec)+49u;
    if(registers.eax==1)registers.eax+=6u;else if(registers.eax==3)registers.eax+=5u;else registers.eax=5;
    memory.word(registers.ecx+registers.eax,registers.edx);
    if(memory.word(0xb43255)==2){++registers.eax;memory.word(registers.ecx+registers.eax,112);}
   }
  }
  registers.eax=0;memory.word(0xb43081,0);++registers.eax;memory.word(0xb41215,registers.eax);
 }
 void prompt(){
  call(MenuReplayApi::set_font_size,{30});auto ink=selection_color(memory.word(0xb43081),0);formatted(252,255,ink,0xb43075);
  ink=memory.word(0xb43081)==1?call(MenuReplayApi::color,{255,255,255}):call(MenuReplayApi::color,{128,128,64});formatted(335,255,ink,0xb4307b);formatted(255,223,0xffff0080,0xb43064);call(MenuReplayApi::set_font_size,{16});
  if(input(0xa733e0)==1){registers.eax=memory.word(0xb43081)+1u;if(registers.eax==2)registers.eax=0;memory.word(0xb43081,registers.eax);return;}
  if(input(0xa733dc)==1){registers.eax=memory.word(0xb43081)-1u;if(registers.eax==0xffffffffu)registers.eax=1;memory.word(0xb43081,registers.eax);return;}
  if(input(0xa733f0)==1){reject_save();return;}if(input(0xa733f8)==1){reject_save();return;}
  if(input(0xa733ec)==1){if(memory.word(0xb43081)!=0)reject_save();else accept_save();}
 }
 void route(){
  if(memory.word(0xb41215)==1){call(MenuReplayApi::help,{0xb43050,0xb411ce});list();this->scan();}
  else if(memory.word(0xb41215)==2){call(MenuReplayApi::help,{0xb43030,0xb411ce});name();}
  else if(memory.word(0xb41215)==3){call(MenuReplayApi::help,{0xb43064,0xb411ce});prompt();}
  else{call(MenuReplayApi::help,{0x6c7a90,0x6c7a88});call(MenuReplayApi::options_body,{});}
 }
};
}
