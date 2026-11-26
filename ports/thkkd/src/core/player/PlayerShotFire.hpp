// 恢复 0x005c4a90 发射入口、公共弹、七种机体弹及原 Replay 输入钩子。
#pragma once
#include "core/player/PlayerShotPools.hpp"
#include "core/player/PlayerShotFireContext.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/math/GameMath.hpp"
#include "core/math/FiniteDivide.hpp"
#include "core/math/Random.hpp"
#include <cstring>

namespace kaikeidou {
namespace player_fire_detail {
inline std::int64_t signed_word(std::uint32_t value) {
    return value >> 31 ? static_cast<std::int64_t>(value) - 0x100000000ll : value;
}
inline double real(std::uint64_t bits) { double value; std::memcpy(&value, &bits, 8); return value; }
inline std::uint32_t floor_integer(binary_float::Number number) {
    using namespace binary_float;
    number = binary64(to_binary64(number));
    auto value = truncate(number);
    if (number.negative && number.mantissa && (number.exponent < 0 ||
        (number.exponent < 63 && (number.mantissa & ((std::uint64_t(1) << (63 - number.exponent)) - 1))))) --value;
    return value;
}
inline std::uint32_t scaled_integer(std::uint32_t value, std::uint64_t scale, unsigned precision) {
    return floor_integer(binary_float::multiply(binary_float::integer(value), binary_float::binary64(scale), precision));
}
inline std::uint32_t phase(std::uint32_t value, std::uint32_t period, std::uint32_t divisor, unsigned precision) {
    const auto remainder = static_cast<std::uint32_t>(signed_word(value) % period);
    return floor_integer(binary_float::divide(binary_float::integer(remainder), binary_float::integer(divisor), precision));
}
inline std::uint64_t subtract(std::uint64_t left, std::uint64_t right, unsigned precision) {
    auto other = binary_float::binary64(right); other.negative = !other.negative;
    return binary_float::to_binary64(binary_float::add(binary_float::binary64(left), other, precision));
}
template<std::size_t Size, std::size_t Count>
inline std::int32_t first_empty(const std::array<ObjectRecord<Size>, Count>& pool, std::size_t count) {
    for (std::size_t index = 0; index < count; ++index) if (!pool[index].word(0)) return static_cast<std::int32_t>(index);
    return -1;
}
// 0x5c2a90/2ad0/2b10/2b50/2b90/2bd0/2c10 的搜索范围分别保留。
inline std::int32_t allocate_sanae_c(const PlayerShotPools& pool) { return first_empty(pool.weapon, 16); }
inline std::int32_t allocate_sanae_a(const PlayerShotPools& pool) { return first_empty(pool.weapon, 80); }
inline std::int32_t allocate_marisa_c(const PlayerShotPools& pool) { return first_empty(pool.weapon, 40); }
inline std::int32_t allocate_marisa_a(const PlayerShotPools& pool) { return first_empty(pool.weapon, 40); }
inline std::int32_t allocate_reimu_c(const PlayerShotPools& pool) { return first_empty(pool.weapon, 40); }
inline std::int32_t allocate_reimu_a(const PlayerShotPools& pool) { return first_empty(pool.weapon, 40); }
inline std::int32_t allocate_common(const PlayerShotPools& pool) { return first_empty(pool.common, 20); }
// 0x643be0 只检查坐标；state==1 由调用方检查。
inline std::uint32_t target_inside(const EnemyPool& enemies, std::uint32_t index) {
    const auto& enemy = enemies.enemies[index];
    const auto x = real(enemy.quad(0x38)), y = real(enemy.quad(0x40));
    return x > 0. && x < 384. && y > 0. && y < 448.;
}
inline std::uint32_t count_targets(const EnemyPool& enemies) {
    std::uint32_t count = 0;
    for (std::uint32_t index = 0; index < 80; ++index)
        if (enemies.enemies[index].word(0) == 1 && target_inside(enemies, index)) ++count;
    return count;
}
inline void common_shots(PlayerShotPools& pool, ObjectRecord<240>& player, const PlayerShotFireContext& context,
    std::array<std::uint32_t, 100>& sounds, unsigned precision) {
    player.word(0xa0, player.word(0xa0) + 1u);
    if (signed_word(player.word(0xa0)) % 3) return;
    const auto level = scaled_integer(player.word(8), 0x3f847ae147ae147bull, precision);
    for (unsigned index = 0; index < 2; ++index) {
        const auto slot = allocate_common(pool);
        if (slot == -1) continue;
        auto& shot = pool.common[static_cast<std::size_t>(slot)];
        shot.word(0, 1);
        shot.quad(8, binary_float::to_binary64(binary_float::add(binary_float::integer(index ? 12u : 0xfffffff4u),
            binary_float::binary64(player.quad(0x90)), precision)));
        shot.quad(0x10, subtract(player.quad(0x98), 0x4042000000000000ull, precision));
        shot.word(4, context.options.character == 2 ? 36u - 4u * level : 30u - 3u * level);
    }
    sounds[10] = 1;
}
inline std::uint32_t slow_input(PlayerShotFireContext& context, PlayerShotFireInput& input, bool sanae_a) {
    if (!context.replay_variant) return input.read_counter(context.slow_binding);
    if (!sanae_a) {
        if (!context.replay_mode) return input.read_counter(context.slow_binding);
        const auto bit = context.slow_binding == 11 ? 7u : context.slow_binding;
        return bit<32 ? (context.replay_buttons >> bit) & 1u : 0u;
    }
    const auto down = input.read_counter(context.slow_binding) != 0;
    if (context.replay_mode) return (context.replay_buttons >> 7) & 1u;
    context.replay_buttons |= down ? 0x80u : 0;
    return down;
}
inline void option_shots(PlayerShotPools& pool, const ObjectRecord<240>& player, const OptionsPool& options,
    std::uint32_t selector, std::array<std::uint32_t, 100>& sounds, unsigned precision) {
    const auto divisor = selector == 2 ? 5 : 3;
    if (signed_word(player.word(0xa0)) % divisor != 1) return;
    const auto level = scaled_integer(player.word(8), 0x3f847ae147ae147bull, precision);
    const auto angle = selector == 12 ? binary_float::to_binary64(binary_float::add(
        binary_float::binary64(0x4012d97c7f3321deull), binary_float::binary64(options.options[0].quad(0x190)), precision)) : 0;
    for (std::uint32_t index = 0; index < level; ++index) {
        const auto slot = selector == 0 ? allocate_reimu_a(pool) : (selector == 2 ? allocate_reimu_c(pool) :
            (selector == 10 ? allocate_marisa_a(pool) : allocate_marisa_c(pool)));
        if (slot == -1) continue;
        auto& shot = pool.weapon[static_cast<std::size_t>(slot)];
        const auto& option = options.options[index];
        shot.word(0, 1);
        if (selector == 2) shot.quad(0x28, binary_float::to_binary64(binary_float::multiply(
            binary_float::integer(player.word(0xa4)), binary_float::binary64(0x3fb47ae147ae147bull), precision)));
        shot.quad(0x10, option.quad(8));
        shot.quad(0x18, selector == 0 ? subtract(option.quad(0x10), 0x4038000000000000ull, precision) : option.quad(0x10));
        if (selector == 12) { shot.quad(0x28, angle); shot.quad(0x20, 0x4038000000000000ull); }
        shot.word(4, selector == 2 ? 10 : 6);
    }
    sounds[10] = 1;
}
inline void reimu_b(PlayerShotPools& pool, const ObjectRecord<240>& player, OptionsPool& options,
    const EnemyPool& enemies, std::array<std::uint32_t, 100>& sounds, GameMath& math, unsigned precision) {
    if (signed_word(player.word(0xa4)) % 20) return;
    const auto level = scaled_integer(player.word(8), 0x3f847ae147ae147bull, precision);
    const auto group = phase(player.word(0xa4), 80, 20, precision);
    if (signed_word(group) >= signed_word(level)) return;
    const auto first = group * 20u;
    for (auto index = first; index < first + 20u; ++index) if (pool.weapon[index].word(0)) return;
    for (auto index = first; index < first + 20u; ++index) {
        auto& shot = pool.weapon[index]; auto& option = options.options[group];
        shot.word(0, 1); shot.word(4, 7); shot.quad(0x20, 0x4020000000000000ull); shot.word(8, index - first);
        if (index == first) {
            std::int32_t target = group == 1 || group == 3 ? 79 : 0;
            const auto direction = group == 1 || group == 3 ? -1 : 1;
            const auto middle = group >= 2 ? count_targets(enemies) / 2u : 0u;
            std::uint32_t found = 0;
            for (; target >= 0 && target < 80; target += direction) {
                if (enemies.enemies[static_cast<std::size_t>(target)].word(0) != 1 || !target_inside(enemies, target)) continue;
                if (group < 2 || ++found >= middle) break;
            }
            shot.quad(0x10, option.quad(8)); shot.quad(0x18, option.quad(0x10));
            if (target < 0 || target >= 80) { shot.quad(0x28, 0xbff921fb54442d28ull); option.word(0x4c, 0xffffffffu); }
            else {
                const auto& enemy = enemies.enemies[static_cast<std::size_t>(target)];
                const auto y = subtract(enemy.quad(0x40), shot.quad(0x18), precision);
                const auto x = subtract(enemy.quad(0x38), shot.quad(0x10), precision);
                shot.quad(0x28, binary_float::to_binary64(math.atan2(y, x)));
                option.word(0x4c, enemy.word(0x24));
            }
            option.word(0x48, static_cast<std::uint32_t>(target));
            option.quad(0x50, shot.quad(0x10)); option.quad(0xf0, shot.quad(0x18)); option.quad(0x190, shot.quad(0x28));
        } else {
            shot.quad(0x10, option.quad(0x50)); shot.quad(0x18, option.quad(0xf0)); shot.quad(0x28, option.quad(0x190));
        }
    }
    sounds[10] = 1;
}
inline void sanae_a(PlayerShotPools& pool, const ObjectRecord<240>& player, PlayerShotFireContext& context,
    PlayerShotFireInput& input, std::array<std::uint32_t, 100>& sounds, unsigned precision) {
    if (!slow_input(context, input, true) || signed_word(player.word(0xa0)) % 3 != 1) return;
    const auto level = scaled_integer(player.word(8), 0x3f847ae147ae147bull, precision);
    auto spread = level >= 4 ? 0xbfd70a3d70a3d70aull : (level == 3 ? 0xbfceb851eb851eb8ull : (level == 2 ? 0xbfbeb851eb851eb8ull : 0ull));
    for (std::uint32_t index = 0; index < level; ++index) {
        const auto slot = allocate_sanae_a(pool); if (slot == -1) continue;
        auto& shot = pool.weapon[static_cast<std::size_t>(slot)];
        shot.word(0, 1); shot.quad(0x10, player.quad(0x90));
        shot.quad(0x18, subtract(player.quad(0x98), 0x4030000000000000ull, precision));
        shot.quad(0x20, spread);
        spread = binary_float::to_binary64(binary_float::add(binary_float::binary64(spread), binary_float::binary64(0x3fceb851eb851eb8ull), precision));
        shot.word(4, 5); shot.word(8, phase(player.word(0xa0), 180, 60, precision));
    }
    sounds[10] = 1;
}
inline void sanae_c(PlayerShotPools& pool, const ObjectRecord<240>& player, const OptionsPool& options,
    PlayerShotFireContext& context, PlayerShotFireInput& input, std::array<std::uint32_t, 100>& sounds, unsigned precision) {
    if (!slow_input(context, input, false)) {
        if (signed_word(player.word(0xa0)) % 12) return;
        const auto level = scaled_integer(player.word(8), 0x3f847ae147ae147bull, precision);
        const auto option_index = phase(player.word(0xa4), 48, 12, precision);
        if (signed_word(level) <= signed_word(option_index)) return;
        const auto slot = allocate_sanae_c(pool);
        if (slot != -1) {
            auto& shot = pool.weapon[static_cast<std::size_t>(slot)]; const auto& option = options.options[option_index];
            shot.word(0, 1); shot.quad(0x10, option.quad(8)); shot.quad(0x18, option.quad(0x10));
            shot.quad(0x20, 0x4024000000000000ull); shot.word(4, 3); shot.quad(0x30, 0x3fc47ae147ae147bull); shot.word(8, 0);
        }
    } else {
        if (signed_word(player.word(0xa0)) % 60 || signed_word(player.word(8)) < 100) return;
        const auto slot = allocate_sanae_c(pool);
        if (slot != -1) {
            auto& shot = pool.weapon[static_cast<std::size_t>(slot)];
            shot.word(0, 1); shot.quad(0x10, player.quad(0x90)); shot.quad(0x18, player.quad(0x98)); shot.quad(0x20, 0);
            shot.word(4, player.word(0x2c) ? scaled_integer(player.word(8), 0x3f847ae147ae147bull, precision) :
                scaled_integer(player.word(8), 0x3fb999999999999aull, precision) + 10u);
            shot.quad(0x30, 0x3fc47ae147ae147bull); shot.word(8, 0);
        }
    }
    sounds[10] = 1;
}
}
inline void fire_player_shots(PlayerShotPools& pool, ObjectRecord<240>& player, OptionsPool& options, const EnemyPool& enemies,
    PlayerShotFireContext& context, PlayerShotFireInput& input, std::array<std::uint32_t, 100>& sounds,
    GameMath& math, RandomPrecision precision) {
    std::uint32_t firing = 0;
    if (context.replay_variant) {
        if (!context.fire_lock) {
            firing = input.read_counter(context.fire_binding) != 0;
            if (context.replay_mode) firing = (context.replay_buttons >> 5) & 1u;
            else context.replay_buttons |= firing << 5;
        }
    } else firing = input.read_counter(context.fire_binding);
    if (!firing || (!context.replay_variant && context.fire_lock)) { player.word(0xa0, 0); return; }
    const auto working = static_cast<unsigned>(precision);
    player_fire_detail::common_shots(pool, player, context, sounds, working);
    switch (context.options.character * 10u + context.options.weapon) {
    case 0: case 2: case 10: case 12:
        player_fire_detail::option_shots(pool, player, options, context.options.character * 10u + context.options.weapon, sounds, working); break;
    case 1: player_fire_detail::reimu_b(pool, player, options, enemies, sounds, math, working); break;
    case 20: player_fire_detail::sanae_a(pool, player, context, input, sounds, working); break;
    case 22: player_fire_detail::sanae_c(pool, player, options, context, input, sounds, working); break;
    default: break;
    }
}
}
