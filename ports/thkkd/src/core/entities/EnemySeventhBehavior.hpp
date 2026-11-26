// 恢复完整 0x00641310..0x006415d8 行为7，包含移除后继续发射和两组原栈参数顺序。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"

namespace kaikeidou {
inline void update_seventh_enemy(Enemy& enemy, ProjectilePools& projectiles, std::uint64_t player_x, std::uint64_t player_y,
    PatternParameters& parameters, Random& random, RandomPrecision precision, GameMath& functions) {
    using namespace binary_float;
    const auto working = static_cast<unsigned>(precision);
    enemy.quad(0x38, to_binary64(add(binary64(enemy.quad(0x38)), binary64(enemy.quad(0x48)), working)));
    enemy.quad(0x40, to_binary64(add(binary64(enemy.quad(0x40)), binary64(crt_sse2::bits(1.8)), working)));
    if (crt_sse2::real(enemy.quad(0x40)) >= 468.) enemy.word(0, 0);
    if (enemy.word(0xc) != 92) return;
    const std::array<double, 6> speeds{{1.7, 1.9, 1.9, 2.1, 0., 0.}};
    const std::array<double, 6> spacings{{.78, .52, .26, .17, 0., 0.}};
    const auto emit = [&](std::uint32_t configuration, const std::array<std::uint32_t, 6>& counts) {
        const auto difficulty = parameters.difficulty < 6 ? parameters.difficulty : 3;
        ProjectileEmission emission;
        emission.allocation.kind = 3; emission.allocation.group = 0;
        emission.allocation.color = {{0, 204, 204}};
        emission.allocation.blend = 0; emission.allocation.opacity = 255;
        emission.allocation.delayed = 1; emission.allocation.sound = 41;
        emission.speed = crt_sse2::bits(speeds[difficulty]); emission.spacing = crt_sse2::bits(spacings[difficulty]);
        emission.count = counts[difficulty]; emission.x = enemy.quad(0x38); emission.y = enemy.quad(0x40);
        emission.angle = 0; emission.absolute = 0; emission.configuration = configuration;
        emit_projectiles(projectiles, emission, player_x, player_y, parameters, random, precision, functions);
    };
    const auto signed_difficulty = parameters.difficulty >> 31 ? static_cast<std::int64_t>(parameters.difficulty) - 0x100000000ll : parameters.difficulty;
    if (signed_difficulty >= 2) {
        parameters.reals[0] = crt_sse2::bits(2.); parameters.reals[1] = crt_sse2::bits(.93);
        emit(1, {{2, 2, 4, 6, 0, 0}});
    }
    emit(0, {{2, 4, 3, 5, 0, 0}});
}
}
