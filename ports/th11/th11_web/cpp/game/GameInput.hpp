#pragma once
#include "Types.hpp"
#include <array>
namespace th11 {
// SDL supplies the same normalized axes and button bytes as the original
// controller sampler. Mapping stays in the game layer, including cfg deadzones.
u32 controller_keys(u32 held,const u8* buttons,u32 count,i32 x,i32 y,const u8* config)noexcept;
u32 keyboard_keys(const bool* keys)noexcept;
// Native game input mapper 435fe0 and repeat/edge sampler 40de10. This is
// separate from host keys, so saved replays contain game-visible inputs.
struct GameInput {
    u32 held=0,previous=0,pressed=0,released=0,long_repeat=0,short_repeat=0;
    std::array<u32,32> counts{};
    u32 auto_focus_frames=0;
    void update(u32 keys,bool auto_focus=false)noexcept;
};
}
