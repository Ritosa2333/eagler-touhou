// 原敌87..89；保留重复难度分支、两次独立选择器及每个服务后的坐标重读。
#pragma once
#include "core/stages/Stage5ThirdArithmetic.hpp"
#include "core/stages/Stage5ThirdEmitter.hpp"
#include "core/stages/StageResourceContext.hpp"
namespace kaikeidou {
inline void update_stage5_third_enemy(std::uint32_t behavior,Enemy& e,ProjectilePools& pool,const ObjectRecord<240>& player,
    PatternParameters& p,const StageResourceContext& background,Random& random,RandomPrecision precision,GameMath& math){
    using namespace stage5_third_detail;const Arithmetic m{static_cast<unsigned>(precision)};
    const auto q=[&](std::size_t at){return m.q(e.quad(at));};const auto put=[&](std::size_t at,binary_float::Number value){e.quad(at,m.store(value));};
    const auto age=[&](){return signed_word(e.word(12));};
    const auto emit=[&](std::uint32_t kind,std::int32_t group,std::array<std::uint32_t,3> color,std::uint32_t count,
        std::uint64_t spacing,std::uint64_t speed,std::uint64_t angle,std::uint32_t absolute){
        const auto y=e.quad(0x40),x=e.quad(0x38);
        stage5_third_emit(pool,{{kind,group,color,0,255,1,41},x,y,count,spacing,speed,angle,absolute,2},player,p,random,precision,math);
    };
    if(behavior==87||behavior==88){
        put(0x38,m.add(q(0x38),q(0xa8)));put(0x40,m.add(q(0x40),q(0xb0)));
        const auto x=crt_sse2::real(e.quad(0x38)),y=crt_sse2::real(e.quad(0x40));
        if(x< -30.||x>414.||y< -30.||y>478.){e.word(0,0);return;}
        const auto divisor=behavior==87?p.integers[2]:p.integers[2]<<1;
        if(remainder(e.word(12),divisor)!=0||age()>signed_word(p.integers[3]))return;
        if(behavior==87)emit(10,0,{{0,0,255}},4,constant(0x6d1b60),p.reals[1],constant(0x6d22d8),0);
        else{
            const auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
            const auto speed=p.reals[1];
            const auto denominator=stage5_third_pick_integer(p.difficulty,{{8,9,10,11,0,0}});
            const auto spacing=stage3_initial_turn_rate(denominator,m.precision);
            const auto count=stage5_third_pick_integer(p.difficulty,{{8,9,10,11,0,0}});
            emit(10,0,{{0,255,0}},count,spacing,speed,angle,1);
        }
        return;
    }
    if(behavior==89){
        const auto cosine=math.cosine(e.quad(0xa8));put(0x38,m.add(m.mul(cosine,q(0xb0)),m.q(background.reals[5])));
        const auto sine=math.sine(e.quad(0xa8));put(0x40,m.add(m.mul(sine,q(0xb0)),m.q(background.reals[6])));
        put(0xb0,m.sub(q(0xb0),m.c(0x6d2180)));
        if(crt_sse2::real(e.quad(0xb0))<=0.){e.word(0,0);return;}
        if(remainder(e.word(12),p.integers[2]+20u)!=0)return;
        const auto x=crt_sse2::real(e.quad(0x38)),y=crt_sse2::real(e.quad(0x40));
        if(x<0.||x>384.||y<0.||y>448.)return;
        if(p.difficulty==0){
            emit(4,1,{{255,255,0}},1,0,constant(0x6d1920),constant(0x6d1860),0);
            emit(10,2,{{255,0,0}},3,constant(0x6d2138),p.reals[1],0,0);
        }else if(p.difficulty==1){
            emit(4,1,{{255,255,0}},3,constant(0x6d2138),constant(0x6d1920),constant(0x6d1860),0);
            emit(10,2,{{255,0,0}},3,constant(0x6d2138),p.reals[1],0,0);
        }else if(p.difficulty==1){
            // 原63290B仍比较1；保留这个不可达分支对应的两次发射。
            emit(4,1,{{255,255,0}},3,constant(0x6d2138),constant(0x6d1920),constant(0x6d1860),0);
            emit(10,2,{{255,0,0}},5,constant(0x6d2618),p.reals[1],0,0);
        }else{
            emit(4,1,{{255,255,0}},5,constant(0x6d2618),constant(0x6d1920),0,0);
            emit(10,2,{{255,0,0}},5,constant(0x6d2618),p.reals[1],0,0);
        }
        return;
    }
    throw std::out_of_range("Stage5 third enemy outside87..89");
}
}
