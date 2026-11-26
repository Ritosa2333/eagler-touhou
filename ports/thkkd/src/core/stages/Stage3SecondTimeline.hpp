// 三面中Boss完整679360..679750，缓存入口Boss槽并保留服务后的字段重读。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/stages/StageBossServices.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/stages/Stage3SecondConstants.hpp"
namespace kaikeidou {
namespace stage3_second_detail {
template<std::size_t N> inline void copy_name(std::array<std::uint8_t,64>& target,const std::array<std::uint8_t,N>& source){
    std::size_t i=0;for(;i+4<=N;i+=4)std::memcpy(target.data()+i,source.data()+i,4);
    if(i+2<=N){std::memcpy(target.data()+i,source.data()+i,2);i+=2;}if(i<N)target[i]=source[i];
}
}
inline void stage3_second_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,
    EffectPool& effects,PatternParameters& p,BossPhaseContext& phase,BossStageContext& context,
    StagePatternContext& extra,StageBossServices& services){
    using namespace stage3_second_detail;
    auto boss=phase.boss_index;
    const auto enemy=[&]() -> Enemy& {return boss==0xffffffffu?context.enemy_prefix:enemies.enemies.at(boss);};
    const auto pick=[&](std::array<std::uint32_t,6> values){return values[p.difficulty<6?p.difficulty:3];};
    const auto integer=[](std::uint32_t value){return binary_float::to_binary64(binary_float::integer(value));};
    const auto effect=[&](std::uint32_t r,std::uint32_t g,std::uint32_t b){
        const auto y=enemy().quad(0x40),x=enemy().quad(0x38);const auto color=services.color(r,g,b);
        allocate_effect(effects,projectiles.sound_triggers,{12,color,x,y});
    };
    switch(timeline.subphase){
    case 0:
        if(timeline.frame==1){boss=static_cast<std::uint32_t>(services.allocate_boss(2));enemy().word(8,14);
            enemy().quad(0x38,constant(0x6d1bb8));enemy().quad(0x40,constant(0x6d2678));
            enemy().quad(0xa8,constant(0x6d1a60));enemy().quad(0xb0,constant(0x6d1f08));}
        else if(timeline.frame==60)services.dialog(4);
        break;
    case 1:
        if(timeline.frame==1)services.begin_boss_effect();
        else if(timeline.frame==40){timeline.subphase=2;timeline.frame=0;p.game_frame=0;effect(0,255,0);}
        break;
    case 2:
        if(timeline.frame==1){services.configure_phase(0,30,extra.boss_hp_multiplier*13000u,0xfffffff6u);
            enemy().word(8,51);enemy().word(0x80,0);p.integers[0]=pick({{3,4,5,6,0,0}});
            p.integers[1]=pick({{3,3,4,5,0,0}});p.integers[2]=pick({{160,140,120,120,0,0}});}
        break;
    case 3:
        if(timeline.frame==1){enemy().word(8,14);enemy().word(0x80,1);
            enemy().quad(0xa8,integer(enemy().word(0x80)*120u+192u));enemy().quad(0xb0,constant(0x6d1d00));}
        else if(timeline.frame==140){effect(0,255,255);
            switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c6930);break;case 1:copy_name(context.spell_name,name_6c6948);break;case 2:copy_name(context.spell_name,name_6c6960);break;case 3:copy_name(context.spell_name,name_6c6978);break;default:break;}
            services.configure_phase(1,40,11000,27);enemy().quad(0x38,integer(enemy().word(0x80)*120u+192u));
            enemy().quad(0x40,constant(0x6d1d00));enemy().word(8,52);enemy().word(0x84,pick({{230,220,210,210,0,0}}));}
        break;
    case 4:
        if(timeline.frame==32)services.retire_boss(1);
        else if(timeline.frame==64){++timeline.part;timeline.subphase=0;timeline.frame=0;p.game_frame=0;}
        break;
    default:break;
    }
}
}
