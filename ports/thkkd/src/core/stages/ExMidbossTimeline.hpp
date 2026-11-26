// 原68ACB0..68B8A5；缓存入口Boss槽，按原顺序推进五个阶段。
#pragma once
#include "core/stages/ExMidbossArithmetic.hpp"
#include "core/stages/ExMidbossEmission.hpp"
#include "core/stages/Stage1Timeline.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/shared/EffectPool.hpp"
namespace kaikeidou {
inline void ex_midboss_segment(StageTimeline& t,EnemyPool& enemies,ProjectilePools& pool,EffectPool& effects,const ObjectRecord<240>& player,PatternParameters& p,BossPhaseContext& phase,BossStageContext& context,Random& random,RandomPrecision precision,GameMath& math,BossStageServices& services){
    using namespace ex_midboss_detail;
    const Arithmetic m{{static_cast<unsigned>(precision)}};
    auto boss=phase.boss_index;
    const auto enemy=[&]()->Enemy&{return boss==0xffffffffu?context.enemy_prefix:enemies.enemies.at(boss);};
    const auto reset=[&](){t.frame=0;p.game_frame=0;};
    const auto costume=[&](std::uint32_t value){enemy().word(0x2c,value);enemy().word(0x30,0);enemy().word(0x34,0);};
    const auto target=[&](std::uint32_t y){enemy().word(8,17);enemy().quad(0xa8,constant(0x6d1a60));enemy().quad(0xb0,constant(y));enemy().quad(0xb8,constant(0x6d1ae8));enemy().quad(0xc0,constant(0x6d1ae8));};
    const auto move_target=[&](){
        enemy().quad(0xa8,player.quad(0x90));
        const auto ry=random.real_number(constant(0x6d2028),precision);
        enemy().quad(0xb0,m.store(m.add(m.mul(ry,m.c(0x6d1960)),m.c(0x6d1d50))));
        if(compare(m.c(0x6d21f0),m.q(enemy().quad(0xa8)))>0)enemy().quad(0xa8,constant(0x6d21f0));
        else if(compare(m.c(0x6d2fa0),m.q(enemy().quad(0xa8)))<0)enemy().quad(0xa8,constant(0x6d2fa0));
        const auto rx=random.real_number(constant(0x6d1aa8),precision);
        enemy().quad(0xa8,m.store(m.add(m.add(rx,m.c(0x6d1b58)),m.q(enemy().quad(0xa8)))));
    };
    switch(t.subphase){
    case 0:
        if(t.frame==1){boss=static_cast<std::uint32_t>(services.allocate_boss(6));enemy().word(8,14);enemy().quad(0x38,constant(0x6d1a60));enemy().quad(0x40,constant(0x6d1b58));enemy().quad(0xa8,constant(0x6d1a60));enemy().quad(0xb0,constant(0x6d1f08));}
        else if(t.frame==10)services.begin_boss_effect();
        else if(t.frame==60){t.subphase=1;reset();}
        break;
    case 1:
        if(t.frame==1)target(0x6d1b38);
        else if(t.frame==100){ex_midboss_name(context.spell_name,0x6c5f8c);services.configure_phase(1,60,27000,122);}
        else if(t.frame==160){const auto color=services.color(0,0,255);allocate_effect(effects,pool.sound_triggers,{10,color,constant(0x6d1a60),constant(0x6d1b38)});costume(3);}
        else if(t.frame==200){
            enemy().quad(0x40,constant(0x6d1b38));p.reals[0]=constant(0x6d2aa0);
            const auto slot=allocate_projectile(pool,{18,0,{{0,0,255}},2,255,1,41});
            if(slot!=-1){auto& b=pool.bullets[slot];b.quad(0x40,enemy().quad(0x38));b.quad(0x48,enemy().quad(0x40));b.word(8,109);b.word(0x34,1);b.quad(0x68,0);const auto side=random.integer_bits(1,precision,true);b.word(0x78,side);b.quad(0xa0,b.word(0x78)?constant(0x6d1860):0);b.quad(0xa8,constant(0x6d1960));}
            enemy().word(0x2c,0);
        }
        if(signed_word(t.frame)>=200){p.reals[0]=m.store(m.sub(m.q(p.reals[0]),m.c(0x6d1e00)));if(remainder(t.frame,300)==199)move_target();}
        break;
    case 2:
        if(t.frame==200){target(0x6d2780);ex_midboss_name(context.spell_name,0x6c5f9c);services.configure_phase(1,60,8000,123);}
        else if(t.frame==260){const auto y=enemy().quad(0x40),x=enemy().quad(0x38);const auto color=services.color(255,0,0);allocate_effect(effects,pool.sound_triggers,{12,color,x,y});costume(3);}
        else if(t.frame==300){
            for(auto& b:pool.bullets)if(b.word(4)==18&&b.word(8)==25){b.word(8,111);b.quad(0xa8,constant(0x6d1960));break;}
            p.reals[0]=constant(0x6d1f08);
            const auto slot=allocate_projectile(pool,{18,0,{{255,0,0}},2,255,1,41});
            if(slot!=-1){auto& b=pool.bullets[slot];b.quad(0x40,enemy().quad(0x38));b.quad(0x48,enemy().quad(0x40));b.word(8,112);b.word(0x34,1);b.quad(0xa0,0);}
            enemy().word(0x2c,0);
        }
        if(signed_word(t.frame)>=300&&remainder(t.frame,300)==299)move_target();
        break;
    case 3:
        if(t.frame==200){target(0x6d1d00);ex_midboss_name(context.spell_name,0x6c5fac);services.configure_phase(1,60,21000,124);costume(3);}
        else if(t.frame==260){p.reals[0]=constant(0x6d1b60);p.reals[1]=0;p.integers[0]=5;p.integers[1]=255;p.integers[2]=0;p.integers[3]=5;enemy().word(0x2c,0);}
        if(signed_word(t.frame)>=260){
            switch(p.integers[0]){
            case 0:p.integers[2]+=5;if(p.integers[2]==255)p.integers[0]=1;break;
            case 1:p.integers[1]-=5;if(p.integers[1]==0)p.integers[0]=2;break;
            case 2:p.integers[3]+=5;if(p.integers[3]==255)p.integers[0]=3;break;
            case 3:p.integers[2]-=5;if(p.integers[2]==0)p.integers[0]=4;break;
            case 4:p.integers[1]+=5;if(p.integers[1]==255)p.integers[0]=5;break;
            case 5:p.integers[3]-=5;if(p.integers[3]==0)p.integers[0]=0;break;
            default:break;
            }
            const auto emit=[&](bool second){
                auto speed=m.add(m.mul(math.sine(p.reals[1]),m.c(0x6d19c0)),m.c(0x6d1b18));if(second)speed=m.add(speed,speed);
                const auto speed_bits=m.store(speed);
                const auto x=m.store(m.add(m.mul(math.cosine(p.reals[0]),m.c(0x6d1a60)),m.c(0x6d1a60)));
                ex_midboss_emit(pool,{{second?2u:4u,second?1:2,{{p.integers[1],p.integers[2],p.integers[3]}},second?2u:0u,255,0,41},x,constant(second?0x6d2680:0x6d1a30),1,0,speed_bits,constant(0x6d1b60),1,0},player,p,random,precision,math);
            };
            emit(false);if(remainder(t.frame,68)==0)emit(true);
            p.reals[0]=m.store(m.add(m.q(p.reals[0]),m.c(0x6d1fa8)));
            p.reals[1]=m.store(m.add(m.q(p.reals[1]),m.c(0x6d2fb0)));
            context.extra_reals[0]=m.store(m.add(m.q(context.extra_reals[0]),m.c(0x6d2fb8)));
        }
        break;
    case 4:if(t.frame==32)services.retire_boss(1);else if(t.frame==64){++t.part;t.subphase=0;reset();}break;
    default:break;
    }
}
}
