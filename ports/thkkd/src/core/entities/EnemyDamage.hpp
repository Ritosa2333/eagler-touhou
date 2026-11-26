// 恢复 0x0065b830..0x0065b8d4，保持声音、保护判断、扣血、加分与死亡检查的顺序。
#pragma once
#include "core/entities/EnemyPool.hpp"

namespace kaikeidou {
struct EnemyDamageContext {
    std::uint32_t boss_state_a7ea60 = 0, field_a7eaa4 = 0, field_a7eaac = 0, field_a7eab0 = 0;
};
struct EnemyDamageServices {
    virtual ~EnemyDamageServices() = default;
    virtual void check_enemy_death(std::uint32_t slot) = 0;
};
inline void apply_enemy_damage(EnemyPool& enemies, ObjectRecord<240>& player, std::array<std::uint32_t, 100>& sounds,
    const EnemyDamageContext& context, std::uint32_t slot, std::uint32_t damage, EnemyDamageServices& services) {
    sounds[22] = 1;
    auto& enemy = enemies.enemies.at(slot);
    const auto signed_word = [](std::uint32_t value) {
        return value >> 31 ? static_cast<std::int64_t>(value) - 0x100000000ll : static_cast<std::int64_t>(value);
    };
    const auto kind = enemy.word(4);
    const auto elapsed = context.field_a7eaac - context.field_a7eaa4;
    if (kind >= 10 && kind <= 99 &&
        (signed_word(elapsed) <= signed_word(context.field_a7eab0) || signed_word(context.boss_state_a7ea60) >= 1)) {
        if (context.boss_state_a7ea60 != 2) player.word(0x10, player.word(0x10) + 10u);
        return;
    }
    enemy.word(0x14, enemy.word(0x14) - damage);
    player.word(0x10, player.word(0x10) + 10u);
    services.check_enemy_death(slot);
}
}
