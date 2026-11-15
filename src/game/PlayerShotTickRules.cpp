#include "PlayerShotTickRules.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <limits>

namespace th128::playershot {

namespace {

std::uint32_t word(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    return std::uint32_t(bytes[offset]) | std::uint32_t(bytes[offset + 1]) << 8 |
           std::uint32_t(bytes[offset + 2]) << 16 | std::uint32_t(bytes[offset + 3]) << 24;
}

float scalar(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    return std::bit_cast<float>(word(bytes, offset));
}

void store(std::span<std::uint8_t> bytes, std::size_t offset, std::uint32_t value) noexcept {
    for (std::uint32_t index = 0; index < 4; ++index)
        bytes[offset + index] = std::uint8_t(value >> (index * 8));
}

void storeFloat(std::span<std::uint8_t> bytes, std::size_t offset, float value) noexcept {
    store(bytes, offset, std::bit_cast<std::uint32_t>(value));
}

bool bounded(float value) noexcept {
    return std::isfinite(value) && std::abs(value) <= 1048576.f;
}

bool exactPair(float first, float second) noexcept {
    return first == 0 || second == 0 || std::abs(std::ilogb(first) - std::ilogb(second)) <= 28;
}

bool activeState(std::span<const std::uint8_t> bytes) noexcept {
    if (bytes.size() != TickSlotBytes || (word(bytes, 0x44) & 1u))
        return false;
    for (const auto offset : {0x14u, 0x18u, 0x1cu, 0x20u, 0x24u, 0x28u, 0x2cu, 0x30u})
        if (!bounded(scalar(bytes, offset)))
            return false;
    return true;
}

float quantized(float value) noexcept {
    const float scaled = float(double(value) * 100.);
    const float integer = float(std::floor(double(scaled)));
    return float(double(integer) / 100.);
}

void clearRequest(ShotTickPlan& result, ShotTickContinuation continuation) noexcept {
    result.continuation = continuation;
    result.required = {};
}

long double extended(long double value) noexcept {
    if(value==0)return value;
    int exponent=0;const auto fraction=std::frexp(value,&exponent);
    return std::ldexp(std::nearbyint(std::ldexp(fraction,64)),exponent-64);
}

std::optional<float> rotatedCoordinate(float first, float firstBasis, float second,
                                       float secondBasis, float center) noexcept {
    static_assert(std::numeric_limits<long double>::digits>=64);
    const long double firstProduct = static_cast<long double>(first) * firstBasis;
    const long double secondProduct = static_cast<long double>(second) * secondBasis;
    const long double difference = extended(firstProduct-secondProduct);
    const long double result = extended(difference+center);
    if (!std::isfinite(result) || std::abs(result) > 4194304.)
        return std::nullopt;
    return float(result);
}

}

std::optional<ShotTickPlan> prepareOrdinaryShotTick(
    std::span<const std::uint8_t> slot,
    const std::optional<ShotTickRecordBinding>& actualRecord, float clock) noexcept {
    if (slot.size() != TickSlotBytes || clock != 1.f)
        return std::nullopt;
    ShotTickPlan result;
    std::copy(slot.begin(), slot.end(), result.next.begin());
    if (word(slot, 0x48) == 0)
        return result;
    if (!activeState(slot))
        return std::nullopt;
    const auto identity = word(slot, 0x74);
    if (!actualRecord || !actualRecord->nativeTickTarget) {
        result.continuation = ShotTickContinuation::NeedsRecordBinding;
        result.required = {ShotTickCallKind::ResolveRecord, 0x43f7a1, 0, identity};
        return result;
    }
    if (actualRecord->recordIdentity != identity || actualRecord->kind != 0)
        return std::nullopt;
    store(result.next, 0x58, 0);
    if (*actualRecord->nativeTickTarget != 0) {
        result.continuation = ShotTickContinuation::NeedsRecordCallback;
        result.required = {ShotTickCallKind::RecordCallback, 0x43f7ae, *actualRecord->nativeTickTarget, identity};
        return result;
    }
    result.continuation = ShotTickContinuation::NeedsMotion;
    result.required = {ShotTickCallKind::PolarMotion, 0x43f7d4, 0x46c1f0, identity, 0,
                       scalar(slot, 0x30), scalar(slot, 0x2c)};
    return result;
}

std::optional<ShotTickPlan> completeOrdinaryShotMotion(
    const ShotTickPlan& pending, const TickPosition& genuineVelocity) noexcept {
    if (pending.continuation != ShotTickContinuation::NeedsMotion || !activeState(pending.next) ||
        !bounded(genuineVelocity.x) || !bounded(genuineVelocity.y) ||
        std::bit_cast<std::uint32_t>(genuineVelocity.z) != 0)
        return std::nullopt;
    const std::array<float, 3> velocity{genuineVelocity.x, genuineVelocity.y, genuineVelocity.z};
    ShotTickPlan result = pending;
    for (std::size_t index = 0; index < velocity.size(); ++index) {
        const auto offset = 0x14 + index * 4;
        const float prior = scalar(pending.next, offset);
        const float moved = float(double(prior) + double(velocity[index]));
        if (!bounded(moved))
            return std::nullopt;
        storeFloat(result.next, offset, index < 2 ? quantized(moved) : moved);
        storeFloat(result.next, 0x20 + index * 4, velocity[index]);
    }
    result.continuation = ShotTickContinuation::NeedsAnimationLookup;
    result.required = {ShotTickCallKind::AnimationLookup, 0x43f823, 0,
                       word(result.next, 0x74), word(result.next, 0x4c)};
    return result;
}

std::optional<ShotTickPlan> completeShotAnimationLookup(const ShotTickPlan& pending, bool genuinePrimaryFound) noexcept {
    if (pending.continuation != ShotTickContinuation::NeedsAnimationLookup)
        return std::nullopt;
    ShotTickPlan result = pending;
    if (!genuinePrimaryFound) {
        store(result.next, 0x48, 0);
        result.continuation = ShotTickContinuation::NeedsAnimationCleanup;
        result.required = {ShotTickCallKind::AnimationCleanup, 0x43f88c, 0x467420,
                           word(result.next, 0x74), word(result.next, 0x50)};
    } else {
        result.continuation = ShotTickContinuation::NeedsAnimationBounds;
        result.required = {ShotTickCallKind::AnimationBounds, 0x43f8e8, 0,
                           word(result.next, 0x74), word(result.next, 0x4c)};
        result.required.conditionalTargets = {0x45f110, 0x45fad0, 0x45f610};
    }
    return result;
}

std::optional<ShotTickPlan> completeShotAnimationCleanup(const ShotTickPlan& pending) noexcept {
    if (pending.continuation != ShotTickContinuation::NeedsAnimationCleanup)
        return std::nullopt;
    ShotTickPlan result = pending;
    store(result.next, 0x4c, 0);
    store(result.next, 0x50, 0);
    clearRequest(result, ShotTickContinuation::Retired);
    return result;
}

std::optional<ShotTickPlan> completeShotAnimationBounds(
    const ShotTickPlan& pending, const ShotAnimationBounds& genuineBounds) noexcept {
    if (pending.continuation != ShotTickContinuation::NeedsAnimationBounds)
        return std::nullopt;
    for (const auto& corner : genuineBounds.corners)
        if (!bounded(corner[0]) || !bounded(corner[1]))
            return std::nullopt;
    bool survivor = std::bit_cast<std::int32_t>(word(pending.next, 4)) < 10;
    for (const auto& corner : genuineBounds.corners)
        survivor = survivor || (corner[0] > 128 && corner[0] < 512 && corner[1] > 16 && corner[1] < 464);
    ShotTickPlan result = pending;
    if (!survivor) {
        result.continuation = ShotTickContinuation::NeedsAnimationDelete;
        result.required = {ShotTickCallKind::AnimationDelete, 0x43fb63, 0x4679b0,
                           word(result.next, 0x74), word(result.next, 0x4c)};
        return result;
    }
    const float horizontal = scalar(result.next, 0x14);
    const float vertical = scalar(result.next, 0x18);
    if (!exactPair(horizontal, 320.f) || !exactPair(vertical, 16.f))
        return std::nullopt;
    result.continuation = ShotTickContinuation::NeedsAnimationSync;
    result.required = {ShotTickCallKind::AnimationSync, 0x43f9e6, 0,
                       word(result.next, 0x74), word(result.next, 0x4c)};
    result.required.animationPosition = {float((double(horizontal) + 128.) + 192.), float(double(vertical) + 16.),
                                         scalar(result.next, 0x1c)};
    result.required.secondaryHandle = word(result.next, 0x50);
    result.required.angle = scalar(result.next, 0x30);
    if (result.required.secondaryHandle)
        result.required.conditionalTargets[0] = 0x467420;
    return result;
}

std::optional<ShotTickPlan> completeShotAnimationDelete(const ShotTickPlan& pending) noexcept {
    if (pending.continuation != ShotTickContinuation::NeedsAnimationDelete)
        return std::nullopt;
    ShotTickPlan result = pending;
    store(result.next, 0x4c, 0);
    store(result.next, 0x48, 0);
    clearRequest(result, ShotTickContinuation::Retired);
    return result;
}

std::optional<ShotTickPlan> completeShotAnimationSync(const ShotTickPlan& pending) noexcept {
    if (pending.continuation != ShotTickContinuation::NeedsAnimationSync ||
        pending.required.secondaryHandle != 0)
        return std::nullopt;
    ShotTickPlan result = pending;
    clearRequest(result, ShotTickContinuation::ReadyForAgeTail);
    return result;
}

std::optional<ShotTickPlan> finishShotTickAge(const ShotTickPlan& pending, float clock) noexcept {
    if (pending.continuation != ShotTickContinuation::ReadyForAgeTail || clock != 1.f ||
        !(word(pending.next, 0x10) & 1u) || word(pending.next, 0xc) != TickClockIdentity ||
        !bounded(scalar(pending.next, 8)))
        return std::nullopt;
    ShotTickPlan result = pending;
    const auto current = word(result.next, 4);
    store(result.next, 0, current);
    store(result.next, 4, current + 1u);
    storeFloat(result.next, 8, float(double(scalar(result.next, 8)) + 1.));
    clearRequest(result, ShotTickContinuation::Finished);
    return result;
}

std::optional<ShotAnimationBounds> axisAlignedShotAnimationBounds(
    const ShotAxisAlignedAnimationInput& actual) noexcept {
    if (!actual.noParentAndZeroOffsetsConfirmed || ((actual.flags >> 24) & 31u) != 0 ||
        !bounded(actual.position.x) || !bounded(actual.position.y) || !bounded(actual.position.z))
        return std::nullopt;
    const std::array<float, 2> center{actual.position.x, actual.position.y};
    std::array<float, 2> low{}, high{};
    for (std::size_t axis = 0; axis < 2; ++axis) {
        const auto anchor = (actual.flags >> (20 + axis * 2)) & 3u;
        const float size = actual.spriteSize[axis], scale = actual.scale[axis];
        if (anchor > 2 || actual.secondaryScale[axis] != 1.f ||
            !std::isfinite(size) || size < 0 || size > 4096 || std::trunc(size) != size ||
            !std::isfinite(scale) || std::abs(scale) > 16)
            return std::nullopt;
        const float extent = float(double(size) * double(scale));
        const float delta = anchor == 0 ? extent * .5f : anchor == 2 ? extent : 0;
        if (!exactPair(center[axis], delta))
            return std::nullopt;
        low[axis] = float(double(center[axis]) - double(delta));
        if (anchor == 0)
            low[axis] = float(std::floor(double(low[axis])));
        if (anchor == 2) {
            high[axis] = center[axis];
        } else {
            if (!exactPair(low[axis], extent))
                return std::nullopt;
            high[axis] = float(double(low[axis]) + double(extent));
        }
    }
    return ShotAnimationBounds{{{{low[0], low[1]}, {high[0], low[1]},
                                {low[0], high[1]}, {high[0], high[1]}}}};
}

std::optional<ShotAnimationBounds> rotatedShotAnimationBounds(
    const ShotRotatedAnimationInput& input) noexcept {
    const auto& actual = input.animation;
    if (!input.rotationBasisConfirmed || !actual.noParentAndZeroOffsetsConfirmed ||
        ((actual.flags >> 24) & 31u) != 1 || !bounded(actual.position.x) ||
        !bounded(actual.position.y) || !bounded(actual.position.z))
        return std::nullopt;
    const float cosine = input.cosineSine[0], sine = input.cosineSine[1];
    for (const float component : input.cosineSine)
        if (!std::isfinite(component) || std::abs(component) > 1 ||
            (component != 0 && std::abs(component) < 0x1p-32f))
            return std::nullopt;
    if (std::abs(double(cosine) * cosine + double(sine) * sine - 1) > 0x1p-20)
        return std::nullopt;
    std::array<float, 2> low{}, high{};
    for (std::size_t axis = 0; axis < 2; ++axis) {
        const auto anchor = (actual.flags >> (20 + axis * 2)) & 3u;
        const float size = actual.spriteSize[axis], scale = actual.scale[axis];
        if (anchor > 2 || actual.secondaryScale[axis] != 1.f || !std::isfinite(size) ||
            size < 0 || size > 4096 || std::trunc(size) != size || !std::isfinite(scale) ||
            std::abs(scale) > 16 || (scale != 0 && std::abs(scale) < 0x1p-16f))
            return std::nullopt;
        const float extent = float(double(size) * scale);
        low[axis] = anchor == 0 ? -extent * .5f : anchor == 2 ? -extent : 0;
        high[axis] = anchor == 0 ? extent * .5f : anchor == 1 ? extent : 0;
    }
    ShotAnimationBounds result;
    for (std::size_t index = 0; index < result.corners.size(); ++index) {
        const float localX = index & 1 ? high[0] : low[0];
        const float localY = index & 2 ? high[1] : low[1];
        const auto horizontal = rotatedCoordinate(localX, cosine, localY, sine, actual.position.x);
        const auto vertical = rotatedCoordinate(localX, sine, -localY, cosine, actual.position.y);
        if (!horizontal || !vertical)
            return std::nullopt;
        result.corners[index] = {*horizontal, *vertical};
    }
    return result;
}

}
