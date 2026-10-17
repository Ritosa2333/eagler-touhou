#include "GameKeyboard.hpp"
namespace th15 {
u32 keyboard_keys(const bool* keys)noexcept{
 if(!keys)return 0;u32 value=0;
 constexpr std::pair<u32,u32> bindings[]={{90,1},{88,2},{16,8},{38,16},{104,16},{40,32},{98,32},{37,64},{100,64},{39,128},{102,128},{97,0x60},{99,0xa0},{103,0x50},{105,0x90},{27,0x100},{17,0x200},{67,0xa00},{81,0x10000},{83,0x20000},{36,0x40000},{80,0x40000},{13,0x80000},{68,0x100000},{82,0x200000},{121,0x800000}};
 for(const auto& binding:bindings)if(keys[binding.first])value|=binding.second;return value;
}
}
