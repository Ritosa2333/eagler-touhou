#pragma once
#include "PlayerShotBirthRules.hpp"

namespace th128::playershot {
std::optional<ShotMotionResult> computeShotMotion(float angle,float speed) noexcept;
}
