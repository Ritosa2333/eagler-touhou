#include "KeyboardInput.hpp"
namespace th09 {
u16 keyboard_input(const bool (&keys)[256],u32 device){
    struct Binding {u8 key;u16 mask;};
    constexpr Binding common[]={{13,4096},{17,256},{27,8},{36,2048},{68,8192},{80,2048},{81,512},{82,16384},{83,1024}};
    constexpr Binding all[]={{16,4},{37,64},{38,16},{39,128},{40,32},{88,2},{90,1},{97,96},{98,32},{99,160},{100,64},{102,128},{103,80},{104,16},{105,144}};
    constexpr Binding left[]={{16,4},{66,32},{70,64},{72,128},{78,160},{82,80},{84,16},{86,96},{88,2},{89,144},{90,1}};
    constexpr Binding right[]={{37,4},{39,2},{40,1},{97,96},{98,32},{99,160},{100,64},{102,128},{103,80},{104,16},{105,144}};
    u16 result=0;for(const auto& b:common)if(keys[b.key])result|=b.mask;
    const auto add=[&](const auto& list){for(const auto& b:list)if(keys[b.key])result|=b.mask;};
    if(device==2)add(all);else if(device==3)add(left);else if(device==4)add(right);return result;
}
void KeyboardInputBuffer::Key::event(bool down){
    if(held==down)return;
    held=down;
    // Alternating edges need only a count. Keep at most one complete stroke;
    // coalesce older pairs instead of playing a long burst after a stall.
    if(++pending>2)pending-=2;
}
void KeyboardInputBuffer::Key::sample(bool down,bool retain){
    event(down);
    if(!retain){captured=held;pending=0;}
    else if(pending){captured=!captured;--pending;}
}
void KeyboardInputBuffer::event(u32 key,bool down){
    if(key>=256)return;
    if(down&&!held_[key])pressed_[key]=true;
    held_[key]=down;
    if(key==90)keys_[0].event(down);else if(key==40)keys_[1].event(down);
}
void KeyboardInputBuffer::sample(const bool (&keys)[256],bool retain){
    for(u32 n=0;n<256;++n){
        event(n,keys[n]);
        // Only a stroke unseen by any capture needs a movement frame. A held
        // key was already consumed on its previous capture, so its ordinary
        // release stays immediate. Repeats and stalls cannot queue a tail.
        taps_[n]=retain&&pressed_[n]&&!keys[n];pressed_[n]=false;
    }
    keys_[0].sample(keys[90],retain);keys_[1].sample(keys[40],retain);
}
u16 KeyboardInputBuffer::input(const bool (&keys)[256],u32 device) const {
    // Map taps through the selected device: arrows may mean movement, focus,
    // bomb or fire. Only directional bits receive this one-frame protection.
    const u16 input=keyboard_input(keys,device)|(keyboard_input(taps_,device)&0xf0);
    if(device==2||device==3)return (input&~u16(1))|u16(keys_[0].captured);
    if(device==4)return (input&~u16(1))|u16(keys_[1].captured);
    return input;
}
}
