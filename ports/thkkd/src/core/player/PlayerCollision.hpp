// 恢复 0x00658f10 的擦弹与受击，以及 0x00657e90 的敌人接触半径选择。
#pragma once
#include "core/shared/CircleCollision.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectilePool.hpp"
#include <stdexcept>

namespace kaikeidou {
struct CollisionContext {
    std::uint32_t option_a7ebf8 = 0, mode_a7e618 = 0;
    std::uint32_t stage_b3e2ec = 0, part_a77390 = 0, phase_a733c0 = 0;
    std::uint32_t character_a74c0c = 0, mode_a7ea98 = 0;
    std::uint32_t scene_a78e2c = 0, scene_step_a87f48 = 0, scene_choice_a87f50 = 0;
    std::array<std::array<std::uint64_t, 2>, 40> projectile_radii{};
};
struct CollisionServices {
    virtual ~CollisionServices() = default;
    virtual std::uint32_t special_collision(ObjectRecord<240>& player, ProjectilePools& projectiles, CollisionContext& context) = 0;
};
namespace collision_detail {
inline std::int64_t signed_word(std::uint32_t value) {
    return value >> 31 ? static_cast<std::int64_t>(value) - 0x100000000ll : static_cast<std::int64_t>(value);
}
}
inline std::uint32_t enemy_contact_radius(std::uint32_t kind_bits) {
    const auto kind = collision_detail::signed_word(kind_bits);
    if (kind <= 4) return 16;
    if (kind == 5) return 24;
    if (kind <= 19) return 32;
    return 16;
}
inline void update_player_collision(ObjectRecord<240>& player, EnemyPool& enemies, ProjectilePools& projectiles,
    CollisionContext& context, CollisionServices& services, unsigned precision) {
    using collision_detail::signed_word;
    if (context.option_a7ebf8 != 0 && context.mode_a7e618 != 0) return;
    const auto hit = [&]() {
        player.word(0, 1); player.word(4, 0); projectiles.sound_triggers[11] = 1;
    };
    const auto overlap = [&](std::uint64_t x, std::uint64_t y, std::uint64_t radius) {
        return circles_overlap(x, y, player.quad(0x90), player.quad(0x98), radius, player.quad(0xd8), precision);
    };
    for (std::size_t index = 0; index < projectiles.bullets.size(); ++index) {
        auto& bullet = projectiles.bullets[index];
        if (bullet.word(0) != 1 || bullet.word(0x3c) != 0) continue;
        const auto kind = bullet.word(4);
        const auto& radii = context.projectile_radii.at(kind);
        if (!overlap(bullet.quad(0x40), bullet.quad(0x48), radii[0])) continue;
        if (kind == 34) {
            if (bullet.word(0x9c) == 0) {
                bullet.word(0x9c, 1);
                player.word(0xa8, player.word(0xa8) + 1u);
                player.word(0x10, player.word(0x10) + 10u);
                projectiles.sound_triggers[14] = 1;
                const auto resource = player.word(0x1c);
                player.word(0x1c, resource - 1u);
                if (signed_word(resource) <= 0) {
                    const auto lives = player.word(0x14);
                    player.word(0x14, lives - 1u);
                    if (signed_word(lives) <= 0) {
                        context.scene_a78e2c = 5; context.scene_step_a87f48 = 0; context.scene_choice_a87f50 = 0;
                    } else player.word(0x1c, player.word(0x1c) + 64u);
                }
            }
        } else {
            const auto timer = bullet.word(0x14);
            bullet.word(0x14, timer - 1u);
            if (timer == 0) {
                bullet.word(0x14, 60);
                player.word(0xa8, player.word(0xa8) + 1u);
                player.word(0x10, player.word(0x10) + 10u);
                projectiles.sound_triggers[14] = 1;
                const auto numerator = signed_word(player.word(0xa8)), denominator = signed_word(player.word(0xe0));
                if (denominator == 0 || (numerator == -0x80000000ll && denominator == -1))
                    throw std::domain_error("Collision graze division raises original x86 divide exception");
                if (numerator % denominator == 0) player.word(0x1c, player.word(0x1c) + 1u);
            }
        }
        if (overlap(bullet.quad(0x40), bullet.quad(0x48), radii[1])) {
            remove_projectile(projectiles, index, 0);
            if (player.word(0) == 0 && player.word(0x28) == 0) { hit(); return; }
        }
    }
    if (player.word(0) != 0 || player.word(0x28) != 0) return;
    for (const auto& enemy : enemies.enemies) {
        if (enemy.word(0) != 1 || enemy.word(0x28) != 0) continue;
        const auto radius = binary_float::to_binary64(binary_float::integer(enemy_contact_radius(enemy.word(4)) - 8u));
        if (overlap(enemy.quad(0x38), enemy.quad(0x40), radius)) { hit(); return; }
    }
    const auto stage = context.stage_b3e2ec, part = context.part_a77390, phase = context.phase_a733c0;
    const bool special = (stage == 4 && part == 3) || (stage == 5 && part == 1 && phase == 5) ||
        (stage == 5 && part == 3 && phase == 11) || (stage == 6 && part == 3 && (phase == 9 || phase == 14)) ||
        (stage == 7 && part == 5 && phase == 1 && context.character_a74c0c != 2);
    if (special && context.mode_a7ea98 == 2 && services.special_collision(player, projectiles, context) == 0) hit();
}
}
