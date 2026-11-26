// 恢复完整 0x00643840..0x00643aa5，仅写原分配器指定字段，满池原样返回 -1。
#pragma once
#include "core/entities/BossAllocationContext.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/EnemyDamage.hpp"
#include "core/entities/EnemyPool.hpp"

namespace kaikeidou {
inline std::int32_t allocate_boss(EnemyPool& pool,BossAllocationContext& allocation,BossPhaseContext& phase,
    EnemyDeathContext& death,EnemyDamageContext& damage,std::uint32_t id) {
    for(std::size_t index=0;index<pool.enemies.size();++index) {
        auto& enemy=pool.enemies[index];if(enemy.word(0)!=0)continue;
        enemy.word(0,1);enemy.word(4,id+10u);enemy.word(0xc,0);
        allocation.field_a7ea48=1;phase.boss_index=static_cast<std::uint32_t>(index);
        allocation.field_a7ea44=allocation.first_sprite_handles.at(id);
        allocation.field_a7ea58=1;allocation.field_a7ea5c=255;death.boss_state=0;
        damage.boss_state_a7ea60=0;allocation.field_a7ea50=id;
        enemy.word(0x24,1000);enemy.word(0x28,1);enemy.word(0x2c,0);enemy.word(0x30,0);enemy.word(0x34,0);
        switch(death.stage) {
        case 0:phase.spell_remaining=death.part==1?0u:1u;break;
        case 1:case 2:phase.spell_remaining=death.part==1?0u:2u;break;
        case 3:case 4:phase.spell_remaining=death.part==1?0u:3u;break;
        case 5:phase.spell_remaining=death.part==1?0u:4u;break;
        case 6:phase.spell_remaining=death.part==1?2u:(death.subphase==0?9u:0u);break;
        case 7:phase.spell_remaining=0;break;
        default:break;
        }
        return static_cast<std::int32_t>(index);
    }
    return -1;
}
}
