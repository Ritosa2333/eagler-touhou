#include "PlayerShotMotion.hpp"
#include "EnemyRules.hpp"
#include <cmath>

namespace th128::playershot {
std::optional<ShotMotionResult> computeShotMotion(float angle,float speed) noexcept {
    if(!std::isfinite(angle)||std::abs(angle)>4||!std::isfinite(speed)||std::abs(speed)>4096)return std::nullopt;
    enemy::Polar polar;
    if(!enemy::setPolar(polar,angle,speed,0))return std::nullopt;
    enemy::Position scratch,velocity;
    if(!enemy::advancePolarPosition(scratch,velocity,polar))return std::nullopt;
    return ShotMotionResult{{velocity.x,velocity.y,velocity.z},polar.angle};
}
}
