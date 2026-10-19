#include "InputFrame.hpp"
namespace th09 {
void InputFrame::update(u16 keys) noexcept {
    previous=held;held=keys;repeat=0;
    for(u32 bit=0;bit<16;++bit){
        const u16 mask=u16(1u<<bit);
        if(!(keys&mask))duration[bit]=0;
        else {
            duration[bit]=u16(duration[bit]+1);
            if(duration[bit]>25){repeat|=mask;duration[bit]=u16(duration[bit]-8);}
        }
    }
    pressed=u16((previous^held)&held);
    released=u16((previous^held)&~held);
}
}
