// B424AC的局部栈缓冲与已提供参数字；缺失字保留为可诊断的访问故障。
#pragma once
#include "core/frontend/MenuReplayRuntime.hpp"
#include <array>
#include <algorithm>
namespace kaikeidou {
template<class Memory>class MenuReplayFormatFrame:public MenuReplayFormatMemory {
 Memory& memory_;std::uint32_t target_,arguments_;std::vector<std::uint32_t> arguments_words_;
public:
 std::array<std::uint8_t,2048> output{};std::size_t written=0;
 std::vector<MenuReplayWrite> writes;
 MenuReplayFormatFrame(Memory& m,std::uint32_t target,const std::vector<std::uint32_t>& args):memory_(m),target_(target),arguments_(target+0x814u),arguments_words_(args){}
 std::uint32_t target()const{return target_;}std::uint32_t arguments()const{return arguments_;}
 std::uint8_t byte(std::uint32_t address)override{
  const auto offset=address-target_;if(offset<2048u){if(offset>=written)throw MenuReplayFormatFault(MenuReplayFormatFaultKind::address_fault,address,written);return output[offset];}
  const auto arg_offset=address-arguments_;if(arg_offset<arguments_words_.size()*4u)return static_cast<std::uint8_t>(arguments_words_[arg_offset/4u]>>(8*(arg_offset%4u)));
  return memory_.byte(address);
 }
 std::uint32_t argument(std::uint32_t address)override{
  const auto offset=address-arguments_;if(offset%4u||offset/4u>=arguments_words_.size())throw MenuReplayFormatFault(MenuReplayFormatFaultKind::unavailable_argument,address,written);return arguments_words_[offset/4u];
 }
 void byte(std::uint32_t address,std::uint8_t value)override{
  const auto offset=address-target_;if(offset>=2048u)throw MenuReplayFormatFault(MenuReplayFormatFaultKind::target_overflow,address,written);output[offset]=value;writes.push_back({address,{value}});written=std::max(written,std::size_t(offset)+1);
 }
};
template<class Memory>void format_menu_replay_wrapped(MenuReplayRuntime<Memory>& runtime,std::uint32_t x,std::uint32_t y,std::uint32_t ink,std::uint32_t format,const std::vector<std::uint32_t>& arguments,std::uint32_t stack_target=0x0ff00000){
 runtime.memory.word(0xb43264,runtime.registers.ecx);MenuReplayFormatFrame<Memory> frame(runtime.memory,stack_target,arguments);
 const MenuReplayFormatRequest request{format,frame.arguments(),frame.target(),2048};
 try{runtime.accept(runtime.services.format_x86(frame,request));}catch(MenuReplayFormatFault& fault){fault.stack_writes=frame.writes;throw;}
 runtime.call(MenuReplayApi::prepare_default_font,{},&frame);const auto font=runtime.memory.word(0x8b63f4);
 runtime.call(MenuReplayApi::font_text,{x,y,frame.target(),ink,font,0,0},&frame);runtime.registers.ecx=runtime.memory.word(0xb43264);
}
}
