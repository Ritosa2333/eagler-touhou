#pragma once
#include "engine/Random.hpp"
#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

namespace th128::record {

struct NativeRecorderSeedCapture {
    std::uint16_t seed;
    std::uint32_t previousCalls;
};

class NativeRecordBootstrap {
    static constexpr std::size_t RouteBytes = 1644;
    static constexpr std::size_t StatsOffset = RouteBytes * 8;
    static constexpr std::size_t StatsBytes = 1068;
    static constexpr std::size_t ClearOffset = StatsOffset + StatsBytes;
    static constexpr std::size_t ClearBytes = 38012;

    static void write16(std::vector<std::uint8_t>& bytes, std::size_t offset,
                        std::uint16_t value) {
        bytes[offset] = static_cast<std::uint8_t>(value);
        bytes[offset + 1] = static_cast<std::uint8_t>(value >> 8);
    }

    static void write32(std::vector<std::uint8_t>& bytes, std::size_t offset,
                        std::uint32_t value) {
        for (unsigned byte = 0; byte < 4; ++byte)
            bytes[offset + byte] = static_cast<std::uint8_t>(value >> (byte * 8));
    }

    static void validateName(std::span<const std::uint8_t> name) {
        if (name.size() != 9 || name.back() != 0)
            throw std::runtime_error("bootstrap native name must have its ninth-byte terminator");
    }

public:
    static constexpr std::size_t Bytes = ClearOffset + ClearBytes;

    static std::vector<std::uint8_t> create(Random& random,
                                           std::span<const std::uint8_t> clearDefaults,
                                           std::span<const std::uint8_t> routeName,
                                           std::span<const std::uint8_t> statsName) {
        if (clearDefaults.size() != 250)
            throw std::runtime_error("bootstrap requires all250 original clear-data literals");
        validateName(routeName);
        validateName(statsName);
        std::vector<std::uint8_t> bytes(Bytes, 0);
        for (std::size_t route = 0; route < 8; ++route) {
            const auto base = route * RouteBytes;
            write32(bytes, base, 0x00035243u);
            write32(bytes, base + 8, RouteBytes);
            for (std::size_t rank = 0; rank < 5; ++rank) {
                for (std::size_t place = 0; place < 10; ++place) {
                    const auto offset = base + 16 + rank * 320 + place * 32;
                    write32(bytes, offset, static_cast<std::uint32_t>(100000 - place * 10000));
                    bytes[offset + 4] = 1;
                    std::copy(routeName.begin(), routeName.end(), bytes.begin() + offset + 6);
                }
            }
        }
        write32(bytes, StatsOffset, 0x00025453u);
        write32(bytes, StatsOffset + 8, StatsBytes);
        std::copy(statsName.begin(), statsName.end(), bytes.begin() + StatsOffset + 12);
        for (std::size_t index = 0; index < 494; ++index)
            write16(bytes, StatsOffset + 80 + index * 2, random.next16());
        write32(bytes, ClearOffset, 0x00014443u);
        write32(bytes, ClearOffset + 8, ClearBytes);
        for (std::size_t index = 0; index < clearDefaults.size(); ++index) {
            const auto value = std::bit_cast<std::int8_t>(clearDefaults[index]);
            write32(bytes, ClearOffset + 156 + index * 152, static_cast<std::uint32_t>(index));
            write32(bytes, ClearOffset + 160 + index * 152,
                    static_cast<std::uint32_t>(static_cast<std::int32_t>(value)));
        }
        return bytes;
    }

    static NativeRecorderSeedCapture captureRecorderSeed(Random& random) noexcept {
        const NativeRecorderSeedCapture capture{random.seed, random.calls};
        random.calls = 0;
        return capture;
    }
};

}
