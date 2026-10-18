#include "GameInput.hpp"
namespace th09 {
void GameInput::advance(u16 keys) noexcept {
    previous=held;held=keys;
    // These two legacy quirks are present in the original simulation-input routine:
    // it clears device.repeat and sets bit zero for every repeating game key.
    device.repeat=0;
    for(u32 bit=0;bit<16;++bit){
        if(!(keys&(1u<<bit)))duration[bit]=0;
        else {
            duration[bit]=u16(duration[bit]+1);
            if(duration[bit]>=26){repeat|=1;duration[bit]=u16(duration[bit]-8);}
        }
    }
    pressed=u16((previous^held)&held);
    released=u16((previous^held)&~held);
}
void GameInput::update_auto_focus(bool enabled) noexcept {
    if(!enabled)return;
    if(!(held&1))fire_frames=0;
    else {fire_frames=u16(fire_frames+1);if(fire_frames>=8)fire_frames=8;}
}
}
