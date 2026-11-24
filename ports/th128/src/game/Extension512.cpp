#include "Extension512.hpp"

#include <cmath>
#include <cstdint>
#include <limits>

namespace th128::bullet {

std::optional<Extension512Step> initializeExtension512(
    const ExtensionRaw& record,
    std::int32_t extensionIndex,
    const Extension512Parent& parent,
    const Extension512TargetProfile& target) noexcept {
    if (record.type != 512 || (record.control != 0 && record.control != 1)) return std::nullopt;
    if (extensionIndex < 0 || extensionIndex >= 18) return std::nullopt;
    const auto packedType = static_cast<std::uint32_t>(record.parameterA);
    const auto packedColor = static_cast<std::uint32_t>(record.parameterB);
    if ((packedType & 0xffff0000u) != 0) return std::nullopt;
    if ((packedColor & 0xffff0000u) != 0) return std::nullopt;
    const bool highColorInterrupt = (packedColor & 0x8000u) != 0;
    if (static_cast<std::int32_t>(packedType & 0xffffu) != target.type) return std::nullopt;
    const std::int32_t outputColor = static_cast<std::int32_t>(packedColor & 0x7fffu);
    if (outputColor != target.color) return std::nullopt;
    if (target.type < 0 || target.type > 0xffff || target.color < 0 || target.color > 15) return std::nullopt;
    if (target.script < 0 || target.script > 4095 || target.secondaryScript < 0 || target.secondaryScript > 4095) return std::nullopt;
    if (!std::isfinite(target.hitboxSize) || target.hitboxSize <= 0 || target.hitboxSize > 4096
        || !std::isfinite(target.auxiliarySize) || target.auxiliarySize < 0 || target.auxiliarySize > 4096) return std::nullopt;
    if (target.appearance < 0
        || target.cancellationScript < 0 || target.freezePolicy < 0 || target.freezePolicy > 1) return std::nullopt;
    if (parent.type < 0 || parent.color < 0 || parent.color > 15) return std::nullopt;
    if (parent.extensionIndex != extensionIndex) return std::nullopt;
    if (highColorInterrupt) {
        const bool c1HighColor = record.control == 0 && parent.type == 30 && parent.color == 0
            && (extensionIndex == 2 || extensionIndex == 3) && target.type == 4 && target.color == 13
            && target.script == 39 && target.secondaryScript == 0 && target.hitboxSize == 4.f
            && target.auxiliarySize == 16.f && target.appearance == 3
            && target.cancellationScript == 363 && target.freezePolicy == 0;
        const bool a2BlueDeflection = record.control == 0 && parent.type == 24 && parent.color == 0
            && extensionIndex == 3 && target.type == 4 && target.color == 6
            && record.parameterR == -999999.f && record.parameterS == -999999.f
            && target.script == 39 && target.secondaryScript == 0 && target.hitboxSize == 4.f
            && target.auxiliarySize == 16.f && target.appearance == 3
            && target.cancellationScript == 349 && target.freezePolicy == 0;
        if (!c1HighColor && !a2BlueDeflection) return std::nullopt;
    }

    Extension512Step step;
    step.type = target.type;
    step.color = outputColor;
    step.script = target.script;
    step.secondaryScript = target.secondaryScript;
    step.hitboxSize = target.hitboxSize;
    step.auxiliarySize = target.auxiliarySize;
    step.appearance = target.appearance;
    step.cancellationScript = target.cancellationScript;
    step.freezePolicy = target.freezePolicy;
    step.flags = parent.flags | 0x10u;
    step.activeExtensions = parent.activeExtensions;
    step.nextExtensionIndex = extensionIndex + 1;
    step.visualCallback = 0x40ccb0u;
    step.callbackOwnerIsBullet = true;
    step.resetPrimaryVisual = true;
    step.resetSecondaryVisual = true;
    step.secondaryVisualActive = target.secondaryScript != 0;
    step.changedType = true;
    step.changedColor = true;
    step.changedProfile = true;
    step.activeChanged = false;
    step.tick = false;
    step.retired = false;
    step.parentChanged = false;
    step.hostCalls = 0;
    step.rngCalls = 0;
    step.primaryInterrupt = highColorInterrupt ? 2 : 0;
    step.irqCalls = highColorInterrupt ? 1 : 0;
    return step;
}

}
