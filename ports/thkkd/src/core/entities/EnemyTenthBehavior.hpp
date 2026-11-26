// 恢复 0x00640e50..0x00640f6b，保持纵向移动存回后再比较和发射的顺序。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"

namespace kaikeidou {
inline void update_tenth_enemy(Enemy& enemy, ProjectilePools& projectiles, std::uint64_t player_x, std::uint64_t player_y,
    PatternParameters& parameters, Random& random, RandomPrecision precision, GameMath& functions) {
    using namespace binary_float;
    const auto age = enemy.word(0xc);
    const bool descending = age <= 80 || age >= 0x80000000u;
    auto step = binary64(crt_sse2::bits(3.));
    step.negative = !descending;
    enemy.quad(0x40, to_binary64(add(binary64(enemy.quad(0x40)), step, static_cast<unsigned>(precision))));
    if (!descending && crt_sse2::real(enemy.quad(0x40)) < -20.) enemy.word(0, 0);
    if (age == 80 && parameters.difficulty >= 2 && parameters.difficulty < 0x80000000u) {
        ProjectileEmission emission;
        emission.allocation.kind = 10; emission.allocation.group = 2;
        emission.allocation.color = {{0, 0, 255}};
        emission.allocation.blend = 0; emission.allocation.opacity = 255;
        emission.allocation.delayed = 0; emission.allocation.sound = 41;
        emission.x = enemy.quad(0x38); emission.y = enemy.quad(0x40); emission.count = 1;
        emission.spacing = 0; emission.speed = crt_sse2::bits(2.); emission.angle = 0x3ff921fb54442d28ull;
        emission.absolute = 1; emission.configuration = 0;
        emit_projectiles(projectiles, emission, player_x, player_y, parameters, random, precision, functions);
    }
}
}
