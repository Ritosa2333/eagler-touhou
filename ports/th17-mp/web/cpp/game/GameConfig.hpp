#pragma once
#include "Types.hpp"
#include <array>
namespace th17 {
// Original th17.cfg bytes are a file boundary, not the browser's display state.
struct GameConfig {
 std::array<u8,100> bytes{};GameConfig(){reset();}
 void reset(const u8* controller=nullptr)noexcept;bool open(const u8*,u32)noexcept;
 i32 music_volume()const noexcept{return bytes[0x22];}i32 sound_volume()const noexcept{return bytes[0x23];}
 bool auto_focus()const noexcept{return (bytes[0x29]&2)!=0;}
};
}
