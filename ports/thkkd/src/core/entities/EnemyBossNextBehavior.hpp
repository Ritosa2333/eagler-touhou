// 完整恢复61F360与63F4E0，保留32位门槛回绕、颜色回调前坐标缓存和原发射参数。
#pragma once
#include "core/entities/BossStageContext.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/player/PlayerOptions.hpp"
namespace kaikeidou {
inline void update_twenty_second_enemy(Enemy& enemy,RandomPrecision precision){
    using namespace binary_float;const auto working=static_cast<unsigned>(precision);
    if(enemy.word(0x80)==0){
        enemy.quad(0x38,ease_option_value(enemy.quad(0x38),enemy.quad(0xa8),crt_sse2::bits(20.),working));
        enemy.quad(0x40,ease_option_value(enemy.quad(0x40),enemy.quad(0xb0),crt_sse2::bits(20.),working));
    }else enemy.quad(0x38,to_binary64(add(binary64(enemy.quad(0x38)),binary64(enemy.quad(0xb8)),working)));
}
inline void update_twenty_third_enemy(Enemy& enemy,ProjectilePools& projectiles,EffectPool& effects,
    std::uint64_t player_x,std::uint64_t player_y,const PatternParameters& parameters,Random& random,
    RandomPrecision precision,GameMath& math,BossStageServices& services){
    using namespace binary_float;const auto working=static_cast<unsigned>(precision);
    const auto signed_word=[](std::uint32_t value){return value>>31?std::int64_t(value)-0x100000000ll:std::int64_t(value);};
    const auto age=enemy.word(0xc),threshold=parameters.integers[1]+40u;
    if(signed_word(age)<signed_word(threshold)){
        if(age==parameters.integers[1]){
            const auto y=options_detail::stored_bits(enemy.quad(0x40)),x=options_detail::stored_bits(enemy.quad(0x38));
            const auto color=services.color(0,0,255);
            allocate_effect(effects,projectiles.sound_triggers,{12,color,x,y});
            enemy.word(0x2c,3);enemy.word(0x30,0);enemy.word(0x34,0);
        }else{
            enemy.quad(0x38,ease_option_value(enemy.quad(0x38),enemy.quad(0xa8),crt_sse2::bits(20.),working));
            enemy.quad(0x40,ease_option_value(enemy.quad(0x40),enemy.quad(0xb0),crt_sse2::bits(20.),working));
        }
        return;
    }
    if(age==threshold)enemy.word(0x2c,0);
    const auto remainder=signed_word(age)%140;
    if(remainder!=0&&remainder!=12&&remainder!=24&&!(signed_word(parameters.difficulty)>=2&&(remainder==36||remainder==48)))return;
    if(remainder==0){
        const auto sub=[&](Number a,Number b){b.negative=!b.negative;return to_binary64(add(a,b,working));};
        const auto dx=sub(binary64(player_x),binary64(enemy.quad(0x38))),dy=sub(binary64(player_y),binary64(enemy.quad(0x40)));
        enemy.quad(0xb8,to_binary64(math.atan2(dy,dx)));
    }
    const auto difficulty=parameters.difficulty<6?parameters.difficulty:3u;
    constexpr std::array<std::uint32_t,6> counts{{3,3,13,13,0,0}};
    constexpr std::array<std::uint64_t,6> spacing{{0x3fe0a3d70a3d70a4ull,0x3fe0a3d70a3d70a4ull,0x3fc999999999999aull,0x3fc999999999999aull,0,0}};
    ProjectileEmission emission;emission.allocation={1,2,{{0,0,255}},0,255,1,41};
    emission.x=options_detail::stored_bits(enemy.quad(0x38));emission.y=options_detail::stored_bits(enemy.quad(0x40));
    emission.count=counts[difficulty];emission.spacing=spacing[difficulty];emission.speed=crt_sse2::bits(2.);
    emission.angle=options_detail::stored_bits(enemy.quad(0xb8));emission.absolute=1;emission.configuration=0;
    emit_projectiles(projectiles,emission,player_x,player_y,parameters,random,precision,math);
}
}
