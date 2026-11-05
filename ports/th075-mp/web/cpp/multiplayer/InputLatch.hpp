#pragma once
#include <array>
#include <cstdint>
namespace th075::multiplayer {
// Device edges live outside rollback state. A tap that begins and ends while
// prediction is blocked must survive until the next once-only logical capture.
class InputLatch {
    std::array<bool,256> pressed{},held{};
    std::uint8_t touch=0,touchHeld=0;
public:
    void Key(unsigned scan,bool down){if(scan<pressed.size()){if(down&&!held[scan])pressed[scan]=true;held[scan]=down;}}
    bool TakeKey(unsigned scan){if(scan>=pressed.size())return false;const bool down=pressed[scan];pressed[scan]=false;return down;}
    void Touch(std::uint8_t buttons){touch|=buttons&~touchHeld;touchHeld=buttons;}
    std::uint8_t TakeTouch(){const auto buttons=touch;touch=0;return buttons;}
    void Clear(){pressed.fill(false);held.fill(false);touch=touchHeld=0;}
};
}
