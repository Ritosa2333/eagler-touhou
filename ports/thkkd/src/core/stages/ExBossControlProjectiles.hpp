// 原189及191，圆形绘图、邻弹变种和延迟发射分别保留原先后次序。
#pragma once
#include "core/stages/ExBossControlProjectilesArithmetic.hpp"
#include "core/stages/ExBossControlProjectilesRecords.hpp"
#include "core/stages/ExBossControlServices.hpp"
#include "core/stages/Stage5BossFinalArithmetic.hpp"
#include "core/math/GameMath.hpp"
#include "core/math/Random.hpp"
namespace kaikeidou {
inline void update_ex_boss_control_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,
    const ObjectRecord<240>& player,Random& random,RandomPrecision precision,GameMath& math,ExBossControlServices& services){
    using namespace ex_boss_control_projectiles_detail;
    const Arithmetic m{static_cast<unsigned>(precision)};PoolRecord b{pool,0xa8e660u+index*240u};
    if(behavior==189){
        if(b.word(0x7c)==0){
            ex_boss_control_projectiles_ease(b,0x40,b.quad(0xa0),constant(0x6d1a40),m.precision);
            ex_boss_control_projectiles_ease(b,0x48,b.quad(0xa8),constant(0x6d1a40),m.precision);return;
        }
        if(static_cast<std::uint32_t>(remainder(b.word(12),300))==b.word(0x78)*100u+90u){
            b.quad(0xa0,player.quad(0x90));b.quad(0xa8,player.quad(0x98));
        }
        ex_boss_control_projectiles_ease(b,0x40,b.quad(0xa0),constant(0x6d1a90),m.precision);
        ex_boss_control_projectiles_ease(b,0x48,b.quad(0xa8),constant(0x6d1a90),m.precision);
        const auto y=stage5_boss_final_color_integer(m.q(b.quad(0x48)),true)+16u;
        const auto x=stage5_boss_final_color_integer(m.q(b.quad(0x40)),true)+32u;
        services.draw_circle(x,y,50,0xeeeeee,0);
        for(std::uint32_t i=0;i<3000;++i){
            auto& other=pool.bullets[i];if(other.word(0)!=1||other.word(4)!=1)continue;
            const auto dx=m.store(m.sub(m.q(other.quad(0x40)),m.q(b.quad(0x40))));
            const auto dy=m.store(m.sub(m.q(other.quad(0x48)),m.q(b.quad(0x48))));
            const auto square=m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy))));
            const auto distance=ex_boss_control_projectiles_sqrt(square,m.precision);
            if(compare(distance,m.c(0x6d1a88))<=0){other.word(4,4);other.word(0x2c,0);}
        }
        return;
    }
    if(behavior==191){
        const auto prior=b.word(0x78),limit=b.word(0x7c);b.word(0x78,prior+1u);
        if(prior==limit){
            b.word(0x78,0);const auto next=random.integer_bits(60,precision,true);b.word(0x7c,next+180u);
            b.quad(0xc0,constant(0x6b25b8));
            const auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
            const auto radius=m.store(m.mul(random.real_number(constant(0x6d2028),precision),m.c(0x6d1960)));
            auto value=math.cosine(angle);b.quad(0xc8,m.store(m.add(m.mul(value,m.q(radius)),m.q(player.quad(0x90)))));
            value=math.sine(angle);b.quad(0xd0,m.store(m.add(m.mul(value,m.q(radius)),m.q(player.quad(0x98)))));
        }
        ex_boss_control_projectiles_ease(b,0x40,b.quad(0xc8),b.quad(0xc0),m.precision);
        ex_boss_control_projectiles_ease(b,0x48,b.quad(0xd0),b.quad(0xc0),m.precision);
        ex_boss_control_projectiles_ease(b,0xc0,constant(0x6d1ae8),constant(0x6d1ab0),m.precision);
        if(remainder(b.word(12),180)!=0)return;
        auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
        for(unsigned i=0;i<16;++i){
            const auto slot=allocate_projectile(pool,{20,3,{{0,0,0}},0,255,0,41});
            if(slot!=-1){
                auto& child=pool.bullets[static_cast<std::uint32_t>(slot)];
                child.quad(0x40,b.quad(0x40));child.quad(0x48,b.quad(0x48));child.quad(0xa0,angle);
                child.quad(0xa8,constant(0x6d2488));child.quad(0xb0,m.store(m.mul(m.i(b.word(0x80)),m.c(0x6d2128))));child.word(8,190);
            }
            angle=m.store(m.add(m.q(angle),m.c(0x6d27f8)));b.word(0x80,0u-b.word(0x80));
        }
        return;
    }
    throw std::out_of_range("EX boss control projectile outside189/191");
}
}
