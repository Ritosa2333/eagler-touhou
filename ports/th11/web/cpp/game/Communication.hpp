#pragma once
#include "GameEconomy.hpp"
#include "Timer.hpp"
namespace th11 {
// Original 0x420a70 / 0x40baa0. The secondary counter is retained even
// though the point-item formula reads the primary communication gauge.
struct Communication {
    i32 secondary=0;
    Timer reward_delay;
    void reset(const float* rate)noexcept {secondary=0;reward_delay.set(0,rate);}
    void reward(GameEconomy&,i32 amount,const float* rate)noexcept;
    void update(GameEconomy&,float player_y)noexcept;
};
}
