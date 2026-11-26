// 恢复 0x006415e0..0x00641b45，保留移动、退场、曲线推进和发射的原顺序。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"

namespace kaikeidou {
inline void update_sixth_enemy(Enemy& enemy, ProjectilePools& projectiles, std::uint64_t player_x, std::uint64_t player_y,
    PatternParameters& parameters, Random& random, RandomPrecision precision, GameMath& functions) {
    using namespace binary_float;
    const auto working = static_cast<unsigned>(precision);
    const auto n = [](std::uint64_t bits) { return binary64(bits); };
    const auto signed_word = [](std::uint32_t value) {
        return value >> 31 ? static_cast<std::int64_t>(value) - 0x100000000ll : value;
    };
    auto displacement = multiply(n(enemy.quad(0xb8)), n(crt_sse2::bits(50.)), working);
    displacement.negative = !displacement.negative;
    enemy.quad(0x38, to_binary64(add(n(enemy.quad(0x38)), displacement, working)));
    enemy.quad(0x40, to_binary64(add(multiply(functions.sine(enemy.quad(0xb0)), n(crt_sse2::bits(20.)), working),
        n(enemy.quad(0xa8)), working)));
    if (signed_word(enemy.word(0xc)) > 100) {
        const auto x = crt_sse2::real(enemy.quad(0x38));
        if (x < -20. || x > 404.) enemy.word(0, 0);
    }
    enemy.quad(0xb0, to_binary64(add(n(enemy.quad(0xb0)), n(enemy.quad(0xb8)), working)));
    enemy.quad(0xa8, to_binary64(add(n(enemy.quad(0xa8)), n(crt_sse2::bits(.4)), working)));
    const bool low = signed_word(parameters.difficulty) <= 1;
    if (enemy.word(0xc) == (low ? 76u : 36u)) {
        parameters.reals[0] = crt_sse2::bits(2.); parameters.reals[1] = crt_sse2::bits(.95);
        const auto difficulty = parameters.difficulty < 6 ? parameters.difficulty : 3;
        const std::array<double, 6> speeds{{1.1, 1.3, 1.8, 2., 0., 0.}};
        const std::array<double, 6> low_spacings{{.55, .4, 0., 0., 0., 0.}};
        const std::array<double, 6> high_spacings{{0., 0., .2, .18, 0., 0.}};
        ProjectileEmission emission;
        emission.allocation.kind = 10; emission.allocation.group = 0;
        emission.allocation.color = enemy.word(4) == 0 ? std::array<std::uint32_t, 3>{{255, 34, 34}} : std::array<std::uint32_t, 3>{{0, 255, 0}};
        emission.allocation.blend = 0; emission.allocation.opacity = 255;
        emission.allocation.delayed = 1; emission.allocation.sound = 41;
        emission.x = enemy.quad(0x38); emission.y = enemy.quad(0x40); emission.count = low ? 2u : 5u;
        emission.spacing = crt_sse2::bits(low ? low_spacings[difficulty] : high_spacings[difficulty]);
        emission.speed = crt_sse2::bits(speeds[difficulty]); emission.angle = 0; emission.absolute = 0; emission.configuration = 1;
        emit_projectiles(projectiles, emission, player_x, player_y, parameters, random, precision, functions);
    }
}
}
