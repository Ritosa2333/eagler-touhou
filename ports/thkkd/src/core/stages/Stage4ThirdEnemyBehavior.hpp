// 原敌69..72；按原顺序保留signed除法、移动前缀、环上双点发射及原配置53。
#pragma once
#include "core/stages/Stage4ThirdArithmetic.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
inline void update_stage4_third_enemy(std::uint32_t behavior,Enemy& e,ProjectilePools& pool,std::uint64_t px,std::uint64_t py,
    PatternParameters& p,Random& random,RandomPrecision precision,GameMath& math){
    using namespace stage4_third_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};
    const auto q=[&](std::size_t offset){return m.q(e.quad(offset));};
    const auto put=[&](std::size_t offset,binary_float::Number value){e.quad(offset,m.store(value));};
    const auto age=[&](){return signed_word(e.word(12));};
    const auto emit=[&](std::uint32_t kind,std::array<std::uint32_t,3> color,std::uint32_t blend,std::uint64_t x,std::uint64_t y,
        std::uint32_t count,std::uint64_t spacing,std::uint64_t speed,std::uint64_t angle,std::uint32_t absolute,std::uint32_t config){
        emit_projectiles(pool,{{kind,0,color,blend,255,1,41},x,y,count,spacing,speed,angle,absolute,config},px,py,p,random,precision,math);
    };
    if(behavior==69){
        if(!remainder(e.word(12),e.word(0x80))){
            const auto angle=e.quad(0xa8);
            const auto spacing=stage4_third_pick_real(p.difficulty,{{0,constant(0x6d1b60),constant(0x6d22d8),constant(0x6d22d8),0,0}});
            const auto count=stage4_third_pick_integer(p.difficulty,{{1,3,5,5,0,0}});
            emit(2,{{210,16,230}},2,e.quad(0x38),e.quad(0x40),count,spacing,constant(0x6d1a00),angle,1,0);
        }
        if(signed_word(p.game_frame)>=3300){
            if(crt_sse2::real(e.quad(0x38))<192.){put(0x38,m.sub(q(0x38),m.c(0x6d19c0)));if(crt_sse2::real(e.quad(0x38))< -30.)e.word(0,0);}
            else{put(0x38,m.add(q(0x38),m.c(0x6d19c0)));if(crt_sse2::real(e.quad(0x38))>414.)e.word(0,0);}
        }else e.quad(0x40,stage4_third_approach(e.quad(0x40),py,constant(0x6b25b8),m.precision));
        return;
    }
    if(behavior==70){
        const auto cycle=remainder(e.word(12),e.word(0x80));
        if(cycle<=30){if(cycle%10==0&&e.word(0x84)==1)emit(10,{{255,255,0}},0,e.quad(0x38),e.quad(0x40),1,0,e.quad(0xb8),0,0,0);}
        else{put(0x38,m.add(q(0x38),q(0xa8)));put(0x40,m.add(q(0x40),q(0xb0)));}
        if(crt_sse2::real(e.quad(0x38))< -30.||crt_sse2::real(e.quad(0x38))>414.)e.word(0,0);
        return;
    }
    if(behavior==71){
        put(0x40,m.add(m.add(m.q(crt_sse2::bits(1.)),q(0xa8)),q(0x40)));put(0xa8,m.mul(q(0xa8),m.c(0x6d1b08)));
        if(!remainder(e.word(12),e.word(0x80))&&age()<418){
            const auto count=stage4_third_pick_integer(p.difficulty,{{1,2,3,3,0,0}});
            emit(0,{{255,0,0}},0,e.quad(0x38),e.quad(0x40),count,constant(0x6d2688),constant(0x6d1948),constant(0x6d1860),0,51);
        }
        if(crt_sse2::real(e.quad(0x40))>478.)e.word(0,0);
        return;
    }
    if(behavior==72){
        if(age()<=100){put(0x38,m.add(m.mul(q(0xb0),m.c(0x6d1a88)),q(0x38)));put(0x40,m.add(q(0x40),m.c(0x6d1b28)));}
        else if(age()>=160&&age()<=1200){
            if(!remainder(e.word(12),e.word(0x80))){
                for(unsigned side=0;side<2;++side){
                    const auto angle=[&](){return side?m.store(m.add(q(0xa8),m.c(0x6d1860))):e.quad(0xa8);};
                    const auto sine=math.sine(angle());const auto y=m.store(m.add(m.mul(sine,m.c(0x6d1878)),q(0x40)));
                    const auto cosine=math.cosine(angle());const auto x=m.store(m.add(m.mul(cosine,m.c(0x6d1878)),q(0x38)));
                    const auto blue=e.word(0x84)==0xffffffffu?255u:0u;const auto red=e.word(0x84)==0xffffffffu?0u:255u;
                    emit(1,{{red,255,blue}},0,x,y,1,0,constant(0x6d1a00),0,0,53);
                }
            }
            put(0xa8,m.add(q(0xa8),q(0xb0)));
        }else if(age()>=1300){put(0x38,m.add(m.mul(q(0xb0),m.c(0x6d1b90)),q(0x38)));put(0x40,m.sub(q(0x40),m.c(0x6d1a00)));if(crt_sse2::real(e.quad(0x40))< -30.)e.word(0,0);}
        return;
    }
    throw std::out_of_range("Stage4 third enemy outside69..72");
}
}
