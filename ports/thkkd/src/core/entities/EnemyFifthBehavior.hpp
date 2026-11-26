// 恢复 0x0061f530..0x0061f689 的曲线移动，保持旧相位取正弦及逐次存储顺序。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/math/GameMath.hpp"

namespace kaikeidou {
inline void update_fifth_enemy(Enemy& enemy, unsigned precision, GameMath& functions) {
    using namespace binary_float;
    const auto store = [&](unsigned offset, Number value) { enemy.quad(offset, to_binary64(value)); };
    const auto real = [&](unsigned offset) { return binary64(enemy.quad(offset)); };
    const auto constant = [](double value) { return binary64(crt_sse2::bits(value)); };
    store(0x38, add(multiply(functions.sine(enemy.quad(0xb0)), constant(20.), precision), real(0xa8), precision));
    const auto y = crt_sse2::real(enemy.quad(0x40));
    if (y > 100. && y < 138.) {
        store(0x40, add(real(0x40), constant(.4), precision));
        auto displacement = multiply(real(0xb8), constant(80.), precision);
        displacement.negative = !displacement.negative;
        store(0xa8, add(real(0xa8), displacement, precision));
    } else store(0x40, add(real(0x40), constant(.9), precision));
    store(0xb0, add(real(0xb0), real(0xb8), precision));
    if (crt_sse2::real(enemy.quad(0x40)) > 468.) enemy.word(0, 0);
}
}
