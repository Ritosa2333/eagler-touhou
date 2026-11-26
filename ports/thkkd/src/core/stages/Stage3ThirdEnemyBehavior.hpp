// 原敌53/54完整行为，保留暂停移动、难度选择顺序与逐次发射的参数重读。
#pragma once
#include "core/stages/Stage3ThirdEnemyArithmetic.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
inline void update_stage3_third_enemy(std::uint32_t behavior,Enemy& enemy,ProjectilePools& pool,
    std::uint64_t player_x,std::uint64_t player_y,PatternParameters& p,Random& random,RandomPrecision precision,GameMath& math){
    using namespace binary_float;using namespace stage3_third_enemy_detail;
    const auto bits=static_cast<unsigned>(precision);const auto c=[](std::uint32_t a){return binary64(constant(a));};
    const auto pick=[&](std::array<std::uint64_t,6> values){return stage3_third_enemy_pick_real(p.difficulty,values);};
    const auto count=[&](std::array<std::uint32_t,6> values){return stage3_third_enemy_pick_integer(p.difficulty,values);};
    const auto age=[&](){return signed_word(enemy.word(0xc));};
    const auto cull=[&](){const auto x=crt_sse2::real(enemy.quad(0x38));if(x< -20.||x>404.)enemy.word(0,0);};
    if(behavior==53){
        if(age()>=20&&age()<=100){
            if(!remainder(enemy.word(0xc),count({{50,30,22,14,0,0}}))){
                const auto speed=pick({{constant(0x6d2488),constant(0x6d1ff8),constant(0x6d1948),constant(0x6d1ce8),0,0}});
                const auto spacing=pick({{constant(0x6d19c0),constant(0x6d18e0),constant(0x6d1900),constant(0x6d1900),0,0}});
                const auto n=count({{3,5,7,7,0,0}});
                emit_projectiles(pool,{{11,0,{{96,64,255}},0,255,1,41},enemy.quad(0x38),enemy.quad(0x40),n,spacing,speed,0,0,0},player_x,player_y,p,random,precision,math);
            }
        }else{
            enemy.quad(0x38,to_binary64(add(binary64(enemy.quad(0x38)),binary64(enemy.quad(0xa8)),bits)));
            enemy.quad(0xa8,to_binary64(add(binary64(enemy.quad(0xa8)),binary64(enemy.quad(0xb0)),bits)));
            enemy.quad(0x40,to_binary64(add(binary64(enemy.quad(0x40)),c(0x6d1c18),bits)));cull();
        }
        return;
    }
    if(behavior!=54)throw std::out_of_range("Stage3 third enemy outside 53..54");
    if(age()<=50){enemy.quad(0x40,to_binary64(add(binary64(enemy.quad(0x40)),binary64(enemy.quad(0xa8)),bits)));return;}
    if(age()>90){enemy.quad(0x38,to_binary64(add(binary64(enemy.quad(0x38)),binary64(enemy.quad(0xb0)),bits)));cull();return;}
    const auto angle=constant(crt_sse2::real(enemy.quad(0x40))>crt_sse2::real(player_y)?0x6d1de0:0x6d1b60);
    const auto fire=[&](std::uint32_t kind,std::uint32_t green,std::uint32_t blue,std::uint64_t x,std::uint64_t speed){
        emit_projectiles(pool,{{kind,0,{{255,green,blue}},0,255,1,41},x,enemy.quad(0x40),1,0,speed,angle,1,2},player_x,player_y,p,random,precision,math);
    };
    const auto speed=[&](std::uint32_t multiplier){
        const auto base=pick({{constant(0x6d1b28),constant(0x6d1e40),constant(0x6d1948),constant(0x6d2548),0,0}});
        return multiplier?to_binary64(multiply(binary64(base),c(multiplier),bits)):base;
    };
    if(enemy.word(0xc)==60&&signed_word(p.difficulty)>=2){
        p.integers[0]=12;p.integers[1]=18;const auto v=speed(0x6d2a70);fire(3,0,0,enemy.quad(0x38),v);
        if(p.difficulty==3)fire(0,0,255,enemy.quad(0x38),constant(0x6d2a68));
        return;
    }
    std::uint32_t level=0,offset=0,multiplier=0,green=0;
    if(enemy.word(0xc)==62){level=10;offset=0x6d1e20;multiplier=0x6d1a00;green=16;}
    else if(enemy.word(0xc)==64&&signed_word(p.difficulty)>=1){level=8;offset=0x6d1d18;multiplier=0x6d1890;green=48;}
    else if(enemy.word(0xc)==66){level=6;offset=0x6d1c58;multiplier=0x6d1f50;green=96;}
    else if(enemy.word(0xc)==68){level=4;offset=0x6d18c0;multiplier=0x6d2ae8;green=160;}
    else if(enemy.word(0xc)==70&&signed_word(p.difficulty)>=2){level=2;offset=0x6d1d88;green=240;}
    else return;
    p.integers[0]=level;p.integers[1]=level+6;
    const auto first_speed=speed(multiplier);const auto right=to_binary64(add(binary64(enemy.quad(0x38)),c(offset),bits));fire(3,0,green,right,first_speed);
    const auto second_speed=speed(multiplier);const auto left=to_binary64(subtract(binary64(enemy.quad(0x38)),c(offset),bits));fire(3,0,green,left,second_speed);
}
}
