// 653540和656F30/656F50的原写序；半径由输入值计算，保留未初始化槽与参数别名。
#pragma once
#include "core/frontend/MenuBootstrapContext.hpp"
#include "core/frontend/MenuBootstrapConstants.hpp"
#include "core/runtime/GameStartEntryMemory.hpp"
#include "core/math/RandomTable.hpp"
#include "core/math/BinaryFloat.hpp"
namespace kaikeidou {
namespace menu_bootstrap_detail {
inline std::uint64_t loaded_bits(std::uint64_t x){return (x&0x7ff0000000000000ull)==0x7ff0000000000000ull&&(x&0xfffffffffffffull)?x|0x8000000000000ull:x;}
inline std::uint64_t add_margin(std::uint64_t x,unsigned precision){x=loaded_bits(x);if((x&0x7ff0000000000000ull)==0x7ff0000000000000ull)return x;return binary_float::to_binary64(binary_float::add(binary_float::binary64(x),binary_float::integer(32),precision));}
template<class Memory>std::uint32_t read_word(Memory& m,std::uint32_t address,std::uint32_t pc){try{return m.word(address);}catch(const GameStartEntryAccessFault&){throw MenuBootstrapFault{pc,address,4,false};}}
template<class Memory>std::uint64_t read_real(Memory& m,std::uint32_t address,std::uint32_t pc){try{return m.quad(address);}catch(const GameStartEntryAccessFault&){throw MenuBootstrapFault{pc,address,8,false};}}
template<class Memory>void store_real(Memory& m,std::uint32_t address,std::uint64_t value,std::uint32_t pc){try{m.quad(address,value);}catch(const GameStartEntryAccessFault&){throw MenuBootstrapFault{pc,address,8,true};}}
template<class Memory,class Destination,class Value>void pair(Memory& m,Destination destination,Value value,unsigned precision,MenuBootstrapPairRegisters& regs){
 const auto first=add_margin(value(0x656f33),precision);regs.eax=destination(0x656f3c);regs.known|=1;store_real(m,regs.eax,first,0x656f3f);
 regs.ecx=destination(0x656f41);regs.known|=2;const auto second=loaded_bits(value(0x656f44));store_real(m,regs.ecx+8u,second,0x656f47);
}
}
template<class Memory>void initialize_menu_bootstrap_pair(Memory& memory,std::uint32_t destination_argument_address,
 std::uint32_t value_argument_address,unsigned precision,MenuBootstrapPairRegisters& regs){
 menu_bootstrap_detail::pair(memory,[&](std::uint32_t pc){return menu_bootstrap_detail::read_word(memory,destination_argument_address,pc);},[&](std::uint32_t pc){return menu_bootstrap_detail::read_real(memory,value_argument_address,pc);},precision,regs);
}
template<class Memory>void initialize_menu_bootstrap_random_table(Memory& memory){for(std::uint32_t i=0;i<1024;++i)memory.word(0xa77558u+i*4u,static_cast<std::uint32_t>(random_table[i]));}
template<class Memory>void initialize_menu_bootstrap_config_fields(Memory& memory,unsigned precision){
 const auto& first=menu_bootstrap_detail::settings_stores.front();memory.word(first.address,first.value);
 for(const auto& p:menu_bootstrap_detail::parameter_pairs){MenuBootstrapPairRegisters regs;menu_bootstrap_detail::pair(memory,[&](std::uint32_t){return p.destination;},[&](std::uint32_t){return p.bits;},precision,regs);}
 for(std::size_t i=1;i<menu_bootstrap_detail::settings_stores.size();++i){const auto& s=menu_bootstrap_detail::settings_stores[i];memory.word(s.address,s.value);}
}
}
