#pragma once
#include "NativeScoreContainer.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <stdexcept>
#include <vector>

namespace th128::record {

struct NativeScoreRowContext {
    std::uint32_t route;
    std::uint32_t rank;
    std::int32_t score;
    std::uint32_t stage;
    std::uint32_t continues;
    std::uint64_t timestamp;
    double numerator;
    double denominator;
    std::array<std::uint8_t, 9> name;
};

class NativeScoreProfile {
public:
    static constexpr std::size_t Bytes = 52232;
    static constexpr std::size_t RouteBytes = 1644;
    static constexpr std::size_t RouteCount = 8;
    static constexpr std::size_t StatsOffset = RouteBytes * RouteCount;
    static constexpr std::size_t StatsBytes = 1068;
    static constexpr std::size_t ClearOffset = StatsOffset + StatsBytes;
    static constexpr std::size_t ClearBytes = 38012;

private:
    static std::uint32_t read32(std::span<const std::uint8_t> bytes, std::size_t offset) {
        std::uint32_t value = 0;
        for (unsigned byte = 0; byte < 4; ++byte)
            value |= std::uint32_t(bytes[offset + byte]) << (byte * 8);
        return value;
    }

    static void write32(std::span<std::uint8_t> bytes, std::size_t offset,
                        std::uint32_t value) {
        for (unsigned byte = 0; byte < 4; ++byte)
            bytes[offset + byte] = static_cast<std::uint8_t>(value >> (byte * 8));
    }

    static void block(std::span<const std::uint8_t> bytes, std::size_t offset,
                      std::uint32_t tag, std::size_t size) {
        if (read32(bytes, offset) != tag || read32(bytes, offset + 8) != size)
            throw std::runtime_error("profile block identity is outside the owned native domain");
    }

    static void validate(std::span<const std::uint8_t> bytes) {
        if (bytes.size() != Bytes)
            throw std::runtime_error("profile size is outside the fixed native domain");
        for (std::size_t route = 0; route < RouteCount; ++route)
            block(bytes, route * RouteBytes, 0x00035243u, RouteBytes);
        block(bytes, StatsOffset, 0x00025453u, StatsBytes);
        block(bytes, ClearOffset, 0x00014443u, ClearBytes);
    }

    static void checksumAt(std::vector<std::uint8_t>& bytes,
                           std::size_t offset, std::size_t size) {
        write32(bytes, offset + 4,
                NativeScoreContainer::checksum(std::span<const std::uint8_t>(bytes).subspan(offset, size)));
    }

public:
    static std::vector<std::uint8_t> assemble(std::span<const std::uint8_t> memory) {
        validate(memory);
        std::vector<std::uint8_t> prepared(memory.begin(), memory.end());
        for (std::size_t route = 0; route < RouteCount; ++route) {
            const auto offset = route * RouteBytes;
            write32(prepared, offset + 12, static_cast<std::uint32_t>(route));
            checksumAt(prepared, offset, RouteBytes);
        }
        checksumAt(prepared, StatsOffset, StatsBytes);
        checksumAt(prepared, ClearOffset, ClearBytes);
        std::vector<std::uint8_t> output;
        output.reserve(Bytes);
        output.insert(output.end(), prepared.begin(), prepared.begin() + StatsOffset);
        output.insert(output.end(), prepared.begin() + ClearOffset, prepared.end());
        output.insert(output.end(), prepared.begin() + StatsOffset, prepared.begin() + ClearOffset);
        return output;
    }

    static std::int32_t insert(std::span<std::uint8_t> memory,
                               const NativeScoreRowContext& context) {
        validate(memory);
        if (context.route >= RouteCount || context.rank >= 5 || context.stage > 255
            || context.continues > 255 || context.timestamp > 0x793406fffull
            || !std::isfinite(context.numerator) || context.numerator < 0
            || !std::isfinite(context.denominator) || context.denominator <= 0)
            throw std::runtime_error("profile context has an unowned scalar value");
        const auto ending = std::find(context.name.begin(), context.name.end(), std::uint8_t{0});
        if (ending == context.name.end())
            throw std::runtime_error("profile name has no native terminator");
        const auto ratio = static_cast<float>(context.numerator / context.denominator);
        const auto slowdown = static_cast<float>(100.0 - 100.0 * static_cast<double>(ratio));
        if (!std::isfinite(slowdown))
            throw std::runtime_error("profile context slowdown is outside the owned finite domain");
        const auto first = std::size_t(context.route) * RouteBytes + 16
            + std::size_t(context.rank) * 320;
        std::size_t position = 0;
        while (position < 10
               && std::bit_cast<std::int32_t>(read32(memory, first + position * 32)) > context.score)
            ++position;
        if (position == 10) return -1;
        for (std::size_t row = 9; row > position; --row)
            std::memmove(memory.data() + first + row * 32,
                         memory.data() + first + (row - 1) * 32, 32);
        const auto offset = first + position * 32;
        write32(memory, offset, std::bit_cast<std::uint32_t>(context.score));
        memory[offset + 4] = static_cast<std::uint8_t>(context.stage);
        memory[offset + 5] = static_cast<std::uint8_t>(context.continues);
        std::copy(context.name.begin(), ending + 1, memory.begin() + offset + 6);
        for (unsigned byte = 0; byte < 8; ++byte)
            memory[offset + 16 + byte] = static_cast<std::uint8_t>(context.timestamp >> (byte * 8));
        write32(memory, offset + 24, std::bit_cast<std::uint32_t>(slowdown));
        return static_cast<std::int32_t>(position);
    }
};

}
