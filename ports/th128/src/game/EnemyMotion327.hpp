#pragma once
#include "EnemyRules.hpp"
#include <array>
#include <cmath>
#include <cstdint>

namespace th128::enemy {
struct Motion327Owner {
    Position absolute,relative,absoluteVelocity,relativeVelocity;
    Polar polar,relativePolar;
    PositionInterpolation positionInterpolation;
    PolarInterpolation polarInterpolation,relativePolarInterpolation;
    std::array<std::int32_t,6> remainingMotionDurations{};

    bool foldRelativeAndStop() noexcept {
        const Position combined{float(double(absolute.x)+double(relative.x)),
            float(double(absolute.y)+double(relative.y)),float(double(absolute.z)+double(relative.z))};
        if(!std::isfinite(combined.x)||!std::isfinite(combined.y)||!std::isfinite(combined.z))return false;
        absolute=combined;
        relative={};absoluteVelocity={};relativeVelocity={};
        polar.speed=0;relativePolar.speed=0;
        polar.modeFlags&=~3u;relativePolar.modeFlags&=~3u;
        positionInterpolation.duration=0;polarInterpolation.duration=0;relativePolarInterpolation.duration=0;
        remainingMotionDurations.fill(0);
        return true;
    }
};
}
