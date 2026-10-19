#pragma once
#include "InputFrame.hpp"
namespace th09 {
// Device sampling and simulation input are separate: a replay replaces only the latter.
struct GameInput {
    InputFrame device;
    u16 fire_frames=0;
    u16 held=0,previous=0,repeat=0,pressed=0,released=0,reserved=0;
    u16 duration[16]{};
    void advance(u16 keys) noexcept;
    void update_auto_focus(bool enabled) noexcept;
};
static_assert(sizeof(GameInput)==88);
}
