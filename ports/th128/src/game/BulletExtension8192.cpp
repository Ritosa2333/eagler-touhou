#include "BulletExtension8192.hpp"

#include <cmath>
#include <numbers>

namespace th128::bullet {

std::optional<Extension8192> decodeExtension8192(const ExtensionRaw& record) noexcept {
    if (std::uint32_t(record.type) != 0x2000u) return std::nullopt;
    const auto packed = std::uint32_t(record.parameterA);
    Extension8192 result;
    result.packed = packed;
    result.type = int((packed >> 16) & 0xffu);
    result.color = int((packed >> 8) & 0xffu);
    result.flags = 0;
    result.aimMode = int((packed >> 24) & 0x7fu);
    result.signFlag = (packed & 0x80000000u) != 0;
    result.extensionIndex = int(packed & 0xffu);
    result.resolvedParameterB = record.parameterB;
    result.speed = record.parameterR;
    result.endSpeed = record.parameterS;
    const bool authoredColor2 = packed == 0x83100208u && result.resolvedParameterB == 2;
    if ((!authoredColor2 && packed != 0x83100608u) || result.type != 16 || result.color < 0 || result.color > 15
        || result.aimMode < 0 || result.aimMode > 3 || result.extensionIndex < 0
        || result.extensionIndex >= 18 || result.resolvedParameterB <= 0
        || result.resolvedParameterB > 4096
        || !std::isfinite(result.speed) || !std::isfinite(result.endSpeed)) return std::nullopt;
    return result;
}

Extension8192Dispatch advanceExtension8192(int index, std::uint32_t activeExtensions) noexcept {
    return {index + 1, activeExtensions};
}

std::optional<NestedEmissionRequest> makeExtension8192Request(const Extension8192& state,
                                                              const Profile& profile,
                                                              const Birth& parent,
                                                              Position aimPlayer,
                                                              float resolvedNextAngle,
                                                              int count) noexcept {
    if (count <= 0 || count > 4096 || count != state.resolvedParameterB || profile.type != state.type
        || !std::isfinite(resolvedNextAngle) || resolvedNextAngle < -std::numbers::pi_v<float>
        || resolvedNextAngle > std::numbers::pi_v<float>
        || !std::isfinite(parent.state.position.x) || !std::isfinite(parent.state.position.y)
        || !std::isfinite(parent.state.position.z)) return std::nullopt;
    NestedEmissionRequest request;
    request.emitter.type = state.type;
    request.emitter.color = state.color;
    request.emitter.positionX = parent.state.position.x;
    request.emitter.positionY = parent.state.position.y;
    request.emitter.positionZ = parent.state.position.z;
    request.emitter.angle = resolvedNextAngle;
    request.emitter.spacing = 0;
    request.emitter.speed = state.speed;
    request.emitter.endSpeed = state.endSpeed;
    request.emitter.spawnRadius = 0;
    request.emitter.count = std::int16_t(count);
    request.emitter.layers = 1;
    request.emitter.aimMode = std::uint16_t(state.aimMode);
    request.emitter.flags = 0;
    request.emitter.shotSound = 0;
    request.emitter.transformSound = -1;
    request.emitter.extension0 = parent.state.extension0;
    request.emitter.laterExtensions = parent.state.laterExtensions;
    request.birthStartIndex = state.extensionIndex;
    request.profile = profile;
    request.player = aimPlayer;
    return request;
}
}
