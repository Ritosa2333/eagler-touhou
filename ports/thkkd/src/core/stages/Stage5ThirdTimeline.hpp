// 原674F90..67562C；背景先更新，池满后仍推进两个角度缓存。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/stages/StageResourceContext.hpp"
#include "core/math/GameMath.hpp"
#include "core/stages/Stage5ThirdArithmetic.hpp"
namespace kaikeidou {
inline void stage5_third_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,PatternParameters& p,
    BossStageContext& context,StageResourceContext& background,Random&,GameMath& math,RandomPrecision precision){
    using namespace stage5_third_detail;const Arithmetic m{static_cast<unsigned>(precision)};auto& bg=background.reals;
    const auto frame=[&](){return signed_word(timeline.frame);};
    auto sine=math.sine(bg[1]);bg[0]=m.store(m.add(m.add(m.mul(sine,m.c(0x6d1cf8)),m.c(0x6d19e0)),m.q(bg[0])));
    sine=math.sine(bg[3]);bg[2]=m.store(m.add(m.add(m.mul(sine,m.c(0x6d1cf8)),m.c(0x6d1aa0)),m.q(bg[2])));
    sine=math.sine(bg[2]);bg[4]=m.store(m.mul(sine,m.c(0x6d1aa8)));
    auto cosine=math.cosine(bg[0]);bg[5]=m.store(m.add(m.mul(cosine,m.q(bg[4])),m.c(0x6d1a60)));
    sine=math.sine(bg[0]);bg[6]=m.store(m.add(m.mul(sine,m.q(bg[4])),m.c(0x6d1af0)));
    const auto spawn=[&]()->Enemy*{const auto slot=allocate_enemy(enemies);return slot==-1?nullptr:&enemies.enemies[static_cast<std::size_t>(slot)];};
    const auto header=[&](Enemy& e,std::uint32_t kind,std::uint32_t hp_base,std::uint32_t items,std::uint32_t behavior){
        e.word(4,kind);e.word(0x18,p.difficulty*20u+hp_base);e.word(0x14,e.word(0x18));e.word(0x1c,items);e.word(8,behavior);
    };
    if(timeline.frame==1){
        p.reals[0]=constant(0x6d1de0);
        p.reals[1]=stage5_third_pick_real(p.difficulty,{{constant(0x6d1948),constant(0x6d1ce8),constant(0x6d25f0),constant(0x6d1ad8),0,0}});
        context.extra_reals[0]=constant(0x6d25e8);p.integers[0]=100;p.integers[1]=220;
        p.integers[2]=stage5_third_pick_integer(p.difficulty,{{50,45,40,35,0,0}});p.integers[3]=80;
    }else if((frame()>=100&&frame()<=800)||(frame()>=1200&&frame()<=1900)){
        const bool first=frame()<=800;
        if(remainder(timeline.frame,first?22u:26u)==0){
            for(std::uint32_t i=0;i<3;++i)if(auto* e=spawn()){
                header(*e,first?1u:2u,100,1,first?87u:88u);
                const auto angle=m.store(m.add(m.mul(m.i(i),m.c(0x6d2138)),m.q(p.reals[0])));
                cosine=math.cosine(angle);e->quad(0x38,m.store(m.add(m.mul(cosine,m.c(0x6d1e48)),m.q(bg[5]))));
                sine=math.sine(angle);e->quad(0x40,m.store(m.add(m.mul(sine,m.c(0x6d1e48)),m.q(bg[6]))));
                cosine=math.cosine(angle);e->quad(0xa8,m.store(m.mul(cosine,m.c(0x6d2180))));
                sine=math.sine(angle);e->quad(0xb0,m.store(m.mul(sine,m.c(0x6d2180))));
            }
            p.reals[0]=m.store(m.add(m.q(p.reals[0]),m.c(0x6d19c0)));
        }
    }else if(frame()>=2300&&frame()<=3200){
        if(remainder(timeline.frame,20)==0){
            if(auto* e=spawn()){
                header(*e,0,80,2,89);e->quad(0xa8,context.extra_reals[0]);e->quad(0xb0,constant(0x6d19d0));
                cosine=math.cosine(e->quad(0xa8));e->quad(0x38,m.store(m.add(m.mul(cosine,m.q(e->quad(0xb0))),m.q(bg[5]))));
                sine=math.sine(e->quad(0xa8));e->quad(0x40,m.store(m.add(m.mul(sine,m.q(e->quad(0xb0))),m.q(bg[6]))));
                e->word(12,timeline.frame);
            }
            context.extra_reals[0]=m.store(m.sub(m.q(context.extra_reals[0]),m.c(0x6d19c0)));
        }
    }else if(timeline.frame==3650)clear_projectiles(projectiles,100);
    else if(timeline.frame==3750){++timeline.part;timeline.subphase=0;timeline.frame=0;p.game_frame=0;}
}
}
