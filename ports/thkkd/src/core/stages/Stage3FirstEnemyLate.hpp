// 原敌47..50完整行为；字段落点、发射顺序及原IDIV故障边界按原指令保留。
#pragma once
#include "core/stages/Stage3EnemyLateArithmetic.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
inline void update_stage3_first_enemy_late(std::uint32_t behavior,Enemy& enemy,ProjectilePools& projectiles,
    std::uint64_t player_x,std::uint64_t player_y,PatternParameters& parameters,Random& random,RandomPrecision precision,GameMath& math){
    using namespace binary_float;using namespace stage3_enemy_late_detail;const auto bits=static_cast<unsigned>(precision);
    const auto c=[](std::uint32_t address){return binary64(constant(address));};
    const auto r=[&](unsigned offset){return binary64(enemy.quad(offset));};
    const auto w=[&](unsigned offset,Number value){enemy.quad(offset,to_binary64(value));};
    const auto pick=[&](std::array<std::uint64_t,6> values){return stage3_enemy_late_pick_real(parameters.difficulty,values);};
    const auto count=[&](std::array<std::uint32_t,6> values){return stage3_enemy_late_pick_integer(parameters.difficulty,values);};
    const auto fire=[&](std::uint32_t kind,std::int32_t group,std::array<std::uint32_t,3> color,std::uint32_t blend,std::uint32_t n,
                       std::uint64_t spacing,std::uint64_t speed,std::uint64_t angle,std::uint32_t absolute,std::uint32_t configuration){
        emit_projectiles(projectiles,{{kind,group,color,blend,255,1,41},enemy.quad(0x38),enemy.quad(0x40),n,spacing,speed,angle,absolute,configuration},player_x,player_y,parameters,random,precision,math);
    };
    const auto age=signed_word(enemy.word(0xc));
    if(behavior==47){
        if(age<=220)w(0x40,subtract(r(0x40),c(0x6d1948),bits));
        else{w(0x38,add(r(0x38),r(0x48),bits));if(crt_sse2::real(enemy.quad(0x38))< -20.||crt_sse2::real(enemy.quad(0x38))>404.)enemy.word(0,0);}
        if(age>=100&&remainder(enemy.word(0xc),enemy.word(0x80))==signed_word(enemy.word(0x84))){
            const bool positive=crt_sse2::real(enemy.quad(0x48))>0.;
            const auto speed=pick({constant(0x6d1c80),constant(0x6d1948),constant(0x6d1b18),constant(0x6d1ad8),0,0});
            const auto spacing=pick({constant(0x6d19c0),constant(0x6d19c0),constant(0x6d18e0),constant(0x6d18e0),0,0});
            const auto n=count({3,3,5,5,0,0});
            fire(1,0,positive?std::array<std::uint32_t,3>{{255,0,64}}:std::array<std::uint32_t,3>{{64,0,255}},2,n,spacing,speed,0,0,2);
        }return;
    }
    if(behavior==48){
        w(0x38,add(r(0x38),r(0x48),bits));w(0x40,add(r(0x40),r(0x50),bits));w(0x50,multiply(r(0x50),c(0x6d1940),bits));
        const auto d=signed_word(parameters.difficulty);
        const bool periodic=age>=170&&d>1&&age%80==10;
        if(age==10||age==90||(age==170&&d<=1)||periodic){
            const auto angle=constant(crt_sse2::real(player_y)>=crt_sse2::real(enemy.quad(0x40))?0x6d1b60:0x6d1de0);
            auto speed=pick({constant(0x6d1b28),constant(0x6d1948),constant(0x6d1b18),constant(0x6d1ad8),0,0});
            if(age!=10)speed=to_binary64(multiply(binary64(speed),c(age==90?0x6d1f50:0x6d1a00),bits));
            const auto color=age==10?std::array<std::uint32_t,3>{{255,0,0}}:age==90?std::array<std::uint32_t,3>{{0,255,0}}:std::array<std::uint32_t,3>{{0,0,255}};
            fire(2,0,color,2,1,0,speed,angle,1,0);
            if((age==10&&parameters.difficulty==3)||(age==90&&d>=1)||age>=170)
                fire(0,0,{{255,0,255}},0,1,0,constant(periodic?0x6d1b18:0x6d1948),0,0,0);
        }
        if(crt_sse2::real(enemy.quad(0x38))< -20.||crt_sse2::real(enemy.quad(0x40))>crt_sse2::real(constant(0x6d19f8)))enemy.word(0,0);
        return;
    }
    if(behavior==49){
        if(age<=50){w(0x40,add(r(0x40),r(0x50),bits));w(0x50,multiply(r(0x50),c(0x6d1940),bits));return;}
        if(age<=450){
            if(age<=150){w(0x38,add(add(r(0xa8),r(0xa8),bits),r(0x38),bits));w(0xa8,multiply(r(0xa8),c(0x6d1970),bits));}
            if(enemy.word(4)==5){
                if(signed_word(parameters.difficulty)>=1&&remainder(enemy.word(0xc),count({160,90,56,40,0,0}))==0){
                    const auto speed=pick({constant(0x6d1b28),constant(0x6d2130),constant(0x6d1b18),constant(0x6d1a98),0,0});
                    fire(2,1,{{255,0,255}},2,1,0,speed,0,0,0);
                }
            }else{
                if(remainder(enemy.word(0xc),parameters.integers[0])==signed_word(enemy.word(0x80))){
                    const auto angle=enemy.quad(0xb8);
                    const auto speed=pick({constant(0x6d1a00),constant(0x6d1e40),constant(0x6d1948),constant(0x6d2548),0,0});
                    fire(12,2,{{enemy.word(0x8c),enemy.word(0x90),enemy.word(0x94)}},0,1,constant(0x6d19c0),speed,angle,1,0);
                }
                w(0xb8,subtract(r(0xb8),c(0x6d1f78),bits));
            }return;
        }
        if(age>=500){w(0x38,subtract(r(0x38),r(0xb0),bits));w(0xb0,multiply(r(0xb0),c(0x6d2af0),bits));w(0x40,subtract(r(0x40),c(0x6d1f50),bits));if(crt_sse2::real(enemy.quad(0x40))< -20.)enemy.word(0,0);}
        return;
    }
    if(behavior==50){
        if(age<=50){enemy.quad(0x38,ease_option_value(enemy.quad(0x38),constant(0x6d1a60),constant(0x6d1a40),bits));enemy.quad(0x40,ease_option_value(enemy.quad(0x40),constant(0x6d1af0),constant(0x6d1a40),bits));return;}
        if(age<=650){
            if(remainder(enemy.word(0xc),enemy.word(0x80))==0){
                fire(12,0,{{0,enemy.word(0x84),enemy.word(0x84)}},0,enemy.word(0x88),enemy.quad(0xa8),enemy.quad(0xb0),enemy.quad(0xb8),1,0);
                if(signed_word(enemy.word(0x84))>3)enemy.word(0x84,enemy.word(0x84)-3u);
                w(0xb8,add(r(0xb8),r(0xc0),bits));w(0xc0,add(r(0xc0),c(0x6d24f0),bits));
            }return;
        }
        if(age>=700){w(0x38,subtract(r(0x38),c(0x6d1948),bits));w(0x40,subtract(r(0x40),c(0x6d2488),bits));if(crt_sse2::real(enemy.quad(0x40))<crt_sse2::real(constant(0x6d1b58)))enemy.word(0,0);}
        return;
    }
    throw std::out_of_range("Stage3 late enemy behavior outside 47..50");
}
}
