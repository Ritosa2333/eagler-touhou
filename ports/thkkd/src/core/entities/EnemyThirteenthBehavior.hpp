// 恢复 0x006400a0..0x0064036e 的状态及发射作用；原调度方忽略 EAX 返回值。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"

namespace kaikeidou {
struct EnemyThirteenthDivisionByZero : std::domain_error {
    EnemyThirteenthDivisionByZero() : std::domain_error("Enemy 13 firing period is zero") {}
};
inline void update_thirteenth_enemy(Enemy& enemy, ProjectilePools& projectiles, std::uint64_t player_x, std::uint64_t player_y,
    PatternParameters& parameters, Random& random, RandomPrecision precision, GameMath& functions) {
    using namespace binary_float;
    const auto working = static_cast<unsigned>(precision);
    enemy.quad(0x40, to_binary64(add(binary64(enemy.quad(0x40)), binary64(enemy.quad(0x50)), working)));
    const auto y = crt_sse2::real(enemy.quad(0x40));
    if (y > 480.) enemy.word(0, 0);
    const auto difficulty = parameters.difficulty;
    const std::array<std::uint32_t, 6> periods{{29, 25, 13, 8, 0, 0}};
    const auto period = periods[difficulty < 6 ? difficulty : 3];
    if (!period) throw EnemyThirteenthDivisionByZero();
    const auto signed_word = [](std::uint32_t value) { return value >> 31 ? static_cast<std::int64_t>(value) - 0x100000000ll : value; };
    if (signed_word(enemy.word(0xc)) % period != 0 || y >= 454.) return;
    const auto emit = [&](std::array<std::uint32_t, 3> color, std::uint64_t spacing, std::uint64_t angle) {
        ProjectileEmission emission;
        emission.allocation.kind = 0; emission.allocation.group = 0; emission.allocation.color = color;
        emission.allocation.blend = 0; emission.allocation.opacity = 255;
        emission.allocation.delayed = 1; emission.allocation.sound = 41;
        emission.x = enemy.quad(0x38); emission.y = enemy.quad(0x40); emission.count = 2;
        emission.spacing = spacing;
        emission.speed = to_binary64(add(multiply(integer(difficulty), binary64(crt_sse2::bits(.4)), working), binary64(crt_sse2::bits(1.5)), working));
        emission.angle = angle; emission.absolute = 1; emission.configuration = 0;
        emit_projectiles(projectiles, emission, player_x, player_y, parameters, random, precision, functions);
    };
    emit({{204, 204, 0}}, 0x3ff921fb54442d28ull, 0xbff921fb54442d28ull);
    emit({{0, 204, 204}}, 0x3ff921fb54442d28ull, 0x3ff921fb54442d28ull);
    if (signed_word(difficulty) >= 1) {
        emit({{204, 0, 204}}, 0x400921fb54442d28ull, 0x3ff921fb54442d28ull);
        if (signed_word(difficulty) >= 2) emit({{34, 34, 255}}, 0x400921fb54442d28ull, 0);
    }
}
}
