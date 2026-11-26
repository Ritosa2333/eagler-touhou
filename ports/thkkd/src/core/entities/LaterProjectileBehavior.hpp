// 完整恢复弹行为15（5f8500..5f8685）与16（5f8330..5f84f3），保留延迟、转向和逐次写回顺序。
#pragma once
#include "core/entities/ProjectilePool.hpp"
#include "core/math/GameMath.hpp"
#include "core/math/FiniteDivide.hpp"

namespace kaikeidou {
inline void update_later_projectile_motion(Projectile& projectile, std::uint32_t behavior, unsigned precision, GameMath& math) {
    using namespace binary_float;
    if (behavior != 15 && behavior != 16) throw std::out_of_range("Later projectile behavior outside15..16");
    const auto raw_age = projectile.word(0xc);
    const auto age = raw_age >> 31 ? std::int64_t(raw_age) - 0x100000000ll : raw_age;
    if (behavior == 15 && age >= 30 && age <= 45)
        projectile.quad(0x60, to_binary64(add(binary64(projectile.quad(0x60)), binary64(projectile.quad(0xa0)), precision)));
    if (age >= 0) {
        if (behavior == 16) {
            const auto wave = multiply(math.sine(projectile.quad(0xa0)), binary64(0x3fd3333333333333ull), precision);
            const auto scaled = divide(multiply(wave, binary64(projectile.quad(0x68)), precision), binary64(0x400c000000000000ull), precision);
            projectile.quad(0x60, to_binary64(add(scaled, binary64(projectile.quad(0xa8)), precision)));
            const auto advance = divide(multiply(binary64(0x3fc999999999999aull), binary64(projectile.quad(0x68)), precision), binary64(0x400c000000000000ull), precision);
            projectile.quad(0xa0, to_binary64(add(advance, binary64(projectile.quad(0xa0)), precision)));
        }
        const auto cos = math.cosine(projectile.quad(0x60));
        projectile.quad(0x40, to_binary64(add(multiply(cos, binary64(projectile.quad(0x68)), precision), binary64(projectile.quad(0x40)), precision)));
        const auto sin = math.sine(projectile.quad(0x60));
        projectile.quad(0x48, to_binary64(add(multiply(sin, binary64(projectile.quad(0x68)), precision), binary64(projectile.quad(0x48)), precision)));
    }
    const auto x = crt_sse2::real(projectile.quad(0x40)), y = crt_sse2::real(projectile.quad(0x48));
    if (x < -32. || y < -32. || x > 416. || y > 480.) projectile.word(0, 0);
}
}
