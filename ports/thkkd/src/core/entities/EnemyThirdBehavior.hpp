// 恢复 0x006420d0..0x006425f0：先移动及退场判定，再发弹，最后推进随机游标。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"

namespace kaikeidou {
inline void update_third_enemy(Enemy& enemy, ProjectilePools& projectiles, std::uint64_t player_x, std::uint64_t player_y,
    PatternParameters& parameters, Random& random, RandomPrecision precision, GameMath& functions) {
    using namespace binary_float;
    const auto working = static_cast<unsigned>(precision);
    const auto number = [](std::uint64_t value) { return binary64(value); };
    enemy.quad(0x38, to_binary64(add(multiply(functions.sine(enemy.quad(0xb0)), number(crt_sse2::bits(20.)), working),
        number(enemy.quad(0xa8)), working)));
    enemy.quad(0x40, to_binary64(add(number(enemy.quad(0x40)), number(crt_sse2::bits(.9)), working)));
    enemy.quad(0xb0, to_binary64(add(number(enemy.quad(0xb0)), number(enemy.quad(0xb8)), working)));
    if (crt_sse2::real(enemy.quad(0x40)) > 468.) enemy.word(0, 0);
    const auto difficulty_bits = parameters.difficulty;
    const auto signed_difficulty = difficulty_bits >> 31 ? static_cast<std::int64_t>(difficulty_bits) - 0x100000000ll : difficulty_bits;
    const bool low_difficulty = signed_difficulty <= 1;
    const auto age = enemy.word(0xc);
    if (age == 80 || (!low_difficulty && age == 14)) {
        parameters.reals[0] = crt_sse2::bits(2.);
        parameters.reals[1] = crt_sse2::bits(.95);
        const auto difficulty = difficulty_bits < 6 ? difficulty_bits : 3;
        const std::array<double, 6> speeds{{1.1, 1.3, 1.8, 2., 0., 0.}};
        const std::array<double, 6> low_spacings{{.8, .7, 0., 0., 0., 0.}};
        const std::array<double, 6> high_spacings{{0., 0., .2, .18, 0., 0.}};
        const bool red = enemy.word(4) == 0;
        ProjectileEmission emission;
        emission.allocation.kind = 3; emission.allocation.group = 0;
        emission.allocation.color = red ? std::array<std::uint32_t, 3>{{255, 34, 34}} : std::array<std::uint32_t, 3>{{34, 34, 255}};
        emission.allocation.blend = 0; emission.allocation.opacity = 255;
        emission.allocation.delayed = 1; emission.allocation.sound = 41;
        emission.x = enemy.quad(0x38); emission.y = enemy.quad(0x40);
        emission.count = low_difficulty ? (red ? 1u : 2u) : (red ? 7u : 6u);
        emission.spacing = crt_sse2::bits(low_difficulty ? low_spacings[difficulty] : high_spacings[difficulty]);
        emission.speed = crt_sse2::bits(speeds[difficulty]); emission.angle = 0;
        emission.absolute = 0; emission.configuration = 1;
        emit_projectiles(projectiles, emission, player_x, player_y, parameters, random, precision, functions);
    }
    random.integer_bits(1, precision, true);
}
}
