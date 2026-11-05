#include "InputMapping.hpp"
namespace th075 {
static void axis(i32& held,bool negative,bool positive){
    if(negative){if(held>0)held=0;held=increment_wrapped(held,-1);}
    else if(positive){if(held<0)held=0;held=increment_wrapped(held,1);}
    else held=0;
}
void InputMapping::update(const std::array<u8,256>& keyboard,const JoystickState* pad){
    if(joystick>=0){
        if(!pad){clear();return;}
        axis(horizontal,pad->horizontal<-500,pad->horizontal>500);
        axis(vertical,pad->vertical<-500,pad->vertical>500);
        for(u32 i=0;i<buttons.size();++i){const u32 key=bindings[i+4];buttons[i]=key<pad->buttons.size()&&pad->buttons[key]?increment_wrapped(buttons[i],1):0;}
    }else{
        const auto down=[&](u32 slot){const u32 key=bindings[slot];return key<keyboard.size()&&(keyboard[key]&0x80)!=0;};
        axis(horizontal,down(0),down(1));axis(vertical,down(2),down(3));
        for(u32 i=0;i<buttons.size();++i)buttons[i]=down(i+4)?increment_wrapped(buttons[i],1):0;
    }
}
}
