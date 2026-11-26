// 二面part1完整67BF50..67C8C2，保留Boss槽缓存、难度重读及交替弹幕顺序。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/math/FiniteDivide.hpp"
#include "core/stages/Stage2BossMath.hpp"
namespace kaikeidou {
namespace stage2_second_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:std::int64_t(v);}
template<class T> inline T select(std::uint32_t difficulty,const std::array<T,6>& values){return values[difficulty<6?difficulty:3];}
inline std::int64_t remainder(std::uint32_t dividend,std::uint32_t divisor){
    if(!divisor)throw std::domain_error("Original 67C40A integer division by zero");
    if(dividend==0x80000000u&&divisor==0xffffffffu)throw std::domain_error("Original 67C40A signed quotient overflow");
    return signed_word(dividend)%signed_word(divisor);
}
}
inline void stage2_second_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,
    EffectPool& effects,PatternParameters& p,BossPhaseContext& phase,BossStageContext& context,
    Random& random,RandomPrecision precision,BossStageServices& services){
    using namespace binary_float;using stage2_second_detail::signed_word;
    const auto bits=static_cast<unsigned>(precision);const stage_detail::Arithmetic m{bits};
    auto boss=phase.boss_index;
    const auto enemy=[&]() -> Enemy& {return boss==0xffffffffu?context.enemy_prefix:enemies.enemies.at(boss);};
    const auto pick=[&](std::array<std::uint32_t,6> values){return stage2_second_detail::select(p.difficulty,values);};
    const auto real=[&](std::array<std::uint64_t,6> values){return stage2_second_detail::select(p.difficulty,values);};
    const auto q=[](std::uint64_t value){return binary64(value);};
    switch(timeline.subphase){
    case 0:
        if(timeline.frame==1){boss=static_cast<std::uint32_t>(services.allocate_boss(1));
            enemy().quad(0x38,0x4068000000000000ull);enemy().quad(0x40,0xc03e000000000000ull);
            enemy().word(8,29);enemy().word(0x80,0);
        }else if(timeline.frame==10)services.begin_boss_effect();
        else if(timeline.frame==20){const auto y=enemy().quad(0x40),x=enemy().quad(0x38);
            const auto color=services.color(0,255,255);allocate_effect(effects,projectiles.sound_triggers,{12,color,x,y});
        }else if(timeline.frame==60){timeline.subphase=1;timeline.frame=0;p.game_frame=0;}
        break;
    case 1:
        if(timeline.frame==1){enemy().word(0x80,1);services.configure_phase(0,30,9000,0xfffffff6u);
            enemy().quad(0xb0,0xbff921fb54442d28ull);enemy().word(0x84,0);enemy().word(0x88,0);
            p.integers[1]=pick({{50,30,25,20,0,0}});p.reals[0]=0x4008000000000000ull;p.reals[1]=0x3fe6666666666666ull;
            const auto numerator=real({{0x4030000000000000ull,0x402c000000000000ull,0x4010000000000000ull,0x400599999999999aull,0,0}});
            context.extra_reals[0]=stage2_boss_turn_rate(numerator,p.integers[1],bits);
            context.extra_reals[1]=real({{0x4000cccccccccccdull,0x4003333333333333ull,0x400599999999999aull,0x4008000000000000ull,0,0}});
        }break;
    case 2:
        if(timeline.frame==32){
            constexpr std::array<std::uint8_t,19> name{{0xb4,0xc5,0xb9,0xd6,0xa1,0xb8,0xb6,0xed,0xc0,0xd5,0xb8,0xd4,0xe4,0xf6,0xce,0xd0,0xa1,0xb9,0}};
            if(p.difficulty<2){constexpr std::array<std::uint8_t,21> easy{{0xb4,0xc5,0xb7,0xfb,0xa1,0xb8,0xbf,0xb4,0xb2,0xbb,0xbc,0xfb,0xb5,0xc4,0xe4,0xf6,0xce,0xd0,0xa1,0xb9,0}};context.spell_name[0]=easy[0];for(unsigned i=1;i<21;i+=4)std::memcpy(context.spell_name.data()+i,easy.data()+i,4);}
            else if(p.difficulty<4){for(unsigned i=0;i<16;i+=4)std::memcpy(context.spell_name.data()+i,name.data()+i,4);std::memcpy(context.spell_name.data()+16,name.data()+16,2);context.spell_name[18]=0;}
            const auto hp=pick({{12000,12000,11500,11500,0,0}});services.configure_phase(1,40,hp,11);
            enemy().word(8,14);enemy().quad(0xa8,0x4068000000000000ull);enemy().quad(0xb0,0x4057000000000000ull);
            p.integers[0]=0;p.integers[1]=pick({{100,80,60,50,0,0}});p.integers[2]=pick({{9,10,11,12,0,0}});
            p.reals[0]=real({{0x4052c00000000000ull,0x4050400000000000ull,0x404e000000000000ull,0x404b800000000000ull,0,0}});
            p.reals[1]=real({{0x3ff4cccccccccccdull,0x3ff8000000000000ull,0x3ffccccccccccccdull,0x4000000000000000ull,0,0}});
            context.extra_reals[0]=real({{0x3f554c985f06f694ull,0x3f5d7dbf487fcb92ull,0x3f5e4f765fd8adacull,0x3f6205bc01a36e2full,0,0}});
            p.integers[3]=p.difficulty==3?10u:8u;context.extra_reals[1]=m.store(m.i(p.difficulty==3?60u:80u));
        }else if(signed_word(timeline.frame)>=60){
            if(stage2_second_detail::remainder(timeline.frame,p.integers[1])==0){
                const auto offset=m.store(m.sub(divide(random.real_number(0x4082c00000000000ull,precision),m.n(10.),bits),m.n(50.)));
                const bool negative=p.integers[0]==0;p.integers[0]=1u-p.integers[0];
                const auto shoot=[&](bool horizontal,bool reverse,std::uint64_t shift){
                    for(std::uint32_t i=0xffffffffu;signed_word(i)<=signed_word(p.integers[horizontal?3:2]);++i){
                        const auto slot=allocate_projectile(projectiles,{0,0,reverse?std::array<std::uint32_t,3>{{0,16,255}}:std::array<std::uint32_t,3>{{235,0,0}},0,255,1,41});
                        if(slot==-1)continue;auto& bullet=projectiles.bullets[static_cast<std::size_t>(slot)];
                        if(horizontal){bullet.quad(0x40,m.store(m.add(m.mul(m.i(i),q(context.extra_reals[1])),q(shift))));bullet.quad(0x48,0xc018000000000000ull);}
                        else{bullet.quad(0x40,0xc018000000000000ull);bullet.quad(0x48,m.store(m.add(m.mul(m.i(i),q(p.reals[0])),q(shift))));}
                        bullet.quad(0xa0,context.extra_reals[0]^(reverse?0x8000000000000000ull:0ull));bullet.word(8,horizontal?23u:22u);
                    }
                };
                shoot(false,negative,offset);
                if(signed_word(p.difficulty)>=2){const auto x=m.store(m.sub(divide(random.real_number(0x407a400000000000ull,precision),m.n(10.),bits),m.n(21.)));shoot(true,p.integers[0]==0,x);}
            }
            if(signed_word(timeline.frame)%220==210){
                enemy().quad(0xa8,m.store(m.sub(m.add(random.real_number(0x4064000000000000ull,precision),m.n(192.)),m.n(80.))));
                enemy().quad(0xb0,m.store(m.sub(m.add(random.real_number(0x404e000000000000ull,precision),m.n(140.)),m.n(30.))));
            }
        }break;
    case 3:
        if(timeline.frame==32){const auto x=crt_sse2::real(enemy().quad(0x38));enemy().quad(0xa8,x<192.?0xc059000000000000ull:0x407e400000000000ull);
            enemy().quad(0xb0,0xc064000000000000ull);enemy().quad(0xb8,0x4046800000000000ull);enemy().quad(0xc0,0x4041800000000000ull);
            enemy().word(0xc,0);enemy().word(8,17);
        }else if(timeline.frame==114)services.retire_boss(2);
        else if(timeline.frame==128){++timeline.part;timeline.subphase=0;timeline.frame=0;p.game_frame=0;}
        break;
    default:break;
    }
}
}
