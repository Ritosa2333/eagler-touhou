// 三面开场679780..67AA83，保留失败分配前置槽、实时参数重读与原整数回绕。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/math/GameMath.hpp"
#include "core/math/FiniteDivide.hpp"
#include "core/stages/Stage3FirstConstants.hpp"
#include "core/stages/Stage3InitialMath.hpp"
#include "core/stages/Stage3FirstArithmetic.hpp"
namespace kaikeidou {
inline void stage3_first_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,
    PatternParameters& p,BossStageContext& context,Random& random,GameMath& math,RandomPrecision precision,StageServices& services){
    using namespace binary_float;using stage3_first_detail::constant;using stage3_first_detail::signed_word;
    const auto bits=static_cast<unsigned>(precision);const stage_detail::Arithmetic m{bits};
    const auto c=[](std::uint32_t address){return constant(address);};const auto n=[&](std::uint32_t address){return binary64(c(address));};
    const auto frame=[&](){return signed_word(timeline.frame);};
    const auto pick=[&](std::array<std::uint32_t,6> values){return values[p.difficulty<6?p.difficulty:3];};
    const auto real=[&](std::array<std::uint64_t,6> values){return values[p.difficulty<6?p.difficulty:3];};
    const auto spawn=[&]()->Enemy*{const auto slot=allocate_enemy(enemies);return slot==-1?nullptr:&enemies.enemies[static_cast<std::size_t>(slot)];};
    const auto header=[](Enemy& e,std::uint32_t kind,std::uint32_t hp,std::uint32_t items,std::uint32_t behavior){e.word(4,kind);e.word(0x18,hp);e.word(0x14,hp);e.word(0x1c,items);e.word(8,behavior);};
    if(timeline.frame==1)services.music(5,1);
    else if(timeline.frame==20){
        for(std::uint32_t i=0;i<=9;++i)if(auto* e=spawn()){
            header(*e,i<=1?2u:i<=3?1u:0u,80,1,43);e->quad(0x38,c(0x6d1a60));e->quad(0x40,c(0x6d19f0));
            const auto half=stage3_first_detail::floor_half(i),twice=m.add(half,half);
            e->quad(0xa8,m.store(i%2?m.sub(n(0x6d2658),twice):m.add(twice,n(0x6d26a0))));e->quad(0xb0,e->quad(0xa8));
        }
    }else if(timeline.frame==140)services.title();
    else if(timeline.frame==400||timeline.frame==520){
        const bool right=timeline.frame==520;
        for(std::uint32_t i=0;i<=9;++i)if(auto* e=spawn()){
            header(*e,i<=1?2u:i<=3?1u:0u,80,1,44);e->quad(0x38,c(right?0x6d22c8:0x6d1e48));e->quad(0x40,c(0x6d19f0));
            e->quad(0xa8,m.store(m.i(right?0u-(9u-i)*2u:(9u-i)*2u)));e->quad(0xb0,m.store(m.mul(binary64(e->quad(0xa8)),n(0x6d1920))));e->word(0x80,pick({{5,4,4,4,0,0}}));
        }
    }else if(timeline.frame==640||timeline.frame==720||timeline.frame==1200||timeline.frame==1320){
        const auto count=frame()<=720?4u:8u;const auto period=frame()<=720?pick({{25,14,10,8,0,0}}):pick({{35,25,16,12,0,0}});
        for(std::uint32_t i=0;i<count;++i){
            const auto kind=timeline.frame==640||timeline.frame==1200?2u:0u;auto* allocated=spawn();if(allocated)allocated->word(4,kind);auto& e=allocated?*allocated:context.enemy_prefix;
            e.word(0x18,50);e.word(0x14,50);e.word(0x1c,1);e.quad(0xa8,c(0x6d2670));e.quad(0xb0,c(0x6d1d78));
            e.quad(0xb8,m.store(divide(m.mul(m.i(i),n(0x6d2208)),m.i(count),bits)));
            const auto cosine=math.cosine(e.quad(0xb8));e.quad(0x38,m.store(m.add(m.mul(cosine,n(0x6b25b8)),binary64(e.quad(0xa8)))));
            const auto sine=math.sine(e.quad(0xb8));e.quad(0x40,m.store(m.add(m.mul(sine,n(0x6b25b8)),binary64(e.quad(0xb0)))));e.word(0x80,period);e.word(8,45);
        }
    }else if(timeline.frame==900){
        if(auto* e=spawn()){
            header(*e,5,2800,1,46);e->quad(0x38,c(0x6d1a60));e->quad(0x40,c(0x6d19f0));e->quad(0x50,c(0x6d1b88));e->quad(0xa8,0);
            e->quad(0xb0,real({{c(0x6d2688),c(0x6d2690),c(0x6d2698),c(0x6d19c8),0,0}}));e->quad(0xb8,real({{c(0x6d2488),c(0x6d1e40),c(0x6d2548),c(0x6d1ce0),0,0}}));
            e->word(0x80,pick({{6,5,4,3,0,0}}));e->word(0x84,0);e->word(0x88,pick({{6,7,8,9,0,0}}));e->word(0x8c,pick({{60,50,40,30,0,0}}));
        }
    }else if(frame()>=1480&&frame()<=1620){
        if(frame()%20==0){
            if(timeline.frame==1480){p.integers[0]=120;p.integers[1]=200;p.integers[2]=0;}
            if(auto* e=spawn()){
                header(*e,0,60,1,47);e->quad(0x38,m.store(m.i(p.integers[2]*300u+42u)));e->quad(0x40,c(0x6d19f8));e->quad(0x48,m.store(m.i(2u-p.integers[2]*4u)));
                e->word(0x80,pick({{60,50,40,30,0,0}}));e->word(0x84,random.integer_bits(e->word(0x80)-1u,precision,true));p.integers[2]=1u-p.integers[2];
            }
        }
    }else if(frame()>=2000&&frame()<=2300){
        if(frame()%15==0){
            if(timeline.frame==1480)p.integers[2]=0;
            if(auto* e=spawn()){
                e->word(4,p.integers[2]);e->word(0x18,signed_word(p.integers[2])<3?150u:300u);e->word(0x14,e->word(0x18));e->word(0x1c,signed_word(p.integers[2])<3?2u:1u);e->word(8,48);
                e->quad(0x38,c(0x6d1cb0));e->quad(0x40,c(0x6d19f0));e->quad(0x48,m.store(m.sub(n(0x6d1ee8),m.mul(m.i(static_cast<std::uint32_t>(frame()%17)),n(0x6d24f8)))));
                e->quad(0x50,m.store(m.add(m.mul(m.i(static_cast<std::uint32_t>(frame()%6)),n(0x6d1ad0)),n(0x6d1a00))));++p.integers[2];if(p.integers[2]==5)p.integers[2]=0;
            }
        }
    }else if(timeline.frame==2500){
        for(std::uint32_t i=0;i<=30;++i)if(auto* e=spawn()){
            const auto kind=i==0?5u:i<=4?4u:i<=8?3u:i<=12?2u:i<=22?1u:0u;const auto hp=i==0?2000u:i<=8?200u:100u;
            e->word(4,kind);e->word(0x18,hp);e->word(0x14,hp);e->word(0x1c,i<=8?1u:3u);
            if(i){const std::array<std::array<std::uint32_t,3>,5> colors{{{{255,0,0}},{{0,0,255}},{{0,255,0}},{{0,255,255}},{{255,255,0}}}};const auto& color=colors[kind];e->word(0x8c,color[0]);e->word(0x90,color[1]);e->word(0x94,color[2]);}
            e->word(8,49);e->quad(0x38,c(0x6d1a60));e->quad(0x40,i?m.store(m.sub(n(0x6d2680),stage3_first_detail::ceil_half(i))):c(0x6d19f0));e->quad(0x50,c(0x6d2410));
            p.integers[0]=pick({{80,45,28,20,0,0}});
            if(!i)e->quad(0xa8,0);
            else if(i%2){e->quad(0xa8,m.store(m.mul(m.sub(n(0x6aca58),stage3_first_detail::floor_half(i)),n(0x6d19c0))));e->word(0x80,0);e->quad(0xb8,m.store(m.add(n(0x6d1de0),binary64(e->quad(0xa8)))));}
            else{e->quad(0xa8,m.store(m.mul(stage3_first_detail::floor_half(i),n(0x6d19c0))));e->word(0x80,static_cast<std::uint32_t>(signed_word(p.integers[0])/2));e->quad(0xb8,m.store(m.add(n(0x6d1de0),binary64(e->quad(0xa8)))));}
            e->quad(0xb0,e->quad(0xa8));
        }
    }else if(timeline.frame==3000){
        if(auto* e=spawn()){
            header(*e,5,11000,1,50);e->quad(0x38,c(0x6d23c8));e->quad(0x40,c(0x6d1b58));e->word(0x80,pick({{11,9,7,5,0,0}}));e->word(0x84,255);e->word(0x88,pick({{4,7,11,15,0,0}}));
            e->quad(0xa8,stage3_initial_turn_rate(e->word(0x88),bits));e->quad(0xb0,real({{c(0x6d1ad0),c(0x6d1948),c(0x6d1b18),c(0x6d1ce0),0,0}}));e->quad(0xb8,0);e->quad(0xc0,c(0x6d1960));
        }
    }else if(timeline.frame==3800){clear_projectiles(projectiles,100);++timeline.part;timeline.subphase=0;timeline.frame=0;p.game_frame=0;}
}
}
