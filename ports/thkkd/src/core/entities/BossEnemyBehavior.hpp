// 完整恢复敌人行为14..17；行为0对应61f690空函数，计时及动画由原调用方另行推进。
#pragma once
#include "core/entities/BossStageContext.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/player/OptionsMotion.hpp"

namespace kaikeidou {
namespace boss_enemy_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:v;}
inline binary_float::Number subtract(binary_float::Number a,binary_float::Number b,unsigned bits){b.negative=!b.negative;return binary_float::add(a,b,bits);}
inline void ease(Enemy& enemy,unsigned offset,std::uint64_t target,std::uint64_t divisor,RandomPrecision precision){
    enemy.quad(offset,ease_option_value(enemy.quad(offset),target,divisor,static_cast<unsigned>(precision)));
}
inline void move(Enemy& enemy,const PatternParameters& parameters,std::uint64_t divisor,RandomPrecision precision){
    ease(enemy,0x38,parameters.reals[0],divisor,precision);ease(enemy,0x40,parameters.reals[1],divisor,precision);
}
inline std::uint64_t to_degrees(std::uint64_t angle,unsigned bits){using namespace binary_float;return to_binary64(divide(multiply(binary64(angle),integer(180),bits),binary64(0x400921fb54442d28ull),bits));}
inline std::uint64_t to_radians(std::uint64_t angle,unsigned bits){using namespace binary_float;return to_binary64(divide(multiply(binary64(angle),binary64(0x400921fb54442d28ull),bits),integer(180),bits));}
inline void choose_target(Enemy& enemy,std::uint64_t player_x,PatternParameters& parameters,Random& random,RandomPrecision precision,bool sixteenth){
    using namespace binary_float;const auto bits=static_cast<unsigned>(precision);
    enemy.word(0xc,0);
    const auto px=crt_sse2::real(player_x);const auto clamped=px<120.?120.:(px>264.?264.:px);
    if(crt_sse2::real(enemy.quad(0x38))>clamped){
        const auto shift=random.real_number(crt_sse2::bits(80.),precision);
        parameters.reals[0]=to_binary64(add(subtract(binary64(enemy.quad(0x38)),shift,bits),integer(20),bits));
    }else{
        const auto shift=random.real_number(crt_sse2::bits(80.),precision);
        parameters.reals[0]=to_binary64(subtract(add(shift,binary64(enemy.quad(0x38)),bits),integer(20),bits));
    }
    if(crt_sse2::real(parameters.reals[0])<60.)parameters.reals[0]=to_binary64(add(random.real_number(crt_sse2::bits(30.),precision),integer(60),bits));
    else if(crt_sse2::real(parameters.reals[0])>324.)parameters.reals[0]=to_binary64(subtract(integer(324),random.real_number(crt_sse2::bits(30.),precision),bits));
    parameters.reals[1]=to_binary64(add(random.real_number(crt_sse2::bits(sixteenth?40.:70.),precision),integer(sixteenth?35u:40u),bits));
}
inline void fire(Enemy& enemy,ProjectilePools& projectiles,std::uint64_t player_x,std::uint64_t player_y,
    const PatternParameters& parameters,const BossStageContext& context,RandomPrecision precision,GameMath& math,bool sixteenth){
    using namespace binary_float;const auto bits=static_cast<unsigned>(precision);
    const auto dx=to_binary64(subtract(binary64(player_x),binary64(enemy.quad(0x38)),bits));
    const auto dy=to_binary64(subtract(binary64(player_y),binary64(enemy.quad(0x40)),bits));
    const auto aim=to_binary64(math.atan2(dy,dx));
    const auto vx=to_binary64(multiply(math.cosine(aim),binary64(context.extra_reals[1]),bits));
    const auto vy=to_binary64(multiply(math.sine(aim),binary64(context.extra_reals[1]),bits));
    const auto degrees=to_degrees(aim,bits);
    for(unsigned ring=0;ring<(sixteenth?2u:1u);++ring){
        for(std::uint32_t i=0;signed_word(i)<=signed_word(parameters.integers[sixteenth?0:4]);++i){
            const auto slot=allocate_projectile(projectiles,{10,0,{{0,255,10}},0,255,1,41});
            if(slot==-1)continue;
            auto& bullet=projectiles.bullets[static_cast<std::size_t>(slot)];bullet.word(8,12);
            bullet.quad(0x40,enemy.quad(0x38));bullet.quad(0x48,enemy.quad(0x40));
            Number offset;
            if(sixteenth){const auto value=signed_word(i*360u)/signed_word(parameters.integers[0]+1u);offset=integer(static_cast<std::uint32_t>(value));}
            else offset=multiply(integer(i),binary64(context.extra_reals[0]),bits);
            const auto angle=to_radians(to_binary64(add(offset,binary64(degrees),bits)),bits);
            bullet.quad(0x60,angle);
            const auto cos=math.cosine(angle);
            bullet.quad(0x50,to_binary64(sixteenth?multiply(cos,binary64(crt_sse2::bits(3.5)),bits):add(cos,cos,bits)));
            const auto sin=math.sine(angle);
            bullet.quad(0x58,to_binary64(sixteenth?multiply(sin,binary64(crt_sse2::bits(3.5)),bits):add(sin,sin,bits)));
            bullet.quad(0xa0,vx);bullet.quad(0xa8,vy);bullet.quad(0xb0,aim);
            bullet.quad(0xb8,sixteenth?(ring?0x3fef851eb851eb85ull:0x3fedc28f5c28f5c3ull):0x3fefae147ae147aeull);bullet.word(0xc,0);
        }
    }
}
}
inline void update_fourteenth_enemy(Enemy& enemy,RandomPrecision precision){
    boss_enemy_detail::ease(enemy,0x38,enemy.quad(0xa8),crt_sse2::bits(20.),precision);
    boss_enemy_detail::ease(enemy,0x40,enemy.quad(0xb0),crt_sse2::bits(20.),precision);
}
inline void update_seventeenth_enemy(Enemy& enemy,RandomPrecision precision){
    boss_enemy_detail::ease(enemy,0x38,enemy.quad(0xa8),enemy.quad(0xb8),precision);
    boss_enemy_detail::ease(enemy,0x40,enemy.quad(0xb0),enemy.quad(0xc0),precision);
}
inline void update_fifteenth_enemy(Enemy& enemy,ProjectilePools& projectiles,std::uint64_t player_x,std::uint64_t player_y,
    PatternParameters& parameters,const BossStageContext& context,Random& random,RandomPrecision precision,GameMath& math){
    boss_enemy_detail::move(enemy,parameters,crt_sse2::bits(30.),precision);
    const auto age=enemy.word(0xc);
    if(age==parameters.integers[0]||age==parameters.integers[1]||age==parameters.integers[2])
        boss_enemy_detail::fire(enemy,projectiles,player_x,player_y,parameters,context,precision,math,false);
    else if(age==parameters.integers[3])boss_enemy_detail::choose_target(enemy,player_x,parameters,random,precision,false);
}
inline void update_sixteenth_enemy(Enemy& enemy,ProjectilePools& projectiles,std::uint64_t player_x,std::uint64_t player_y,
    PatternParameters& parameters,const BossStageContext& context,Random& random,RandomPrecision precision,GameMath& math){
    using namespace binary_float;const auto bits=static_cast<unsigned>(precision);
    boss_enemy_detail::move(enemy,parameters,crt_sse2::bits(boss_enemy_detail::signed_word(enemy.word(0xc))>=150?18.:30.),precision);
    const auto age=enemy.word(0xc);
    if(age==150||age==200)boss_enemy_detail::fire(enemy,projectiles,player_x,player_y,parameters,context,precision,math,true);
    else if(age==155){
        if(crt_sse2::real(parameters.reals[0])<=192.)parameters.reals[0]=to_binary64(add(random.real_number(crt_sse2::bits(80.),precision),integer(242),bits));
        else parameters.reals[0]=to_binary64(boss_enemy_detail::subtract(integer(142),random.real_number(crt_sse2::bits(80.),precision),bits));
        parameters.reals[1]=to_binary64(add(random.real_number(crt_sse2::bits(40.),precision),integer(35),bits));
    }else if(age==270)boss_enemy_detail::choose_target(enemy,player_x,parameters,random,precision,true);
}
inline void update_boss_enemy(std::uint32_t behavior,Enemy& enemy,ProjectilePools& projectiles,std::uint64_t player_x,std::uint64_t player_y,
    PatternParameters& parameters,const BossStageContext& context,Random& random,RandomPrecision precision,GameMath& math){
    switch(behavior){case 0:return;case 14:update_fourteenth_enemy(enemy,precision);return;case 15:update_fifteenth_enemy(enemy,projectiles,player_x,player_y,parameters,context,random,precision,math);return;
    case 16:update_sixteenth_enemy(enemy,projectiles,player_x,player_y,parameters,context,random,precision,math);return;case 17:update_seventeenth_enemy(enemy,precision);return;default:throw std::out_of_range("Boss enemy behavior outside recovered range");}
}
}
