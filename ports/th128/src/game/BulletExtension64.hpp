#pragma once

#include "BulletRules.hpp"

#include <cstdint>
#include <optional>

namespace th128::bullet {

struct Extension64 {
    float parameterR = 0;
    std::int32_t limit = 0;
    std::int32_t control = 0;
    std::int32_t count = 0;
};

struct Extension64Step {
    Extension64 next;
    float speed = 0;
    float angle = 0;
    Position position;
    Position velocity;
    bool mutated = false;
    bool finished = false;
};

std::optional<Extension64> initializeExtension64(const ExtensionRaw& record) noexcept;
std::optional<Extension64Step> stepExtension64(const Extension64& state, float speed, float angle,
                                               Position position, Position velocity, bool active) noexcept;
}
