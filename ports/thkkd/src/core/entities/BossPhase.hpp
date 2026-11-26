// 完整恢复 0x644040..0x6446b6 阶段推进，保留难度5统计原地址和 Replay 六处写入钩子。
#pragma once
#include "core/entities/BossPhaseContext.hpp"
#include "core/shared/SpellRecordsView.hpp"
#include "core/entities/EnemyDeath.hpp"

namespace kaikeidou {
inline void advance_boss_phase(std::uint32_t mode, EnemyPool& enemies, ObjectRecord<240>& player,
    ItemPool& items, EffectPool& effects, EffectShake& shake, ProjectilePools& projectiles, SaveData& savedata,
    PatternParameters& parameters, EnemyDeathContext& death, BossPhaseContext& context,
    Random& random, GameMath& math, RandomPrecision precision, BossPhaseServices& services) {
    if (death.boss_state == 2) services.restore_stage_music(0);
    ++death.subphase; parameters.game_frame = 0;
    enemies.enemies[context.boss_index].word(8, 0);
    enemies.enemies[context.boss_index].word(0x28, 1);
    enemies.enemies[context.boss_index].word(0x2c, 0);
    if (death.boss_state == 2) --context.spell_remaining;
    const auto increment_record = [&](std::uint32_t pair) {
        const bool fifth = parameters.difficulty == 5;
        const auto address = fifth ? 0xa81f84u + death.character * 0x3c0u + context.weapon * 0x140u + context.spell_index * 8u + pair * 4u :
            0xa7f74cu + death.character * 0xf00u + context.weapon * 0x500u + context.spell_index * 8u + pair * 4u;
        auto& record = spell_record_word(savedata, address);
        const auto value = record + 1u;
        if (!(context.replay_variant && context.replay_mode == 1 && (mode == 0 || fifth))) record = value;
    };
    const auto drop = [&]() { drop_enemy_items(items, enemies, context.boss_index, player, parameters, death, random, math, precision); };
    const auto clear = [&]() {
        if (death.stage == 6 && death.part == 1 && death.subphase == 2) {
            clear_projectiles(projectiles, 0);
            for (auto& bullet : projectiles.bullets) if (bullet.word(4) == 18) { bullet.word(8, 25); break; }
        } else { clear_projectiles(projectiles, 100); clear_projectiles(projectiles, 101); }
    };
    if (mode == 0) {
        if (death.boss_state == 2) {
            increment_record(0); context.result_timer = 160;
            if (!context.hit_flag && !context.bomb_flag) {
                context.result = 1; increment_record(1); player.word(0x10, player.word(0x10) + context.bonus);
            } else context.result = 0;
            services.persist_spell_records(savedata);
        }
        if (!context.hit_flag) drop();
        const auto y = enemies.enemies[context.boss_index].quad(0x40);
        const auto x = enemies.enemies[context.boss_index].quad(0x38);
        const auto color = services.color(255, 255, 255);
        allocate_effect(effects, projectiles.sound_triggers, {2, color, x, y});
        clear();
    } else if (mode == 1) {
        if (death.boss_state == 2) {
            increment_record(0);
            if (!enemies.enemies[context.boss_index].word(0x18)) {
                context.result_timer = 160;
                if (!context.hit_flag && !context.bomb_flag) {
                    context.result = 1; player.word(0x10, player.word(0x10) + context.bonus); increment_record(1);
                } else context.result = 0;
                if (!context.hit_flag) drop();
            } else { context.result_timer = 160; context.result = 0; }
            services.persist_spell_records(savedata);
        }
        clear();
    }
    death.boss_state = 0; context.spell_overlay = 0;
    for (std::uint32_t index = 0; index < 80; ++index) {
        auto& enemy = enemies.enemies[index];
        if (enemy.word(0) != 1) continue;
        const auto kind = enemy_death_detail::signed_word(enemy.word(4));
        if (kind >= 10 && kind < 100) continue;
        enemy.word(0x14, 0); enemy.word(0x1c, 0);
        resolve_enemy_death(enemies, index, items, effects, shake, projectiles, player, parameters, death, random, math, precision, services);
    }
}
}
