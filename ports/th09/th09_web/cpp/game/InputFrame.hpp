#pragma once
#include "Types.hpp"
namespace th09 {
// Prefix of each player's independent input state (original stride 0x8e).
struct InputFrame {
    u16 held=0,previous=0,repeat=0,pressed=0,released=0;
    u16 duration[16]{};
    void update(u16 keys) noexcept;
};
static_assert(sizeof(InputFrame)==42);
}
