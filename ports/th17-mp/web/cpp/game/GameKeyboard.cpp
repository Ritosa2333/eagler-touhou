#include "GameKeyboard.hpp"
#include <utility>
namespace th17 {
u32 keyboard_keys(const bool* keys)noexcept{
 if(!keys)return 0;
 constexpr std::pair<u8,u32> bindings[]={{13,0x80000},{16,8},{17,0x200},{27,0x100},{36,0x40000},{37,0x40},{38,0x10},{39,0x80},{40,0x20},{67,0x200},{68,0x100000},{80,0x40000},{81,0x10000},{82,0x200000},{83,0x20000},{88,2},{90,1},{97,0x60},{98,0x20},{99,0xa0},{100,0x40},{102,0x80},{103,0x50},{104,0x10},{105,0x90},{121,0x800000}};
 u32 result=0;for(const auto& binding:bindings)if(keys[binding.first])result|=binding.second;return result;
}
}
