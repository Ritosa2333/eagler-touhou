// 恢复完整 0x00640f70..0x00641140 行为9，包含零发数时的原 +Inf 间距栈参数。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
#include "core/math/FiniteDivide.hpp"

namespace kaikeidou {
namespace enemy_ninth_detail {
inline std::uint32_t difficulty_index(std::uint32_t difficulty) { return difficulty < 6 ? difficulty : 3; }
inline std::uint32_t count(std::uint32_t difficulty) {
    const std::array<std::uint32_t, 6> counts{{9, 13, 31, 39, 0, 0}};
    return counts[difficulty_index(difficulty)];
}
}
inline void update_ninth_enemy(Enemy& enemy, ProjectilePools& projectiles, std::uint64_t player_x, std::uint64_t player_y,
    PatternParameters& parameters, Random& random, RandomPrecision precision, GameMath& functions) {
    using namespace binary_float;
    const auto working = static_cast<unsigned>(precision);
    enemy.quad(0x38, to_binary64(add(binary64(enemy.quad(0x38)), binary64(crt_sse2::bits(-.9)), working)));
    enemy.quad(0x40, to_binary64(add(binary64(enemy.quad(0x40)), binary64(enemy.quad(0x50)), working)));
    enemy.quad(0x50, to_binary64(add(binary64(enemy.quad(0x50)), binary64(0x3f8eb851eb851eb8ull), working)));
    if (crt_sse2::real(enemy.quad(0x40)) >= 468.) enemy.word(0, 0);
    const auto signed_difficulty = parameters.difficulty >> 31 ? static_cast<std::int64_t>(parameters.difficulty) - 0x100000000ll : parameters.difficulty;
    if (!((signed_difficulty >= 2 && enemy.word(0xc) == 80) || enemy.word(0xc) == 170)) return;
    parameters.integers[0] = 0; parameters.integers[1] = 120;
    const std::array<double, 6> speeds{{1.3, 1.6, 2.2, 2.5, 0., 0.}};
    const auto speed = crt_sse2::bits(speeds[enemy_ninth_detail::difficulty_index(parameters.difficulty)]);
    const auto divisor = enemy_ninth_detail::count(parameters.difficulty);
    const auto spacing = divisor ? to_binary64(divide(binary64(0x401921fb54442d28ull), integer(divisor), working)) : 0x7ff0000000000000ull;
    const auto count = enemy_ninth_detail::count(parameters.difficulty);
    ProjectileEmission emission;
    emission.allocation.kind = 10; emission.allocation.group = 0; emission.allocation.color = {{17, 119, 255}};
    emission.allocation.blend = 0; emission.allocation.opacity = 255;
    emission.allocation.delayed = 1; emission.allocation.sound = 41;
    emission.speed = speed; emission.spacing = spacing; emission.count = count;
    emission.x = enemy.quad(0x38); emission.y = enemy.quad(0x40);
    emission.angle = 0; emission.absolute = 0; emission.configuration = 2;
    emit_projectiles(projectiles, emission, player_x, player_y, parameters, random, precision, functions);
}
}
