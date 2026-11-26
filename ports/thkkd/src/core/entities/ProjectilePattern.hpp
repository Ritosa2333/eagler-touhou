// 从 0x005eb160 恢复弹幕行为参数初始化，保留未列出配置的原字段。
#pragma once
#include "core/entities/ProjectilePool.hpp"
#include "core/math/Random.hpp"
#include "core/math/CrtSse2Trig.hpp"

namespace kaikeidou {
struct PatternParameters {
    std::array<std::uint32_t, 5> integers{};
    std::array<std::uint64_t, 2> reals{};
    std::uint32_t difficulty = 0, game_frame = 0;
};
inline void initialize_projectile_pattern(ProjectilePools& pools, std::size_t index, std::uint32_t configuration,
    const PatternParameters& parameters, Random& random, RandomPrecision precision) {
    auto& bullet = pools.bullets.at(index);
    const auto negative_or_nan = [](std::uint64_t value) {
        const auto absolute = value & 0x7fffffffffffffffull;
        return ((value >> 63) && absolute != 0) || absolute > 0x7ff0000000000000ull;
    };
    const auto quadrant = [&]() {
        return (negative_or_nan(bullet.quad(0x50)) ? 0u : 2u) +
               (negative_or_nan(bullet.quad(0x58)) ? 0u : 1u);
    };
    const auto set_angle_base = [&]() { bullet.quad(0xa0, bullet.quad(0x60)); bullet.quad(0xa8, 0); };
    const auto set = [&](std::uint32_t behavior) { bullet.word(8, behavior); };
    const auto& integer = parameters.integers;
    const auto& real = parameters.reals;
    switch (configuration) {
    case 0: set(quadrant()); break;
    case 1:
        bullet.quad(0xa0, real[0]); bullet.quad(0xa8, real[1]); set(4u + quadrant()); break;
    case 2:
        bullet.word(0x78, integer[0]); bullet.word(0x7c, integer[1]); set(8u + quadrant()); break;
    case 15:
        bullet.quad(0xa0, real[0]); bullet.word(0xc, integer[0]); set(15); break;
    case 16:
    case 70:
        bullet.quad(0xa0, real[0]); bullet.quad(0xa8, bullet.quad(0x60));
        bullet.word(0xc, integer[0]); set(configuration); break;
    case 19:
        bullet.word(0x78, integer[0]); bullet.word(0x7c, integer[1]); bullet.word(0x84, integer[3]);
        set(19); bullet.word(0x34, 1); break;
    case 24: {
        set_angle_base(); bullet.quad(0xb0, crt_sse2::bits(.14));
        const std::array<double, 6> values{{.91, .915, .93, .94, 0., 0.}};
        bullet.quad(0xb8, crt_sse2::bits(values[parameters.difficulty < 6 ? parameters.difficulty : 3]));
        bullet.word(0xc, integer[0]); set(24); break;
    }
    case 29:
        set_angle_base(); bullet.quad(0xb0, crt_sse2::bits(.05));
        bullet.word(0x78, integer[0]); bullet.word(0xc, 0u - integer[0] * 2u); set(29); break;
    case 30:
        set_angle_base();
        bullet.quad(0xb0, binary_float::to_binary64(binary_float::multiply(binary_float::integer(integer[2]),
            binary_float::binary64(crt_sse2::bits(.03)), static_cast<unsigned>(precision))));
        bullet.word(0x78, integer[0]); bullet.word(0xc, 0u - integer[0] * 3u); set(30); break;
    case 34:
        set_angle_base(); set(34); bullet.word(0x34, 1); break;
    case 36:
        set_angle_base(); bullet.quad(0xb0, crt_sse2::bits(.1));
        bullet.word(0x78, integer[1] << 1); bullet.word(0xc, 0u - integer[1] * 3u); set(36); break;
    case 50:
        bullet.word(0x78, random.integer_bits(2, precision, true) * 30u - 30u); set(50); break;
    case 51: set(51); break;
    case 53:
        bullet.word(0x78, integer[3]); bullet.word(0x7c, integer[4]); set(8u + quadrant()); break;
    case 60: set(60); break;
    case 62: set(62); break;
    case 63:
        if (parameters.game_frame == 121) bullet.quad(0xa0, 0);
        else bullet.quad(0xa0, binary_float::to_binary64(binary_float::multiply(
            random.real_number(crt_sse2::bits(3141.), precision), binary_float::binary64(crt_sse2::bits(.001)), static_cast<unsigned>(precision))));
        set(63); break;
    case 96:
        bullet.word(0x78, integer[0]); bullet.word(0x7c, integer[1]); bullet.quad(0xa0, crt_sse2::bits(2.)); set(96); break;
    case 115: set(115); bullet.quad(0xa0, 0); break;
    default: break;
    }
}
}
