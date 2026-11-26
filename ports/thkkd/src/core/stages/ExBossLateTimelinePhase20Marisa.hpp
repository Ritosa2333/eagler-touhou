#pragma once
#include "core/stages/ExBossLateTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_timeline_detail {
inline void Engine::phase20_marisa(){
    if(t.frame==200){ex_boss_late_timeline_name(context.spell_name,0x6c5f40);services.configure_phase(1,90,extra.boss_hp_multiplier*33000u,150);enemy().word(8,0);integer(1)=90000;}
    if(frame()<300)return;
    if(remainder(t.frame,300)==0){effect(12,{{0,0,255}});effect_at(13,{{0,0,255}},player.quad(0x90),player.quad(0x98));}
    else if(remainder(t.frame,300)==40){
        effect(14,{{0,0,255}});effect_at(14,{{0,0,255}},player.quad(0x90),player.quad(0x98));const auto left=compare(m.c(0x6d1a60),q(0x38))>=0;
        auto value=random.real_number(constant(0x6d1ab0),precision);const auto target_x=m.store(left?m.sub(m.c(0x6d2160),m.mul(value,m.c(0x6d1960))):m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d1a40)));
        real(0)=m.store(m.div(m.sub(m.q(target_x),q(0x38)),m.c(0x6d2a48)));value=random.real_number(constant(0x6d2710),precision);real(1)=m.store(m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d1b38)));
    }else if(remainder(t.frame,300)>=80&&remainder(t.frame,300)<=299){
        put(0x38,m.add(q(0x38),m.q(real(0))));ex_boss_late_timeline_enemy_ease(enemy(),0x40,real(1),constant(0x6d1a90),m.precision);
        const auto spawn=[&](unsigned level,std::uint32_t angle_max,std::uint32_t angle_base){const auto slot=allocate_projectile(pool,{1,0,{{0,0,255}},2,255,1,41});if(slot==-1)return;auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,0);b.word(0x7c,level);
            auto value=random.real_number(constant(angle_max),precision);b.quad(0xa0,m.store(m.add(m.mul(value,m.c(0x6d1950)),m.c(angle_base))));value=random.real_number(constant(0x6d1f60),precision);b.quad(0xa8,m.store(m.add(m.mul(value,m.c(0x6a7888)),m.c(0x6d2d88))));b.quad(0x40,enemy().quad(0x38));b.quad(0x48,enemy().quad(0x40));b.word(8,185);b.word(0x34,1);
        };
        if(signed_word(enemy().word(0x14))>22000){if(remainder(t.frame,22)==0)spawn(0,0x6d2710,0x6d2818);}
        else if(signed_word(enemy().word(0x14))>11000){if(remainder(t.frame,34)==0)spawn(1,0x6d2028,0x6d2d80);}
        else if(remainder(t.frame,48)==0)spawn(2,0x6d2710,0x6d2818);else if(remainder(t.frame,48)==24)spawn(2,0x6d2710,0x6d2f48);
    }
    if(signed_word(enemy().word(0x14))<=11000){if(signed_word(integer(1))>11000)clear_projectiles(pool,100);}
    else if(signed_word(enemy().word(0x14))<=22000&&signed_word(integer(1))>22000)clear_projectiles(pool,100);integer(1)=enemy().word(0x14);
}
}}
