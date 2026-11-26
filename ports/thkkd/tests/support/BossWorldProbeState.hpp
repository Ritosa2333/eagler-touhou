// Boss连续对照协议的字节映射，未命名字段保留原采样值。
#pragma once
#include "core/entities/BossAllocationContext.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/BossPresentationContext.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/entities/EnemyDamage.hpp"
#include <iostream>
#include <algorithm>

namespace kaikeidou {
struct BossWorldProbeState {
    ObjectRecord<0x168> globals;
    ObjectRecord<160> sprites;
    BossAllocationContext allocation;
    BossPhaseContext phase;
    BossPresentationContext presentation;
    BossStageContext stage;
    EnemyDeathContext death;
    EnemyDamageContext protection;
    SaveData savedata;
    std::uint32_t game_frame = 0;
    void map(bool load) {
        const auto word = [&](std::size_t offset, std::uint32_t& value) {
            if (load) value = globals.word(offset); else globals.word(offset, value);
        };
        word(0x0, phase.boss_index);
        word(0x4, allocation.field_a7ea44);
        word(0x8, allocation.field_a7ea48);
        word(0x10, allocation.field_a7ea50);
        word(0x18, allocation.field_a7ea58);
        word(0x1c, allocation.field_a7ea5c);
        word(0x20, protection.boss_state_a7ea60);
        word(0x58, death.boss_state);
        word(0x5c, phase.hit_flag);
        word(0x60, phase.bomb_flag);
        word(0x64, protection.field_a7eaa4);
        word(0x68, presentation.field_a7eaa8);
        word(0x6c, protection.field_a7eaac);
        word(0x70, protection.field_a7eab0);
        word(0x74, presentation.field_a7eab4);
        word(0xf8, presentation.entry_state);
        word(0xfc, phase.spell_overlay);
        word(0x100, presentation.entry_timer);
        word(0x150, phase.result);
        word(0x154, phase.result_timer);
        word(0x158, phase.bonus);
        word(0x15c, presentation.bonus_decrement);
        word(0x160, phase.spell_remaining);
        word(0x164, phase.spell_index);
        const auto reals = [&](std::size_t offset, auto& values) {
            for (std::size_t i=0; i<values.size(); ++i) {
                if (load) values[i]=globals.quad(offset+i*8); else globals.quad(offset+i*8,values[i]);
            }
        };
        reals(0xd8, presentation.spell_panel_reals);
        reals(0x108, presentation.entry_reals);
        reals(0x130, presentation.spell_ring_reals);
        const auto bytes = [&](std::size_t offset, auto& values) {
            if (load) std::copy_n(globals.data.begin()+offset,values.size(),values.begin());
            else std::copy(values.begin(),values.end(),globals.data.begin()+offset);
        };
        bytes(0x78,stage.spell_name); bytes(0xb8,presentation.boss_name);
        for (std::size_t i=0;i<8;++i) {
            if (load) allocation.first_sprite_handles[i]=sprites.word(i*20);
            else sprites.word(i*20,allocation.first_sprite_handles[i]);
        }
    }
    void read(std::istream& input) {
        const auto raw = [&](auto& data) { input.read(reinterpret_cast<char*>(&data),sizeof(data)); };
        raw(globals.data); raw(death.pattern_integer_5); raw(stage.extra_reals); raw(stage.enemy_prefix.data);
        raw(sprites.data); raw(game_frame); raw(savedata.options); raw(savedata.progress); raw(savedata.scores);
        raw(savedata.extra_scores); raw(savedata.spells); raw(savedata.extra_spells);
        if (!input) throw std::runtime_error("Boss world input truncated");
        map(true);
    }
    void write(std::ostream& output) {
        map(false);
        const auto raw = [&](const auto& data) { output.write(reinterpret_cast<const char*>(&data),sizeof(data)); };
        raw(globals.data); raw(death.pattern_integer_5); raw(stage.extra_reals); raw(stage.enemy_prefix.data);
        raw(sprites.data); raw(game_frame); raw(savedata.options); raw(savedata.progress); raw(savedata.scores);
        raw(savedata.extra_scores); raw(savedata.spells); raw(savedata.extra_spells);
    }
};
}
