#pragma once
#include "Types.hpp"
#include <array>
namespace th15 {
// Imported display settings are retained; the shared SDL platform owns display.
struct GameConfig {
    std::array<u8,0x6c> bytes{};
    GameConfig(){reset();}
    void reset(const u8* controller=nullptr)noexcept;bool open(const u8*,u32)noexcept;
    i32 music_volume()const noexcept{return bytes[0x22];}
    i32 sound_volume()const noexcept{return bytes[0x23];}
    bool auto_focus()const noexcept{return bytes[0x24]!=0;}
};
}
