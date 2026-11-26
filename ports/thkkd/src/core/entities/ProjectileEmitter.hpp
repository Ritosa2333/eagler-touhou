// 从 0x0060e600 恢复瞄准与绝对方向的奇偶扇形发射，保持每次分配及初始化顺序。
#pragma once
#include "core/entities/ProjectilePattern.hpp"
#include "core/math/GameMath.hpp"

namespace kaikeidou {
struct ProjectileEmission {
    ProjectileAllocation allocation;
    std::uint64_t x = 0, y = 0;
    std::uint32_t count = 0;
    std::uint64_t spacing = 0, speed = 0, angle = 0;
    std::uint32_t absolute = 0, configuration = 0;
};
inline void emit_projectiles(ProjectilePools& pools, const ProjectileEmission& input,
    std::uint64_t player_x, std::uint64_t player_y, const PatternParameters& parameters,
    Random& random, RandomPrecision precision, GameMath& math) {
    using namespace binary_float;
    const auto working = static_cast<unsigned>(precision);
    const auto encoded = [](std::uint64_t value) { return binary64(value); };
    const auto subtract = [working](Number left, Number right) { right.negative = !right.negative; return add(left, right, working); };
    auto base_angle = input.angle;
    if (!input.absolute) {
        const auto dy = to_binary64(subtract(encoded(player_y), encoded(input.y)));
        const auto dx = to_binary64(subtract(encoded(player_x), encoded(input.x)));
        base_angle = to_binary64(add(math.atan2(dy, dx), encoded(input.angle), working));
    }
    const auto signed_count = input.count >> 31 ? static_cast<std::int64_t>(input.count) - 0x100000000ll : static_cast<std::int64_t>(input.count);
    const auto half = signed_count / 2;
    const bool even = (input.count & 1u) == 0;
    for (auto element = -half + (even ? 1 : 0); element <= half; ++element) {
        const auto index = allocate_projectile(pools, input.allocation);
        if (index < 0) continue;
        auto& bullet = pools.bullets[static_cast<std::size_t>(index)];
        bullet.quad(0x40, input.x); bullet.quad(0x48, input.y);
        auto position = integer(static_cast<std::uint32_t>(element));
        if (even) position = subtract(position, encoded(crt_sse2::bits(.5)));
        const auto angle = to_binary64(add(multiply(position, encoded(input.spacing), working), encoded(base_angle), working));
        bullet.quad(0x60, angle); bullet.quad(0x68, input.speed);
        bullet.quad(0x50, to_binary64(multiply(math.cosine(angle), encoded(input.speed), working)));
        bullet.quad(0x58, to_binary64(multiply(math.sine(angle), encoded(input.speed), working)));
        initialize_projectile_pattern(pools, static_cast<std::size_t>(index), input.configuration, parameters, random, precision);
    }
}
}
