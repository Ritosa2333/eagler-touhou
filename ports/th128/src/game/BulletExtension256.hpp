#pragma once
#include "BulletRules.hpp"
#include <cstdint>
#include <optional>

namespace th128::bullet {

struct Extension256 {
    int previous = 0;
    int remaining = 0;
    float accumulator = 0;
    std::uint32_t flags = 0;
    int mode = 0;
};

struct Extension256Step {
    Extension256 next;
    bool finished = false;
};

std::optional<Extension256> initializeExtension256(const ExtensionRaw& record);
std::optional<Extension256Step> stepExtension256(const Extension256& state);
}
