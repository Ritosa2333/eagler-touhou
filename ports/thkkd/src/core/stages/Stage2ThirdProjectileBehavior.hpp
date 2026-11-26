// 原二面第三段弹行为26..28；离场检查只在各自年龄阈值之后执行。
#pragma once
#include "core/stages/Stage2ThirdDependenciesConstants.hpp"
#include "core/stages/Stage2EnemyArithmetic.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
#include "core/math/GameMath.hpp"
namespace kaikeidou {
inline void update_stage2_third_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,GameMath& math,RandomPrecision precision){
    using namespace binary_float;using stage2_enemy_detail::subtract;using stage2_enemy_detail::signed_word;using stage2_third_dependencies_detail::constant;
    const auto bits=static_cast<unsigned>(precision);final_projectile_detail::Record bullet{pool,index};const auto c=[](std::uint32_t a){return binary64(constant(a));};const auto age=signed_word(bullet.word(0xc));
    if(behavior<26||behavior>28)throw std::out_of_range("Stage2 third projectile outside 26..28");
    const bool first=behavior==26&&age<=30;const auto angle=behavior==26&&!first?0xb0u:0xa0u,speed=behavior==26&&!first?0xb8u:0xa8u;
    const bool scaled=(behavior==26&&!first&&age<=150)||(behavior==27&&age<=120);
    for(unsigned axis=0;axis<2;++axis){
        auto shift=multiply(axis?math.sine(bullet.quad(angle)):math.cosine(bullet.quad(angle)),binary64(bullet.quad(speed)),bits);
        if(scaled)shift=divide(multiply(shift,integer(behavior==26?bullet.word(0xc)-30u:bullet.word(0xc)),bits),c(0x6d1d00),bits);
        const auto offset=axis?0x48u:0x40u;bullet.quad(offset,to_binary64(add(shift,binary64(bullet.quad(offset)),bits)));
    }
    if(first){bullet.quad(0xa8,to_binary64(multiply(binary64(bullet.quad(0xa8)),c(0x6d18f8),bits)));return;}
    if(scaled||(behavior==28&&age<=40))return;
    if(behavior==26)bullet.quad(0xb0,to_binary64(add(binary64(bullet.quad(0xb0)),c(0x6d1cf8),bits)));
    else if(behavior==27)bullet.quad(0xa0,to_binary64(subtract(binary64(bullet.quad(0xa0)),c(0x6d1cf8),bits)));
    else bullet.quad(0xa0,to_binary64(add(binary64(bullet.quad(0xa0)),binary64(bullet.quad(0xb0)),bits)));
    const auto x=crt_sse2::real(bullet.quad(0x40)),y=crt_sse2::real(bullet.quad(0x48));const auto lower=behavior==28?-64.:-32.,right=behavior==28?448.:416.,bottom=behavior==28?512.:480.;if(x<lower||y<lower||x>right||y>bottom)bullet.word(0,0);
}
}
