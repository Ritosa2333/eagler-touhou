// 原二面关底弹29..36，包含34生成35和既有-1槽别名。
#pragma once
#include "core/stages/Stage2BossDependenciesConstants.hpp"
#include "core/stages/Stage2BossArithmetic.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/math/GameMath.hpp"
namespace kaikeidou {
namespace stage2_boss_dependencies_detail {
inline void orbit_step(final_projectile_detail::Record& p,GameMath& math,unsigned bits){
    using namespace binary_float;using stage2_midboss_detail::subtract;
    const auto c=[](std::uint32_t a){return binary64(constant(a));};
    auto v=math.cosine(p.quad(0xa0));p.quad(0x40,to_binary64(add(multiply(v,binary64(p.quad(0xa8)),bits),c(0x6d1a60),bits)));
    v=math.sine(p.quad(0xa0));p.quad(0x48,to_binary64(add(multiply(v,binary64(p.quad(0xa8)),bits),c(0x6d1af0),bits)));
    p.quad(0xa0,to_binary64(add(binary64(p.quad(0xa0)),binary64(p.quad(0xb0)),bits)));
    p.quad(0xb0,to_binary64(add(binary64(p.quad(0xb0)),binary64(p.quad(0xb8)),bits)));
    p.quad(0xb8,to_binary64(add(binary64(p.quad(0xb8)),c(0x6d1fe0),bits)));
    p.quad(0xa8,to_binary64(subtract(binary64(p.quad(0xa8)),c(0x6d1fd8),bits)));
    v=math.cosine(p.quad(0xa0));const auto x=to_binary64(add(multiply(v,binary64(p.quad(0xa8)),bits),c(0x6d1a60),bits));
    v=math.sine(p.quad(0xa0));const auto y=to_binary64(add(multiply(v,binary64(p.quad(0xa8)),bits),c(0x6d1af0),bits));
    const auto dx=to_binary64(subtract(binary64(x),binary64(p.quad(0x40)),bits)),dy=to_binary64(subtract(binary64(y),binary64(p.quad(0x48)),bits));p.quad(0x60,to_binary64(math.atan2(dy,dx)));
}
}
inline void update_stage2_boss_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,
    const PatternParameters& parameters,const BossStageContext& stage,GameMath& math,RandomPrecision precision){
    using namespace binary_float;using stage2_midboss_detail::subtract;using stage2_midboss_detail::signed_word;using stage2_boss_dependencies_detail::constant;
    const auto bits=static_cast<unsigned>(precision);final_projectile_detail::Record p{pool,index};
    const auto c=[](std::uint32_t a){return binary64(constant(a));};const auto r=[&](unsigned off){return binary64(p.quad(off));};const auto w=[&](unsigned off,Number value){p.quad(off,to_binary64(value));};
    const auto move=[&](std::uint32_t factor=0,std::uint64_t speed=0,bool local=false){
        auto value=math.cosine(p.quad(0x60));value=multiply(value,local?binary64(speed):r(0x68),bits);if(factor)value=multiply(value,c(factor),bits);w(0x40,add(value,r(0x40),bits));
        value=math.sine(p.quad(0x60));value=multiply(value,local?binary64(speed):r(0x68),bits);if(factor)value=multiply(value,c(factor),bits);w(0x48,add(value,r(0x48),bits));
    };
    const auto bend=[&](){w(0x60,add(divide(multiply(r(0xa8),r(0x68),bits),c(0x6d1c90),bits),r(0xa0),bits));};
    const auto cull=[&](){const auto x=crt_sse2::real(p.quad(0x40)),y=crt_sse2::real(p.quad(0x48));if(x< -64.||y< -64.||x>448.||y>512.)p.word(0,0);};
    const auto age=signed_word(p.word(0xc));
    switch(behavior){
    case 29:{
        if(age<0)return;
        if(age<=50){bend();w(0xa8,subtract(r(0xa8),r(0xb0),bits));move();}
        else if(age<=signed_word(80u-p.word(0x78))){
            bend();w(0xa8,add(multiply(r(0xb0),c(0x6d1b28),bits),r(0xa8),bits));move(0x6d1880);
            if(p.word(0xc)==80u-p.word(0x78)){
                p.word(4,9);if(parameters.difficulty<4){constexpr std::uint32_t bases[]{0x6d1ce8,0x6d1b18,0x6d1ce0,0x6d1ad8};w(0x68,subtract(c(bases[parameters.difficulty]),multiply(integer(p.word(0x78)),c(0x6d1cf0),bits),bits));}
                w(0xa8,subtract(r(0xa8),multiply(integer(p.word(0x78)),c(0x6d1960),bits),bits));w(0x60,add(r(0xa0),r(0xa8),bits));
            }
        }else move();cull();return;
    }
    case 30:{
        if(age<0)return;const auto remainder=static_cast<std::uint32_t>(age%100);const bool first=remainder<=49;
        auto time=integer(remainder);if(!first)time=subtract(time,c(0x6d1a88),bits);
        const auto speed=to_binary64(add(divide(r(0x68),subtract(c(0x6d1898),multiply(time,binary64(stage.extra_reals[0]),bits),bits),bits),multiply(integer(p.word(0xc)),c(0x6d1968),bits),bits));
        bend();w(0xa8,first?add(r(0xa8),r(0xb0),bits):subtract(r(0xa8),r(0xb0),bits));move(0,speed,true);cull();return;
    }
    case 31:{
        if(age<0)return;auto value=math.cosine(p.quad(0xa0));w(0x40,add(multiply(value,r(0xb0),bits),r(0xc8),bits));value=math.sine(p.quad(0xa0));w(0x48,add(multiply(value,r(0xb0),bits),r(0xd0),bits));
        w(0xa0,add(add(r(0xc0),r(0xa8),bits),r(0xa0),bits));
        if(signed_word(p.word(0xc))<=120)w(0xb0,add(r(0xb0),r(0xb8),bits));
        else if(signed_word(p.word(0xc))<=140){w(0xb0,add(r(0xb0),r(0xb8),bits));w(0xa8,multiply(r(0xa8),c(0x6d18f0),bits));if(p.word(0x7c)==1)w(0xb8,multiply(r(0xb8),c(0x6d1cd0),bits));}
        else{w(0xa8,multiply(r(0xa8),c(0x6d1b10),bits));w(0xb0,add(r(0xb0),r(0xb8),bits));}
        if(crt_sse2::real(p.quad(0xb0))< -400.)p.word(0,0);return;
    }
    case 32:{
        if(age<0)return;auto value=math.sine(p.quad(0xa0));const auto x=to_binary64(add(multiply(value,c(0x6d1cc0),bits),r(0xb0),bits));
        if(crt_sse2::real(x)< -13.)w(0x40,add(add(binary64(x),c(0x6d1cb0),bits),c(0x6d1ca8),bits));
        else if(crt_sse2::real(x)>397.)w(0x40,subtract(subtract(binary64(x),c(0x6d1cb0),bits),c(0x6d1ca8),bits));else p.quad(0x40,x);
        w(0xa0,add(r(0xa0),r(0xa8),bits));if(crt_sse2::real(p.quad(0xa8))<=crt_sse2::real(constant(0x6d1c98)))w(0xa8,add(r(0xa8),c(0x6b2620),bits));return;
    }
    case 33:{
        if(p.word(0x78)<=1){const auto first=p.word(0x78)==0;auto value=math.sine(p.quad(0xa0));w(0x40,add(multiply(value,c(0x6d1fe8),bits),c(0x6d1a60),bits));w(0xa0,add(r(0xa0),r(0xc0),bits));w(0x48,add(r(0x48),r(0xa8),bits));
            if(first){if(crt_sse2::real(p.quad(0x48))<16.)p.word(0x78,1);}
            else if(crt_sse2::real(p.quad(0xa8))<=crt_sse2::real(p.quad(0xb8))){w(0xa8,add(r(0xa8),c(0x6d1b48),bits));w(0xc0,multiply(r(0xc0),c(0x6d1a20),bits));}
            else{p.word(0x78,2);final_projectile_recolor(pool,index,0,255,255);}
        }else{w(0x40,add(r(0x40),r(0xb0),bits));w(0x48,add(r(0x48),r(0xa8),bits));if(crt_sse2::real(p.quad(0x48))>456.)p.word(0,0);}return;
    }
    case 34:{
        auto value=math.cosine(p.quad(0xa0));w(0x40,add(multiply(value,r(0xa8),bits),c(0x6d1a60),bits));value=math.sine(p.quad(0xa0));w(0x48,add(multiply(value,r(0xa8),bits),c(0x6d1af0),bits));
        w(0xa0,subtract(r(0xa0),binary64(parameters.reals[0]),bits));if(crt_sse2::real(p.quad(0xa8))<=320.)w(0xa8,add(r(0xa8),c(0x6d1868),bits));
        if(stage2_midboss_detail::remainder(p.word(0xc),parameters.integers[0]))return;
        for(std::uint32_t i=0;i<=4;++i){const auto slot=allocate_projectile(pool,{11,0,{{0,96,255}},2,255,1,41});if(slot==-1)continue;final_projectile_detail::Record child{pool,static_cast<std::uint32_t>(slot)};
            child.word(8,35);child.word(0xc,0u-i*2u);child.quad(0xa0,p.quad(0xa0));child.quad(0xa8,p.quad(0xa8));child.quad(0xb0,constant(0x6d28c0));child.quad(0xb8,parameters.reals[1]);stage2_boss_dependencies_detail::orbit_step(child,math,bits);
        }return;
    }
    case 35:{if(age<0)return;stage2_boss_dependencies_detail::orbit_step(p,math,bits);if(crt_sse2::real(p.quad(0xa8))<=0.)stage2_boss_remove_projectile(pool,index);return;}
    case 36:{
        if(age<0)return;
        if(age>=signed_word(p.word(0x78)+80u)){
            if(p.word(0xc)==p.word(0x78)+80u){p.word(4,9);const auto dx=to_binary64(subtract(r(0x40),c(0x6d1a60),bits)),dy=to_binary64(subtract(r(0x48),c(0x6d1af0),bits));w(0x60,math.atan2(dy,dx));p.quad(0x68,constant(0x6d1960));w(0xb8,subtract(c(0x6d1a08),multiply(integer(p.word(0x78)),c(0x6d1950),bits),bits));}
            move();w(0x68,add(r(0x68),r(0xb8),bits));
        }else{bend();w(0xa8,add(r(0xa8),r(0xb0),bits));const auto now=signed_word(p.word(0xc));if(now>=20&&now<60)w(0xb0,multiply(r(0xb0),c(0x6d1c88),bits));move(now>=20?0x6d1b28:0x6d1c80);}
        cull();return;
    }
    default:throw std::out_of_range("Stage2 boss projectile outside 29..36");
    }
}
}
