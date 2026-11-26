// 完整恢复关底弹17/18/19/20/21/25/160/161/162，保留帧阈值、关联槽别名和原数值落点。
#pragma once
#include "core/entities/ProjectilePattern.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/math/GameMath.hpp"
#include "core/math/FiniteDivide.hpp"
#include "core/shared/FinalProjectileConstants.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
namespace kaikeidou {
inline void update_final_boss_projectile(ProjectilePools& pools,std::uint32_t index,std::uint32_t behavior,
    const ObjectRecord<240>& player,const EnemyPool& enemies,const BossPhaseContext& phase,const Enemy& enemy_prefix,
    const PatternParameters& parameters,Random& random,GameMath& math,RandomPrecision precision){
    using namespace binary_float;using namespace final_projectile_detail;
    Record p{pools,index};const auto bits=static_cast<unsigned>(precision);
    const auto c=[](std::uint32_t a){return constant(a);};const auto n=[&](std::uint32_t a){return binary64(c(a));};
    const auto sub=[&](Number a,Number b){b.negative=!b.negative;return add(a,b,bits);};
    const auto value=[&](std::size_t at){return crt_sse2::real(p.quad(at));};
    const auto move=[&](std::size_t field,std::uint64_t by){p.quad(field,to_binary64(add(binary64(p.quad(field)),binary64(by),bits)));};
    const auto degrees=[&](std::uint64_t a){return final_projectile_degrees(a,bits);};
    const auto pick=[&](std::array<std::uint64_t,6> a){return final_projectile_pick(parameters.difficulty,a);};
    const auto age=[&](){return signed_word(p.word(0xc));};
    switch(behavior){
    case 17:
        if(p.word(0xc)==40){
            if(p.word(0x78)==2){
                final_projectile_recolor(pools,index,0,136,102);
                const auto angle=degrees(p.quad(0xb0));
                const auto speed=to_binary64(multiply(binary64(pick({{c(0x6d1a00),c(0x6d1b28),c(0x6d1ff8),c(0x6d1ff8),0,0}})),n(0x6d1880),bits));
                auto cosine=math.cosine(angle);cosine.negative=!cosine.negative;p.quad(0x50,to_binary64(multiply(cosine,binary64(speed),bits)));
                auto sine=math.sine(angle);sine.negative=!sine.negative;p.quad(0x58,to_binary64(multiply(sine,binary64(speed),bits)));
            }else if(p.word(0x78)==1){
                final_projectile_recolor(pools,index,238,119,0);
                const bool easier=signed_word(parameters.difficulty)<=1;
                const auto angle=degrees(to_binary64(add(random.real_number(c(easier?0x6d1d00:0x6d1aa8),precision),n(easier?0x6b25f8:0x6d1aa8),bits)));
                const auto speed=pick({{c(0x6d1a00),c(0x6d1b28),c(0x6d1ff8),c(0x6d1ff8),0,0}});
                p.quad(0x50,to_binary64(multiply(math.cosine(angle),binary64(speed),bits)));p.quad(0x58,to_binary64(multiply(math.sine(angle),binary64(speed),bits)));p.quad(0x60,angle);
            }else{
                final_projectile_recolor(pools,index,221,204,0);
                const auto dx=to_binary64(sub(binary64(player.quad(0x90)),binary64(p.quad(0x40))));const auto dy=to_binary64(sub(binary64(player.quad(0x98)),binary64(p.quad(0x48))));
                const auto angle=to_binary64(math.atan2(dy,dx));const auto speed=to_binary64(multiply(binary64(pick({{c(0x6d18f0),c(0x6d1f50),c(0x6d1ff8),c(0x6d1ff8),0,0}})),n(0x6d1e40),bits));
                p.quad(0x50,to_binary64(multiply(math.cosine(angle),binary64(speed),bits)));p.quad(0x58,to_binary64(multiply(math.sine(angle),binary64(speed),bits)));p.quad(0x60,angle);
            }
        }else{move(0x40,p.quad(0x50));move(0x48,p.quad(0x58));if(age()>=110&&(value(0x48)>472.||value(0x40)<-24.||value(0x40)>408.||value(0x48)<=-100.))p.word(0,0);}
        break;
    case 18:case 20:
        if(behavior==20)move(0x40,p.quad(0x50));move(0x48,p.quad(0x58));if(value(0x48)>=463.)p.word(0,0);break;
    case 19:
        if(age()>=signed_word(p.word(0x7c))){
            move(0x40,p.quad(0x50));
            if(age()%signed_word(parameters.integers[2])==signed_word(p.word(0x84))){
                const auto slot=allocate_projectile(pools,{0,2,{{128,32,0}},0,255,1,41});
                if(slot!=-1){
                    Record child{pools,static_cast<std::uint32_t>(slot)};
                    child.quad(0x40,to_binary64(sub(add(multiply(random.real_number(c(0x6d1898),precision),n(0x6d1960),bits),binary64(p.quad(0x40)),bits),n(0x6d19c0))));child.quad(0x48,p.quad(0x48));
                    const auto difficulty=parameters.difficulty;const auto vx=p.quad(0x50)^0x8000000000000000ull;
                    const auto xmax=c(difficulty==3?0x6d1d00:difficulty==2?0x6d2780:difficulty==1?0x6d1b38:0x6d1d50);
                    const auto xfactor=add(multiply(random.real_number(xmax,precision),n(0x6a7708),bits),n(0x6d1960),bits);
                    child.quad(0x50,to_binary64(multiply(multiply(xfactor,binary64(vx),bits),n(0x6d19c0),bits)));
                    const auto ymax=c(difficulty==3?0x6d1af8:difficulty==2?0x6d1ec8:difficulty==1?0x6d1858:0x6d1d00);
                    const auto ybase=difficulty==3?n(0x6d19d8):difficulty==2?n(0x6d1880):difficulty==1?n(0x6d18f0):integer(1);
                    child.quad(0x58,to_binary64(add(multiply(random.real_number(ymax,precision),n(0x6a7708),bits),ybase,bits)));child.word(8,20);
                }
            }
        }else if(age()>=signed_word(p.word(0x78))){
            const auto fraction=to_binary64(divide(integer(p.word(0xc)-p.word(0x78)),integer(p.word(0x7c)-p.word(0x78)),bits));
            p.quad(0x40,to_binary64(add(multiply(binary64(p.quad(0x50)),binary64(fraction),bits),binary64(p.quad(0x40)),bits)));
        }
        if(value(0x40)<-64.||value(0x40)>448.)p.word(0,0);break;
    case 21:{
        const auto linked=p.word(0x78);Record other{pools,linked};
        if(p.word(0x7c)==0){
            move(0x40,p.quad(0x50));other.quad(0x40,to_binary64(add(binary64(other.quad(0x40)),binary64(p.quad(0xa0)),bits)));
            const auto distance=final_projectile_absolute(to_binary64(sub(binary64(p.quad(0x40)),binary64(other.quad(0x40)))));
            if(crt_sse2::real(distance)<=48.){p.word(0x7c,1);p.quad(0xc0,p.quad(0x40));p.quad(0xc8,other.quad(0x40));}
        }else{
            p.quad(0xd0,degrees(p.quad(0xa8)));move(0xa8,p.quad(0xd8));
            p.quad(0x40,to_binary64(add(multiply(math.sine(p.quad(0xd0)),binary64(p.quad(0xb0)),bits),binary64(p.quad(0xc0)),bits)));
            other.quad(0x40,to_binary64(add(multiply(math.sine(p.quad(0xd0)),binary64(p.quad(0xb0)),bits),binary64(p.quad(0xc8)),bits)));
        }break;
    }
    case 25:break;
    case 160:
        p.quad(0x40,to_binary64(add(multiply(math.cosine(p.quad(0xa0)),n(0x6d1b20),bits),binary64(p.quad(0xa8)),bits)));move(0xa0,p.quad(0xb0));break;
    case 161:case 162:
        p.quad(0x40,to_binary64(add(multiply(binary64(p.quad(0x50)),binary64(p.quad(0xa8)),bits),binary64(p.quad(0x40)),bits)));
        p.quad(0x48,to_binary64(add(multiply(binary64(p.quad(0x58)),binary64(p.quad(0xa8)),bits),binary64(p.quad(0x48)),bits)));
        if(age()>=180){
            if(p.word(0xc)==180){
                const auto& boss=phase.boss_index==0xffffffffu?enemy_prefix:enemies.enemies[phase.boss_index];
                const auto dx=to_binary64(behavior==161?sub(binary64(player.quad(0x90)),binary64(boss.quad(0x38))):sub(binary64(boss.quad(0x38)),binary64(player.quad(0x90))));
                const auto dy=to_binary64(behavior==161?sub(binary64(player.quad(0x98)),binary64(boss.quad(0x40))):sub(binary64(boss.quad(0x40)),binary64(player.quad(0x98))));
                p.quad(0x60,to_binary64(math.atan2(dy,dx)));auto cos=math.cosine(p.quad(0x60));p.quad(0x50,to_binary64(behavior==161?multiply(cos,n(0x6d1b18),bits):add(cos,cos,bits)));
                auto sin=math.sine(p.quad(0x60));p.quad(0x58,to_binary64(behavior==161?multiply(sin,n(0x6d1b18),bits):add(sin,sin,bits)));p.word(4,11);p.quad(0xa8,0);
            }
            if(value(0xa8)<1.)move(0xa8,c(0x6a7708));
            if(value(0x58)>=0.?value(0x48)>468.:value(0x48)<-20.)p.word(0,0);
            if(value(0x50)>=0.?value(0x40)>404.:value(0x40)<-20.)p.word(0,0);
        }else if(age()>=120)p.quad(0xa8,to_binary64(multiply(binary64(p.quad(0xa8)),n(0x6d1b10),bits)));
        break;
    default:throw std::out_of_range("Final boss projectile behavior outside recovered set");
    }
}
}
