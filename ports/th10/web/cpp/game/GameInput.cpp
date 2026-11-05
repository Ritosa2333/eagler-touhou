#include "GameInput.hpp"
namespace th10 {
void GameInput::update_raw(u16 buttons) noexcept {
    raw_previous=raw;raw=buttons;raw_repeat=0;
    u16 bit=1;
    for(auto& count:raw_held_frames){
        if(buttons&bit){count=static_cast<u16>(count+1);if(count>=26){raw_repeat|=bit;count=static_cast<u16>(count-8);}}
        else count=0;
        bit=static_cast<u16>(bit<<1);
    }
    const u16 changed=raw^raw_previous;raw_pressed=changed&raw;raw_released=changed&~raw;
}
// 0x40ac20. The original repeat loop writes bit zero for every held key;
// it clears raw_repeat, while the separate repeat field retains earlier bits.
void GameInput::update_edges() noexcept {
    raw_repeat=0;u16 remaining=current;
    for(auto& count:held_frames){if(remaining&1){count=static_cast<u16>(count+1);if(count>=26){repeat|=1;count=static_cast<u16>(count-8);}}else count=0;remaining>>=1;}
    const u16 changed=current^previous;pressed=changed&current;released=changed&~current;
}
}
