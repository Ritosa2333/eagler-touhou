// 原63E110..63E5E5：二面中Boss敌行为29，保留两轮六色循环及参数重复写入。
#pragma once
#include "core/stages/Stage2MidbossArithmetic.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
#include "core/player/OptionsMotion.hpp"
namespace kaikeidou {
inline void update_stage2_midboss_enemy(Enemy& enemy,ProjectilePools& projectiles,std::uint64_t player_x,std::uint64_t player_y,
    PatternParameters& parameters,const BossStageContext& stage,Random& random,RandomPrecision precision,GameMath& math){
    using namespace binary_float;using namespace stage2_midboss_detail;const auto bits=static_cast<unsigned>(precision);
    const auto c=[](std::uint32_t a){return binary64(constant(a));};
    enemy.quad(0x38,to_binary64(add(multiply(math.cosine(enemy.quad(0xa8)),c(0x6d1a88),bits),c(0x6d1a60),bits)));
    enemy.quad(0xa8,to_binary64(add(binary64(enemy.quad(0xa8)),c(0x6d2b30),bits)));
    enemy.quad(0x40,ease_option_value(enemy.quad(0x40),constant(0x6d18c0),constant(0x6d1898),bits));
    if(enemy.word(0x80)!=1||remainder(enemy.word(0xc),parameters.integers[1])!=0)return;
    constexpr std::array<std::array<std::uint32_t,3>,6> colors{{{{0,0,255}},{{255,0,255}},{{255,0,0}},{{255,255,0}},{{0,255,0}},{{0,255,255}}}};
    const auto fire=[&](unsigned color,std::uint64_t angle,std::uint64_t speed,bool first){
        ProjectileEmission input{{first?11u:1u,first?2:1,colors[color],first?2u:0u,255,first?0u:1u,41},enemy.quad(0x38),enemy.quad(0x40),1,0,speed,angle,first?1u:0u,first?24u:0u};
        emit_projectiles(projectiles,input,player_x,player_y,parameters,random,precision,math);
    };
    for(std::uint32_t ring=0;ring<=4;++ring){
        parameters.integers[0]=0u-ring*4u;
        for(unsigned color=0;color<=5;++color){
            const auto angle=to_binary64(add(multiply(integer(color),c(0x6d2750),bits),binary64(enemy.quad(0xb0)),bits));
            fire(color,angle,stage.extra_reals[1],true);
        }
    }
    enemy.word(0x84,enemy.word(0x84)+1u);
    const auto threshold=midboss_pick_integer(parameters.difficulty,{{4,4,5,5,0,0}});
    if(enemy.word(0x84)==threshold){
        enemy.word(0x84,0);enemy.word(0x88,enemy.word(0x88)+1u);
        const auto end=midboss_pick_integer(parameters.difficulty,{{0,0,3,4,0,0}});
        auto ring=midboss_pick_integer(parameters.difficulty,{{0,0,0xfffffffeu,0xfffffffdu,0,0}});
        for(;signed_word(ring)<=signed_word(end);++ring){
            for(unsigned color=0;color<=5;++color){
                const auto angle=to_binary64(add(multiply(integer(color+enemy.word(0x88)),c(0x6d2750),bits),multiply(integer(ring),c(0x6d1fa8),bits),bits));
                const auto base=midboss_pick_real(parameters.difficulty,{{constant(0x6d1c80),constant(0x6d1948),constant(0x6d1ce8),constant(0x6d1b18),0,0}});
                const auto speed=to_binary64(add(binary64(base),multiply(integer(ring),c(0x6d1920),bits),bits));
                fire(color,angle,speed,false);
            }
        }
    }
    enemy.quad(0xb0,to_binary64(add(binary64(enemy.quad(0xb0)),binary64(stage.extra_reals[0]),bits)));
}
}
