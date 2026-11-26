// 恢复 0x00642870 的一面开场敌人行为，以及 0x006446c0 的通常敌人动画更新段。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
#include "core/stages/Stage1Timeline.hpp"
#include <cmath>

namespace kaikeidou {
inline void update_opening_enemy(Enemy& enemy, ProjectilePools& projectiles, std::uint64_t player_x, std::uint64_t player_y,
    const PatternParameters& parameters, Random& random, RandomPrecision precision, GameMath& functions) {
    const stage_detail::Arithmetic math{static_cast<unsigned>(precision)};
    const auto age_bits = enemy.word(0xc);
    const auto age = age_bits >> 31 ? static_cast<std::int64_t>(age_bits) - 0x100000000ll : static_cast<std::int64_t>(age_bits);
    const bool left_side = enemy.word(0x80) == 0;
    if (age < 101) {
        enemy.quad(0x38, math.store(math.add(math.n(left_side ? 2. : -2.), binary_float::binary64(enemy.quad(0x38)))));
        const auto dy = math.i(static_cast<std::uint32_t>(4 - age / 20));
        enemy.quad(0x40, math.store(math.add(dy, binary_float::binary64(enemy.quad(0x40)))));
    } else if (age == 150) {
        const auto kind_bits = enemy.word(4);
        const auto kind = kind_bits >> 31 ? static_cast<std::int64_t>(kind_bits) - 0x100000000ll : static_cast<std::int64_t>(kind_bits);
        if (parameters.difficulty == 0 && kind > 1) return;
        const std::array<std::uint32_t, 6> counts{{1, 1, 5, 7, 0, 0}};
        const std::array<double, 6> speeds{{1.1, 1.3, 1.6, 1.6, 0., 0.}};
        const auto difficulty = parameters.difficulty < 6 ? parameters.difficulty : 3;
        ProjectileEmission emission;
        emission.allocation.kind = 0; emission.allocation.group = 0;
        emission.allocation.color = left_side ? std::array<std::uint32_t, 3>{{255, 0, 0}} : std::array<std::uint32_t, 3>{{0, 204, 0}};
        emission.allocation.blend = 0; emission.allocation.opacity = 255; emission.allocation.delayed = 1; emission.allocation.sound = 41;
        emission.x = enemy.quad(0x38); emission.y = enemy.quad(0x40); emission.count = counts[difficulty];
        emission.spacing = crt_sse2::bits(.2); emission.speed = crt_sse2::bits(speeds[difficulty]); emission.angle = 0;
        emission.absolute = 0; emission.configuration = 0;
        emit_projectiles(projectiles, emission, player_x, player_y, parameters, random, precision, functions);
    } else if (age > 169) {
        auto horizontal = math.add(binary_float::binary64(enemy.quad(0x48)), math.n(1.));
        if (!left_side) horizontal.negative = !horizontal.negative;
        enemy.quad(0x38, math.store(math.add(binary_float::binary64(enemy.quad(0x38)), horizontal)));
        enemy.quad(0x40, math.store(math.sub(binary_float::binary64(enemy.quad(0x40)), math.n(.7))));
        enemy.quad(0x48, math.store(math.add(binary_float::binary64(enemy.quad(0x48)), math.n(.09))));
        const auto x = crt_sse2::real(enemy.quad(0x38));
        if (x > 404. || x < -20.) enemy.word(0, 0);
    }
    random.integer_bits(1, precision, true);
}
inline void update_enemy_animation(Enemy& enemy, RandomPrecision precision) {
    enemy.word(0xc, enemy.word(0xc) + 1u);
    const auto kind_bits = enemy.word(4);
    const auto kind = kind_bits >> 31 ? static_cast<std::int64_t>(kind_bits) - 0x100000000ll : static_cast<std::int64_t>(kind_bits);
    if (kind >= 10 && kind <= 99) return;
    const auto current = enemy.quad(0x38), previous = enemy.quad(0x78);
    const auto phase = binary_float::multiply(binary_float::integer(enemy.word(0xc)),
        binary_float::binary64(crt_sse2::bits(.1)), static_cast<unsigned>(precision));
    const auto animation = static_cast<std::int64_t>(std::floor(crt_sse2::real(binary_float::to_binary64(phase)))) % 2;
    if (!std::isnan(crt_sse2::real(current)) && !std::isnan(crt_sse2::real(previous)) && crt_sse2::real(current) != crt_sse2::real(previous)) {
        enemy.word(0x10, crt_sse2::real(previous) <= crt_sse2::real(current) ? 2u : 1u);
        enemy.word(0x20, static_cast<std::uint32_t>(animation) + 2u);
    } else {
        enemy.word(0x10, 0); enemy.word(0x20, static_cast<std::uint32_t>(animation));
    }
    enemy.quad(0x78, current);
}
}
