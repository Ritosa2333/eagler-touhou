#include "IceBirthRules.hpp"
#include <bit>
#include <cmath>

namespace th128::ice {

namespace {

bool bounded(float value) noexcept {
    return std::isfinite(value) && std::abs(value) <= 16777216.f;
}

std::optional<std::uint32_t> bucketFor(const Position& position) noexcept {
    const auto cell=iceCell(position);if(!cell)return std::nullopt;
    const auto bucket=std::int64_t((*cell)[0])*10+(*cell)[1];
    if (bucket < 0 || bucket >= 80)
        return std::nullopt;
    return static_cast<std::uint32_t>(bucket);
}

void initialize(Timer& timer) noexcept {
    if (!(timer.flags & 1u)) {
        timer.clockSource = NativeClockSource;
        timer.flags |= 1u;
    }
}

}

std::optional<std::array<std::int32_t,2>> iceCell(Position position) noexcept {
    if(!bounded(position.x)||!bounded(position.y)||!bounded(position.z))return std::nullopt;
    double columnValue=(double(position.x)+192.)/-48.;
    if(std::abs(position.x)<0x1p-32f){
        const double scaled=double(position.x)*0x1p56,lower=std::floor(scaled),fraction=scaled-lower;
        const double ticks=lower+(fraction>.5||(fraction==.5&&std::fmod(lower,2.)!=0)?1.:0.);
        columnValue=-4.-ticks*(0x1p-56/48.);
    }
    return std::array<std::int32_t,2>{-std::int32_t(columnValue),-std::int32_t(double(position.y)/-44.79999923706055)};
}

std::optional<SlotSelection> selectIceSlot(
    std::span<const std::int32_t> occupancy, std::uint32_t cursor) noexcept {
    if (occupancy.size() != SlotCount || cursor >= SlotCount)
        return std::nullopt;
    SlotSelection result;
    for (std::uint32_t examined = 1; examined <= SlotCount; ++examined) {
        result.slot = cursor;
        cursor = (cursor + 1) % SlotCount;
        result.nextCursor = cursor;
        result.examined = examined;
        result.overwritesOccupied = occupancy[result.slot] != 0;
        if (!result.overwritesOccupied)
            return result;
    }
    return result;
}

std::optional<IceBirthPlan> prepareIceBirth(
    std::span<const std::int32_t> occupancy, std::uint32_t cursor,
    const IceBirthInput& input, const IceObjectState& prior, bool linksDetached) noexcept {
    const auto selection = selectIceSlot(occupancy, cursor);
    const auto bucket = bucketFor(input.position);
    if (!selection || selection->overwritesOccupied || prior.occupied != 0 || !linksDetached ||
        !bucket || !bounded(input.value))
        return std::nullopt;
    IceBirthPlan result;
    result.selection = *selection;
    result.next = prior;
    auto& state = result.next;
    state.occupied = 1;
    state.position = input.position;
    state.value = input.value;
    initialize(state.lifetime);
    state.lifetime.previous = std::bit_cast<std::int32_t>(std::uint32_t(input.duration) - 1u);
    state.lifetime.current = input.duration;
    state.lifetime.time = float(input.duration);
    initialize(state.delay);
    state.delay.current = input.mode == 0 ? 3 : 0;
    state.delay.previous = state.delay.current - 1;
    state.delay.time = float(state.delay.current);
    state.state48 = 1;
    state.mode = input.mode;
    state.maskSelector = 0;
    result.requiredInsertions = {{{0x65a1cu + *bucket * 12u, 0}, {0x65a10, 0x5c}}};
    return result;
}

std::optional<ActivationIcePlan> planActivationIceBirth(
    std::int32_t nativeFrozenPower, const Position& position) noexcept {
    if (nativeFrozenPower < 0 || nativeFrozenPower > 10000 || !bucketFor(position))
        return std::nullopt;
    ActivationIcePlan result;
    result.nativeFrozenPower = nativeFrozenPower;
    result.birth = {position, 50.f, nativeFrozenPower / 100, 1};
    result.requiredCalls[0] = {RequiredCallKind::AnimationEffect, 0x410276, 0x407ec0,
                              23, 36, 33, 0x65ddc, -1, position, 0x50, 0x6b218};
    result.requiredCalls[1] = {RequiredCallKind::PositionedSound, 0x41029b, 0x4584a0,
                              -1, -1, -1, 0, 47, position, 0, 0};
    return result;
}

}
