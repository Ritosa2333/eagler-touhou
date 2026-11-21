#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <span>

namespace th128::playershot {

inline constexpr std::uint32_t TickSlotBytes = 0x78;
inline constexpr std::uint32_t TickClockIdentity = 0x4b6f58;

struct TickPosition {
    float x = 0, y = 0, z = 0;
};

struct ShotTickRecordBinding {
    std::uint32_t recordIdentity = 0;
    std::uint8_t kind = 0;
    std::optional<std::uint32_t> nativeTickTarget;
};

enum class ShotTickContinuation {
    Inactive, NeedsRecordBinding, NeedsRecordCallback, NeedsMotion,
    NeedsAnimationLookup, NeedsAnimationCleanup, NeedsAnimationBounds,
    NeedsAnimationDelete, NeedsAnimationSync, ReadyForAgeTail, Finished, Retired
};

enum class ShotTickCallKind {
    None, ResolveRecord, RecordCallback, PolarMotion, AnimationLookup,
    AnimationCleanup, AnimationBounds, AnimationDelete, AnimationSync
};

struct RequiredShotTickCall {
    ShotTickCallKind kind = ShotTickCallKind::None;
    std::uint32_t callSite = 0;
    std::uint32_t target = 0;
    std::uint32_t recordIdentity = 0;
    std::uint32_t handle = 0;
    float angle = 0;
    float speed = 0;
    std::array<std::uint32_t, 3> conditionalTargets{};
    TickPosition animationPosition{};
    std::uint32_t secondaryHandle = 0;
};

struct ShotTickPlan {
    std::array<std::uint8_t, TickSlotBytes> next{};
    ShotTickContinuation continuation = ShotTickContinuation::Inactive;
    RequiredShotTickCall required;
};

struct ShotAnimationBounds {
    std::array<std::array<float, 2>, 4> corners{};
};

struct ShotAxisAlignedAnimationInput {
    TickPosition position;
    std::array<float, 2> spriteSize{};
    std::array<float, 2> scale{1, 1};
    std::array<float, 2> secondaryScale{1, 1};
    std::uint32_t flags = 0;
    bool noParentAndZeroOffsetsConfirmed = false;
};

std::optional<ShotAnimationBounds> axisAlignedShotAnimationBounds(
    const ShotAxisAlignedAnimationInput& actualAnimation) noexcept;

struct ShotRotatedAnimationInput {
    ShotAxisAlignedAnimationInput animation;
    std::array<float, 2> cosineSine{1, 0};
    bool rotationBasisConfirmed = false;
};

std::optional<ShotAnimationBounds> rotatedShotAnimationBounds(
    const ShotRotatedAnimationInput& actualAnimation) noexcept;

std::optional<ShotTickPlan> prepareOrdinaryShotTick(
    std::span<const std::uint8_t> slot,
    const std::optional<ShotTickRecordBinding>& actualRecord, float clock = 1) noexcept;
std::optional<ShotTickPlan> completeOrdinaryShotMotion(
    const ShotTickPlan& pending, const TickPosition& genuineVelocity) noexcept;
std::optional<ShotTickPlan> completeShotAnimationLookup(
    const ShotTickPlan& pending, bool genuinePrimaryFound) noexcept;
std::optional<ShotTickPlan> completeShotAnimationCleanup(const ShotTickPlan& pending) noexcept;
std::optional<ShotTickPlan> completeShotAnimationBounds(
    const ShotTickPlan& pending, const ShotAnimationBounds& genuineBounds) noexcept;
std::optional<ShotTickPlan> completeShotAnimationDelete(const ShotTickPlan& pending) noexcept;
std::optional<ShotTickPlan> completeShotAnimationSync(const ShotTickPlan& pending) noexcept;
std::optional<ShotTickPlan> finishShotTickAge(
    const ShotTickPlan& readyAfterRealAnimationWork, float clock = 1) noexcept;

}
