// 三面后半段678C00..67935D；保留随机中间落点、池满翻转和环形敌参数缓存。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/math/GameMath.hpp"
#include "core/math/FiniteDivide.hpp"
#include "core/stages/Stage3ThirdConstants.hpp"
namespace kaikeidou {
inline void stage3_third_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,
    PatternParameters& p,Random& random,GameMath& math,RandomPrecision precision){
    using namespace binary_float;using stage3_third_detail::constant;
    const auto bits=static_cast<unsigned>(precision);const stage_detail::Arithmetic m{bits};
    const auto n=[](std::uint32_t a){return binary64(constant(a));};
    const auto frame=[&](){return timeline.frame>>31?std::int64_t(timeline.frame)-0x100000000ll:std::int64_t(timeline.frame);};
    const auto pick=[&](std::array<std::uint32_t,6> values){return values[p.difficulty<6?p.difficulty:3];};
    const auto spawn=[&]()->Enemy*{const auto i=allocate_enemy(enemies);return i==-1?nullptr:&enemies.enemies[static_cast<std::size_t>(i)];};
    const auto header=[](Enemy& e,std::uint32_t kind,std::uint32_t hp,std::uint32_t items){e.word(4,kind);e.word(0x18,hp);e.word(0x14,hp);e.word(0x1c,items);};
    if(frame()>=60&&frame()<=250){
        if(frame()%30==0)if(auto* e=spawn()){
            header(*e,1,80,2);e->word(8,53);
            const auto cosine=math.cosine(m.store(m.mul(m.i(timeline.frame),n(0x6d1a00))));
            e->quad(0x38,m.store(m.add(m.mul(cosine,n(0x6d2240)),n(0x6d1a60))));e->quad(0x40,constant(0x6d19f0));
            e->quad(0xa8,constant(!(crt_sse2::real(e->quad(0x38))<=192.)?0x6d2488:0x6d1fc0));
            e->quad(0xb0,constant(!(crt_sse2::real(e->quad(0x38))<=192.)?0x6d2448:0x6d1a18));
        }
    }else if(frame()>=400&&frame()<=700){
        if(timeline.frame==400)p.integers[2]=1;
        if(frame()%12==0){
            if(auto* e=spawn()){
                header(*e,p.integers[2],120,1);e->word(8,54);
                const auto angle=[&](){return m.store(m.mul(random.real_number(constant(0x6d1c60),precision),n(0x6d1950)));};
                const auto x=m.store(m.add(m.mul(math.sine(angle()),n(0x6b25b8)),n(0x6d1a60)));
                const auto xcos=math.cosine(angle());e->quad(0x38,m.store(m.add(m.mul(xcos,n(0x6d1aa8)),binary64(x))));e->quad(0x40,constant(0x6d19f0));
                const auto speed=m.store(m.add(m.mul(math.sine(angle()),n(0x6d1f50)),n(0x6d1ad8)));
                const auto speed_sine=math.sine(angle());e->quad(0xa8,m.store(m.add(m.mul(speed_sine,n(0x6d1868)),binary64(speed))));
                e->quad(0xb0,constant(p.integers[2]?0x6d1ce0:0x6d2218));
            }
            p.integers[2]=1u-p.integers[2];
        }
    }else if(timeline.frame==900||timeline.frame==1650||timeline.frame==1840){
        const bool first=timeline.frame==900;const std::uint32_t count=first?16u:18u;
        const auto period=first?pick({{52,39,30,25,0,0}}):pick({{64,47,36,25,0,0}});
        for(std::uint32_t i=0;i<count;++i)if(auto* e=spawn()){
            header(*e,first?(i%4?3u:4u):(i%3?1u:0u),first?500u:130u,first?1u:2u);
            e->quad(0xa8,constant(0x6d2670));e->quad(0xb0,constant(0x6d1d78));
            e->quad(0xb8,m.store(divide(m.mul(m.i(i),n(0x6d2208)),m.i(count),bits)));e->quad(0xc0,constant(0x6d1ff8));
            const auto cosine=math.cosine(e->quad(0xb8));e->quad(0x38,m.store(m.add(m.mul(cosine,n(0x6b25b8)),binary64(e->quad(0xa8)))));
            const auto sine=math.sine(e->quad(0xb8));e->quad(0x40,m.store(m.add(m.mul(sine,n(0x6b25b8)),binary64(e->quad(0xb0)))));
            e->word(0x80,period);e->word(8,45);
        }
    }else if(timeline.frame==2400)clear_projectiles(projectiles,100);
    else if(timeline.frame==2440){++timeline.part;timeline.subphase=0;timeline.frame=0;p.game_frame=0;}
}
}
