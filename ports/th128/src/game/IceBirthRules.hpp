#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <span>

namespace th128::ice {

inline constexpr std::uint32_t SlotCount = 4000;
inline constexpr std::uint32_t NativeObjectBytes = 0x68;
inline constexpr std::uint32_t NativeClockSource = 0x4b6f58;

struct Position {
    float x = 0, y = 0, z = 0;
};
std::optional<std::array<std::int32_t,2>> iceCell(Position position) noexcept;

struct Timer {
    std::int32_t previous = 0;
    std::int32_t current = 0;
    float time = 0;
    std::uint32_t clockSource = 0;
    std::uint32_t flags = 0;
};

struct IceObjectState {
    std::int32_t occupied = 0;
    Position position;
    float value = 0;
    Timer lifetime;
    Timer delay;
    std::uint32_t state48 = 0;
    std::int32_t mode = 0;
    std::uint32_t animation50 = 0;
    std::uint32_t animation54 = 0;
    std::uint32_t maskSelector = 0;
};

struct SlotSelection {
    std::uint32_t slot = 0;
    std::uint32_t nextCursor = 0;
    std::uint32_t examined = 0;
    bool overwritesOccupied = false;
};

struct IceBirthInput {
    Position position;
    float value = 50;
    std::int32_t duration = 100;
    std::int32_t mode = 1;
};

struct ListInsertion {
    std::uint32_t managerHeadOffset = 0;
    std::uint32_t objectNodeOffset = 0;
};

enum class BirthContinuation { NeedsListInsertion };

struct IceBirthPlan {
    SlotSelection selection;
    IceObjectState next;
    std::array<ListInsertion, 2> requiredInsertions;
    BirthContinuation continuation = BirthContinuation::NeedsListInsertion;
};

std::optional<SlotSelection> selectIceSlot(
    std::span<const std::int32_t> occupancy, std::uint32_t cursor) noexcept;
std::optional<IceBirthPlan> prepareIceBirth(
    std::span<const std::int32_t> occupancy, std::uint32_t cursor,
    const IceBirthInput& input, const IceObjectState& prior, bool linksDetached) noexcept;

enum class RequiredCallKind { AnimationEffect, PositionedSound };

struct RequiredActivationCall {
    RequiredCallKind kind = RequiredCallKind::AnimationEffect;
    std::uint32_t callSite = 0;
    std::uint32_t target = 0;
    std::int32_t nativeLayer = -1;
    std::int32_t nativeResourceSlot = -1;
    std::int32_t script = -1;
    std::uint32_t resourceManagerOffset = 0;
    std::int32_t sound = -1;
    Position position;
    std::uint32_t resultActivationOffset = 0;
    std::uint32_t managerActiveFlagOffset = 0;
};

struct ActivationIcePlan {
    std::int32_t nativeFrozenPower = 0;
    IceBirthInput birth;
    std::array<RequiredActivationCall, 2> requiredCalls;
    std::uint32_t builderCallSite = 0x4102c9;
};

std::optional<ActivationIcePlan> planActivationIceBirth(
    std::int32_t nativeFrozenPower, const Position& position) noexcept;

}
