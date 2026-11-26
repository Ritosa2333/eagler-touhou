// 恢复完整0x61D280..0x61D448，保留旧计时值判断、32位回绕及超时前写入。
#pragma once
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/BossPresentationContext.hpp"
#include "core/entities/EnemyDamage.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/player/PlayerOptions.hpp"
namespace kaikeidou{
inline void update_boss_timer(BossPhaseContext& phase,EnemyDeathContext& death,EnemyDamageContext& damage,
    BossPresentationContext& presentation,EnemyDeathServices& services,unsigned precision){
    if(phase.result_timer!=0)--phase.result_timer;
    if(death.boss_state==0)return;
    if(death.boss_state==2){
        auto& panel=presentation.spell_panel_reals;
        const auto subtract=[&](unsigned index,std::uint64_t operand){
            auto negative=binary_float::binary64(operand);negative.negative=!negative.negative;
            panel[index]=binary_float::to_binary64(binary_float::add(binary_float::binary64(panel[index]),negative,precision));
        };
        if(presentation.field_a7eab4!=0){
            const auto value=crt_sse2::real(panel[2]);
            if(value<184.||value>216.){
                if(value>0.)subtract(2,0x4028000000000000ull);
                else{
                    subtract(2,0x4028000000000000ull);subtract(3,0x4008000000000000ull);
                    if(crt_sse2::real(panel[3])<=0.)presentation.field_a7eab4=0;
                }
            }else{subtract(2,0x3fe3333333333333ull);subtract(3,0x3ff8000000000000ull);}
        }
        if(crt_sse2::real(panel[1])>19.){
            if(crt_sse2::real(panel[0])>384.)panel[0]=ease_option_value(panel[0],0x4077f00000000000ull,0x4028000000000000ull,precision);
            else panel[1]=ease_option_value(panel[1],0x4033000000000000ull,0x4028000000000000ull,precision);
        }
        if(options_detail::signed_word(damage.field_a7eaa4)<options_detail::signed_word(damage.field_a7eaac-5u))phase.bonus-=presentation.bonus_decrement;
    }
    const auto old_tick=presentation.field_a7eaa8++;
    if(old_tick==59){
        presentation.field_a7eaa8=0;const auto old_seconds=damage.field_a7eaa4--;
        if(old_seconds==0)services.advance_phase(1);
    }
}
}
