#pragma once
#include "EnemyRules.hpp"
#include "engine/Random.hpp"

namespace th128::enemy {
std::optional<float> randomMovementDirection(const Position& position,const MovementArea& area,
                                            float playerX,Random& random) noexcept;
bool beginRandomMovement(Polar& polar,PolarInterpolation& interpolation,float angle,std::int32_t duration,
                         std::int32_t mode,float speed) noexcept;
}
