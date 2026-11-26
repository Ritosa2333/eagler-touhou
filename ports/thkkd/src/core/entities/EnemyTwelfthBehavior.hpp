// 恢复完整 0x00640370..0x006408d2 行为12，保留三段移动与group1多组发射的共享参数顺序。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"

namespace kaikeidou {
inline void update_twelfth_enemy(Enemy& enemy, ProjectilePools& projectiles, std::uint64_t player_x, std::uint64_t player_y,
    PatternParameters& parameters, Random& random, RandomPrecision precision, GameMath& functions) {
    using namespace binary_float;
    const auto working = static_cast<unsigned>(precision);
    const auto signed_word = [](std::uint32_t value) {
        return value >> 31 ? static_cast<std::int64_t>(value) - 0x100000000ll : value;
    };
    const auto age = signed_word(enemy.word(0xc));
    if (age <= 80) {
        enemy.quad(0x38, to_binary64(add(binary64(enemy.quad(0x38)), binary64(crt_sse2::bits(-2.)), working)));
    } else if (age <= 110) {
        enemy.quad(0x40, to_binary64(add(binary64(enemy.quad(0x40)), binary64(crt_sse2::bits(2.)), working)));
    } else {
        enemy.quad(0x38, to_binary64(add(binary64(enemy.quad(0x38)), binary64(crt_sse2::bits(3.)), working)));
        if (crt_sse2::real(enemy.quad(0x38)) > 414.) enemy.word(0, 0);
    }
    if (!((enemy.word(0xc) == 30 && signed_word(parameters.difficulty) >= 2) || enemy.word(0xc) == 110)) return;
    const auto emit = [&](std::uint32_t configuration, std::uint32_t timer) {
        if (configuration == 2) { parameters.integers[0] = 0; parameters.integers[1] = timer; }
        const auto difficulty = parameters.difficulty < 6 ? parameters.difficulty : 3;
        const std::array<double, 6> speeds{{1.1, 1.5, 2., 2., 0., 0.}};
        const std::array<double, 6> spacings{{.3, .25, .2, .18, 0., 0.}};
        const std::array<std::uint32_t, 6> counts{{1, 1, 5, 7, 0, 0}};
        ProjectileEmission emission;
        emission.allocation.kind = 0; emission.allocation.group = 1; emission.allocation.color = {{255, 128, 0}};
        emission.allocation.blend = 0; emission.allocation.opacity = 255;
        emission.allocation.delayed = 0; emission.allocation.sound = 41;
        emission.speed = crt_sse2::bits(speeds[difficulty]); emission.spacing = crt_sse2::bits(spacings[difficulty]); emission.count = counts[difficulty];
        emission.x = enemy.quad(0x38); emission.y = enemy.quad(0x40);
        emission.angle = 0; emission.absolute = 0; emission.configuration = configuration;
        emit_projectiles(projectiles, emission, player_x, player_y, parameters, random, precision, functions);
    };
    if (signed_word(parameters.difficulty) >= 1) {
        if (signed_word(parameters.difficulty) >= 2) emit(2, 45);
        emit(2, 30);
    }
    emit(2, 15);
    emit(0, 0);
}
}
