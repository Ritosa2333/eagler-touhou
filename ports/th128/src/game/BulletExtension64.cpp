#include "BulletExtension64.hpp"

#include <cmath>
#include <optional>

namespace th128::bullet {
namespace {

bool bounded(float value) noexcept {
    return std::isfinite(value) && std::abs(value) <= 1e20f;
}

bool bounded(const Position& position) noexcept {
    return bounded(position.x) && bounded(position.y) && bounded(position.z);
}

std::optional<float> normalizeAngle(float angle) noexcept {
    constexpr float pi = 3.1415927410125732421875f;
    constexpr float tau = 6.283185482025146484375f;
    if (!bounded(angle)) return std::nullopt;
    for (int iteration = 0; iteration < 32 && angle > pi; ++iteration)
        angle = float(double(angle) - double(tau));
    for (int iteration = 0; iteration < 32 && angle < -pi; ++iteration)
        angle = float(double(angle) + double(tau));
    if (angle < -pi || angle > pi) return std::nullopt;
    return angle;
}

}

std::optional<Extension64> initializeExtension64(const ExtensionRaw& record) noexcept {
    if (record.type != 64 || record.control < 0 || record.parameterA < 0 || record.parameterA > 4096
        || record.parameterB < 0 || !bounded(record.parameterR)) return std::nullopt;
    return Extension64{record.parameterR, record.parameterA, record.parameterB, 0};
}

std::optional<Extension64Step> stepExtension64(const Extension64& state, float speed, float angle,
                                               Position position, Position velocity, bool active) noexcept {
    if (!active || !bounded(speed) || !bounded(angle) || !bounded(position) || !bounded(velocity)
        || state.limit < 0 || state.count < 0 || state.control < 0) return std::nullopt;
    Extension64Step result;
    result.next = state;
    result.speed = speed;
    result.angle = angle;
    result.position = position;
    result.velocity = velocity;
    const bool outside = position.x <= -192.f || position.x >= 192.f
        || position.y <= 0.f || position.y >= 448.f;
    if (!outside) return result;
    if (!(state.control & 16)) {
        if ((state.control & 1) && position.y < 0.f) {
            result.position.y = -position.y;
            result.angle = -angle;
            result.mutated = true;
        }
        if ((state.control & 2) && position.y >= 448.f) {
            result.position.y = 896.f - position.y;
            result.angle = -result.angle;
            result.mutated = true;
        }
        if ((state.control & 8) && position.x >= 192.f) {
            result.position.x = 384.f - position.x;
            const auto normalized = normalizeAngle(3.1415927410125732421875f - result.angle);
            if (!normalized) return std::nullopt;
            result.angle = *normalized;
            result.mutated = true;
        }
        if ((state.control & 4) && position.x < -192.f) {
            result.position.x = -384.f - position.x;
            const auto normalized = normalizeAngle(3.1415927410125732421875f - result.angle);
            if (!normalized) return std::nullopt;
            result.angle = *normalized;
            result.mutated = true;
        }
    }
    if (state.parameterR > -990.f) result.speed = state.parameterR;
    result.velocity = {float(std::cos(double(result.angle)) * result.speed),
                       float(std::sin(double(result.angle)) * result.speed), velocity.z};
    if (!result.mutated) return result;
    ++result.next.count;
    if (result.next.count >= result.next.limit) result.finished = true;
    return result;
}
}
