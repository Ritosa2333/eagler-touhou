#pragma once

#include <cstdint>
#include <optional>

namespace th128::player {

inline constexpr std::uint16_t ShootFreezeKey = 1;
inline constexpr std::uint16_t BombKey = 2;
inline constexpr std::uint16_t FocusKey = 8;
inline constexpr std::uint16_t UpKey = 16;
inline constexpr std::uint16_t DownKey = 32;
inline constexpr std::uint16_t LeftKey = 64;
inline constexpr std::uint16_t RightKey = 128;
inline constexpr std::uint16_t RapidKey = 512;
inline constexpr std::int32_t RawUnitsPerPixel = 128;
inline constexpr std::int32_t MinimumHorizontalRaw = -23552;
inline constexpr std::int32_t MaximumHorizontalRaw = 23552;
inline constexpr std::int32_t MinimumVerticalRaw = 4096;
inline constexpr std::int32_t MaximumVerticalRaw = 55296;
inline constexpr std::int32_t NormalCardinalRaw = 576;
inline constexpr std::int32_t FocusedCardinalRaw = 256;
inline constexpr std::int32_t NormalDiagonalRaw = 407;
inline constexpr std::int32_t FocusedDiagonalRaw = 181;

struct Input {
    std::uint16_t holding = 0;
    std::uint16_t pressed = 0;
    std::uint16_t released = 0;
};

enum class Direction : std::uint8_t {
    None = 0, Up = 1, Down = 2, Left = 3, Right = 4,
    UpLeft = 5, UpRight = 6, DownLeft = 7, DownRight = 8
};

struct FocusContext {
    bool managerPresent = false;
    bool managerActive = false;
    std::int32_t counterF98 = 0;
    std::int32_t holdCount43914 = 0;
};

struct RawPosition {
    std::int32_t horizontal = 0;
    std::int32_t vertical = 0;
};

struct RawDelta {
    std::int32_t horizontal = 0;
    std::int32_t vertical = 0;
};

struct PixelPosition {
    float horizontal = 0;
    float vertical = 0;
};

struct Movement {
    Direction direction;
    bool focused;
    RawDelta delta;
    RawPosition position;
};

struct ShotCycleCounts {
    std::int32_t previous;
    std::int32_t current;
};

Direction direction(Input input) noexcept;
bool isFocused(Input input, FocusContext context) noexcept;
std::optional<RawDelta> rawDelta(Direction direction, bool focused) noexcept;
RawPosition advancePosition(RawPosition position, RawDelta delta) noexcept;
PixelPosition toPixels(RawPosition position) noexcept;
Movement moveOneTick(RawPosition position, Input input, FocusContext context) noexcept;
bool startsShotCycle(Input input, std::int32_t currentCount) noexcept;
std::optional<ShotCycleCounts> advanceShotCycle(Input input, std::int32_t currentCount) noexcept;
std::optional<bool> recordDue(std::int32_t period, std::int32_t phase, std::int32_t count) noexcept;

}
