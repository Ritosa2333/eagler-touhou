// 原弹57..64；动态参数EA、分配自别名、随机调用顺序及x87中间落点按原指令保留。
#pragma once
#include "core/stages/Stage4BossFinalArithmetic.hpp"
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
inline void update_stage4_boss_projectile_final(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,
    std::uint64_t px,std::uint64_t py,const PatternParameters& p,BossStageContext& boss,const StagePatternContext& extra,
    const Stage3BossTimelineContext& final,const Stage4PatternContext& stage4,const std::uint32_t& integer5,
    Random& random,RandomPrecision precision,GameMath& math){
    using namespace stage4_boss_final_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};final_projectile_detail::Record b{pool,index};
    const auto q=[&](std::size_t at){return m.q(b.quad(at));};const auto put=[&](std::size_t at,binary_float::Number value){b.quad(at,m.store(value));};
    const auto age=[&](){return signed_word(b.word(12));};
    const auto colors=[&](){
        const auto blue=integer_address(0xa78588u+b.word(0x7c)*40u,p,extra,stage4,integer5);
        const auto green=integer_address(0xa78584u+b.word(0x7c)*40u,p,extra,stage4,integer5);
        const auto red=integer_address(0xa78580u+b.word(0x7c)*40u,p,extra,stage4,integer5);
        return std::array<std::uint32_t,3>{{red,green,blue}};
    };
    const auto emit=[&](std::uint32_t kind,std::int32_t group,std::array<std::uint32_t,3> color,std::uint32_t blend,
        std::uint64_t x,std::uint64_t y,std::uint32_t count,std::uint64_t spacing,std::uint64_t speed,std::uint64_t angle,
        std::uint32_t absolute,std::uint32_t configuration,std::uint32_t delayed){
        emit_projectiles(pool,{{kind,group,color,blend,255,delayed,41},x,y,count,spacing,speed,angle,absolute,configuration},px,py,p,random,precision,math);
    };
    if(behavior==57){
        if(!remainder(b.word(12),p.integers[3])){
            const auto color=colors();const auto child=allocate_projectile(pool,{21,3,color,0,255,1,41});
            if(child!=-1){
                final_projectile_detail::Record c{pool,static_cast<std::uint32_t>(child)};
                c.quad(0x40,b.quad(0x40));c.quad(0x48,b.quad(0x48));c.word(8,58);c.word(0x3c,0);
                const auto dx=m.store(m.sub(m.q(c.quad(0x40)),m.c(0x6d1a60)));const auto dy=m.store(m.sub(m.q(c.quad(0x48)),m.c(0x6d1af0)));
                c.quad(0xa0,m.store(math.atan2(dy,dx)));
                const auto square=m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy))));
                c.quad(0xa8,m.store(stage4_boss_final_sqrt(square,m.precision)));c.word(0x7c,b.word(0x7c));
            }
        }
        const auto bounce=[&](std::size_t position,std::size_t velocity,std::uint32_t low,std::uint32_t high){
            if(crt_sse2::real(b.quad(position))<crt_sse2::real(constant(low))){put(velocity,m.mul(q(velocity),m.c(0x6aca58)));put(position,m.add(q(position),m.c(0x6d1960)));}
            else if(crt_sse2::real(b.quad(position))>crt_sse2::real(constant(high))){put(velocity,m.mul(q(velocity),m.c(0x6aca58)));put(position,m.sub(q(position),m.c(0x6d1960)));}
        };
        if(b.word(0x78)==0){
            put(0x40,m.add(q(0x40),q(0x50)));put(0x48,m.add(q(0x48),q(0x58)));
            if(crt_sse2::real(b.quad(0x40))<crt_sse2::real(constant(0x6d28a8)))put(0x40,m.add(q(0x40),m.c(0x6d28a0)));
            if(crt_sse2::real(b.quad(0x48))<crt_sse2::real(constant(0x6d2898)))put(0x48,m.add(q(0x48),m.c(0x6d2890)));
        }else if(b.word(0x78)==1){put(0x40,m.add(q(0x40),q(0x50)));put(0x48,m.add(q(0x48),q(0x58)));bounce(0x40,0x50,0x6d28a8,0x6d2888);bounce(0x48,0x58,0x6d2898,0x6d2880);}
        else{put(0x40,m.add(q(0x40),q(0x50)));bounce(0x40,0x50,0x6d28a8,0x6d2888);}
        return;
    }
    if(behavior==58){
        if(signed_word(b.word(0x7c))>=0){
            const auto cosine=math.cosine(b.quad(0xa0));put(0x40,m.add(m.mul(cosine,q(0xa8)),m.c(0x6d1a60)));
            const auto sine=math.sine(b.quad(0xa0));put(0x48,m.add(m.mul(sine,q(0xa8)),m.c(0x6d1af0)));
            const auto increment=real_address(0xa87dc0u+b.word(0x7c)*8u,p,boss,extra,final);put(0xa0,m.add(q(0xa0),m.q(increment)));
        }else{
            const auto cosine=m.store(math.cosine(b.quad(0xa0)));const auto radius_x=math.cosine(b.quad(0xb0));
            put(0x40,m.add(m.mul(m.add(m.mul(radius_x,m.c(0x6d1e48)),q(0xa8)),m.q(cosine)),m.c(0x6d1a60)));
            const auto sine=m.store(math.sine(b.quad(0xa0)));const auto radius_y=math.cosine(b.quad(0xb0));
            put(0x48,m.add(m.mul(m.add(m.mul(radius_y,m.c(0x6d1e48)),q(0xa8)),m.q(sine)),m.c(0x6d1af0)));
            const auto increment=m.q(real_address(0xa87db8u+(0u-b.word(0x7c))*8u,p,boss,extra,final));
            put(0xa0,m.add(m.add(increment,increment),q(0xa0)));put(0xb0,m.add(q(0xb0),m.c(0x6d1b48)));
        }
        if(b.word(12)==700)stage4_boss_final_remove100(pool,index);
        return;
    }
    if(behavior==59||behavior==61){
        b.quad(0x40,stage4_boss_final_approach(b.quad(0x40),b.quad(0xa0),constant(0x6d1aa8),m.precision));
        b.quad(0x48,stage4_boss_final_approach(b.quad(0x48),b.quad(0xa8),constant(0x6d1a40),m.precision));
        if(signed_word(p.game_frame)<260)return;
        if(remainder(b.word(12),282)==281){
            const auto base=m.store(m.sub(q(0xb0),m.c(0x6d1a90)));const auto value=random.real_number(constant(0x6d18c0),precision);
            put(0xa0,m.add(value,m.q(base)));put(0xa8,m.add(random.real_number(constant(0x6d1ae8),precision),m.c(0x6d1a90)));
        }
        if(remainder(b.word(12),p.integers[3]))return;
        const bool first=behavior==59;const auto kind=first?23u:24u,configuration=first?60u:62u;
        if(b.word(0x78)==0){
            const auto angle=first?0:m.store(m.add(m.mul(random.real_number(constant(0x6d1d88),precision),m.c(0x6a7708)),m.c(0x6d1e70)));
            const auto speed=p.reals[1];const auto spacing=m.store(m.mul(random.real_number(constant(0x6d1af8),precision),m.c(0x6a7708)));
            const auto y=b.quad(0x48),x=b.quad(0x40);const auto color=colors();emit(kind,1,color,0,x,y,first?2u:3u,spacing,speed,angle,0,configuration,0);
        }else if(b.word(0x78)==1){
            const auto angle=boss.extra_reals[0];const auto speed=first?m.store(m.add(m.q(p.reals[1]),m.c(0x6d1900))):p.reals[1];
            const auto y=b.quad(0x48),x=b.quad(0x40);const auto color=colors();emit(kind,1,color,0,x,y,first?5u:6u,constant(first?0x6d2618:0x6d2750),speed,angle,1,configuration,0);
            boss.extra_reals[0]=m.store(m.add(m.q(boss.extra_reals[0]),m.c(0x6d3088)));
        }else{
            const auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));const auto speed=p.reals[1];
            const auto y=b.quad(0x48),x=b.quad(0x40);const auto color=colors();emit(kind,1,color,0,x,y,first?4u:5u,constant(first?0x6d1b60:0x6d2618),speed,angle,1,configuration,0);
        }
        return;
    }
    if(behavior==60||behavior==62){
        const auto dx=m.store(m.sub(q(0x40),m.q(px))),dy=m.store(m.sub(q(0x48),m.q(py)));
        const auto square=m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy))));
        const auto distance=stage4_boss_final_sqrt(square,m.precision);
        if(compare(distance,m.c(0x6d1c58))<=0){b.word(0x3c,0);put(0x40,m.add(q(0x40),q(0x50)));put(0x48,m.add(q(0x48),q(0x58)));}
        else{b.word(0x3c,1);put(0x40,m.add(m.mul(q(0x50),m.c(0x6d1c18)),q(0x40)));put(0x48,m.add(m.mul(q(0x58),m.c(0x6d1c18)),q(0x48)));}
        const auto x=crt_sse2::real(b.quad(0x40)),y=crt_sse2::real(b.quad(0x48));const bool first=behavior==60;
        if(y>crt_sse2::real(constant(first?0x6d1a48:0x6d1c50))||x<crt_sse2::real(constant(first?0x6d1a30:0x6d1c48))||x>crt_sse2::real(constant(first?0x6d1a28:0x6d1c40))||y<crt_sse2::real(constant(first?0x6d1a30:0x6d1c48)))b.word(0,0);
        return;
    }
    if(behavior==63){
        put(0x40,m.add(q(0x40),q(0x50)));put(0x48,m.add(q(0x48),q(0x58)));put(0xa0,m.add(q(0xa0),m.c(0x6d1b48)));
        if(age()>=60&&age()<=280){
            if(b.word(12)==60)b.word(0x2c,0);
            const auto dx=m.store(m.sub(m.q(px),q(0x40))),dy=m.store(m.sub(m.q(py),q(0x48)));const auto angle=m.store(math.atan2(dy,dx));
            const auto cosine=math.cosine(angle);put(0x50,m.add(m.mul(cosine,m.c(0x6a7708)),q(0x50)));
            const auto sine=math.sine(angle);put(0x58,m.add(m.mul(sine,m.c(0x6a7708)),q(0x58)));
            put(0x50,m.mul(q(0x50),m.c(0x6d1ac0)));put(0x58,m.mul(q(0x58),m.c(0x6d1ac0)));
        }else if(age()>=281&&age()<=300){put(0x50,m.mul(q(0x50),m.c(0x6d1c38)));put(0x58,m.mul(q(0x58),m.c(0x6d1c38)));}
        else if(age()>=301){
            const auto x=crt_sse2::real(b.quad(0x40)),y=crt_sse2::real(b.quad(0x48));
            if(y>crt_sse2::real(constant(0x6d1a68))||x<crt_sse2::real(constant(0x6d1a80))||x>crt_sse2::real(constant(0x6d1a78))||y<crt_sse2::real(constant(0x6d1a80)))b.word(0,0);
        }
        return;
    }
    if(behavior==64){
        if(b.word(12)==40){b.quad(0x48,b.quad(0xa0));put(0xa8,m.mul(random.real_number(constant(0x6d18c0),precision),m.c(0x6d1960)));b.quad(0xb8,0);}
        else if(age()>=80){
            if(remainder(b.word(12),20)==0){
                put(0xb0,m.mul(q(0xb0),m.c(0x6aca58)));
                const auto phase=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
                for(std::uint32_t i=0;signed_word(i)<signed_word(integer5);++i){
                    const auto angle=m.store(m.add(m.mul(m.div(m.c(0x6d2208),m.i(integer5)),m.i(i)),m.q(phase)));
                    const auto direction=m.store(m.add(m.q(angle),q(0xb0)));const auto speed=m.store(m.add(m.q(p.reals[0]),q(0xb8)));
                    const auto sine=math.sine(angle);const auto y=m.store(m.add(m.mul(sine,q(0xa8)),q(0x48)));
                    const auto cosine=math.cosine(angle);const auto x=m.store(m.add(m.mul(cosine,q(0xa8)),q(0x40)));
                    emit(9,2,{{0,255,200}},2,x,y,1,0,speed,direction,1,53,1);
                }
                put(0xa8,m.add(q(0xa8),m.c(0x6d1828)));put(0xb8,m.add(q(0xb8),m.c(0x6d1900)));
            }
            if(b.word(12)==200)b.word(12,extra.integers_6_to_19[0]);
        }
        return;
    }
    throw std::out_of_range("Stage4 final projectile outside57..64");
}
}
