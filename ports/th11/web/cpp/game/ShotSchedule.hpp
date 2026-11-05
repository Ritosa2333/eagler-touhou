#pragma once
#include "ShtResource.hpp"
#include "Timer.hpp"
namespace th11 {
struct ShotSchedule {
    Timer timer;
    // Reports the frame to emit before advancing the original 15-frame cycle.
    // A stopped player retains the timer but clears its homing lock elsewhere.
    i32 update(i32 player_state,i32 warp_state,u32 held,const float* rate)noexcept;
    i32 begin(i32 player_state,i32 warp_state,u32 held,const float* rate)noexcept;
    void finish(u32 held,const float* rate)noexcept;
};
}
