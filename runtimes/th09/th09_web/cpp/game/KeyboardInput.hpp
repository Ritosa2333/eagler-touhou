#pragma once
#include "Types.hpp"
namespace th09 {
// Windows virtual-key numbers are an input interchange format only. The SDL
// adapter maps physical keys into this snapshot; no Win32 calls are required.
u16 keyboard_input(const bool (&keys)[256],u32 device);
// Local physical input only. Capture once per fresh frame, outside rollback
// snapshots; retained network/replay frames must never sample this again.
class KeyboardInputBuffer {
    struct Key {
        bool held=false,captured=false;
        u8 pending=0;
        void event(bool down);
        void sample(bool down,bool retain);
    } keys_[2];
    bool held_[256]{},pressed_[256]{},taps_[256]{};
public:
    void event(u32 virtual_key,bool down);
    void sample(const bool (&keys)[256],bool retain);
    u16 input(const bool (&keys)[256],u32 device) const;
    void clear(){*this={};}
};
}
