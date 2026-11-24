#pragma once
#include "game/BulletRules.hpp"
#include <cstdint>
#include <optional>

namespace th128::bullet {

struct Extension16384 {
    std::uint32_t packedA = 0;
    std::int32_t control = -1;
    std::int32_t sourceIndex = -1;
    std::int32_t nextExtensionIndex = -1;
    bool consumed = false;
};

struct Extension16384Step {
    Extension16384 next;
    bool activeChanged = false;
    bool tick = false;
    bool retired = false;
    bool parentChanged = false;
    std::uint32_t hostCalls = 0;
    std::uint32_t rngCalls = 0;
};

std::optional<Extension16384> initializeExtension16384(const ExtensionRaw& record, std::int32_t index);
std::optional<Extension16384Step> consumeExtension16384(const Extension16384& state);
}
