// 原109/111撞边生成110，112追踪玩家并清除附近零号外观普通弹。
#pragma once
#include "core/stages/ExMidbossArithmetic.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/shared/EffectUpdate.hpp"
namespace kaikeidou {
inline void update_ex_midboss_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,const ObjectRecord<240>& player,PatternParameters& p,EffectShake& shake,Random& random,RandomPrecision precision,GameMath& math){
    using namespace ex_midboss_detail;
    const Arithmetic m{{static_cast<unsigned>(precision)}};
    final_projectile_detail::Record b{pool,index};
    const auto q=[&](std::size_t offset){return m.q(b.quad(offset));};
    const auto put=[&](std::size_t offset,binary_float::Number value){b.quad(offset,m.store(value));};
    if(behavior==109||behavior==111){
        const bool left=b.word(0x78)!=0;
        const auto wall=left?0x6d1a40u:0x6d2160u;
        const auto comparison=compare(m.c(wall),q(0x40));
        const bool bounce=left?comparison>=0:comparison<=0;
        if(!bounce){
            const auto cosine=math.cosine(b.quad(0xa0));put(0x40,m.add(m.mul(cosine,q(0x68)),q(0x40)));
            const auto sine=math.sine(b.quad(0xa0));put(0x48,m.add(m.mul(sine,q(0x68)),q(0x48)));
            put(0x68,m.add(q(0x68),q(0xa8)));return;
        }
        b.quad(0x40,constant(wall));b.word(0x78,left?0u:1u);
        const auto dx=m.store(m.sub(m.c(left?0x6d2160:0x6d1a40),q(0x40)));
        const auto destination=random.real_number(constant(behavior==109?0x6d1f20:0x6d2870),precision);
        const auto dy=m.store(m.sub(m.add(m.mul(destination,m.c(0x6d1960)),m.c(0x6d19f0)),q(0x48)));
        const auto angle=math.atan2(dy,dx);put(0xa0,angle);b.quad(0x68,0);
        if(behavior==109)put(0xa8,m.add(q(0xa8),m.c(0x6d2878)));
        else if(compare(m.c(0x6d1ce8),q(0xa8))>0)put(0xa8,m.add(q(0xa8),m.c(0x6d1900)));
        for(unsigned i=0;i<(behavior==109?26u:32u);++i){
            const auto red=random.integer_bits(100,precision,true);
            const auto blue=red+random.integer_bits(35,precision,true)+120u;
            const auto green=red+random.integer_bits(90,precision,true)+30u;
            const auto slot=allocate_projectile(pool,{0,0,{{red,green,blue}},0,255,0,41});
            if(slot==-1)continue;
            auto& spawned=pool.bullets[slot];
            const auto direction1=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
            const auto radius=m.store(m.mul(random.real_number(constant(0x6d2188),precision),m.c(0x6a7708)));
            const auto direction2=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
            const auto speed=m.store(m.add(m.mul(random.real_number(constant(0x6d1cc0),precision),m.c(0x6a7708)),m.n(1.)));
            const auto xcos=math.cosine(direction1);spawned.quad(0x40,m.store(m.add(m.mul(xcos,m.q(radius)),q(0x40))));
            const auto ysin=math.sine(direction1);spawned.quad(0x48,m.store(m.add(m.mul(ysin,m.q(radius)),q(0x48))));
            spawned.word(8,110);spawned.quad(0x68,0);
            const auto vx=math.cosine(direction2);spawned.quad(0xa0,m.store(m.mul(vx,m.q(speed))));
            const auto vy=math.sine(direction2);spawned.quad(0xa8,m.store(m.sub(m.mul(vy,m.q(speed)),m.c(0x6d2660))));
            spawned.quad(0xb0,m.store(m.add(m.mul(random.real_number(constant(0x6b25b8),precision),m.c(0x6d1950)),m.c(0x6d1960))));
            spawned.quad(0xb8,constant(0x6d1940));
            spawned.quad(0xc0,m.store(m.add(m.mul(random.real_number(constant(0x6b25b8),precision),m.c(0x6d1950)),m.c(0x6d1a20))));
        }
        if(shake.word(0)==0)set_effect_screen_shake(shake,6,12);
        return;
    }
    if(behavior==110){
        put(0x40,m.add(q(0x40),q(0xa0)));put(0x48,m.add(q(0x48),q(0xa8)));
        put(0xa8,m.add(q(0xa8),q(0xb0)));put(0xb0,m.mul(q(0xb0),q(0xb8)));
        if(compare(m.q(p.reals[0]),q(0x48))<0&&compare(q(0xc0),q(0xa8))<0){put(0xa0,m.mul(q(0xa0),m.c(0x6d18f8)));put(0xa8,m.mul(q(0xa8),m.c(0x6d18f8)));}
        if(compare(m.c(0x6d1a30),q(0x40))>0||compare(m.c(0x6d1a28),q(0x40))<0||compare(m.c(0x6d1a48),q(0x48))<0)b.word(0,0);
        return;
    }
    if(behavior==112){
        const auto dx=m.store(m.sub(m.q(player.quad(0x90)),q(0x40)));
        const auto dy=m.store(m.sub(m.q(player.quad(0x98)),q(0x48)));
        const auto angle=m.store(math.atan2(dy,dx));
        const auto square=m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy))));
        const auto distance=ex_midboss_sqrt(square,m.precision);
        const auto speed=m.store(m.mul(distance,q(0xa0)));
        const auto cosine=math.cosine(angle);put(0x40,m.add(m.mul(cosine,m.q(speed)),q(0x40)));
        const auto sine=math.sine(angle);put(0x48,m.add(m.mul(sine,m.q(speed)),q(0x48)));
        if(compare(m.c(0x6d1fa0),q(0xa0))>0)put(0xa0,m.add(q(0xa0),m.c(0x6a7888)));
        for(std::size_t i=0;i<pool.bullets.size();++i){
            const auto& other=pool.bullets[i];if(other.word(0)!=1||other.word(4)!=0)continue;
            const auto x=m.store(m.sub(m.q(other.quad(0x40)),q(0x40)));
            const auto y=m.store(m.sub(m.q(other.quad(0x48)),q(0x48)));
            const auto s=m.store(m.add(m.mul(m.q(x),m.q(x)),m.mul(m.q(y),m.q(y))));
            if(compare(ex_midboss_sqrt(s,m.precision),m.c(0x6d1c58))<=0)remove_projectile(pool,i,100);
        }
        return;
    }
    throw std::out_of_range("EX midboss projectile behavior");
}
}
