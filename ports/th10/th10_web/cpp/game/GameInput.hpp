#pragma once
#include "Types.hpp"
namespace th10 {
struct GameInput {
    u16 raw,raw_previous,raw_repeat;
    u16 raw_pressed,raw_released,raw_held_frames[16];
    u16 focus_hold,current,previous,repeat,pressed,released,reserved_036;
    u16 held_frames[16];
    void update_edges() noexcept;
    void update_raw(u16 buttons) noexcept;
};
static_assert(offsetof(GameInput,current)==0x2c && sizeof(GameInput)==0x58);
}
