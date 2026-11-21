#pragma once
#include <cstdint>

namespace th128 {
struct Random {
    std::uint16_t seed = 0;
    std::uint32_t calls = 0;

    std::uint16_t next16() {
        const auto transformed = std::uint16_t((seed ^ 0x9630u) - 0x6553u);
        seed = std::uint16_t((std::uint32_t(transformed) << 2) | (transformed >> 14));
        ++calls;
        return seed;
    }

    std::uint32_t next32() {
        const auto first = std::uint16_t((seed ^ 0x9630u) - 0x6553u);
        const auto intermediate = std::uint16_t((std::uint32_t(first) << 2) | (first >> 14));
        const auto second = std::uint16_t((intermediate ^ 0x9630u) - 0x6553u);
        seed = std::uint16_t((std::uint32_t(second) << 2) | (second >> 14));
        calls += 2;
        return (std::uint32_t(first) << 16) | second;
    }

    float unit() { return float(double(next32())/4294967296.0); }
    float signedUnit() { return float(double(next32())/2147483648.0-1.0); }
    float signedAngle() { return float(double(signedUnit())*3.1415927410125732); }
};
}
