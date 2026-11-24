#include "PlayerRules.hpp"

#include <algorithm>
#include <bit>
#include <limits>

namespace th128::player {
namespace {
static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559
              && std::numeric_limits<float>::digits == 24);

std::int32_t wrappingAdd(std::int32_t first, std::int32_t second) noexcept {
    const auto sum = static_cast<std::uint32_t>(first) + static_cast<std::uint32_t>(second);
    return std::bit_cast<std::int32_t>(sum);
}
}

Direction direction(Input input) noexcept {
    if ((input.holding & 0x50) == 0x50) return Direction::UpLeft;
    if ((input.holding & 0x60) == 0x60) return Direction::DownLeft;
    if ((input.holding & 0x90) == 0x90) return Direction::UpRight;
    if ((input.holding & 0xa0) == 0xa0) return Direction::DownRight;
    if (input.holding & DownKey) return Direction::Down;
    if (input.holding & UpKey) return Direction::Up;
    if (input.holding & LeftKey) return Direction::Left;
    if (input.holding & RightKey) return Direction::Right;
    return Direction::None;
}

bool isFocused(Input input, FocusContext context) noexcept {
    return context.managerPresent && context.managerActive && context.counterF98 >= 4
           && ((input.holding & FocusKey) != 0 || context.holdCount43914 >= 12);
}

std::optional<RawDelta> rawDelta(Direction direction, bool focused) noexcept {
    const auto cardinal = focused ? FocusedCardinalRaw : NormalCardinalRaw;
    const auto diagonal = focused ? FocusedDiagonalRaw : NormalDiagonalRaw;
    switch (direction) {
    case Direction::None: return RawDelta{0, 0};
    case Direction::Up: return RawDelta{0, -cardinal};
    case Direction::Down: return RawDelta{0, cardinal};
    case Direction::Left: return RawDelta{-cardinal, 0};
    case Direction::Right: return RawDelta{cardinal, 0};
    case Direction::UpLeft: return RawDelta{-diagonal, -diagonal};
    case Direction::UpRight: return RawDelta{diagonal, -diagonal};
    case Direction::DownLeft: return RawDelta{-diagonal, diagonal};
    case Direction::DownRight: return RawDelta{diagonal, diagonal};
    }
    return std::nullopt;
}

RawPosition advancePosition(RawPosition position, RawDelta delta) noexcept {
    return {std::clamp(wrappingAdd(position.horizontal, delta.horizontal), MinimumHorizontalRaw, MaximumHorizontalRaw),
            std::clamp(wrappingAdd(position.vertical, delta.vertical), MinimumVerticalRaw, MaximumVerticalRaw)};
}

PixelPosition toPixels(RawPosition position) noexcept {
    return {static_cast<float>(position.horizontal) * (1.0f / RawUnitsPerPixel),
            static_cast<float>(position.vertical) * (1.0f / RawUnitsPerPixel)};
}

Movement moveOneTick(RawPosition position, Input input, FocusContext context) noexcept {
    const auto resolvedDirection = direction(input);
    const bool focused = isFocused(input, context);
    const auto delta = *rawDelta(resolvedDirection, focused);
    return {resolvedDirection, focused, delta, advancePosition(position, delta)};
}

bool startsShotCycle(Input input, std::int32_t currentCount) noexcept {
    return currentCount < 0 && ((input.pressed & ShootFreezeKey) != 0
           || ((input.holding & RapidKey) != 0 && (input.holding & ShootFreezeKey) == 0));
}

std::optional<ShotCycleCounts> advanceShotCycle(Input input, std::int32_t currentCount) noexcept {
    if (currentCount < 0 || currentCount > 14) return std::nullopt;
    if (currentCount >= 6 && (input.pressed & ShootFreezeKey) != 0) return ShotCycleCounts{-1, 0};
    if (currentCount == 14) {
        if ((input.holding & RapidKey) != 0 && (input.holding & ShootFreezeKey) == 0) return ShotCycleCounts{-1, 0};
        return ShotCycleCounts{-2, -1};
    }
    return ShotCycleCounts{currentCount, currentCount + 1};
}

std::optional<bool> recordDue(std::int32_t period, std::int32_t phase, std::int32_t count) noexcept {
    if (period < 1 || period > 127 || phase < -128 || phase > 127) return std::nullopt;
    return count % period == phase;
}

}
