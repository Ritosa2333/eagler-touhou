#pragma once
#include "Types.hpp"
namespace th09 {
struct ChargeGauge {
    float value=0;
    // Returns the new level on a transition, otherwise -1. The game manager
    // forwards transitions to the HUD; character 1 gets the original 15% bonus.
    i32 add(float amount,u32 character) noexcept;
};
}
