// 恢复 0x006408e0..0x00640e42，保留三段移动和逐轮共享计时参数写入。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"

namespace kaikeidou {
inline void update_eleventh_enemy(Enemy& enemy, ProjectilePools& projectiles, std::uint64_t player_x, std::uint64_t player_y,
    PatternParameters& parameters, Random& random, RandomPrecision precision, GameMath& functions) {
    using namespace binary_float;
    const auto age = enemy.word(0xc);
    const auto working = static_cast<unsigned>(precision);
    const auto move = [&](std::size_t offset, double amount) {
        enemy.quad(offset, to_binary64(add(binary64(enemy.quad(offset)), binary64(crt_sse2::bits(amount)), working)));
    };
    if (age <= 80 || age >= 0x80000000u) move(0x38, 2.);
    else if (age <= 110) move(0x40, 2.);
    else {
        move(0x38, -3.);
        if (crt_sse2::real(enemy.quad(0x38)) < -30.) enemy.word(0, 0);
    }
    const auto difficulty = parameters.difficulty;
    const bool high = difficulty >= 2 && difficulty < 0x80000000u;
    if ((age != 30 || !high) && age != 110) return;
    const auto index = difficulty < 6 ? difficulty : 3;
    const std::array<std::uint32_t, 6> counts{{1, 1, 5, 7, 0, 0}};
    const std::array<double, 6> spacings{{.3, .25, .2, .18, 0., 0.}};
    const std::array<double, 6> speeds{{1.1, 1.5, 2., 2., 0., 0.}};
    const auto fire = [&](std::uint32_t configuration) {
        ProjectileEmission emission;
        emission.allocation.kind = 0; emission.allocation.group = 1;
        emission.allocation.color = {{0, 255, 255}};
        emission.allocation.blend = 0; emission.allocation.opacity = 255;
        emission.allocation.delayed = 0; emission.allocation.sound = 41;
        emission.x = enemy.quad(0x38); emission.y = enemy.quad(0x40); emission.count = counts[index];
        emission.spacing = crt_sse2::bits(spacings[index]); emission.speed = crt_sse2::bits(speeds[index]);
        emission.angle = 0; emission.absolute = 0; emission.configuration = configuration;
        emit_projectiles(projectiles, emission, player_x, player_y, parameters, random, precision, functions);
    };
    const auto timed_fire = [&](std::uint32_t delay) {
        parameters.integers[0] = 0; parameters.integers[1] = delay; fire(2);
    };
    if (difficulty >= 1 && difficulty < 0x80000000u) {
        if (high) timed_fire(45);
        timed_fire(30);
    }
    timed_fire(15);
    fire(0);
}
}
