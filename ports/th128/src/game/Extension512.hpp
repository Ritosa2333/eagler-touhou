#pragma once

#include "BulletRules.hpp"
#include <cstdint>
#include <optional>

namespace th128::bullet {

struct Extension512TargetProfile {
    std::int32_t type = 0;
    std::int32_t color = 0;
    std::int32_t script = -1;
    std::int32_t secondaryScript = 0;
    float hitboxSize = 0;
    float auxiliarySize = 0;
    std::int32_t appearance = 0;
    std::int32_t cancellationScript = -1;
    std::int32_t freezePolicy = 0;
};

struct Extension512Parent {
    std::int32_t type = 0;
    std::int32_t color = 0;
    std::uint32_t flags = 0;
    std::uint32_t activeExtensions = 0;
    std::int32_t extensionIndex = 0;
};

struct Extension512Step {
    std::int32_t type = 0;
    std::int32_t color = 0;
    std::int32_t script = -1;
    std::int32_t secondaryScript = 0;
    float hitboxSize = 0;
    float auxiliarySize = 0;
    std::int32_t appearance = 0;
    std::int32_t cancellationScript = -1;
    std::int32_t freezePolicy = 0;
    std::uint32_t flags = 0;
    std::uint32_t activeExtensions = 0;
    std::int32_t nextExtensionIndex = 0;
    std::uint32_t visualCallback = 0x40ccb0u;
    bool callbackOwnerIsBullet = true;
    bool resetPrimaryVisual = true;
    bool resetSecondaryVisual = true;
    bool secondaryVisualActive = false;
    std::int32_t primaryInterrupt = 0;
    bool changedType = true;
    bool changedColor = true;
    bool changedProfile = true;
    bool activeChanged = false;
    bool tick = false;
    bool retired = false;
    bool parentChanged = false;
    std::uint32_t hostCalls = 0;
    std::uint32_t rngCalls = 0;
    std::uint32_t irqCalls = 0;
};

std::optional<Extension512Step> initializeExtension512(
    const ExtensionRaw& record,
    std::int32_t extensionIndex,
    const Extension512Parent& parent,
    const Extension512TargetProfile& target) noexcept;

}
