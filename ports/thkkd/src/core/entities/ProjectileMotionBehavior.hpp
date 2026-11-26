// 从 0x005f9050..0x005f8690 恢复行为 4..11，按原 x87 指令与 double 写回顺序执行。
#pragma once
#include "core/entities/ProjectilePool.hpp"
#include "core/math/BinaryFloat.hpp"
#include "core/math/FiniteDivide.hpp"
#include "core/math/CrtSse2Trig.hpp"

namespace kaikeidou {
namespace projectile_motion_detail {
inline std::int64_t signed_word(std::uint32_t value) {
    return value >> 31 ? static_cast<std::int64_t>(value) - 0x100000000ll : value;
}
inline void boundary(Projectile& projectile, std::uint32_t quadrant) {
    const auto x = crt_sse2::real(projectile.quad(0x40));
    const auto y = crt_sse2::real(projectile.quad(0x48));
    if ((quadrant < 2 ? x < -32. : x > 416.) ||
        (quadrant % 2 == 0 ? y < -32. : y > 480.)) projectile.word(0, 0);
}
}
inline void update_accelerated_projectile(Projectile& projectile, std::uint32_t behavior, unsigned precision) {
    if (behavior < 4 || behavior > 7) throw std::out_of_range("Accelerated projectile behavior outside range");
    using namespace binary_float;
    for (const auto offset : {0x40u, 0x48u}) {
        const auto multiplier = add(binary64(crt_sse2::bits(1.)), binary64(projectile.quad(0xa0)), precision);
        const auto displacement = multiply(multiplier, binary64(projectile.quad(offset + 0x10)), precision);
        projectile.quad(offset, to_binary64(add(displacement, binary64(projectile.quad(offset)), precision)));
    }
    projectile.quad(0xa0, to_binary64(multiply(binary64(projectile.quad(0xa0)), binary64(projectile.quad(0xa8)), precision)));
    projectile_motion_detail::boundary(projectile, behavior - 4);
}
inline void update_ramping_projectile(Projectile& projectile, std::uint32_t behavior, unsigned precision) {
    if (behavior < 8 || behavior > 11) throw std::out_of_range("Ramping projectile behavior outside range");
    using namespace binary_float;
    const auto age = projectile_motion_detail::signed_word(projectile.word(0xc));
    const auto start = projectile_motion_detail::signed_word(projectile.word(0x78));
    const auto end = projectile_motion_detail::signed_word(projectile.word(0x7c));
    if (age >= end) {
        for (const auto offset : {0x40u, 0x48u}) projectile.quad(offset,
            to_binary64(add(binary64(projectile.quad(offset)), binary64(projectile.quad(offset + 0x10)), precision)));
    } else if (age >= start) {
        const auto numerator = integer(projectile.word(0xc) - projectile.word(0x78));
        const auto denominator = integer(projectile.word(0x7c) - projectile.word(0x78));
        const auto multiplier = binary64(to_binary64(divide(numerator, denominator, precision)));
        for (const auto offset : {0x40u, 0x48u}) {
            const auto displacement = multiply(binary64(projectile.quad(offset + 0x10)), multiplier, precision);
            projectile.quad(offset, to_binary64(add(displacement, binary64(projectile.quad(offset)), precision)));
        }
    }
    projectile_motion_detail::boundary(projectile, behavior - 8);
}
inline void update_projectile_motion(Projectile& projectile, std::uint32_t behavior, unsigned precision) {
    if (behavior < 8) update_accelerated_projectile(projectile, behavior, precision);
    else update_ramping_projectile(projectile, behavior, precision);
}
}
