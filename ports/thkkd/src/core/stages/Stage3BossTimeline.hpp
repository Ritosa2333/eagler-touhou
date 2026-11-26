// 原692200..69414A完整十阶段，保留入口Boss槽缓存及时间轴直接发弹。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/stages/StageBossServices.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/stages/StageResourceContext.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/EnemyDeathContext.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
#include "core/stages/Stage3InitialMath.hpp"
#include "core/stages/Stage3BossTimelineContext.hpp"
#include "core/stages/Stage3BossTimelineArithmetic.hpp"
namespace kaikeidou {
inline void stage3_boss_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,
    EffectPool& effects,ObjectRecord<240>& player,PatternParameters& p,BossPhaseContext& phase,BossStageContext& context,
    StagePatternContext& extra,Stage3BossTimelineContext& extended,EnemyDeathContext& death,StageResourceContext& resources,
    Random& random,RandomPrecision precision,GameMath& math,StageBossServices& services,StageResourceServices& platform){
    (void)player;(void)resources;(void)platform;
    using namespace stage3_boss_timeline_detail;using Number=binary_float::Number;
    const auto bits=static_cast<unsigned>(precision);const Arithmetic m{bits};auto boss=phase.boss_index;
    const auto enemy=[&]() -> Enemy& {return boss==0xffffffffu?context.enemy_prefix:enemies.enemies.at(boss);};
    const auto c=[](std::uint32_t address){return constant(address);};
    const auto r=[&](std::size_t offset){return m.q(enemy().quad(offset));};
    const auto w=[&](std::size_t offset,Number value){enemy().quad(offset,m.store(value));};
    const auto pick=[&](std::array<std::uint32_t,6> values){return stage3_boss_timeline_pick_integer(p.difficulty,values);};
    const auto real=[&](std::array<std::uint64_t,6> values){return stage3_boss_timeline_pick_real(p.difficulty,values);};
    const auto effect_at=[&](std::uint32_t kind,std::uint64_t x,std::uint64_t y,std::uint32_t red,std::uint32_t green,std::uint32_t blue){
        const auto color=services.color(red,green,blue);allocate_effect(effects,projectiles.sound_triggers,{kind,color,x,y});
    };
    const auto effect=[&](std::uint32_t red,std::uint32_t green,std::uint32_t blue){const auto y=enemy().quad(0x40),x=enemy().quad(0x38);effect_at(12,x,y,red,green,blue);};
    const auto emit=[&](ProjectileAllocation allocation,std::uint64_t x,std::uint64_t y,std::uint32_t count,std::uint64_t spacing,std::uint64_t speed,std::uint64_t angle,std::uint32_t absolute,std::uint32_t configuration){
        emit_projectiles(projectiles,{allocation,x,y,count,spacing,speed,angle,absolute,configuration},player.quad(0x90),player.quad(0x98),p,random,precision,math);
    };
    const auto move=[&](std::uint32_t target_y,std::uint32_t divisor){enemy().word(8,55);w(0xa8,m.div(m.sub(m.c(0x6d1a60),r(0x38)),m.c(divisor)));w(0xb0,m.div(m.sub(m.c(target_y),r(0x40)),m.c(divisor)));};
    const auto rgb=[&](){return std::array<std::uint32_t,3>{{extra.integers_6_to_19[4],extra.integers_6_to_19[5],extra.integers_6_to_19[6]}};};
    auto& real20=extended.reals_20_to_29[0];
    switch(timeline.subphase){
    case 0:
        if(timeline.frame==1){boss=static_cast<std::uint32_t>(services.allocate_boss(3));enemy().word(8,55);enemy().quad(0x38,c(0x6d1bb8));enemy().quad(0x40,c(0x6d1b38));enemy().quad(0xa8,c(0x6d3168));enemy().quad(0xb0,0);}
        else if(timeline.frame==40)services.begin_boss_effect();
        else if(timeline.frame==80)enemy().word(8,0);
        else if(timeline.frame==130){timeline.subphase=1;timeline.frame=0;p.game_frame=0;}
        break;
    case 1:
        if(timeline.frame==10){services.configure_phase(0,30,extra.boss_hp_multiplier*12000u,0xfffffff6u);enemy().word(8,56);
            enemy().word(0x80,pick({{200,170,200,180,0,0}}));enemy().quad(0xa8,0);enemy().quad(0xb0,0);
            enemy().quad(0xb8,real({{c(0x6d21c8),c(0x6d21c8),c(0x6d1e68),c(0x6d1e68),0,0}}));
            p.integers[0]=10;p.integers[1]=60;p.integers[2]=pick({{20,16,12,10,0,0}});p.reals[0]=real({{c(0x6d1ad0),c(0x6d1e40),c(0x6d1c80),c(0x6d1c80),0,0}});
        }else if(timeline.frame==26)effect(128,128,128);
        break;
    case 2:if(timeline.frame==1)services.dialog(5);break;
    case 3:
        if(timeline.frame==1)move(0x6d1f08,0x6d1a88);
        else if(timeline.frame==51){effect(255,255,255);
            switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c67f4);break;case 1:copy_name(context.spell_name,name_6c6818);break;case 2:copy_name(context.spell_name,name_6c683c);break;case 3:copy_name(context.spell_name,name_6c6858);break;}
            services.configure_phase(1,40,extra.boss_hp_multiplier*12000u,31);enemy().word(8,59);enemy().word(0xc,70);
            enemy().word(0x80,pick({{300,280,250,250,0,0}}));enemy().word(0x84,pick({{14,11,7,5,0,0}}));p.reals[0]=0;p.reals[1]=c(0x6d1cb0);
            context.extra_reals[0]=real({{c(0x6d1f50),c(0x6d1b28),c(0x6d1ad0),c(0x6d1e40),0,0}});p.integers[0]=0;
        }break;
    case 4:
        if(timeline.frame==1)move(0x6d1b38,0x6d1a88);
        else if(timeline.frame==51){services.configure_phase(0,30,extra.boss_hp_multiplier*12000u,0xfffffff6u);enemy().word(8,57);enemy().word(0xc,40);
            p.integers[0]=10;p.integers[1]=60;p.integers[2]=pick({{60,38,14,10,0,0}});p.reals[0]=real({{c(0x6d1f50),c(0x6d1a00),c(0x6d2488),c(0x6d1b28),0,0}});
            effect_at(10,c(0x6d1a80),c(0x6d1cc0),128,128,128);
        }else if(timeline.frame==20){effect_at(10,c(0x6d1a78),c(0x6d1cc0),128,128,128);enemy().word(0x2c,3);enemy().word(0x30,0);enemy().word(0x34,0);}
        break;
    case 5:
        if(timeline.frame==1)move(0x6d1a88,0x6d1a88);
        else if(timeline.frame==21){
            const bool normal=signed_word(p.difficulty)<=2;extra.integers_6_to_19[4]=normal?20u:220u;extra.integers_6_to_19[5]=normal?80u:50u;extra.integers_6_to_19[6]=normal?200u:10u;
            for(unsigned i=0;i<4;++i){const auto color=rgb();effect_at(10,i>=2?c(0x6d19a0):0,i%2?c(0x6d19a0):0,color[0],color[1],color[2]);}
            enemy().word(0x2c,3);enemy().word(0x30,0);enemy().word(0x34,0);
        }else if(timeline.frame==51){
            switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c686c);break;case 1:copy_name(context.spell_name,name_6c6884);break;case 2:copy_name(context.spell_name,name_6c689c);break;case 3:copy_name(context.spell_name,name_6c68b4);break;}
            const auto hp=pick({{23000,23000,17000,17000,0,0}})*extra.boss_hp_multiplier;services.configure_phase(1,50,hp,35);enemy().word(8,0);const auto color=rgb();effect(color[0],color[1],color[2]);
        }else if(timeline.frame==60){p.reals[0]=c(0x6d1b60);extra.real_4=0;p.reals[1]=0;extended.reals_5_to_9[0]=0;context.extra_reals[0]=0;extended.reals_5_to_9[1]=0;context.extra_reals[1]=0;extended.reals_5_to_9[2]=0;
            extra.reals_10_to_19[0]=0;extra.reals_10_to_19[1]=0;extra.reals_10_to_19[2]=0;extra.reals_10_to_19[3]=0;extra.reals_10_to_19[4]=c(0x6d1af8);real20=c(0x6d1b60);
        }else if(signed_word(timeline.frame)>=61){
            auto value=math.sine(extra.real_4);p.reals[0]=m.store(m.add(m.add(m.mul(value,m.c(0x6d1d28)),m.c(0x6a7708)),m.q(p.reals[0])));
            value=math.sine(extended.reals_5_to_9[1]);context.extra_reals[0]=m.store(m.mul(value,m.c(0x6d1ab0)));extended.reals_5_to_9[1]=m.store(m.add(m.q(extended.reals_5_to_9[1]),m.c(0x6d3160)));
            value=math.sine(extended.reals_5_to_9[2]);context.extra_reals[1]=m.store(m.mul(value,m.c(0x6d1a90)));extended.reals_5_to_9[2]=m.store(m.add(m.q(extended.reals_5_to_9[2]),m.c(0x6d3158)));
            extra.reals_10_to_19[4]=m.store(m.mul(m.q(extra.reals_10_to_19[4]),m.c(0x6d3150)));
            if(p.difficulty<=3){
                const auto difficulty=p.difficulty;
                value=math.sine(extended.reals_5_to_9[0]);p.reals[1]=m.store(m.mul(value,m.c(difficulty==0?0x6d1ab0:difficulty==1?0x6d1bc0:0x6b25b8)));
                extended.reals_5_to_9[0]=m.store(m.add(m.q(extended.reals_5_to_9[0]),m.c(0x6d1950)));
                value=math.sine(extra.reals_10_to_19[1]);
                if(difficulty<=1)extra.reals_10_to_19[0]=m.store(m.mul(value,m.c(difficulty==0?0x6d1bb0:0x6d2b98)));
                else extra.reals_10_to_19[0]=m.store(m.add(m.add(m.mul(value,m.c(difficulty==2?0x6d1cf8:0x6d19e0)),m.c(difficulty==2?0x6d21e0:0x6d1b48)),m.q(extra.reals_10_to_19[0])));
                extra.reals_10_to_19[1]=m.store(m.add(m.q(extra.reals_10_to_19[1]),m.c(difficulty<=1?0x6d3148:0x6d3138)));
                value=math.sine(extra.reals_10_to_19[3]);extra.reals_10_to_19[2]=m.store(m.add(m.add(m.mul(value,m.c(difficulty%2?0x6d1b90:0x6d1a40)),m.c(difficulty==0?0x6d2740:difficulty==1?0x6d2700:difficulty==2?0x6d1a88:0x6d1ae8)),m.q(extra.reals_10_to_19[4])));
                extra.reals_10_to_19[3]=m.store(m.add(m.q(extra.reals_10_to_19[3]),m.c(difficulty==0?0x6d3140:difficulty==1?0x6d2a58:0x6d1b68)));
                if(difficulty==3&&remainder(timeline.frame,90)==0){emit({10,0,{{255,16,0}},2,255,1,41},enemy().quad(0x38),enemy().quad(0x40),7,c(0x6d2970),c(0x6d2488),real20,1,0);real20=m.store(m.sub(m.q(real20),m.c(0x6d1920)));}
            }
            if(timeline.frame==61){
                for(std::uint32_t i=0;i<=4;++i)for(std::uint32_t j=0;j<=6;++j)for(std::uint32_t k=0;k<6;++k){
                    const auto base_x=m.store(m.mul(m.sub(m.c(0x6d1ad8),m.i(j)),m.c(0x6d1d88)));const auto theta=m.store(m.mul(m.i(k),m.c(0x6d2da0)));
                    auto v=math.cosine(theta);const auto x=m.store(m.add(m.mul(v,m.c(0x6d1d00)),m.q(base_x)));
                    const auto base_y=m.store(m.i(i*50u));v=math.sine(theta);const auto y=m.store(m.add(m.mul(v,m.c(0x6d1cc0)),m.q(base_y)));
                    v=math.cosine(p.reals[0]);const auto dx=m.store(m.sub(m.q(x),m.add(m.mul(v,m.q(p.reals[1])),m.c(0x6d1a60))));
                    v=math.sine(p.reals[0]);const auto dy=m.store(m.sub(m.q(y),m.add(m.mul(v,m.q(p.reals[1])),m.c(0x6d1af0))));
                    auto angle=m.store(math.atan2(dy,dx));const auto square=m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy))));const auto distance=m.store(finite_square_root(square,bits));
                    for(unsigned side=0;side<2;++side){
                        if(side)angle=m.store(m.add(m.q(angle),m.c(0x6d1860)));
                        const auto index=allocate_projectile(projectiles,{18,0,rgb(),2,255,1,41});if(index==-1)continue;auto& bullet=projectiles.bullets[static_cast<std::size_t>(index)];
                        bullet.word(8,48);bullet.word(0x78,side?0xffffffffu:1u);bullet.quad(0xa0,angle);bullet.quad(0xa8,distance);
                        auto origin=m.store(m.add(m.c(0x6d1a60),m.q(context.extra_reals[0])));v=math.cosine(p.reals[0]);auto partial=m.store(m.add(m.mul(m.mul(m.i(bullet.word(0x78)),m.q(p.reals[1])),v),m.q(origin)));
                        auto argument=m.store(m.add(m.q(bullet.quad(0xa0)),m.q(extra.reals_10_to_19[0])));v=math.cosine(argument);bullet.quad(0x40,m.store(m.add(m.mul(m.add(m.q(bullet.quad(0xa8)),m.q(extra.reals_10_to_19[2])),v),m.q(partial))));
                        origin=m.store(m.add(m.c(0x6d1af0),m.q(context.extra_reals[1])));v=math.sine(p.reals[0]);partial=m.store(m.add(m.mul(m.mul(m.i(bullet.word(0x78)),m.q(p.reals[1])),v),m.q(origin)));
                        argument=m.store(m.add(m.q(bullet.quad(0xa0)),m.q(extra.reals_10_to_19[0])));v=math.sine(argument);bullet.quad(0x48,m.store(m.add(m.mul(m.add(m.q(bullet.quad(0xa8)),m.q(extra.reals_10_to_19[2])),v),m.q(partial))));bullet.word(0x34,1);
                    }
                }
            }
        }break;
    case 6:
        if(timeline.frame==1)move(0x6d1b38,0x6d1a88);
        else if(timeline.frame==51){services.configure_phase(0,30,extra.boss_hp_multiplier*12000u,0xfffffff6u);enemy().word(8,58);p.integers[0]=10;p.integers[1]=60;p.integers[2]=pick({{17,15,13,12,0,0}});p.reals[0]=c(0x6d1f50);effect(128,128,128);}
        break;
    case 7:
        if(timeline.frame==1)move(0x6d1f08,0x6b25b8);
        else if(timeline.frame==90){p.reals[1]=m.store(m.i(pick({{160,150,140,130,0,0}})));effect_at(10,m.store(m.sub(m.c(0x6d1a60),m.q(p.reals[1]))),0,255,255,255);effect_at(10,m.store(m.add(m.c(0x6d1a60),m.q(p.reals[1]))),0,255,255,255);enemy().word(0x2c,3);enemy().word(0x30,0);enemy().word(0x34,0);}
        else if(timeline.frame==101){
            switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c68d0);break;case 1:copy_name(context.spell_name,name_6c68e8);break;case 2:copy_name(context.spell_name,name_6c6900);break;case 3:copy_name(context.spell_name,name_6c6918);break;}
            services.configure_phase(1,50,26000,39);enemy().word(8,0);enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d1f08));p.integers[0]=pick({{160,100,80,65,0,0}});p.integers[1]=pick({{8,10,13,13,0,0}});p.integers[3]=0;
        }else if(signed_word(timeline.frame)>=130){
            if(remainder(timeline.frame,5)==0)for(unsigned side=0;side<2;++side)for(unsigned position=0;position<3;++position){
                auto x=side?m.add(m.c(0x6d1a60),m.q(p.reals[1])):m.sub(m.c(0x6d1a60),m.q(p.reals[1]));if(position==0)x=m.sub(x,m.c(0x6b25f8));else if(position==2)x=m.add(x,m.c(0x6b25f8));
                emit({14,2,{{255,255,255}},0,255,0,41},m.store(x),c(0x6d1a30),1,0,c(0x6d2cb8),c(0x6d1b60),1,0);
            }
            if(remainder(timeline.frame,20)==0)p.integers[2]=random.integer_bits(255,precision,true);
            if(timeline.frame==200){
                p.reals[0]=c(0x6d3130);
                for(unsigned side=0;side<2;++side){
                    const auto index=allocate_projectile(projectiles,{19,1,{{255,100,0}},2,255,0,41});if(index==-1)continue;auto& bullet=projectiles.bullets[static_cast<std::size_t>(index)];
                    bullet.word(8,49);bullet.quad(0xa0,side?0:c(0x6d1de0));bullet.quad(0xa8,c(side?0x6d3128:0x6a7708));bullet.quad(0xb0,c(0x6d1de0));bullet.quad(0xb8,c(side?0x6d2578:0x6d2800));
                    bullet.quad(0xc0,m.store(m.i(pick({{144,184,224,224,0,0}}))));bullet.quad(0xc8,m.store(m.i(pick({{80,90,100,100,0,0}}))));bullet.quad(0x40,m.store(side?m.add(m.c(0x6d1a60),m.q(p.reals[1])):m.sub(m.c(0x6d1a60),m.q(p.reals[1]))));
                    auto value=math.sine(bullet.quad(0xa0));const auto partial=m.store(m.add(m.mul(value,m.q(bullet.quad(0xc8))),m.q(bullet.quad(0xc0))));value=math.sine(bullet.quad(0xb0));bullet.quad(0x48,m.store(m.add(m.add(m.mul(value,m.q(bullet.quad(0xc8))),m.q(partial)),m.q(p.reals[0]))));bullet.word(0x34,1);
                }
            }
            p.reals[0]=m.store(m.mul(m.q(p.reals[0]),m.c(0x6d1c38)));
            if(signed_word(timeline.frame)>signed_word(p.integers[0]+40u)){
                if(p.integers[3]==0&&remainder(timeline.frame,p.integers[0])==0){p.integers[3]=1;effect(255,255,255);}
                else if(remainder(timeline.frame,p.integers[0])==40){const auto spacing=stage3_initial_turn_rate(p.integers[1],bits);emit({12,3,{{0,0,0}},0,255,1,41},enemy().quad(0x38),enemy().quad(0x40),p.integers[1],spacing,c(0x6d1ad0),0,0,50);}
            }
        }break;
    case 8:if(timeline.frame==32)services.retire_boss(0);else if(timeline.frame==196)services.dialog(6);break;
    case 9:if(timeline.frame==1)services.award_stage_clear();else if(timeline.frame==200){++death.stage;timeline.part=0;timeline.subphase=0;timeline.frame=0;p.game_frame=0;services.change_stage_resources(death.stage);}break;
    }
}
}
