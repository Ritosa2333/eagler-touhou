#pragma once
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>

namespace th128::record {

struct NativeFreshRecordFields {
    std::int32_t highScore;
    std::int32_t highScoreContinue;
    std::int32_t playCount;
};

class NativeRecordAccounting {
    static constexpr std::size_t ProfileBytes = 52232;
    static constexpr std::size_t RouteBytes = 1644;

    static void validate(std::span<const std::uint8_t> profile, unsigned route, unsigned rank = 0) {
        if (profile.size() != ProfileBytes || route >= 8 || rank >= 5)
            throw std::runtime_error("native record field domain is unavailable");
    }

    static std::uint32_t read32(std::span<const std::uint8_t> bytes, std::size_t offset) {
        std::uint32_t value = 0;
        for (unsigned byte = 0; byte < 4; ++byte)
            value |= std::uint32_t(bytes[offset + byte]) << (byte * 8);
        return value;
    }

    static void write32(std::span<std::uint8_t> bytes, std::size_t offset, std::uint32_t value) {
        for (unsigned byte = 0; byte < 4; ++byte)
            bytes[offset + byte] = static_cast<std::uint8_t>(value >> (byte * 8));
    }

    static std::int32_t increment(std::span<std::uint8_t> bytes, std::size_t offset,
                                  std::int32_t cap) {
        auto value = read32(bytes, offset);
        if (std::bit_cast<std::int32_t>(value) < cap) {
            ++value;
            write32(bytes, offset, value);
        }
        return std::bit_cast<std::int32_t>(value);
    }

public:
    static constexpr std::size_t PlaytimeOffset = 0x3360 + 0x48;

    static NativeFreshRecordFields beginFresh(std::span<std::uint8_t> profile,
                                               unsigned route, unsigned rank,
                                               std::uint32_t sessionField74) {
        validate(profile, route, rank);
        const auto base = std::size_t(route) * RouteBytes;
        const auto first = base + 16 + std::size_t(rank) * 320;
        const auto highScore = std::bit_cast<std::int32_t>(read32(profile, first));
        const auto continued = std::bit_cast<std::int8_t>(profile[first + 5]);
        const auto playCount = sessionField74 == 0 ? increment(profile, base + 0x650, 99999)
            : std::bit_cast<std::int32_t>(read32(profile, base + 0x650));
        return {highScore, continued, playCount};
    }

    static void recordClear(std::span<std::uint8_t> profile, unsigned route, unsigned rank,
                            std::uint32_t replayMode) {
        validate(profile, route, rank);
        if (replayMode != 1)
            increment(profile, std::size_t(route) * RouteBytes + 0x658 + rank * 4, 99999);
    }

    static void recordActiveFrame(std::span<std::uint8_t> profile, unsigned route,
                                  std::uint32_t replayMode, std::uint32_t sessionFlags60,
                                  bool returnedBeforeActiveTail) {
        validate(profile, route);
        if (!returnedBeforeActiveTail && !(sessionFlags60 & 0x74u) && replayMode != 1)
            increment(profile, std::size_t(route) * RouteBytes + 0x654, 0x0cdfe5ff);
    }

    static void addPlaytime(std::span<std::uint8_t> profile, double elapsedSeconds,
                            std::uint32_t sessionField74, std::int32_t requestedScene) {
        validate(profile, 0);
        if (!std::isfinite(elapsedSeconds))
            throw std::runtime_error("native playtime requires a finite real interval");
        if (sessionField74 != 0 || requestedScene == -1 || requestedScene == 3 || elapsedSeconds <= 0)
            return;
        const auto scaled = elapsedSeconds * 100.0;
        if (!std::isfinite(scaled) || scaled >= 9223372036854775808.0)
            throw std::runtime_error("native playtime interval exceeds the owned int64 domain");
        const auto incrementValue = static_cast<std::uint64_t>(static_cast<std::int64_t>(scaled));
        std::uint64_t current = 0;
        for (unsigned byte = 0; byte < 8; ++byte)
            current |= std::uint64_t(profile[PlaytimeOffset + byte]) << (byte * 8);
        current += incrementValue;
        for (unsigned byte = 0; byte < 8; ++byte)
            profile[PlaytimeOffset + byte] = static_cast<std::uint8_t>(current >> (byte * 8));
    }
};

}
