// 四面后半段676B70..6772E6；池满仍推进横向出敌游标，保留选择器调用顺序。
#pragma once
#include "core/stages/Stage4ThirdArithmetic.hpp"
#include "core/stages/Stage1Timeline.hpp"
#include "core/math/GameMath.hpp"
namespace kaikeidou {
inline void stage4_third_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,
    PatternParameters& p,Random&,GameMath&,RandomPrecision precision){
    using namespace stage4_third_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};
    const auto pick=[&](std::array<std::uint32_t,6> values){return stage4_third_pick_integer(p.difficulty,values);};
    const auto spawn=[&]()->Enemy*{const auto index=allocate_enemy(enemies);return index<0?nullptr:&enemies.enemies[static_cast<std::size_t>(index)];};
    const auto header=[](Enemy& e,std::uint32_t kind,std::uint32_t hp,std::uint32_t items,std::uint32_t behavior){e.word(4,kind);e.word(0x18,hp);e.word(0x14,hp);e.word(0x1c,items);e.word(8,behavior);};
    const auto frame=signed_word(timeline.frame);
    if(timeline.frame==50){
        for(std::int32_t side=-1;side<=1;side+=2)if(auto* e=spawn()){
            header(*e,5,8000,1,69);e->quad(0x38,m.store(m.i(static_cast<std::uint32_t>(side*185+192))));e->quad(0x40,constant(0x6d1b58));
            e->quad(0xa8,m.store(m.add(m.mul(m.i(static_cast<std::uint32_t>(side)),m.c(0x6d1b60)),m.c(0x6d1b60))));
            e->word(0x80,pick({{150,130,110,90,0,0}}));e->word(12,side==-1?0:pick({{75,65,55,45,0,0}}));
        }
    }else if(timeline.frame==200||timeline.frame==400){
        const bool first=timeline.frame==200;
        for(std::uint32_t i=0;i<12;++i)if(auto* e=spawn()){
            header(*e,1,160,first?1u:2u,70);e->quad(0x38,constant(first?0x6d1b58:0x6d1bb8));e->quad(0x40,first?0:constant(0x6b25f8));
            const auto offset=m.mul(m.i(i),m.c(0x6d1bb0));
            e->quad(0xa8,m.store(first?m.add(offset,m.c(0x6d1880)):m.sub(m.c(0x6d1ee8),offset)));
            e->quad(0xb0,m.store(m.add(m.mul(m.i(i),m.c(0x6d1e00)),m.c(0x6d1b70))));
            e->quad(0xb8,m.store(m.add(m.mul(m.i(i),m.c(first?0x6d1900:0x6d24f8)),m.c(0x6d19c0))));
            e->word(0x80,pick({{150,130,110,90,0,0}}));e->word(0x84,pick({{1u-i%2,1,1,1,0,0}}));
        }
    }else if(frame>=1000&&frame<=1600){
        if(timeline.frame==1000){p.reals[0]=constant(0x6d1fa8);p.reals[1]=constant(0x6d1a60);}
        if(frame%40==0){
            if(auto* e=spawn()){
                header(*e,0,200,2,71);e->quad(0x38,p.reals[1]);e->quad(0x40,constant(0x6d19f0));e->quad(0xa8,constant(0x6d1910));
                e->word(0x80,pick({{90,85,70,60,0,0}}));
            }
            p.reals[1]=m.store(m.add(m.q(p.reals[1]),m.c(0x6d1878)));
            if(crt_sse2::real(p.reals[1])>crt_sse2::real(constant(0x6d22c8)))p.reals[1]=m.store(m.sub(m.q(p.reals[1]),m.c(0x6d18d8)));
        }
    }else if(timeline.frame==1800){
        p.integers[3]=60;p.integers[4]=90;
        for(std::int32_t side=-1;side<=1;side+=2)if(auto* e=spawn()){
            header(*e,side==-1?3u:4u,6000,3,72);e->quad(0x38,constant(0x6d1a60));e->quad(0x40,constant(0x6d1b58));e->quad(0xa8,constant(0x6d1de0));
            e->quad(0xb0,m.store(m.mul(m.i(static_cast<std::uint32_t>(side)),m.c(0x6d1a08))));e->word(0x80,pick({{44,36,28,24,0,0}}));e->word(0x84,static_cast<std::uint32_t>(side));
        }
    }else if(timeline.frame==3500)clear_projectiles(projectiles,100);
    else if(timeline.frame==3540){++timeline.part;timeline.subphase=0;timeline.frame=0;p.game_frame=0;}
}
}
