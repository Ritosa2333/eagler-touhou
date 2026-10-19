#pragma once
#include "Types.hpp"
#include <array>
namespace th11 {
// Original fixed-size th11.cfg. Platform-specific display fields are retained
// when imported, while the SDL backend owns its actual window and swap chain.
struct GameConfig {
    std::array<u8,0x3c> bytes{};
    GameConfig(){reset();}
    void reset(const u8* controller=nullptr)noexcept;
    bool open(const u8*,u32)noexcept;
    i32 music_volume()const noexcept{return bytes[0x20];}
    i32 sound_volume()const noexcept{return bytes[0x21];}
    bool auto_focus()const noexcept{return bytes[0x22]!=0;}
};
}
