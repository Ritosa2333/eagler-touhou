// 恢复完整 0x00641b50..0x006420cb 一面敌人行为4；移除后仍继续发射判断和随机推进。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"

namespace kaikeidou {
inline void update_fourth_enemy(Enemy& enemy, ProjectilePools& projectiles, std::uint64_t player_x, std::uint64_t player_y,
    PatternParameters& parameters, Random& random, RandomPrecision precision, GameMath& functions) {
    using namespace binary_float;
    const auto working = static_cast<unsigned>(precision);
    const auto signed_word = [](std::uint32_t value) {
        return value >> 31 ? static_cast<std::int64_t>(value) - 0x100000000ll : value;
    };
    const auto horizontal = multiply(functions.sine(enemy.quad(0xb0)), binary64(crt_sse2::bits(20.)), working);
    enemy.quad(0x38, to_binary64(add(horizontal, binary64(enemy.quad(0xa8)), working)));
    auto drift = multiply(binary64(enemy.quad(0xb8)), binary64(crt_sse2::bits(30.)), working);
    drift.negative = !drift.negative;
    enemy.quad(0xa8, to_binary64(add(binary64(enemy.quad(0xa8)), drift, working)));
    enemy.quad(0x40, to_binary64(add(binary64(enemy.quad(0x40)), binary64(crt_sse2::bits(.9)), working)));
    enemy.quad(0xb0, to_binary64(add(binary64(enemy.quad(0xb0)), binary64(enemy.quad(0xb8)), working)));
    const auto x = crt_sse2::real(enemy.quad(0x38));
    if (x < -50. || x > 434.) enemy.word(0, 0);
    const bool low_difficulty = signed_word(parameters.difficulty) <= 1;
    const auto age = enemy.word(0xc);
    if ((low_difficulty && age == 120) || (!low_difficulty && (age == 14 || age == 80))) {
        parameters.reals[0] = crt_sse2::bits(2.); parameters.reals[1] = crt_sse2::bits(.95);
        const bool nonpositive = crt_sse2::real(enemy.quad(0xb8)) <= 0.;
        const std::array<double, 6> speeds{{1.1, 1.3, 1.8, 2., 0., 0.}};
        const std::array<double, 6> low_spacing{{.8, .7, 0., 0., 0., 0.}};
        const std::array<double, 6> high_spacing{{0., 0., .2, .18, 0., 0.}};
        const auto difficulty = parameters.difficulty < 6 ? parameters.difficulty : 3;
        ProjectileEmission emission;
        emission.allocation.kind = 0; emission.allocation.group = 0;
        emission.allocation.color = nonpositive ? std::array<std::uint32_t, 3>{{34, 255, 34}} : std::array<std::uint32_t, 3>{{255, 119, 0}};
        emission.allocation.blend = 0; emission.allocation.opacity = 255;
        emission.allocation.delayed = 1; emission.allocation.sound = 41;
        emission.x = enemy.quad(0x38); emission.y = enemy.quad(0x40);
        emission.count = low_difficulty ? (nonpositive ? 2u : 1u) : (nonpositive ? 6u : 7u);
        emission.spacing = crt_sse2::bits(low_difficulty ? low_spacing[difficulty] : high_spacing[difficulty]);
        emission.speed = crt_sse2::bits(speeds[difficulty]); emission.angle = 0;
        emission.absolute = 0; emission.configuration = 1;
        emit_projectiles(projectiles, emission, player_x, player_y, parameters, random, precision, functions);
    }
    random.integer_bits(1, precision, true);
}
}
