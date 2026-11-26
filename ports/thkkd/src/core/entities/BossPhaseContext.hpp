// Boss阶段本体的独立字段；关卡、子阶段和发弹参数由既有Context共同持有。
#pragma once
#include "core/entities/EnemyDeathContext.hpp"
#include "core/frontend/SaveData.hpp"

namespace kaikeidou {
struct BossPhaseContext {
    std::uint32_t boss_index = 0, weapon = 0, hit_flag = 0, bomb_flag = 0;
    std::uint32_t spell_remaining = 0, spell_index = 0, result = 0, result_timer = 0, bonus = 0, spell_overlay = 0;
    bool replay_variant = false;
    std::uint32_t replay_mode = 0;
};
struct BossPhaseServices : EnemyDeathServices {
    virtual void restore_stage_music(std::uint32_t mode) = 0;
    virtual void persist_spell_records(const SaveData& data) = 0;
};
}
