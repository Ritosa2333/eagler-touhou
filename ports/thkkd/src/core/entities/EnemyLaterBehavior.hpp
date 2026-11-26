// 完整恢复 63fe10/63fbd0/63f9a0/63f7b0 的敌人18..21，保留触发、共享参数和出界判断顺序。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
#include "core/player/OptionsMotion.hpp"

namespace kaikeidou {
struct EnemyLaterDivisionByZero : std::domain_error {
    EnemyLaterDivisionByZero() : std::domain_error("Enemy 21 firing period is zero") {}
};
inline void update_later_enemy(std::uint32_t behavior, Enemy& enemy, ProjectilePools& projectiles,
    std::uint64_t player_x, std::uint64_t player_y, PatternParameters& parameters,
    Random& random, RandomPrecision precision, GameMath& functions) {
    using namespace binary_float;
    const auto working = static_cast<unsigned>(precision);
    const auto signed_word = [](std::uint32_t v) { return v >> 31 ? std::int64_t(v) - 0x100000000ll : v; };
    const auto subtract = [&](Number a, Number b) { b.negative = !b.negative; return add(a, b, working); };
    const auto age = signed_word(enemy.word(0xc));
    const auto difficulty = parameters.difficulty < 6 ? parameters.difficulty : 3;
    const auto emit = [&](std::uint32_t kind, std::array<std::uint32_t, 3> color, std::uint32_t opacity,
        std::uint32_t count, double spacing, double speed, std::uint64_t angle, std::uint32_t absolute,
        std::uint32_t configuration, std::uint32_t delayed) {
        ProjectileEmission e;
        e.allocation.kind = kind; e.allocation.group = 0; e.allocation.color = color;
        e.allocation.blend = behavior == 21 ? 0 : 2; e.allocation.opacity = opacity;
        e.allocation.delayed = delayed; e.allocation.sound = 41;
        e.x = enemy.quad(0x38); e.y = enemy.quad(0x40); e.count = count;
        e.spacing = crt_sse2::bits(spacing); e.speed = crt_sse2::bits(speed); e.angle = angle;
        e.absolute = absolute; e.configuration = configuration;
        emit_projectiles(projectiles, e, player_x, player_y, parameters, random, precision, functions);
    };
    if (behavior == 21) {
        if (age <= 30 || age >= 900) {
            enemy.quad(0x40, ease_option_value(enemy.quad(0x40), crt_sse2::bits(age <= 30 ? 120. : -40.), crt_sse2::bits(20.), working));
            if (age >= 900 && crt_sse2::real(enemy.quad(0x40)) < -30.) enemy.word(0, 0);
        } else {
            const auto period = signed_word(parameters.integers[0]);
            if (!period) throw EnemyLaterDivisionByZero();
            if (age % period == 0) {
                constexpr std::array<std::uint32_t, 6> counts{{5, 7, 15, 19, 0, 0}};
                constexpr std::array<double, 6> spacing{{.31, .25, .11, .08, 0., 0.}}, speed{{1.1, 1.4, 2., 2.2, 0., 0.}};
                emit(10, {{0, 0, 255}}, 255, counts[difficulty], spacing[difficulty], speed[difficulty], 0, 0, 1, 1);
            }
        }
        return;
    }
    if (behavior < 18 || behavior > 20) throw std::out_of_range("Enemy behavior outside 18..21");
    enemy.quad(0x38, to_binary64(add(binary64(enemy.quad(0x38)), binary64(enemy.quad(0x48)), working)));
    if (age < (behavior == 18 ? 8 : 20)) return;
    const auto opacity = [&](std::uint32_t i) {
        auto value = subtract(integer(255), divide(integer(i * 255u), integer(7), working));
        return truncate(rounded(value, 0, 53));
    };
    if (behavior == 18 && age == 8 && enemy.word(0x80) != 0) {
        constexpr std::array<double, 6> speed{{1.3, 1.6, 2.7, 3.2, 0., 0.}};
        parameters.reals[0] = to_binary64(multiply(subtract(random.real_number(crt_sse2::bits(100.), precision), integer(50)), binary64(crt_sse2::bits(.001)), working));
        for (std::uint32_t i = 0; i < 8; ++i) {
            parameters.integers[0] = 0u - i * 2u;
            emit(10, enemy.word(4) == 1 ? std::array<std::uint32_t, 3>{{0, 0, 255}} : std::array<std::uint32_t, 3>{{0, 255, 0}},
                opacity(i), 1, 0., speed[difficulty], 0x3ff921fb54442d28ull, 1, 15, 0);
        }
    } else if (behavior == 19 && enemy.word(0x84) == 0 && crt_sse2::real(enemy.quad(0x38)) >= 192.) {
        enemy.word(0x84, 1);
        constexpr std::array<double, 6> speed{{1.5, 1.9, 4., 5., 0., 0.}}, spacing{{.25, .2, .14, .1, 0., 0.}};
        constexpr std::array<std::uint32_t, 6> counts{{1, 1, 5, 7, 0, 0}};
        parameters.reals[0] = 0;
        for (std::uint32_t i = 0; i < 8; ++i) {
            parameters.integers[0] = 0u - i;
            emit(10, {{255, 0, 0}}, opacity(i), counts[difficulty], spacing[difficulty], speed[difficulty], 0, 0, 15, 0);
        }
    } else if (behavior == 20 && age == 30 && ((parameters.difficulty <= 2 && signed_word(enemy.word(4)) <= std::int64_t(parameters.difficulty) + 1) || parameters.difficulty == 3)) {
        constexpr std::array<double, 6> speed{{1.5, 1.9, 3., 3.8, 0., 0.}};
        constexpr std::array<std::uint32_t, 6> counts{{4, 8, 18, 28, 0, 0}};
        parameters.reals[0] = random.integer_bits(1, precision, true) == 0 ? 0x400921fb54442d28ull : 0;
        for (std::uint32_t i = 0; i <= counts[difficulty]; ++i) {
            parameters.integers[0] = 0u - i;
            emit(15, {{255, 32, 255}}, 255, 1, 0., speed[difficulty], 0, 0, 16, 0);
        }
    }
    const auto x = crt_sse2::real(enemy.quad(0x38));
    if (x > 404. || x < -20.) enemy.word(0, 0);
}
}
