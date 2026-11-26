// 原二面中Boss弹行为22、23、24；Boss槽-1使用既有敌池前置记录。
#pragma once
#include "core/stages/Stage2MidbossArithmetic.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/math/GameMath.hpp"
#include "core/math/FiniteSqrt.hpp"
namespace kaikeidou {
inline void update_stage2_midboss_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,const EnemyPool& enemies,
    const BossPhaseContext& phase,const ObjectRecord<248>& enemy_prefix,const PatternParameters& parameters,GameMath& math,RandomPrecision precision){
    using namespace binary_float;using namespace stage2_midboss_detail;const auto bits=static_cast<unsigned>(precision);final_projectile_detail::Record bullet{pool,index};
    const auto c=[](std::uint32_t a){return binary64(constant(a));};
    if(behavior==22||behavior==23){
        const auto& boss=phase.boss_index==0xffffffffu?enemy_prefix:enemies.enemies.at(phase.boss_index);
        const auto dx=to_binary64(subtract(binary64(boss.quad(0x38)),binary64(bullet.quad(0x40)),bits));
        const auto dy=to_binary64(subtract(binary64(boss.quad(0x40)),binary64(bullet.quad(0x48)),bits));
        const auto angle=to_binary64(math.atan2(dy,dx));
        const auto square=to_binary64(add(multiply(binary64(dx),binary64(dx),bits),multiply(binary64(dy),binary64(dy),bits),bits));
        const auto distance=to_binary64(finite_square_root(square,bits));
        const auto forward=behavior==22?0x40u:0x48u;const auto transverse=behavior==22?0x48u:0x40u;
        bullet.quad(forward,to_binary64(add(binary64(bullet.quad(forward)),binary64(parameters.reals[1]),bits)));
        const auto direction=behavior==22?math.sine(angle):math.cosine(angle);
        const auto shift=multiply(multiply(direction,binary64(distance),bits),binary64(bullet.quad(0xa0)),bits);
        bullet.quad(transverse,to_binary64(crt_sse2::real(behavior==22?dx:dy)>=0.?add(shift,binary64(bullet.quad(transverse)),bits):subtract(binary64(bullet.quad(transverse)),shift,bits)));
        if(crt_sse2::real(bullet.quad(forward))>=crt_sse2::real(constant(behavior==22?0x6d1d10:0x6d1d08)))bullet.word(0,0);
        return;
    }
    if(behavior!=24)throw std::out_of_range("Stage2 midboss projectile outside 22..24");
    const auto age=signed_word(bullet.word(0xc));
    if(age>=0){
        bullet.quad(0x60,to_binary64(add(divide(multiply(binary64(bullet.quad(0xa8)),binary64(bullet.quad(0x68)),bits),c(0x6d1c90),bits),binary64(bullet.quad(0xa0)),bits)));
        if(age<=55){
            bullet.quad(0xa8,to_binary64(add(binary64(bullet.quad(0xa8)),binary64(bullet.quad(0xb0)),bits)));
            bullet.quad(0xb0,to_binary64(multiply(binary64(bullet.quad(0xb0)),c(0x6d1b08),bits)));
        }else if(age<=70){
            bullet.quad(0xa8,to_binary64(subtract(binary64(bullet.quad(0xa8)),binary64(bullet.quad(0xb0)),bits)));
            bullet.quad(0xb0,to_binary64(multiply(binary64(bullet.quad(0xb0)),binary64(bullet.quad(0xb8)),bits)));
        }else{
            bullet.quad(0xa8,to_binary64(subtract(binary64(bullet.quad(0xa8)),binary64(bullet.quad(0xb0)),bits)));
            bullet.quad(0xb0,to_binary64(add(binary64(bullet.quad(0xb0)),c(0x6a7888),bits)));
        }
        const auto cosine=math.cosine(bullet.quad(0x60));bullet.quad(0x40,to_binary64(add(multiply(cosine,binary64(bullet.quad(0x68)),bits),binary64(bullet.quad(0x40)),bits)));
        const auto sine=math.sine(bullet.quad(0x60));bullet.quad(0x48,to_binary64(add(multiply(sine,binary64(bullet.quad(0x68)),bits),binary64(bullet.quad(0x48)),bits)));
    }
    const auto x=crt_sse2::real(bullet.quad(0x40)),y=crt_sse2::real(bullet.quad(0x48));if(x< -32.||y< -32.||x>416.||y>480.)bullet.word(0,0);
}
}
