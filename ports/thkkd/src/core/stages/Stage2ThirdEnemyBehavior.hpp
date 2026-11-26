// 原二面第三段敌行为30..35，保留选择器调用、HP缩减和分配失败后的循环。
#pragma once
#include "core/stages/Stage2EnemyArithmetic.hpp"
#include "core/stages/Stage2ThirdDependenciesConstants.hpp"
#include "core/stages/Stage2MidbossArithmetic.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
inline void update_stage2_third_enemy(std::uint32_t behavior,Enemy& enemy,ProjectilePools& pool,
    std::uint64_t player_x,std::uint64_t player_y,PatternParameters& parameters,Random& random,RandomPrecision precision,GameMath& math){
    using namespace binary_float;using stage2_enemy_detail::subtract;using stage2_enemy_detail::signed_word;
    using stage2_third_dependencies_detail::constant;const auto bits=static_cast<unsigned>(precision);
    const auto c=[](std::uint32_t a){return binary64(constant(a));};
    const auto pick=[&](std::array<std::uint64_t,6> v){return stage2_enemy_pick_real(parameters.difficulty,v);};
    const auto count=[&](std::array<std::uint32_t,6> v){return stage2_enemy_pick_integer(parameters.difficulty,v);};
    const auto fire=[&](std::uint32_t kind,std::array<std::uint32_t,3> color,std::uint32_t n,std::uint64_t spacing,std::uint64_t speed,std::uint32_t absolute){
        ProjectileEmission in{{kind,0,color,0,255,1,41},enemy.quad(0x38),enemy.quad(0x40),n,spacing,speed,enemy.quad(0xb0),absolute,0};
        emit_projectiles(pool,in,player_x,player_y,parameters,random,precision,math);
    };
    const auto reduce_hp=[&](){const auto value=to_binary64(multiply(integer(enemy.word(0x14)),c(0x6a7708),bits));enemy.word(0x18,stage2_enemy_ceil_integer(value));enemy.word(0x14,enemy.word(0x18));};
    if(behavior==30||behavior==31){
        enemy.quad(0x38,to_binary64(add(binary64(enemy.quad(0x38)),binary64(enemy.quad(0xa8)),bits)));
        enemy.quad(0xa8,to_binary64(behavior==30?add(binary64(enemy.quad(0xa8)),c(0x6d1968),bits):subtract(binary64(enemy.quad(0xa8)),c(0x6d1968),bits)));
        if(behavior==30){if(crt_sse2::real(enemy.quad(0x38))>414.)enemy.word(0,0);if(!(crt_sse2::real(enemy.quad(0x38))<384.))return;}
        else{if(crt_sse2::real(enemy.quad(0x38))< -30.)enemy.word(0,0);if(!(crt_sse2::real(enemy.quad(0x38))>0.))return;}
        if(stage2_midboss_detail::remainder(enemy.word(0xc),count({60,38,30,20,0,0})))return;
        if(behavior==30){
            const auto speed=pick({constant(0x6d1948),constant(0x6d1948),constant(0x6d1948),constant(0x6d1948),0,0});
            const auto spacing=to_binary64(divide(c(0x6d2208),integer(count({1,2,3,4,0,0})),bits));const auto n=count({1,2,3,4,0,0});
            fire(1,{{255,0,0}},n,spacing,speed,0);enemy.quad(0xb0,to_binary64(add(binary64(enemy.quad(0xb0)),c(0x6d1920),bits)));
        }else{
            for(std::uint32_t i=0;signed_word(i)<=signed_word(count({1,2,3,3,0,0}));++i){
                const auto base=pick({constant(0x6d2548),constant(0x6d1ce0),constant(0x6d2150),constant(0x6d26e0),0,0});
                const auto speed=to_binary64(multiply(binary64(base),add(multiply(integer(i),c(0x6d18e0),bits),c(0x6d18e0),bits),bits));
                fire(1,{{0,0,255}},1,0,speed,0);
            }
            enemy.quad(0xb0,to_binary64(subtract(binary64(enemy.quad(0xb0)),c(0x6d1920),bits)));
        }return;
    }
    if(behavior==32){
        enemy.quad(0x40,to_binary64(add(add(integer(1),binary64(enemy.quad(0xa8)),bits),binary64(enemy.quad(0x40)),bits)));
        enemy.quad(0xa8,to_binary64(multiply(binary64(enemy.quad(0xa8)),c(0x6d2b70),bits)));
        if(crt_sse2::real(enemy.quad(0x40))>478.)enemy.word(0,0);
        const auto age=signed_word(enemy.word(0xc));if(age>=61&&age<448&&stage2_midboss_detail::remainder(enemy.word(0xc),count({95,54,38,26,0,0}))==0){
            const auto speed=pick({0x3ff0000000000000ull,constant(0x6d1c80),constant(0x6d2130),constant(0x6d1ce8),0,0});fire(11,{{0,255,0}},3,constant(0x6d2138),speed,1);
        }
        enemy.quad(0xb0,to_binary64(add(binary64(enemy.quad(0xb0)),c(0x6d19c0),bits)));return;
    }
    if(behavior<33||behavior>35)throw std::out_of_range("Stage2 third enemy outside 30..35");
    enemy.quad(0x40,to_binary64(add(binary64(enemy.quad(0x40)),binary64(enemy.quad(0xa8)),bits)));
    enemy.quad(0xa8,to_binary64(multiply(binary64(enemy.quad(0xa8)),c(behavior==34?0x6d2390:0x6d18f0),bits)));
    const auto age=signed_word(enemy.word(0xc));if(age>=300){enemy.quad(0x40,to_binary64(subtract(binary64(enemy.quad(0x40)),c(0x6d1ad8),bits)));if(crt_sse2::real(enemy.quad(0x40))< -30.)enemy.word(0,0);return;}
    if(age<(behavior==35?70:90)||age>(behavior==33?195:behavior==34?240:260))return;
    const auto period=behavior==35?enemy.word(0x80):count(behavior==33?std::array<std::uint32_t,6>{{30,15,15,15,0,0}}:std::array<std::uint32_t,6>{{16,8,8,8,0,0}});
    if(stage2_midboss_detail::remainder(enemy.word(0xc),period))return;
    if(behavior==33&&age==90){
        reduce_hp();const auto dx=to_binary64(subtract(binary64(player_x),binary64(enemy.quad(0x38)),bits));const auto dy=to_binary64(subtract(binary64(player_y),binary64(enemy.quad(0x40)),bits));enemy.quad(0xb0,to_binary64(math.atan2(dy,dx)));
        enemy.quad(0xb8,constant(0x6d1910));enemy.quad(0xc0,pick({constant(0x6d1a00),constant(0x6d1c80),constant(0x6d1948),constant(0x6d1ce8),0,0}));
    }else if(behavior==34&&enemy.word(0x80)==0){enemy.word(0x80,1);reduce_hp();enemy.quad(0xb0,constant(0x6d1b60));}
    else if(behavior==35&&enemy.word(0x84)==0){enemy.word(0x84,1);reduce_hp();}
    const auto angle=to_binary64(divide(random.real_number(constant(0x6d2378),precision),c(0x6d1e50),bits));
    const auto n=count(behavior==33?std::array<std::uint32_t,6>{{16,24,32,40,0,0}}:behavior==34?std::array<std::uint32_t,6>{{8,10,16,22,0,0}}:std::array<std::uint32_t,6>{{9,15,19,23,0,0}});
    for(unsigned ring=0;ring<(behavior==35?2u:1u);++ring)for(std::uint32_t i=0;signed_word(i)<signed_word(n);++i){
        const std::array<std::uint32_t,3> color=behavior==33?std::array<std::uint32_t,3>{{0,255,255}}:behavior==34?std::array<std::uint32_t,3>{{255,255,0}}:ring?std::array<std::uint32_t,3>{{0,0,255}}:std::array<std::uint32_t,3>{{255,0,0}};
        const auto slot=allocate_projectile(pool,{9,0,color,2,255,1,41});if(slot==-1)continue;auto& bullet=pool.bullets[slot];
        if(behavior==34){
            bullet.quad(0x40,to_binary64(add(multiply(math.cosine(enemy.quad(0xb0)),c(0x6d1c58),bits),binary64(enemy.quad(0x38)),bits)));
            const auto y=to_binary64(subtract(binary64(enemy.quad(0x40)),c(0x6d1c58),bits));bullet.quad(0x48,to_binary64(add(multiply(math.sine(enemy.quad(0xb0)),c(0x6d1c58),bits),binary64(y),bits)));
        }else{bullet.quad(0x40,enemy.quad(0x38));bullet.quad(0x48,enemy.quad(0x40));}
        bullet.word(0xc,0);bullet.word(8,behavior-7);
        if(behavior==33){bullet.quad(0xa0,enemy.quad(0xb0));bullet.quad(0xa8,enemy.quad(0xb8));}
        const auto direction=to_binary64(add(multiply(divide(c(0x6d2208),integer(n),bits),integer(i),bits),binary64(angle),bits));bullet.quad(behavior==33?0xb0:0xa0,direction);
        if(behavior==33)bullet.quad(0xb8,enemy.quad(0xc0));else bullet.quad(0xa8,enemy.quad(behavior==34?0xb8:0xb0));
        if(behavior==35)bullet.quad(0xb0,ring?constant(0x6d1ab8):0);
    }
    if(behavior==33)enemy.quad(0xb0,to_binary64(add(binary64(enemy.quad(0xb0)),c(0x6d1b60),bits)));
    else if(behavior==34)enemy.quad(0xb0,to_binary64(subtract(binary64(enemy.quad(0xb0)),c(signed_word(parameters.difficulty)>=1?0x6d2388:0x6d2380),bits)));
}
}
