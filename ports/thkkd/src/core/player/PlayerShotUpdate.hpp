// 恢复 0x005c4580、通用及七个武器更新函数和 0x00643be0 目标检查。
#pragma once
#include "core/player/PlayerShotPools.hpp"
#include "core/player/OptionsPool.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/player/PlayerShotEnemyView.hpp"
#include "core/math/Random.hpp"
#include "core/math/GameMath.hpp"

namespace kaikeidou {
template<class Target> inline bool player_shot_target_in_view(const Target& enemy) {
    const auto x = enemy.quad(0x38), y = enemy.quad(0x40);
    if ((x & 0x7fffffffffffffffull) > 0x7ff0000000000000ull ||
        (y & 0x7fffffffffffffffull) > 0x7ff0000000000000ull) return false;
    return crt_sse2::real(x) > 0. && crt_sse2::real(x) < 384. &&
        crt_sse2::real(y) > 0. && crt_sse2::real(y) < 448.;
}
namespace player_shot_detail {
struct Arithmetic {
    unsigned precision;
    using Number = binary_float::Number;
    Number q(std::uint64_t value) const { return binary_float::binary64(value); }
    Number n(double value) const { return q(crt_sse2::bits(value)); }
    Number add(Number a, Number b) const { return binary_float::add(a, b, precision); }
    Number sub(Number a, Number b) const { b.negative = !b.negative; return add(a, b); }
    Number mul(Number a, Number b) const { return binary_float::multiply(a, b, precision); }
    std::uint64_t store(Number value) const { return binary_float::to_binary64(value); }
};
// 原 FLD/FSTP 的复制会静默化 sNaN，保留符号及其余 payload，不按精度控制字舍入。
inline std::uint64_t copied_bits(std::uint64_t value) {
    return (value & 0x7fffffffffffffffull) > 0x7ff0000000000000ull ? value | 0x0008000000000000ull : value;
}
inline void update_common(PlayerShotPools& pools, RandomPrecision precision) {
    const Arithmetic m{static_cast<unsigned>(precision)};
    for (auto& shot : pools.common) if (shot.word(0) == 1) {
        shot.quad(0x10, m.store(m.sub(m.q(shot.quad(0x10)), m.n(24.))));
        if (crt_sse2::real(shot.quad(0x10)) < -56.) shot.word(0, 0);
    }
}
inline void update_straight(PlayerShotPools& pools, double threshold, RandomPrecision precision) {
    const Arithmetic m{static_cast<unsigned>(precision)};
    for (std::size_t index = 0; index < 40; ++index) {
        auto& shot = pools.weapon[index];
        if (shot.word(0) != 1) continue;
        shot.quad(0x18, m.store(m.sub(m.q(shot.quad(0x18)), m.n(24.))));
        if (crt_sse2::real(shot.quad(0x18)) < threshold) shot.word(0, 0);
    }
}
inline void update_5c2780(PlayerShotPools& pools, RandomPrecision precision, GameMath& functions) {
    const Arithmetic m{static_cast<unsigned>(precision)};
    for (std::size_t index = 0; index < 40; ++index) {
        auto& shot = pools.weapon[index];
        if (shot.word(0) != 1) continue;
        shot.quad(0x10, m.store(m.add(m.mul(functions.cosine(shot.quad(0x28)), m.n(24.)), m.q(shot.quad(0x10)))));
        shot.quad(0x18, m.store(m.add(m.mul(functions.sine(shot.quad(0x28)), m.n(24.)), m.q(shot.quad(0x18)))));
        const auto x = crt_sse2::real(shot.quad(0x10)), y = crt_sse2::real(shot.quad(0x18));
        if (y < -48. || x < -48. || x > 432.) shot.word(0, 0);
    }
}
inline void update_5c26e0(PlayerShotPools& pools, RandomPrecision precision) {
    const Arithmetic m{static_cast<unsigned>(precision)};
    for (auto& shot : pools.weapon) if (shot.word(0) == 1) {
        shot.quad(0x10, m.store(m.add(m.q(shot.quad(0x10)), m.q(shot.quad(0x20)))));
        shot.quad(0x18, m.store(m.sub(m.q(shot.quad(0x18)), m.n(24.))));
        if (crt_sse2::real(shot.quad(0x18)) < -46.) shot.word(0, 0);
    }
}
inline void update_5c25e0(PlayerShotPools& pools, RandomPrecision precision) {
    const Arithmetic m{static_cast<unsigned>(precision)};
    for (std::size_t index = 0; index < 16; ++index) {
        auto& shot = pools.weapon[index];
        if (shot.word(0) != 1) continue;
        shot.quad(0x18, m.store(m.sub(m.q(shot.quad(0x18)), m.q(shot.quad(0x20)))));
        const auto old_timer = shot.word(8); shot.word(8, old_timer + 1u);
        shot.quad(0x30, m.store(old_timer <= 7 || old_timer >= 0x80000000u ?
            m.add(m.q(shot.quad(0x30)), m.n(.24)) : m.sub(m.q(shot.quad(0x30)), m.n(.04))));
        if (crt_sse2::real(shot.quad(0x30)) < .04) shot.word(0, 0);
    }
}
inline void update_5c2c50(PlayerShotPools& pools, OptionsPool& options, const EnemyPool& enemies,
    RandomPrecision precision, GameMath& functions, const Enemy* enemy_prefix = nullptr, const ObjectRecord<240>* enemy_suffix = nullptr) {
    const Arithmetic m{static_cast<unsigned>(precision)};
    for (std::size_t group = 0; group < 4; ++group) {
        auto& option = options.options[group];
        for (std::size_t index = 19; index != 0; --index) {
            for (const auto base : {0x50u, 0xf0u, 0x190u})
                option.quad(base + index * 8, copied_bits(option.quad(base + (index - 1) * 8)));
        }
        bool have_head = false;
        for (std::size_t index = 0; index < 20; ++index) {
            auto& shot = pools.weapon[group * 20 + index];
            if (shot.word(0) != 1) continue;
            if (shot.word(8) != 0) { shot.word(8, shot.word(8) - 1u); continue; }
            if (have_head) {
                shot.quad(0x10, copied_bits(option.quad(0x50 + index * 8)));
                shot.quad(0x18, copied_bits(option.quad(0xf0 + index * 8)));
                shot.quad(0x28, copied_bits(option.quad(0x190 + index * 8)));
                continue;
            }
            have_head = true;
            const auto target_index = option.word(0x48);
            const PlayerShotEnemyView target(enemies,target_index,enemy_prefix,enemy_suffix);
            if (target.word(0) != 0 && target.word(0x24) == option.word(0x4c) && player_shot_target_in_view(target)) {
                const auto dx = m.store(m.sub(m.q(target.quad(0x38)), m.q(shot.quad(0x10))));
                const auto dy = m.store(m.sub(m.q(target.quad(0x40)), m.q(shot.quad(0x18))));
                shot.quad(0x28, m.store(functions.atan2(dy, dx)));
            }
            shot.quad(0x10, m.store(m.add(m.mul(functions.cosine(shot.quad(0x28)), m.q(shot.quad(0x20))), m.q(shot.quad(0x10)))));
            shot.quad(0x18, m.store(m.add(m.mul(functions.sine(shot.quad(0x28)), m.q(shot.quad(0x20))), m.q(shot.quad(0x18)))));
            option.quad(0x50, copied_bits(shot.quad(0x10)));
            option.quad(0xf0, copied_bits(shot.quad(0x18)));
            option.quad(0x190, copied_bits(shot.quad(0x28)));
            const auto x = crt_sse2::real(shot.quad(0x10)), y = crt_sse2::real(shot.quad(0x18));
            if (x < -20. || x > 404. || y < -20. || y > 468.) shot.word(0, 0);
        }
    }
}
}
inline void update_player_shots(PlayerShotPools& pools, OptionsPool& options, const EnemyPool& enemies,
    const OptionsContext& context, RandomPrecision precision, GameMath& functions, const Enemy* enemy_prefix = nullptr, const ObjectRecord<240>* enemy_suffix = nullptr) {
    using namespace player_shot_detail;
    update_common(pools, precision);
    switch (context.character * 10u + context.weapon) {
    case 0: update_straight(pools, -56., precision); break;
    case 1: update_5c2c50(pools, options, enemies, precision, functions, enemy_prefix, enemy_suffix); break;
    case 2: update_straight(pools, -40., precision); break;
    case 10: update_straight(pools, -32., precision); break;
    case 12: update_5c2780(pools, precision, functions); break;
    case 20: update_5c26e0(pools, precision); break;
    case 22: update_5c25e0(pools, precision); break;
    default: break;
    }
}
}
