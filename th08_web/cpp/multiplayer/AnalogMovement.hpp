#pragma once
#include "InputSample.hpp"
#include "../game/PlayerMovement.hpp"
namespace th08::multiplayer {
bool ResolveAnalogMovement(const AnalogInput&,TouchRemainder&,const PlayerMovementState&,
                          const Vec2& minimum,const Vec2& extent,float speed,
                          const FrameTiming&,float& x,float& y) noexcept;
}
