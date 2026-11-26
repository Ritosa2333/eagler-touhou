// 恢复完整弹行为12..14，保留停顿帧、反弹顺序及共享方向对Boss槽的即时写入。
#pragma once
#include "core/entities/ProjectilePool.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/player/PlayerOptions.hpp"

namespace kaikeidou {
inline void update_boss_projectile_motion(Projectile& projectile,std::uint32_t behavior,EnemyPool& enemies,
    const BossPhaseContext& phase,PatternParameters& parameters,unsigned precision){
    using namespace binary_float;
    const auto add_to=[&](std::size_t field,std::uint64_t value){projectile.quad(field,to_binary64(add(binary64(projectile.quad(field)),binary64(value),precision)));};
    const auto multiply_by=[&](std::size_t field,std::uint64_t value){projectile.quad(field,to_binary64(multiply(binary64(projectile.quad(field)),binary64(value),precision)));};
    const auto age=options_detail::signed_word(projectile.word(0xc));
    if(behavior==12){
        if(age==80){
            projectile.word(0x18,255);projectile.word(0x1c,170);projectile.word(0x20,0);
            projectile.quad(0x50,options_detail::stored_bits(projectile.quad(0xa0)));
            projectile.quad(0x58,options_detail::stored_bits(projectile.quad(0xa8)));
            projectile.quad(0x60,options_detail::stored_bits(projectile.quad(0xb0)));
            return;
        }
        add_to(0x40,projectile.quad(0x50));add_to(0x48,projectile.quad(0x58));
        if(age<80){multiply_by(0x50,projectile.quad(0xb8));multiply_by(0x58,projectile.quad(0xb8));}
        else{
            const auto x=crt_sse2::real(projectile.quad(0x40)),y=crt_sse2::real(projectile.quad(0x48));
            if(y>468.||x<=-100.||x>=484.||y<=-150.)projectile.word(0,0);
        }
        return;
    }
    if(behavior!=13&&behavior!=14)throw std::out_of_range("Boss projectile behavior outside12..14");
    if(age<=180){
        add_to(0x40,projectile.quad(0x50));add_to(0x48,projectile.quad(0x58));
        const auto x=crt_sse2::real(projectile.quad(0x40));
        if(x<0.)projectile.quad(0x50,projectile.quad(0x50)&0x7fffffffffffffffull);
        else if(x>384.)projectile.quad(0x50,projectile.quad(0x50)|0x8000000000000000ull);
        if(crt_sse2::real(projectile.quad(0x48))<0.)multiply_by(0x58,0xbff0000000000000ull);
        if(age>=160){multiply_by(0x50,0x3fee666666666666ull);multiply_by(0x58,0x3fee666666666666ull);}
    }else if(age<=200){
        const auto x=crt_sse2::real(projectile.quad(0x40));
        if(behavior==13&&age==182){
            parameters.integers[0]=x<192.?0u:1u;
            if(parameters.difficulty==3){
                auto& enemy=enemies.enemies.at(phase.boss_index);enemy.word(0xc,0);
                enemy.quad(0xa8,to_binary64(integer(parameters.integers[0]*200u+92u)));
                enemy.quad(0xb0,0x4051800000000000ull);
            }
        }else if(behavior==14&&age==200&&((parameters.integers[0]==0&&x<192.)||(parameters.integers[0]!=0&&x>=192.))){
            projectile.word(4,2);projectile.word(0xc,201);projectile.word(0x2c,2);projectile.word(8,13);
            add_to(0x48,0xc014000000000000ull);
        }
    }else{
        add_to(0x48,behavior==13?0x4014000000000000ull:0x400c000000000000ull);
        if(crt_sse2::real(projectile.quad(0x48))>480.)projectile.word(0,0);
    }
}
}
