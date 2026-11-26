// 从 0x006425f0 恢复一面第二批敌人，保持共享发弹参数、分支优先级及随机调用顺序。
// 坐标加法与瞄准沿用既有有限数接口；年龄和入场阈值按原 32 位指令解释。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"

namespace kaikeidou {
inline void update_second_enemy(Enemy& enemy, ProjectilePools& projectiles, std::uint64_t player_x, std::uint64_t player_y,
    PatternParameters& parameters, Random& random, RandomPrecision precision, GameMath& functions) {
    const auto signed_word = [](std::uint32_t value) {
        return value >> 31 ? static_cast<std::int64_t>(value) - 0x100000000ll : static_cast<std::int64_t>(value);
    };
    const auto add_to = [&](unsigned destination, std::uint64_t operand) {
        enemy.quad(destination, binary_float::to_binary64(binary_float::add(
            binary_float::binary64(enemy.quad(destination)), binary_float::binary64(operand), static_cast<unsigned>(precision))));
    };
    const auto age = signed_word(enemy.word(0xc));
    const auto entry_end = signed_word(enemy.word(0x80) << 1);
    if (age <= entry_end) {
        add_to(0x38, enemy.quad(0x48));
        add_to(0x40, enemy.quad(0x50));
    } else if (age == 50) {
        parameters.reals[0] = crt_sse2::bits(2.);
        parameters.reals[1] = crt_sse2::bits(.95);
        const std::array<std::uint32_t, 6> counts{{2, 3, 9, 11, 0, 0}};
        const std::array<double, 6> spacings{{.7, .45, .22, .2, 0., 0.}};
        const std::array<double, 6> speeds{{1.1, 1.3, 2., 2.2, 0., 0.}};
        const auto difficulty = parameters.difficulty < 6 ? parameters.difficulty : 3;
        ProjectileEmission emission;
        emission.allocation.kind = 0; emission.allocation.group = 0;
        emission.allocation.color = {{221, 34, 0}};
        emission.allocation.blend = 0; emission.allocation.opacity = 255;
        emission.allocation.delayed = 1; emission.allocation.sound = 41;
        emission.x = enemy.quad(0x38); emission.y = enemy.quad(0x40);
        emission.count = counts[difficulty]; emission.spacing = crt_sse2::bits(spacings[difficulty]);
        emission.speed = crt_sse2::bits(speeds[difficulty]); emission.angle = 0;
        emission.absolute = 0; emission.configuration = 1;
        emit_projectiles(projectiles, emission, player_x, player_y, parameters, random, precision, functions);
    } else if (age >= 160) {
        add_to(0x38, enemy.quad(0xa8));
        add_to(0x40, crt_sse2::bits(1.));
        const auto x = crt_sse2::real(enemy.quad(0x38));
        if (x <= -20. || x >= 404.) enemy.word(0, 0);
    }
    random.integer_bits(1, precision, true);
}
}
