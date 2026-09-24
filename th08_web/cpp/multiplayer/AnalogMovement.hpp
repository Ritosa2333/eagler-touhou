#pragma once
#include "InputSample.hpp"
#include "../game/PlayerMovement.hpp"
namespace th08::multiplayer {
bool ResolveAnalogMovement(const AnalogInput&,const PlayerMovementState&,float speed,
                          const FrameTiming&,float& x,float& y) noexcept;
}
