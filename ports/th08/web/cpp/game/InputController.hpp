#pragma once
#include "GameConfiguration.hpp"
namespace th08 {
enum InputButton : u16 {Shoot=1,Bomb=2,Focus=4,Menu=8,Up=16,Down=32,Left=64,Right=128,Skip=256,Q=512,S=1024,Home=2048,Enter=4096,D=8192,Reset=16384};
struct ControllerSnapshot {
    i32 x=0,y=0;
    u32 legacy_buttons=0,x_min=0,x_max=65535,y_min=0,y_max=65535;
    u8 buttons[128]{};
    bool available=false,direct=true;
};
struct InputFrame {
    u16 current=0,previous=0,scrolling=0,held=0;
    void update(u16 buttons,bool sticky=false) noexcept;
    bool pressed(u16 mask) const noexcept {return (current&mask)&&(current&mask)!=(previous&mask);}
};
class InputController {
public:
    u16 focus_conflict=0;
    static u16 keyboard(const u8* keys,bool direct=true) noexcept;
    u16 controller(u16 buttons,const ControllerSnapshot&,const GameConfiguration&) noexcept;
    static void bindings(const ControllerSnapshot&,u8* output) noexcept;
    static u16 direct_button(u16& output,i16 index,u16 mask,const u8* buttons) noexcept;
    static u16 legacy_button(u16& output,i16 index,u16 mask,u32 buttons) noexcept;
};
}
