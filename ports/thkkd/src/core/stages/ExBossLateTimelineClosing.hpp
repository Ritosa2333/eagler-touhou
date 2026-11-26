#pragma once
#include "core/stages/ExBossLateTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_timeline_detail {
inline void Engine::phase21(){
    if(t.frame==32)services.retire_boss(0);else if(t.frame==128)services.dialog(14);
    for(unsigned key=0;key<5;++key)if(signed_word(ex_boss_late_timeline_input(bindings[key],fire,input))>0)for(unsigned n=0;n<=key;++n)random.real_number(0x3ff0000000000000ull,precision);
}
inline void Engine::phase22(){
    if(player.word(0x2c)){protection.boss_state_a7ea60=1;allocation.field_a7ea58=0;}else{protection.boss_state_a7ea60=0;allocation.field_a7ea58=1;}
    if(t.frame==1){services.begin_boss_effect();presentation.entry_timer=1;}
    else if(t.frame==30){
        enemy_at(integer(3)).quad(0xa8,m.store(m.add(m.q(enemy_at(integer(3)).quad(0xa8)),m.c(0x6b25f8))));enemy_at(integer(4)).quad(0xa8,m.store(m.sub(m.q(enemy_at(integer(4)).quad(0xa8)),m.c(0x6b25f8))));
        enemy_at(integer(2)).quad(0xb0,m.store(m.add(m.q(enemy_at(integer(2)).quad(0xb0)),m.c(0x6d1ae8))));enemy_at(integer(3)).quad(0xb0,m.store(m.add(m.q(enemy_at(integer(3)).quad(0xb0)),m.c(0x6d1a88))));enemy_at(integer(4)).quad(0xb0,m.store(m.add(m.q(enemy_at(integer(4)).quad(0xb0)),m.c(0x6d1a88))));
    }else if(t.frame==100){
        integer(5)=0;ex_boss_late_timeline_name(context.spell_name,0x6c5f6c);services.configure_phase(1,150,extra.boss_hp_multiplier*16000u,152);
        enemy_at(integer(3)).word(0x18,13000);enemy_at(integer(3)).word(0x14,13000);enemy_at(integer(4)).word(0x18,13000);enemy_at(integer(4)).word(0x14,13000);
    }else if(t.frame==160){
        for(unsigned i=2;i<=4;++i){enemy_at(integer(i)).word(8,137);enemy_at(integer(i)).word(0x84,0);enemy_at(integer(i)).word(0x88,1);
            auto value=random.real_number(constant(0x6d1c60),precision);enemy_at(integer(i)).quad(0xa8,m.store(m.mul(value,m.c(0x6d1950))));value=random.real_number(constant(0x6d1c60),precision);enemy_at(integer(i)).quad(0xb0,m.store(m.mul(value,m.c(0x6d1950))));
            enemy_at(integer(i)).quad(0xb8,m.store(m.add(m.add(m.mul(m.i(i),m.c(0x6d20c8)),m.c(0x6d2500)),m.c(0x6d1860))));enemy_at(integer(i)).quad(0xc0,m.store(m.add(m.add(m.mul(m.i(i),m.c(0x6d2d58)),m.c(0x6d2500)),m.c(0x6d1860))));enemy_at(integer(i)).word(0xc,i==2?0u:60u);
        }
        bullet(integer(0)).word(0x7c,1);bullet(integer(1)).word(0x7c,1);
    }
}
inline void Engine::phase23(){
    if(t.frame==1){for(unsigned i=2;i<=4;++i)enemy_at(integer(i)).word(8,0);clear_projectiles(pool,100);}
    else if(t.frame==32){allocation.field_a7ea48=0;presentation.entry_state=0;set_effect_screen_shake(shake,10,8);
        for(unsigned i=0;i<80;++i)if(enemy_at(i).word(0)==1){const auto y=enemy_at(i).quad(0x40),x=enemy_at(i).quad(0x38);const auto color=services.color(0,0,0);allocate_effect(effects,pool.sound_triggers,{1,color,x,y});enemy_at(i).word(0,0);}clear_projectiles(pool,100);
    }else if(t.frame==33)clear_projectiles(pool,100);else if(t.frame==196){++t.subphase;reset();}
}
inline void Engine::phase24(){if(t.frame==128)services.dialog(15);}
inline void Engine::phase25(){
    if(t.frame==1)services.award_stage_clear();else if(t.frame==256)runtime.ending.word(0x20,1);
    else if(t.frame==511){services.pause_background();services.stop_music();runtime.outer_scene=10;services.return_to_title(fire.replay_variant);services.initialize_title_visuals();}
}
}}
