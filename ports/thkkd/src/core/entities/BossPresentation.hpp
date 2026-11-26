// 恢复完整Boss阶段配置6434F0、入场6437F0和退场643C60，保留服务前后读取时点。
#pragma once
#include "core/entities/BossPresentationContext.hpp"
#include "core/entities/BossAllocationContext.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/EnemyDamage.hpp"
#include "core/player/ItemPool.hpp"
#include "core/shared/EffectUpdate.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/player/PlayerOptions.hpp"
#include "core/entities/BossPhaseMath.hpp"
#include <cstring>

namespace kaikeidou {
inline void configure_boss_phase(EnemyPool& enemies,BossAllocationContext& allocation,BossPhaseContext& phase,
    EnemyDeathContext& death,EnemyDamageContext& damage,BossPresentationContext& presentation,
    PatternParameters& parameters,BossPresentationServices& services,RandomPrecision precision,
    std::uint32_t spell,std::uint32_t seconds,std::uint32_t hp,std::uint32_t spell_index){
    const auto index=phase.boss_index;
    auto& enemy=enemies.enemies.at(index);
    enemy.word(0x18,hp);enemy.word(0x14,hp);enemy.word(0xc,0);enemy.word(0x28,0);
    phase.hit_flag=0;phase.bomb_flag=0;allocation.field_a7ea58=1;
    damage.field_a7eaa4=seconds-1u;presentation.field_a7eaa8=0;
    damage.field_a7eaac=seconds;damage.field_a7eab0=2;damage.boss_state_a7ea60=0;
    if(spell==0)death.boss_state=1;
    else{
        services.prepare_spell_music();
        death.boss_state=2;presentation.field_a7eab4=1;
        presentation.spell_panel_reals[2]=0x4080000000000000ull;
        presentation.spell_panel_reals[3]=0x406fe00000000000ull;
        presentation.spell_panel_reals[0]=0x4078000000000000ull;
        presentation.spell_panel_reals[1]=0x407a800000000000ull;
        phase.spell_overlay=1;presentation.spell_ring_reals[2]=0;
        presentation.spell_ring_reals[0]=options_detail::stored_bits(enemy.quad(0x38));
        presentation.spell_ring_reals[1]=options_detail::stored_bits(enemy.quad(0x40));
        presentation.spell_ring_reals[3]=boss_spell_ring_rate(seconds,precision);
        phase.spell_index=options_detail::signed_word(parameters.difficulty)<4?spell_index+parameters.difficulty:spell_index;
        phase.bonus=parameters.difficulty==5?8000000u:(death.stage+1u+parameters.difficulty)*2000000u;
        presentation.bonus_decrement=enemies.enemies.at(phase.boss_index).word(0x18)==0?0u:
            boss_spell_bonus_decrement(phase.bonus,damage.field_a7eaac,precision);
    }
    const auto kind=enemy.word(4);
    constexpr const char* names[]{"Susanomori Konoha","Uzushima Naruko","Amazu Miyo","Minazuki Tubame",
        "Sindou Kasumi","Yaobi Megumi","Yaobi Mikoto","Kanpukugu xxxx"};
    if(kind>=10&&kind<=17){const auto name=names[kind-10];std::memcpy(presentation.boss_name.data(),name,std::strlen(name)+1);}
}
inline void begin_boss_effect(BossPresentationContext& presentation){
    presentation.entry_state=1;
    presentation.entry_reals[3]=0x4030000000000000ull;
    presentation.entry_reals[4]=0x3ff0000000000000ull;
    presentation.entry_reals[1]=0;presentation.entry_reals[2]=0;presentation.entry_reals[0]=0;
    presentation.entry_timer=0;
}
inline void retire_boss(std::uint32_t mode,EnemyPool& enemies,ItemPool& items,EffectPool& effects,EffectShake& shake,
    std::array<std::uint32_t,100>& sounds,BossAllocationContext& allocation,const BossPhaseContext& phase,
    BossPresentationContext& presentation,BossPresentationServices& services){
    const auto index=phase.boss_index;
    if(mode==0){for(auto& item:items.items)if(item.word(0)!=0&&item.word(0xc)==0)item.word(0xc,1);}
    else if(mode!=1&&mode!=2)return;
    allocation.field_a7ea48=0;enemies.enemies.at(index).word(0,0);presentation.entry_state=0;
    if(mode==2)return;
    set_effect_screen_shake(shake,10,8);
    const auto kind=enemies.enemies[index].word(4);
    if(kind<10||kind>17)return;
    constexpr std::array<std::array<std::uint32_t,3>,8> colors{{{{0,221,0}},{{255,128,0}},{{0,255,255}},{{128,128,128}},
        {{221,0,255}},{{255,0,0}},{{0,0,255}},{{0,0,0}}}};
    const auto y=options_detail::stored_bits(enemies.enemies[index].quad(0x40));
    const auto x=options_detail::stored_bits(enemies.enemies[index].quad(0x38));
    const auto& rgb=colors[kind-10];const auto color=services.color(rgb[0],rgb[1],rgb[2]);
    allocate_effect(effects,sounds,{1,color,x,y});
}
}
