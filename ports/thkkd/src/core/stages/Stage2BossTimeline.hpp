// 二面关底67AA90..67B737，保留入口Boss槽缓存、阶段写入及资源调用顺序。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/stages/StageBossServices.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/stages/StageResourceContext.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/EnemyDeathContext.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/stages/Stage2BossConstants.hpp"
#include "core/stages/Stage2BossOpeningMath.hpp"
namespace kaikeidou {
namespace stage2_boss_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:std::int64_t(v);}
template<std::size_t N> inline void copy_name(std::array<std::uint8_t,64>& target,const std::array<std::uint8_t,N>& source){
    std::size_t i=0;for(;i+4<=N;i+=4)std::memcpy(target.data()+i,source.data()+i,4);
    if(i+2<=N){std::memcpy(target.data()+i,source.data()+i,2);i+=2;}if(i<N)target[i]=source[i];
}
}
inline void stage2_boss_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,
    EffectPool& effects,ObjectRecord<240>& player,PatternParameters& p,BossPhaseContext& phase,BossStageContext& context,
    StagePatternContext& extra,EnemyDeathContext& death,StageResourceContext& resources,
    RandomPrecision precision,StageBossServices& services,StageResourceServices& platform){
    using namespace stage2_boss_detail;const auto bits=static_cast<unsigned>(precision);
    const auto boss=phase.boss_index;
    const auto enemy=[&]() -> Enemy& {return boss==0xffffffffu?context.enemy_prefix:enemies.enemies.at(boss);};
    const auto next_quad=[&](std::size_t offset,std::uint64_t value){
        if(boss==79)player.quad(offset,value);else enemies.enemies.at(boss==0xffffffffu?0:boss+1).quad(offset,value);
    };
    const auto pick=[&](std::array<std::uint32_t,6> values){return values[p.difficulty<6?p.difficulty:3];};
    const auto real=[&](std::array<std::uint64_t,6> values){return values[p.difficulty<6?p.difficulty:3];};
    const auto c=[](std::uint32_t address){return constant(address);};
    const auto integer=[](std::uint32_t value){return binary_float::to_binary64(binary_float::integer(value));};
    const auto effect=[&](std::uint32_t r,std::uint32_t g,std::uint32_t b){
        const auto y=enemy().quad(0x40),x=enemy().quad(0x38);const auto color=services.color(r,g,b);
        allocate_effect(effects,projectiles.sound_triggers,{12,color,x,y});
    };
    switch(timeline.subphase){
    case 0:
        if(timeline.frame==1){services.dialog(2);resources.integers[0]=1;resources.integers[1]=255;
            platform.movie_seek(resources.handles[3],0);platform.movie_play(resources.handles[3],1,0);}
        break;
    case 1:
        if(timeline.frame==1)services.begin_boss_effect();
        else if(timeline.frame==10)effect(0,255,255);
        else if(timeline.frame==50){
            enemy().word(0x80,1);services.configure_phase(0,30,extra.boss_hp_multiplier*8500u,0xfffffff6u);
            enemy().quad(0xb0,c(0x6d1de0));enemy().word(0x84,0);enemy().word(0x88,0);
            p.integers[1]=pick({{100,80,48,42,0,0}});p.integers[2]=pick({{3,4,5,5,0,0}});
            p.reals[0]=c(0x6d1ad8);p.reals[1]=c(0x6d19d8);
            const auto selected=real({{c(0x6d2480),c(0x6d1ad8),c(0x6d25f0),c(0x6d2660),0,0}});
            context.extra_reals[0]=stage2_boss_opening_turn_rate(selected,p.integers[1],bits);
            context.extra_reals[1]=real({{c(0x6d26e0),c(0x6d26e8),c(0x6d26f0),c(0x6d1eb8),0,0}});
        }
        break;
    case 2:
        if(timeline.frame==1){enemy().word(8,14);enemy().quad(0xa8,c(0x6d1a60));enemy().quad(0xb0,integer(signed_word(p.difficulty)>1?100u:160u));}
        else if(timeline.frame==60){
            switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c6994);break;case 1:copy_name(context.spell_name,name_6c69a8);break;case 2:copy_name(context.spell_name,name_6c69bc);break;case 3:copy_name(context.spell_name,name_6c69d8);break;}
            const auto hp=pick({{18000,18000,12000,12000,0,0}})*extra.boss_hp_multiplier;
            services.configure_phase(1,40,hp,15);
            enemy().word(0xc,0xffffffd8u);enemy().word(8,37);enemy().word(0x80,0);enemy().word(0x84,0);
            enemy().quad(0xa8,c(0x6d1a60));enemy().quad(0xb0,integer(signed_word(p.difficulty)>1?100u:160u));
            p.reals[0]=0;p.reals[1]=0;context.extra_reals[0]=0;if(signed_word(p.difficulty)>=2)context.extra_reals[1]=0;
        }else if(timeline.frame==100){if(signed_word(p.difficulty)<=1)effect(255,96,0);else effect(255,0,0);}
        break;
    case 3:
        if(timeline.frame==20){enemy().word(8,14);enemy().quad(0xa8,c(0x6d2108));enemy().quad(0xb0,c(0x6d1f08));}
        else if(timeline.frame==120)effect(0,255,255);
        else if(timeline.frame==160){
            services.configure_phase(0,30,extra.boss_hp_multiplier*9000u,0xfffffff6u);
            enemy().quad(0x38,c(0x6d2108));enemy().quad(0x40,c(0x6d1f08));enemy().word(8,38);
            enemy().quad(0xa8,c(0x6d1860));enemy().quad(0xb0,c(0x6d1de0));enemy().word(0x84,0);enemy().word(0x88,0);enemy().word(0x8c,0);
            enemy().word(0x90,pick({{294,258,186,171,0,0}}));enemy().word(0x94,0);enemy().word(0x98,pick({{7,9,11,12,0,0}}));
            p.integers[1]=pick({{98,86,62,57,0,0}});p.integers[2]=1;p.reals[0]=c(0x6d1ad8);p.reals[1]=c(0x6d19d8);
            context.extra_reals[0]=real({{c(0x6d26c0),c(0x6d26c8),c(0x6d26d0),c(0x6d26d8),0,0}});
            context.extra_reals[1]=real({{c(0x6d26e8),c(0x6d26f0),c(0x6d1eb8),c(0x6d1eb8),0,0}});
            extra.real_4=real({{0,c(0x6d1ab8),c(0x6d1b68),c(0x6d1b48),0,0}});
        }
        break;
    case 4:
        if(timeline.frame==1){enemy().word(8,14);enemy().quad(0xa8,integer(pick({{242,242,192,192,0,0}})));enemy().quad(0xb0,integer(pick({{140,140,35,428,0,0}})));}
        else if(timeline.frame==60){
            switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c69f4);break;case 1:copy_name(context.spell_name,name_6c6a14);break;case 2:copy_name(context.spell_name,name_6c6a34);break;case 3:copy_name(context.spell_name,name_6c6a50);break;}
            const auto hp=pick({{10000,10000,8500,9000,0,0}})*extra.boss_hp_multiplier;
            const auto seconds=pick({{40,40,48,48,0,0}});services.configure_phase(1,seconds,hp,19);
            if(signed_word(p.difficulty)<=1){enemy().word(8,39);enemy().word(0x80,0);enemy().word(0x84,pick({{40,90,20,20,0,0}}));enemy().quad(0xa8,c(0x6d1de0));next_quad(0,c(0x6d2100));next_quad(8,c(0x6d1f08));}
            else if(p.difficulty==2){enemy().word(8,40);enemy().word(0x80,0);enemy().quad(0xa8,0);enemy().quad(0xb0,0);}
            else{enemy().word(8,41);enemy().word(0x80,0);enemy().quad(0xa8,0);}
        }
        break;
    case 5:
        if(timeline.frame==1){enemy().word(8,14);enemy().quad(0xa8,c(0x6d1a60));enemy().quad(0xb0,c(0x6d1af0));}
        else if(timeline.frame==200){
            effect(0,255,255);
            switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c6a70);break;case 1:copy_name(context.spell_name,name_6c6a88);break;case 2:copy_name(context.spell_name,name_6c6aa0);break;case 3:copy_name(context.spell_name,name_6c6ab8);break;}
            services.configure_phase(1,40,24000,23);enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d1af0));enemy().word(8,42);
            enemy().word(0x80,pick({{8,8,10,15,0,0}}));p.reals[0]=c(0x6d26b8);
            p.reals[1]=real({{c(0x6d2570),c(0x6d26a8),c(0x6d26a8),c(0x6d26b0),0,0}});p.integers[0]=pick({{80,65,55,50,0,0}});
        }
        break;
    case 6:if(timeline.frame==32)services.retire_boss(0);else if(timeline.frame==196)services.dialog(3);break;
    case 7:if(timeline.frame==1)services.award_stage_clear();else if(timeline.frame==200){++death.stage;timeline.part=0;timeline.subphase=0;timeline.frame=0;p.game_frame=0;services.change_stage_resources(death.stage);}break;
    }
}
}
