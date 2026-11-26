// 从 0x005f9360、0x005f92b0、0x005f9200、0x005f9150 恢复四象限直线弹。
#pragma once
#include "core/entities/ProjectilePool.hpp"
#include "core/math/BinaryFloat.hpp"
#include "core/math/CrtSse2Trig.hpp"

namespace kaikeidou {
inline void update_basic_projectile(Projectile& projectile, std::uint32_t behavior, unsigned precision) {
    if (behavior > 3) throw std::out_of_range("Basic projectile behavior outside range");
    for (const auto offset : {0x40u, 0x48u}) projectile.quad(offset,
        binary_float::to_binary64(binary_float::add(binary_float::binary64(projectile.quad(offset)),
            binary_float::binary64(projectile.quad(offset + 0x10)), precision)));
    const auto x = crt_sse2::real(projectile.quad(0x40)), y = crt_sse2::real(projectile.quad(0x48));
    if ((behavior < 2 ? x < -32. : x > 416.) || (behavior % 2 == 0 ? y < -32. : y > 480.)) projectile.word(0, 0);
}
}
