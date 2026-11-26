// 原 63F1D0、63EF50、63ED00、63EA90、63E620 的完整敌行为 24..28。
#pragma once
#include "core/stages/Stage2EnemyArithmetic.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
inline void update_stage2_first_enemy(std::uint32_t behavior,Enemy& enemy,ProjectilePools& projectiles,
    std::uint64_t player_x,std::uint64_t player_y,PatternParameters& parameters,Random& random,RandomPrecision precision,GameMath& math){
    using namespace binary_float;using namespace stage2_enemy_detail;const auto bits=static_cast<unsigned>(precision);
    const auto c=[](std::uint32_t a){return binary64(constant(a));};
    const auto pick=[&](std::array<std::uint64_t,6> values){return stage2_enemy_pick_real(parameters.difficulty,values);};
    const auto count=[&](std::array<std::uint32_t,6> values){return stage2_enemy_pick_integer(parameters.difficulty,values);};
    const auto fire=[&](std::uint32_t kind,std::int32_t group,std::array<std::uint32_t,3> color,std::uint32_t n,std::uint64_t spacing,std::uint64_t speed,std::uint64_t angle,std::uint32_t absolute,std::uint32_t configuration){
        ProjectileEmission input{{kind,group,color,0,255,1,41},enemy.quad(0x38),enemy.quad(0x40),n,spacing,speed,angle,absolute,configuration};
        emit_projectiles(projectiles,input,player_x,player_y,parameters,random,precision,math);
    };
    switch(behavior){
    case 24:{
        enemy.quad(0x40,to_binary64(add(binary64(enemy.quad(0x40)),binary64(enemy.quad(0x50)),bits)));
        enemy.quad(0x50,to_binary64(subtract(binary64(enemy.quad(0x50)),c(0x6d1968),bits)));
        if(crt_sse2::real(enemy.quad(0x40))<=-20.)enemy.word(0,0);
        if(enemy.word(0xc)!=100u-parameters.difficulty*10u)return;
        enemy.quad(0x50,constant(0x6d23b0));
        if(signed_word(parameters.difficulty)<1&&(enemy.word(0x84)&1u)==0)return;
        const bool odd=enemy.word(0x80)!=0;
        const auto speed=pick({constant(0x6d1ad0),constant(0x6d1ff8),constant(0x6d1948),constant(0x6d1948),0,0});
        const auto spacing=pick({constant(0x6d2b90),constant(0x6d2688),constant(0x6d2638),constant(0x6d19b8),0,0});
        const auto n=count(odd?std::array<std::uint32_t,6>{{5,7,9,11,0,0}}:std::array<std::uint32_t,6>{{6,8,10,12,0,0}});
        fire(0,0,odd?std::array<std::uint32_t,3>{{255,96,32}}:std::array<std::uint32_t,3>{{240,140,0}},n,spacing,speed,0,0,0);return;
    }
    case 25:{
        enemy.quad(0x38,to_binary64(add(multiply(math.sine(enemy.quad(0xa8)),c(0x6d1d78),bits),c(0x6d1a60),bits)));
        enemy.quad(0x40,to_binary64(add(binary64(enemy.quad(0x40)),c(0x6d1ad0),bits)));
        enemy.quad(0xa8,to_binary64(add(binary64(enemy.quad(0xa8)),c(0x6d1968),bits)));
        if(crt_sse2::real(enemy.quad(0x40))>=460.)enemy.word(0,0);
        if(enemy.word(0xc)==20){
            const auto spacing=pick({constant(0x6d2b78),constant(0x6d2b80),constant(0x6d2008),constant(0x6d1fa8),0,0});const auto n=count({2,5,7,9,0,0});
            fire(11,1,{{255,0,0}},n,spacing,constant(0x6d1ad0),0,0,0);
        }else if(enemy.word(0xc)==60&&signed_word(parameters.difficulty)>=2){
            const auto spacing=pick({constant(0x6d19c0),constant(0x6d2b80),constant(0x6d2688),constant(0x6d1fb8),0,0});const auto n=count({1,3,11,13,0,0});
            fire(10,2,{{255,0,255}},n,spacing,constant(0x6d1948),constant(0x6d1b60),1,0);
        }return;
    }
    case 26:case 27:{
        enemy.quad(0xa8,to_binary64(add(binary64(enemy.quad(0xa8)),binary64(enemy.quad(0xb0)),bits)));
        const auto angle=stage2_enemy_radians(enemy.quad(0xa8),bits);
        const auto cosine=math.cosine(angle);enemy.quad(0x38,to_binary64(add(multiply(cosine,c(0x6d1d78),bits),binary64(enemy.quad(0xb8)),bits)));
        enemy.quad(0x40,to_binary64(add(multiply(math.sine(angle),c(0x6d1d78),bits),c(0x6d22c0),bits)));
        if(enemy.word(0xc)==(behavior==26?28u:68u)||(signed_word(parameters.difficulty)>=2&&enemy.word(0xc)==(behavior==26?56u:120u))){
            for(std::uint32_t i=0;signed_word(i)<=signed_word(count({0,2,3,4,0,0}));++i){
                parameters.integers[0]=0;parameters.integers[1]=10;
                std::array<std::uint64_t,6> values{};
                values[3]=to_binary64(subtract(c(0x6d2548),multiply(integer(i),c(0x6d19c0),bits),bits));
                values[2]=to_binary64(subtract(c(0x6d2660),multiply(integer(i),c(0x6d18b0),bits),bits));
                values[1]=to_binary64(subtract(c(0x6d1948),multiply(integer(i),c(0x6d19c0),bits),bits));
                values[0]=to_binary64(subtract(c(0x6d2488),multiply(integer(i),c(0x6d18e0),bits),bits));
                const auto speed=pick(values);
                fire(10,0,behavior==26?std::array<std::uint32_t,3>{{0,i<<6,255u-(i<<5)}}:std::array<std::uint32_t,3>{{i<<6,255u-(i<<5),0}},3,constant(0x6d1fb8),speed,0,0,2);
            }
        }
        if(behavior==26){if(crt_sse2::real(enemy.quad(0x40))<=-20.)enemy.word(0,0);}
        else if(crt_sse2::real(enemy.quad(0x38))< -20.||crt_sse2::real(enemy.quad(0x38))>404.)enemy.word(0,0);
        return;
    }
    case 28:{
        if(signed_word(enemy.word(0xc))<=100){
            enemy.quad(0x40,to_binary64(add(binary64(enemy.quad(0x40)),binary64(enemy.quad(0xa8)),bits)));
            enemy.quad(0xa8,to_binary64(multiply(binary64(enemy.quad(0xa8)),c(0x6d1a20),bits)));
            if(enemy.word(0xc)==100){const auto hp=to_binary64(multiply(integer(enemy.word(0x14)),c(0x6a7708),bits));enemy.word(0x14,stage2_enemy_ceil_integer(hp));}return;
        }
        const auto age=signed_word(enemy.word(0xc));
        if((age>=128&&age<=158)||(age>=224&&age<=256)){
            if(age%8==0){
                const auto speed=to_binary64(add(binary64(pick({constant(0x6d2488),constant(0x6d1ff8),constant(0x6d1948),constant(0x6d1948),0,0})),c(0x6d1868),bits));
                const auto spacing=pick({constant(0x6d1920),constant(0x6d19b8),constant(0x6d1990),constant(0x6d1990),0,0});const auto n=count({5,9,11,11,0,0});
                fire(11,0,{{0,0,255}},n,spacing,speed,constant(0x6d1b60),1,0);
            }
        }else if((age>=180&&age<=200)||(age>=260&&age<=280)){
            if(age%5==0){
                const auto speed=to_binary64(add(binary64(pick({constant(0x6d2488),constant(0x6d1ff8),constant(0x6d1948),constant(0x6d1948),0,0})),c(0x6d1948),bits));
                const auto spacing=pick({constant(0x6d1920),constant(0x6d1990),constant(0x6d1fa8),constant(0x6d1fa8),0,0});const auto n=count({6,10,12,12,0,0});
                fire(11,0,{{255,128,0}},n,spacing,speed,constant(0x6d1b60),1,0);
            }
        }else if(age>=430){
            enemy.quad(0x38,to_binary64(add(binary64(enemy.quad(0x38)),c(0x6d1868),bits)));
            enemy.quad(0x40,to_binary64(subtract(binary64(enemy.quad(0x40)),c(0x6d1ad0),bits)));
            if(crt_sse2::real(enemy.quad(0x40))<=-20.)enemy.word(0,0);
        }return;
    }
    default:throw std::out_of_range("Stage2 first enemy behavior outside 24..28");
    }
}
}
