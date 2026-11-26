// 恢复完整 0x6596c0 掉落和 0x65a950 死亡；阶段推进与像素颜色由调用方服务提供。
#pragma once
#include "core/entities/EnemyDeathContext.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/player/ItemPool.hpp"
#include "core/shared/EffectUpdate.hpp"
#include "core/entities/ProjectileEmitter.hpp"

namespace kaikeidou {
namespace enemy_death_detail {
inline std::int64_t signed_word(std::uint32_t value) { return value >> 31 ? static_cast<std::int64_t>(value) - 0x100000000ll : value; }
inline double real(std::uint64_t bits) { double result; std::memcpy(&result, &bits, 8); return result; }
inline bool special_five(const EnemyDeathContext& c) { return c.stage == 5 && c.part == 3 && c.subphase == 11; }
inline std::uint64_t sum(std::uint64_t left, std::uint64_t right, unsigned precision) {
    return binary_float::to_binary64(binary_float::add(binary_float::binary64(left), binary_float::binary64(right), precision));
}
inline std::uint64_t loaded_coordinate(std::uint64_t value) {
    return (value & 0x7fffffffffffffffull) > 0x7ff0000000000000ull ? value | 0x0008000000000000ull : value;
}
// 固定三件只给坐标加有限常数；原FLD/FSTP安静化NaN并保留符号和载荷。
inline std::uint64_t coordinate_offset(std::uint64_t value, std::uint64_t offset, unsigned precision) {
    if ((value & 0x7fffffffffffffffull) >= 0x7ff0000000000000ull) return loaded_coordinate(value);
    return sum(value, offset, precision);
}
inline std::uint64_t degrees(binary_float::Number value, unsigned precision) {
    using namespace binary_float;
    return to_binary64(divide(multiply(binary64(to_binary64(value)), binary64(0x400921fb54442d28ull), precision), integer(180), precision));
}
inline void set_item_base(Item& item, std::uint32_t reward, std::uint32_t kind) {
    item.word(0, 1); item.word(0x10, reward); item.word(8, kind); item.word(4, 0); item.word(0xc, 0);
}
inline void fixed_three(ItemPool& items, std::uint64_t x, std::uint64_t y, std::uint32_t reward, unsigned precision) {
    using namespace binary_float;
    x = loaded_coordinate(x); y = loaded_coordinate(y);
    for (std::uint32_t index = 0; index < 3; ++index) {
        const auto slot = find_free_item(items); if (slot == -1) continue;
        auto& item = items.items[static_cast<std::size_t>(slot)];
        item.word(0, 1); item.word(0x10, reward); item.word(4, 0); item.word(0xc, 0);
        if (!index) { item.word(8, 7); item.quad(0x18, x); item.quad(0x20, coordinate_offset(y, 0xc010000000000000ull, precision)); }
        else {
            item.word(8, 5);
            if ((x & 0x7fffffffffffffffull) >= 0x7ff0000000000000ull) item.quad(0x18, x);
            else {
                const auto shifted = add(integer(index * 48u), binary64(x), precision);
                item.quad(0x18, to_binary64(add(shifted, integer(0xffffffb8u), precision)));
            }
            item.quad(0x20, coordinate_offset(y, 0x4034000000000000ull, precision));
        }
        item.quad(0x28, 0); item.quad(0x30, 0); item.quad(0x38, 0xc008000000000000ull);
    }
}
inline std::uint64_t clamped_x(std::uint64_t x) {
    if (real(x) < 0.) return 0;
    return real(x) > 384. ? 0x4078000000000000ull : x;
}
inline void scatter(ItemPool& items, std::uint32_t count, std::uint32_t kind, std::uint32_t reward,
    std::uint64_t x, std::uint64_t y, bool boss, Random& random, GameMath& math, RandomPrecision precision) {
    using namespace binary_float; const auto working = static_cast<unsigned>(precision);
    for (std::uint32_t element = 0; signed_word(element) < signed_word(count); ++element) {
        const auto slot = find_free_item(items); if (slot == -1) continue;
        auto& item = items.items[static_cast<std::size_t>(slot)];
        set_item_base(item, reward, kind); item.quad(0x18, loaded_coordinate(x));
        if (real(item.quad(0x18)) < 0.) item.quad(0x18, 0);
        else if (real(item.quad(0x18)) > 384.) item.quad(0x18, 0x4078000000000000ull);
        item.quad(0x20, loaded_coordinate(y));
        std::uint64_t angle = 0, speed = 0;
        if (boss || count != 1) {
            angle = degrees(random.real_number(0x4076700000000000ull, precision), working);
            auto velocity = random.real_number(boss ? 0x4040000000000000ull : 0x404e000000000000ull, precision);
            if (boss) velocity = add(velocity, integer(48), working);
            speed = to_binary64(divide(velocity, integer(20), working));
        }
        item.quad(0x28, to_binary64(multiply(math.cosine(angle), binary64(speed), working)));
        item.quad(0x30, to_binary64(multiply(math.sine(angle), binary64(speed), working)));
        item.quad(0x38, 0xc008000000000000ull);
    }
}
}
inline void drop_enemy_items(ItemPool& items, const EnemyPool& enemies, std::uint32_t index,
    const ObjectRecord<240>& player, const PatternParameters& parameters, const EnemyDeathContext& context,
    Random& random, GameMath& math, RandomPrecision precision) {
    using namespace enemy_death_detail;
    const auto& enemy = enemies.enemies[index]; const auto kind = enemy.word(4);
    const auto working = static_cast<unsigned>(precision);
    if (signed_word(kind) >= 10) {
        auto x = enemy.quad(0x38), y = enemy.quad(0x40);
        if (context.stage == 6 && context.part == 3 && (context.subphase == 17 || context.subphase == 18)) {
            x = 0x4068000000000000ull; y = 0x4059000000000000ull;
        } else if (context.stage == 7 && context.part == 7) {
            if (context.character == 1) { const auto& other = enemies.enemies[parameters.integers[0]]; x = other.quad(0x38); y = other.quad(0x40); }
            else { x = 0x4068000000000000ull; y = context.character == 0 ? 0x4054000000000000ull : 0x406c000000000000ull; }
        }
        fixed_three(items, x, y, 0, working);
        scatter(items, 20, 3, 0, x, y, true, random, math, precision);
        scatter(items, 10, 0, 0, x, y, true, random, math, precision);
    }
    if (kind == 5) {
        if (signed_word(enemy.word(0x1c)) > 0) fixed_three(items, enemy.quad(0x38), enemy.quad(0x40), special_five(context) ? 1 : 0, working);
        return;
    }
    std::uint32_t item_kind;
    switch (kind) {
    case 0: item_kind = special_five(context) || signed_word(player.word(8)) < 400 ? 0u : 4u; break;
    case 1: item_kind = 3; break; case 2: item_kind = 4; break; case 3: item_kind = 5; break; case 4: item_kind = 7; break;
    default: return;
    }
    scatter(items, enemy.word(0x1c), item_kind, special_five(context) ? 1 : 0, enemy.quad(0x38), enemy.quad(0x40), false, random, math, precision);
}
inline void resolve_enemy_death(EnemyPool& enemies, std::uint32_t index, ItemPool& items, EffectPool& effects,
    EffectShake& shake, ProjectilePools& projectiles, const ObjectRecord<240>& player, PatternParameters& parameters,
    EnemyDeathContext& context, Random& random, GameMath& math, RandomPrecision precision, EnemyDeathServices& services) {
    using namespace binary_float; using namespace enemy_death_detail;
    auto& enemy = enemies.enemies[index];
    if (signed_word(enemy.word(0x14)) >= 1) return;
    const auto kind = enemy.word(4); const auto working = static_cast<unsigned>(precision);
    const auto effect = [&](std::uint32_t effect_kind, std::uint32_t r, std::uint32_t g, std::uint32_t b) {
        const auto y = enemy.quad(0x40), x = enemy.quad(0x38);
        const auto color = services.color(r, g, b);
        allocate_effect(effects, projectiles.sound_triggers, {effect_kind, color, x, y});
    };
    const auto ordinary_effect = [&](std::uint32_t color_index) {
        const std::array<std::array<std::uint32_t, 3>, 6> colors{{{{255,0,0}},{{0,0,255}},{{0,255,0}},{{0,255,255}},{{255,128,0}},{{255,0,255}}}};
        if (color_index < colors.size()) effect(0, colors[color_index][0], colors[color_index][1], colors[color_index][2]);
    };
    if (signed_word(kind) >= 100) { enemy.word(0, 0); ordinary_effect(kind - 100u); return; }
    if (signed_word(kind) < 10) {
        enemy.word(0, 0); ordinary_effect(kind);
        if (special_five(context) && enemy.word(0x1c)) {
            if (kind <= 5) {
                const std::array<std::array<std::uint32_t, 3>, 6> colors{{{{255,0,0}},{{0,0,255}},{{0,255,0}},{{0,255,255}},{{255,255,0}},{{255,0,255}}}};
                const std::array<std::uint64_t, 6> spacing{{0x3f947ae147ae147bull,0x3fb1eb851eb851ecull,0x3fbeb851eb851eb8ull,0x3fa999999999999aull,0x3fa999999999999aull,0x3fe8000000000000ull}};
                ProjectileEmission emission;
                emission.allocation = {kind < 3 ? 4u : kind - 3u, 0, colors[kind], kind == 5 ? 2u : 0u, 255, 1, 41};
                emission.x = enemy.quad(0x38); emission.y = enemy.quad(0x40);
                emission.count = kind < 3 ? parameters.integers[2] : (kind == 3 ? parameters.integers[3] : (kind == 4 ? parameters.integers[4] : context.pattern_integer_5));
                emission.spacing = spacing[kind]; emission.speed = parameters.reals[0];
                emit_projectiles(projectiles, emission, player.quad(0x90), player.quad(0x98), parameters, random, precision, math);
            }
            if (!player.word(0x2c)) drop_enemy_items(items, enemies, index, player, parameters, context, random, math, precision);
        } else drop_enemy_items(items, enemies, index, player, parameters, context, random, math, precision);
        return;
    }
    if (context.stage == 6 && context.part == 3 && context.subphase == 22) {
        ++context.pattern_integer_5;
        if (context.pattern_integer_5 == 3) {
            for (unsigned field = 2; field <= 4; ++field) enemies.enemies[parameters.integers[field]].word(8, 0);
            for (unsigned field = 2; field <= 4; ++field) enemies.enemies[parameters.integers[field]].word(0x28, 1);
            for (unsigned field = 2; field <= 4; ++field) enemies.enemies[parameters.integers[field]].word(0x2c, 0);
            services.advance_phase(0);
        } else {
            const auto slot = allocate_projectile(projectiles, {18,1,{{0,0,0}},0,255,0,41});
            if (slot != -1) {
                auto& bullet = projectiles.bullets[static_cast<std::size_t>(slot)];
                bullet.quad(0x40, enemy.quad(0x38)); bullet.quad(0x48, enemy.quad(0x40)); bullet.word(8, 191);
                for (unsigned field = 0; field < 4; ++field) bullet.quad(0xa0 + field * 8, enemy.quad(0xa8 + field * 8));
                bullet.quad(0xc0, 0x4059000000000000ull); bullet.quad(0xc8, player.quad(0x90)); bullet.quad(0xd0, player.quad(0x98));
                bullet.word(0x78, 0); bullet.word(0x7c, 180);
                bullet.word(0x80, random.integer_bits(1, precision, true) * 2u - 1u); bullet.word(0x34, 1);
            }
            set_effect_screen_shake(shake, 10, 8); effect(1, 0, 0, 0); enemy.word(0, 0);
        }
    } else if (context.stage == 7 && context.part == 7 && context.character == 1) {
        for (unsigned iteration = 0; iteration < 10; ++iteration) {
            const auto angle = to_binary64(multiply(random.real_number(0x40b88b0000000000ull, precision), binary64(0x3f50624dd2f1a9fcull), working));
            Number radius;
            if (!random.integer_bits(1, precision, true)) radius = add(multiply(random.real_number(0x4072c00000000000ull, precision), binary64(0x3fb999999999999aull), working), integer(20), working);
            else radius = multiply(random.real_number(0x407f400000000000ull, precision), binary64(0x3fb999999999999aull), working);
            const auto distance = binary64(to_binary64(radius));
            const auto slot = allocate_projectile(projectiles, {iteration < 5 ? 35u : 36u,3,{{255,255,255}},0,255,0,41});
            if (slot == -1) continue;
            auto& bullet = projectiles.bullets[static_cast<std::size_t>(slot)];
            bullet.quad(0x40, to_binary64(add(multiply(math.cosine(angle), distance, working), binary64(enemy.quad(0x38)), working)));
            bullet.quad(0x48, to_binary64(add(multiply(math.sine(angle), distance, working), binary64(enemy.quad(0x40)), working)));
            for (auto offset : {0xa0u,0xa8u}) bullet.quad(offset, to_binary64(multiply(random.real_number(0x40b88b0000000000ull, precision), binary64(0x3f50624dd2f1a9fcull), working)));
            bullet.quad(0xb0, to_binary64(add(multiply(random.real_number(0x4059000000000000ull, precision), binary64(0x3ee4f8b588e368f1ull), working), binary64(0x3f826e978d4fdf3bull), working)));
            bullet.quad(0xb8, to_binary64(multiply(random.real_number(0x40b88b0000000000ull, precision), binary64(0x3f50624dd2f1a9fcull), working)));
            bullet.quad(0xc0, to_binary64(add(multiply(integer(parameters.integers[1]), integer(60), working), integer(30), working)));
            Number turn;
            if (!random.integer_bits(1, precision, true)) {
                auto amount = multiply(random.real_number(0x4069000000000000ull, precision), binary64(0x3f1a36e2eb1c432dull), working); amount.negative = !amount.negative;
                turn = add(binary64(0xbf847ae147ae147bull), amount, working);
            } else turn = add(multiply(random.real_number(0x4069000000000000ull, precision), binary64(0x3f1a36e2eb1c432dull), working), binary64(0x3f847ae147ae147bull), working);
            bullet.quad(0xc8, to_binary64(turn));
            bullet.quad(0xd0, to_binary64(multiply(random.real_number(0x40b88b0000000000ull, precision), binary64(0x3f50624dd2f1a9fcull), working)));
            bullet.quad(0xd8, to_binary64(add(multiply(random.real_number(0x4072c00000000000ull, precision), binary64(0x3f1a36e2eb1c432dull), working), binary64(0x3f847ae147ae147bull), working)));
            bullet.quad(0xe0, 0x4010000000000000ull); bullet.word(0x78, 0xffffffffu); bullet.word(8, 225); bullet.word(0x34, 1);
        }
        ++parameters.integers[1]; parameters.integers[2] = 90;
        set_effect_screen_shake(shake, 10, 8); effect(1, 0, 0, 0); enemy.word(0, 0);
    } else if (context.boss_state && enemy.word(0x18)) services.advance_phase(0);
}
}
