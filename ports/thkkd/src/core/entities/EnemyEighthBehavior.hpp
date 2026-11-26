// 恢复 0x00641140..0x00641310，保留移动、退场后发射及 configuration2 的共享参数写入。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
#include "core/math/FiniteDivide.hpp"

namespace kaikeidou {
inline void update_eighth_enemy(Enemy& enemy, ProjectilePools& projectiles, std::uint64_t player_x, std::uint64_t player_y,
    PatternParameters& parameters, Random& random, RandomPrecision precision, GameMath& functions) {
    using namespace binary_float;
    const auto working = static_cast<unsigned>(precision);
    const auto n = [](std::uint64_t bits) { return binary64(bits); };
    enemy.quad(0x38, to_binary64(add(n(enemy.quad(0x38)), n(crt_sse2::bits(.9)), working)));
    enemy.quad(0x40, to_binary64(add(n(enemy.quad(0x40)), n(enemy.quad(0x50)), working)));
    enemy.quad(0x50, to_binary64(add(n(enemy.quad(0x50)), n(crt_sse2::bits(.015)), working)));
    if (crt_sse2::real(enemy.quad(0x40)) >= 468.) enemy.word(0, 0);
    const auto difficulty = parameters.difficulty;
    const bool high = difficulty < 0x80000000u && difficulty >= 2;
    if ((high && enemy.word(0xc) == 60) || enemy.word(0xc) == 170) {
        parameters.integers[0] = 0; parameters.integers[1] = 120;
        const auto index = difficulty < 6 ? difficulty : 3;
        const std::array<std::uint32_t, 6> counts{{9, 13, 31, 39, 0, 0}};
        const std::array<double, 6> speeds{{1.3, 1.6, 2.2, 2.5, 0., 0.}};
        ProjectileEmission emission;
        emission.allocation.kind = 10; emission.allocation.group = 0;
        emission.allocation.color = {{255, 17, 119}};
        emission.allocation.blend = 0; emission.allocation.opacity = 255;
        emission.allocation.delayed = 1; emission.allocation.sound = 41;
        emission.x = enemy.quad(0x38); emission.y = enemy.quad(0x40); emission.count = counts[index];
        // 原除法的零计数得到 +inf；原发射器仍瞄准，随后执行空分配循环。
        emission.spacing = emission.count ? to_binary64(divide(n(0x401921fb54442d28ull), integer(emission.count), working)) : 0x7ff0000000000000ull;
        emission.speed = crt_sse2::bits(speeds[index]); emission.angle = 0; emission.absolute = 0; emission.configuration = 2;
        emit_projectiles(projectiles, emission, player_x, player_y, parameters, random, precision, functions);
    }
}
}
