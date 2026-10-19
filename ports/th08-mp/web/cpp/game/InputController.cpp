// Original TH08 keyboard/pad mappings and shot-slow behavior.
#include "InputController.hpp"
namespace th08 {
void InputFrame::update(u16 buttons,bool sticky) noexcept {
    if(sticky){current|=buttons;return;}
    previous=current;current=buttons;scrolling=0;
    if(previous==current){if(held>=30){if(!(held%8))scrolling=1;if(held>=38)held=30;}++held;}else held=0;
}
u16 InputController::keyboard(const u8* keys,bool direct) noexcept {
    if(!keys)return 0;
    struct Key {u8 scan,virtual_key;u16 button;};
    static constexpr Key table[]{
        {200,38,Up},{208,40,Down},{203,37,Left},{205,39,Right},
        {72,104,Up},{80,98,Down},{75,100,Left},{77,102,Right},
        {71,103,Up|Left},{73,105,Up|Right},{79,97,Down|Left},{81,99,Down|Right},
        {199,36,Home},{25,80,Home},{32,68,D},{44,90,Shoot},{45,88,Bomb},
        {42,16,Focus},{54,16,Focus},{1,27,Menu},{29,17,Skip},{157,17,Skip},
        {16,81,Q},{31,83,S},{19,82,Reset},{28,13,Enter}
    };
    u16 result=0;for(const auto& k:table)if(keys[direct?k.scan:k.virtual_key]&128)result|=k.button;return result;
}
u16 InputController::direct_button(u16& output,i16 index,u16 mask,const u8* buttons) noexcept {const u16 result=index>=0&&index<128&&buttons&&(buttons[index]&128)?mask:0;output|=result;return result;}
u16 InputController::legacy_button(u16& output,i16 index,u16 mask,u32 buttons) noexcept {const u16 result=index>=0&&(buttons&(1u<<(index&31)))?mask:0;output|=result;return result;}
u16 InputController::controller(u16 output,const ControllerSnapshot& pad,const GameConfiguration& config) noexcept {
    if(!pad.available)return output;
    static constexpr u16 masks[]{Shoot,Bomb,Focus,Menu,Up,Down,Left,Right,Skip};
    const auto set=[&](u32 i){return pad.direct?direct_button(output,config.controller[i],masks[i],pad.buttons):legacy_button(output,config.controller[i],masks[i],pad.legacy_buttons);};
    const u16 shot=set(0);
    if(config.shot_slow){if(shot){if(focus_conflict<20)++focus_conflict;if(focus_conflict>=10)output|=Focus;}
        else if(focus_conflict>10){focus_conflict-=10;output|=Focus;}else focus_conflict=0;
    }
    for(u32 i=1;i<9;++i)set(i);
    if(pad.direct){if(pad.x>config.pad_x)output|=Right;if(pad.x<-i32(config.pad_x))output|=Left;if(pad.y>config.pad_y)output|=Down;if(pad.y<-i32(config.pad_y))output|=Up;}
    else{const u32 x_mid=(pad.x_min+pad.x_max)/2,x_range=(pad.x_max-pad.x_min)/2/2,y_mid=(pad.y_min+pad.y_max)/2,y_range=(pad.y_max-pad.y_min)/2/2;
        if(u32(pad.x)>x_mid+x_range)output|=Right;if(x_mid-x_range>u32(pad.x))output|=Left;if(u32(pad.y)>y_mid+y_range)output|=Down;if(y_mid-y_range>u32(pad.y))output|=Up;
    }
    return output;
}
void InputController::bindings(const ControllerSnapshot& pad,u8* output) noexcept {
    std::memset(output,0,128);if(!pad.available)return;if(pad.direct)std::memcpy(output,pad.buttons,128);else for(u32 i=0;i<32;++i)output[i]=(pad.legacy_buttons>>i)&1?128:0;
}
}
