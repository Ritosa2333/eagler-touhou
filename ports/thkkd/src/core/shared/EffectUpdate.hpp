// 恢复效果池更新、独立屏震及包装调用；保留旧计时比较与 32 位回绕。
#pragma once
#include "core/shared/EffectPool.hpp"
#include "core/math/Random.hpp"
#include "core/math/FiniteDivide.hpp"

namespace kaikeidou {
// 0x00a7e5f0 起 32 字节：state0、timer4、保留字段8、magnitude0xc、x0x10、y0x18。
using EffectShake = ObjectRecord<32>;
inline void set_effect_screen_shake(EffectShake& shake, std::uint32_t magnitude, std::uint32_t timer) {
    shake.word(0, 1); shake.word(4, timer); shake.word(0xc, magnitude);
}
inline void update_effects(EffectPool& pool) {
    for (auto& effect : pool.effects) {
        if (effect.word(0) != 1) continue;
        const auto kind = effect.word(0xc), age = effect.word(4);
        std::uint32_t limit = 0;
        if (kind == 0 || kind == 11) limit = 20;
        else if (kind == 1 || kind == 2 || kind == 14) limit = 32;
        else if (kind == 10 || kind == 12 || kind == 13 || kind == 15) limit = 40;
        if (limit != 0 && age == limit) effect.word(0, 0);
        effect.word(4, age + 1u);
    }
}
inline void update_effect_screen_shake(EffectShake& shake, Random& random, RandomPrecision precision) {
    if (shake.word(0) != 1) return;
    using namespace binary_float;
    const auto working = static_cast<unsigned>(precision);
    const auto magnitude = shake.word(0xc);
    const auto maximum = to_binary64(integer(magnitude * 20u));
    auto subtract = integer(magnitude); subtract.negative = !subtract.negative;
    for (const auto destination : {0x10u, 0x18u}) {
        const auto value = divide(random.real_number(maximum, precision), integer(10), working);
        shake.quad(destination, to_binary64(add(value, subtract, working)));
    }
    const auto timer = shake.word(4);
    shake.word(4, timer - 1u);
    if (timer == 0) {
        shake.word(4, 0); shake.word(0, 0); shake.quad(0x10, 0); shake.quad(0x18, 0);
    }
}
inline void update_effect_frame(EffectPool& pool, EffectShake& shake, Random& random, RandomPrecision precision) {
    update_effect_screen_shake(shake, random, precision);
    update_effects(pool);
}
}
