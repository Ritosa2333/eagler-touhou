#include "GameInput.hpp"
namespace th11 {
u32 keyboard_keys(const bool* keys)noexcept{
    // 4576b0's virtual-key path, including numpad diagonals and menu shortcuts.
    constexpr u32 bindings[][2]={{13,0x80000},{16,8},{17,512},{27,256},{36,0x40000},{37,64},{38,16},{39,128},{40,32},{67,4},{68,0x100000},{80,0x40000},{81,0x10000},{82,0x200000},{83,0x20000},{88,2},{90,1},{97,96},{98,32},{99,160},{100,64},{102,128},{103,80},{104,16},{105,144},{121,0x800000}};
    u32 result=0;for(const auto& binding:bindings)if(keys[binding[0]])result|=binding[1];return result;
}
u32 controller_keys(u32 held,const u8* buttons,u32 count,i32 x,i32 y,const u8* config)noexcept{
    const auto word=[&](u32 offset){i16 value;std::memcpy(&value,config+offset,2);return value;};
    constexpr u32 indices[]={0,1,2,3,8},masks[]={1,2,8,256,512};
    for(u32 i=0;i<5;++i){const i32 button=word(4+indices[i]*2);if(button>=0&&u32(button)<count&&(buttons[button]&128))held|=masks[i];}
    const i32 horizontal=word(0x16),vertical=word(0x18);
    if(x<-horizontal)held|=64;if(x>horizontal)held|=128;
    if(y<-vertical)held|=16;if(y>vertical)held|=32;return held;
}
void GameInput::update(u32 keys,bool auto_focus)noexcept{
    previous=held;held=keys;
    if(auto_focus){
        if(keys&8)held|=0x400;
        if(!(held&1))auto_focus_frames=0;
        else if(++auto_focus_frames>7){held|=8;auto_focus_frames=8;}
    }else if((keys&9)==9&&counts[0]<5&&counts[3]<5)held|=0x400;
    long_repeat=short_repeat=0;
    for(u32 bit=0;bit<32;++bit){const u32 mask=1u<<bit;if(!(held&mask))counts[bit]=0;else{
        ++counts[bit];if(counts[bit]>=8)short_repeat|=mask;
        if(counts[bit]>=26){long_repeat|=mask;counts[bit]-=8;}
    }}
    pressed=(held^previous)&held;released=(held^previous)&~held;
}
}
