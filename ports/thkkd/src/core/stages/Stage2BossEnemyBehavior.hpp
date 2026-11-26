// 原二面关底敌行为入口，复用既有发射、效果和颜色服务。
#pragma once
#include "core/stages/Stage2BossDependenciesConstants.hpp"
#include "core/stages/Stage2MidbossArithmetic.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/entities/EnemyDeathContext.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
#include "core/entities/ProjectileEmitter.hpp"
#include "core/player/OptionsMotion.hpp"
#include <cstring>
namespace kaikeidou {
inline void update_stage2_boss_enemy(std::uint32_t behavior,Enemy& enemy,ProjectilePools& projectiles,EffectPool& effects,
    std::uint64_t player_x,std::uint64_t player_y,PatternParameters& parameters,BossStageContext& stage,const StagePatternContext& extra,
    Random& random,RandomPrecision precision,GameMath& math,EnemyDeathServices& services,std::uint8_t* next_record_bytes,const std::array<std::uint32_t,3>* original_stack_rgb=nullptr){
    using namespace binary_float;using stage2_midboss_detail::subtract;using stage2_midboss_detail::signed_word;using stage2_boss_dependencies_detail::constant;
    const auto bits=static_cast<unsigned>(precision);const auto c=[](std::uint32_t a){return binary64(constant(a));};

    const auto r=[&](unsigned off){return binary64(enemy.quad(off));};const auto w=[&](unsigned off,Number value){enemy.quad(off,to_binary64(value));};
    const auto emit=[&](ProjectileAllocation allocation,std::uint64_t x,std::uint64_t y,std::uint32_t count,std::uint64_t spacing,std::uint64_t speed,std::uint64_t angle,std::uint32_t absolute,std::uint32_t configuration){emit_projectiles(projectiles,{allocation,x,y,count,spacing,speed,angle,absolute,configuration},player_x,player_y,parameters,random,precision,math);};
    constexpr std::array<std::array<std::uint32_t,3>,12> spectrum{{{{0,0,255}},{{128,0,255}},{{255,0,255}},{{255,0,128}},{{255,0,0}},{{255,128,0}},{{255,255,0}},{{128,255,0}},{{0,255,0}},{{0,255,128}},{{0,255,255}},{{0,128,255}}}};
    if(behavior==37){
        if(enemy.word(0x80)==0){enemy.quad(0x38,ease_option_value(enemy.quad(0x38),enemy.quad(0xa8),constant(0x6d1a40),bits));enemy.quad(0x40,ease_option_value(enemy.quad(0x40),enemy.quad(0xb0),constant(0x6d1a40),bits));
            if(enemy.word(0xc)==40){enemy.word(0x80,1);enemy.word(0xc,0);parameters.reals[0]=0;enemy.word(0x2c,3);enemy.word(0x30,0);enemy.word(0x34,0);}return;
        }
        if(enemy.word(0x80)!=1||parameters.difficulty>3)return;const auto d=parameters.difficulty;const std::uint32_t threshold=d==0?50:d==1?65:d==2?60:70;bool reset=false;
        if(signed_word(enemy.word(0xc))<=threshold){parameters.integers[0]=threshold-enemy.word(0xc);parameters.integers[1]=parameters.integers[0]+40;
            for(unsigned ring=0;ring<=1;++ring){
                const bool flip=enemy.word(0x84)!=0;
                if(d<=1){
                    const auto angle=stage.extra_reals[0];auto value=math.sine(parameters.reals[1]);const auto y=to_binary64(add(multiply(value,binary64(parameters.reals[0]),bits),r(0x40),bits));value=math.cosine(parameters.reals[1]);const auto x=to_binary64(add(multiply(value,binary64(parameters.reals[0]),bits),r(0x38),bits));
                    emit({10,2,flip?std::array<std::uint32_t,3>{{0,255,96}}:std::array<std::uint32_t,3>{{255,96,0}},d==1&&!flip?1u:0u,255,1,41},x,y,1,0,constant(d?0x6d1c80:0x6d1f50),angle,1,2);
                    parameters.reals[1]=to_binary64(flip?subtract(binary64(parameters.reals[1]),c(d?0x6d2b48:0x6d2b68),bits):add(binary64(parameters.reals[1]),c(d?0x6d2b48:0x6d2b68),bits));
                    stage.extra_reals[0]=to_binary64(subtract(binary64(stage.extra_reals[0]),c(flip?0x6d1900:d?0x6d2b40:0x6d2b60),bits));parameters.reals[0]=to_binary64(add(binary64(parameters.reals[0]),c(d?0x6d1880:0x6d2b58),bits));
                }else{
                    for(unsigned side=0;side<2;++side){const auto angle=side?to_binary64(add(binary64(stage.extra_reals[0]),binary64(stage.extra_reals[1]),bits)):stage.extra_reals[0];const auto y=to_binary64(add(c(0x6d22c0),binary64(parameters.reals[0]),bits));const auto value=multiply(math.cosine(parameters.reals[1]),c(0x6d1878),bits);const auto x=to_binary64(side?subtract(r(0x38),value,bits):add(value,r(0x38),bits));
                        const auto color=flip?(side?std::array<std::uint32_t,3>{{0,0,255}}:std::array<std::uint32_t,3>{{255,255,0}}):(side?std::array<std::uint32_t,3>{{0,255,0}}:std::array<std::uint32_t,3>{{255,0,0}});emit({10,2,color,1,255,1,41},x,y,1,0,constant(0x6d1948),angle,1,2);
                    }
                    parameters.reals[0]=to_binary64(add(binary64(parameters.reals[0]),c(0x6d2b38),bits));parameters.reals[1]=to_binary64(add(binary64(parameters.reals[1]),c(0x6d2b48),bits));stage.extra_reals[0]=to_binary64(subtract(binary64(stage.extra_reals[0]),c(0x6d2008),bits));stage.extra_reals[1]=to_binary64(add(binary64(stage.extra_reals[1]),c(0x6d1810),bits));
                }
            }
            if(enemy.word(0xc)==threshold)enemy.word(0x2c,0);
        }
        if(enemy.word(0xc)==(d==0?160u:d==1?100u:110u)){reset=true;if(d<=1)parameters.reals[1]=to_binary64(add(binary64(parameters.reals[1]),c(0x6d2b50),bits));}
        if(reset){enemy.word(0x80,0);enemy.word(0xc,0);enemy.word(0x84,1u-enemy.word(0x84));w(0xa8,add(random.real_number(constant(0x6d1d00),precision),c(0x6d22b0),bits));const auto y=signed_word(parameters.difficulty)<=1?constant(0x6d1858):to_binary64(add(random.real_number(constant(0x6d1ae8),precision),c(0x6b25b8),bits));enemy.quad(0xb0,y);}return;
    }
    if(behavior==38){
        w(0x38,add(multiply(math.cosine(enemy.quad(0xa8)),c(0x6d1a88),bits),c(0x6d1a60),bits));w(0xa8,add(r(0xa8),c(0x6d2b30),bits));
        if(stage2_midboss_detail::remainder(enemy.word(0xc),parameters.integers[1])==0){auto angle=to_binary64(multiply(random.real_number(constant(0x6d2b10),precision),c(0x6a7708),bits));std::array<std::uint32_t,3> color{};bool defined=false;
            for(std::uint32_t i=0;i<=11;++i){parameters.integers[0]=i;const auto selected=stage2_midboss_detail::remainder(i+enemy.word(0x8c),12);
                if(selected>=0){color=spectrum[static_cast<std::size_t>(selected)];defined=true;}else if(!defined){if(!original_stack_rgb)throw std::runtime_error("Missing original enemy38 stack RGB");color=*original_stack_rgb;defined=true;}
                const auto speed=stage.extra_reals[1];const auto spacing=enemy.word(0x98)?to_binary64(divide(c(0x6d2208),integer(enemy.word(0x98)),bits)):0x7ff0000000000000ull;const auto n=enemy.word(0x98);emit({0,0,color,0,255,0,41},enemy.quad(0x38),enemy.quad(0x40),n,spacing,speed,angle,1,30);
                angle=to_binary64(subtract(binary64(angle),multiply(integer(parameters.integers[2]),binary64(extra.real_4),bits),bits));
            }
            enemy.word(0x8c,enemy.word(0x8c)+1u);parameters.integers[2]=0u-parameters.integers[2];
        }
        if(stage2_midboss_detail::remainder(enemy.word(0xc),enemy.word(0x90))==99){enemy.word(0x94,enemy.word(0x94)+1u);
            for(std::uint32_t i=0;i<=11;++i){const auto angle=to_binary64(multiply(integer(i+enemy.word(0x94)),c(0x6d2140),bits));emit({1,0,spectrum[i],2,255,1,41},enemy.quad(0x38),enemy.quad(0x40),1,0,constant(0x6d1ff8),angle,0,0);
                if(signed_word(parameters.difficulty)>=2){const auto second=to_binary64(multiply(integer(i+enemy.word(0x94)),c(0x6d2140),bits));emit({1,0,spectrum[i],2,255,1,41},enemy.quad(0x38),enemy.quad(0x40),1,0,constant(0x6d1b18),second,0,0);}
            }
        }return;
    }
    const auto effect=[&](std::uint32_t red,std::uint32_t green,std::uint32_t blue){const auto y=enemy.quad(0x40),x=enemy.quad(0x38);const auto color=services.color(red,green,blue);allocate_effect(effects,projectiles.sound_triggers,{12,color,x,y});};
    if(behavior==39){
        const auto adjacent=[&](unsigned off){std::uint64_t value;std::memcpy(&value,next_record_bytes+off,8);return value;};
        const auto adjacent_write=[&](unsigned off,Number value){const auto raw=to_binary64(value);std::memcpy(next_record_bytes+off,&raw,8);};
        if(enemy.word(0x80)==0){enemy.quad(0x38,ease_option_value(enemy.quad(0x38),adjacent(0),constant(0x6d1a40),bits));enemy.quad(0x40,ease_option_value(enemy.quad(0x40),adjacent(8),constant(0x6d1a40),bits));
            if(enemy.word(0xc)==20)effect(0,255,255);else if(enemy.word(0xc)==59){enemy.word(0x80,1);enemy.word(0xc,0xffffffffu);enemy.word(0x2c,3);enemy.word(0x30,0);enemy.word(0x34,0);}return;
        }
        if(enemy.word(0x80)==1){const bool easy=parameters.difficulty==0;
            for(unsigned side=0;side<(easy?1u:2u);++side){const bool first=!easy&&side==0;const auto slot=allocate_projectile(projectiles,{9,0,first?std::array<std::uint32_t,3>{{0,255,96}}:std::array<std::uint32_t,3>{{0,96,255}},2,255,1,41});if(slot==-1)continue;
                auto& bullet=projectiles.bullets[slot];if(easy&&crt_sse2::real(enemy.quad(0x38))<192.)final_projectile_recolor(projectiles,slot,0,255,96);
                auto angle=first?to_binary64(add(r(0xa8),c(0x6d1860),bits)):enemy.quad(0xa8);auto value=multiply(math.cosine(angle),c(first?0x6d1878:0x6d1d78),bits);bullet.quad(0x40,to_binary64(first?subtract(r(0x38),value,bits):add(value,r(0x38),bits)));
                angle=first?to_binary64(add(r(0xa8),c(0x6d1860),bits)):enemy.quad(0xa8);value=multiply(math.sine(angle),c(first?0x6d1878:0x6d1d78),bits);bullet.quad(0x48,to_binary64(first?subtract(r(0x40),value,bits):add(value,r(0x40),bits)));
                bullet.word(0xc,enemy.word(0xc)-(easy?32u:42u));bullet.word(8,31);bullet.quad(0xc8,enemy.quad(0x38));bullet.quad(0xd0,enemy.quad(0x40));bullet.quad(0xa0,first?to_binary64(add(r(0xa8),c(0x6d1860),bits)):enemy.quad(0xa8));
                const bool odd=(enemy.word(0xc)&1)!=0;if(first){bullet.quad(0xa8,constant(odd?0x6d1a08:0x6d2350));bullet.quad(0xc0,constant(odd?0x6d2358:0x6d2348));}
                else{const bool variant=easy?!odd:odd;bullet.quad(0xa8,constant(variant?0x6d2370:0x6d2368));bullet.quad(0xc0,constant(variant?0x6d1d28:0x6d2360));}
                bullet.quad(0xb0,constant(first?0x6d19a8:0x6d1d78));bullet.quad(0xb8,constant(first?0x6d2340:0x6d2280));bullet.word(first?0x78:0x7c,first?0:1);
            }
            w(0xa8,add(r(0xa8),c(easy?0x6d1ba0:0x6d2120),bits));if(enemy.word(0xc)==(easy?31u:41u)){enemy.word(0x80,2);enemy.word(0xc,0xffffffffu);enemy.word(0x2c,0);}return;
        }
        if(enemy.word(0x80)==2&&enemy.word(0xc)==enemy.word(0x84)){enemy.word(0x80,0);enemy.word(0xc,0xffffffffu);w(0xa8,multiply(random.real_number(constant(0x6d1c60),precision),c(0x6d1950),bits));adjacent_write(0,integer(crt_sse2::real(adjacent(0))<192.?242:142));adjacent_write(0,add(add(random.real_number(constant(0x6d1a40),precision),c(0x6d22c0),bits),binary64(adjacent(0)),bits));adjacent_write(8,add(random.real_number(constant(0x6d1ae8),precision),c(0x6d1d00),bits));}return;
    }
    if(behavior==40){
        if(enemy.word(0x80)==0){enemy.quad(0x38,ease_option_value(enemy.quad(0x38),constant(0x6d1a60),constant(0x6d1a40),bits));enemy.quad(0x40,ease_option_value(enemy.quad(0x40),constant(0x6d1e78),constant(0x6d1a40),bits));const auto age=enemy.word(0xc);
            if(age==20)effect(255,128,0);
            else if(signed_word(age)>=60&&signed_word(age)<=174){if(age==60){enemy.word(0x2c,3);enemy.word(0x30,0);enemy.word(0x34,0);}else if(age==174)enemy.word(0x2c,0);
                if(stage2_midboss_detail::remainder(age,6)==0)for(std::uint32_t i=0;i<=4;++i)for(unsigned side=0;side<2;++side){const auto slot=allocate_projectile(projectiles,{1,1,{{255,128,0}},0,255,1,41});if(slot==-1)continue;auto& bullet=projectiles.bullets[slot];bullet.word(8,32);bullet.quad(0xa0,0);bullet.quad(0xa8,0);const auto value=multiply(integer(i),c(0x6d2b28),bits);bullet.quad(0xb0,to_binary64(side?add(value,c(0x6d1da0),bits):subtract(c(0x6d1d78),value,bits)));bullet.quad(0x40,bullet.quad(0xb0));bullet.quad(0x48,to_binary64(add(multiply(integer(age-66u),c(0x6d2b20),bits),integer(i*5u),bits)));bullet.word(0xc,(0u-(age-60u))*3u-120u);bullet.word(0x34,1);}
            }else if(age==210)effect(0,255,255);else if(age==249){enemy.word(0x80,1);enemy.word(0xc,0xffffffffu);}return;
        }
        w(0x38,add(multiply(math.sine(enemy.quad(0xa8)),c(0x6d1cc0),bits),c(0x6d1a60),bits));w(0xa8,add(r(0xa8),r(0xb0),bits));if(crt_sse2::real(enemy.quad(0xb0))<=crt_sse2::real(constant(0x6d1c98)))w(0xb0,add(r(0xb0),c(0x6b2620),bits));if(stage2_midboss_detail::remainder(enemy.word(0xc),10)!=0)return;
        const auto angle=to_binary64(multiply(random.real_number(constant(0x6d1c60),precision),c(0x6d1950),bits));const auto radius=to_binary64(random.real_number(constant(0x6d1878),precision));const auto firing=to_binary64(add(multiply(random.real_number(constant(0x6d1f60),precision),c(0x6d1950),bits),c(0x6d2b18),bits));const auto base=to_binary64(subtract(r(0x40),c(0x6d18e8),bits));const auto y=to_binary64(add(multiply(math.sine(angle),binary64(radius),bits),binary64(base),bits));const auto x=to_binary64(add(multiply(math.cosine(angle),binary64(radius),bits),r(0x38),bits));emit({4,2,{{0,255,255}},0,255,1,41},x,y,1,0,constant(0x6d1f50),firing,1,0);return;
    }
    if(behavior==41){
        if(enemy.word(0x80)==0){enemy.quad(0x38,ease_option_value(enemy.quad(0x38),constant(0x6d1a60),constant(0x6d1a40),bits));enemy.quad(0x40,ease_option_value(enemy.quad(0x40),constant(0x6d1f98),constant(0x6d1a40),bits));
            if(enemy.word(0xc)==40)effect(0,255,255);if(enemy.word(0xc)==79){enemy.quad(0x38,constant(0x6d1a60));enemy.quad(0x40,constant(0x6d1f98));enemy.word(0x80,1);enemy.word(0xc,0xffffffffu);}return;
        }
        enemy.quad(0x40,ease_option_value(enemy.quad(0x40),constant(0x6d1ae8),constant(0x6d1a78),bits));w(0x38,add(multiply(math.sine(enemy.quad(0xa8)),c(0x6d1c58),bits),c(0x6d1a60),bits));w(0xa8,add(r(0xa8),c(0x6d1a08),bits));
        const auto residue=stage2_midboss_detail::remainder(enemy.word(0xc),10);if(residue!=0&&residue!=5)return;const bool second=residue==5;
        const auto slot=allocate_projectile(projectiles,{4,0,second?std::array<std::uint32_t,3>{{0,255,96}}:std::array<std::uint32_t,3>{{0,96,255}},0,255,1,41});if(slot==-1)return;auto& bullet=projectiles.bullets[slot];
        bullet.word(8,33);bullet.word(0x78,0);bullet.quad(0xa0,to_binary64(multiply(r(0xa8),c(0x6d1c18),bits)));bullet.quad(0xa8,constant(second?0x6d2330:0x6d2338));
        bullet.quad(0xb0,to_binary64(add(multiply(random.real_number(constant(0x6d1a40),precision),c(0x6a7708),bits),c(0x6d22a0),bits)));
        bullet.quad(0xb8,to_binary64(add(multiply(random.real_number(constant(second?0x6d1af8:0x6d1ab0),precision),c(0x6d1950),bits),c(second?0x6d1f50:0x6d18f0),bits)));
        bullet.quad(0xc0,constant(0x6d20c8));bullet.quad(0x40,to_binary64(add(multiply(math.sine(bullet.quad(0xa0)),c(0x6d1fe8),bits),c(0x6d1a60),bits)));
        bullet.quad(0x48,to_binary64(subtract(c(0x6d1a48),multiply(random.real_number(constant(0x6d1d88),precision),c(0x6d1960),bits),bits)));return;
    }
    if(behavior==42){
        if(enemy.word(0xc)==40){const auto angle=to_binary64(multiply(random.real_number(constant(0x6d2b10),precision),c(0x6a7708),bits));const bool easy=parameters.difficulty==0;emit({1,0,{{0,255,255}},0,255,1,41},enemy.quad(0x38),enemy.quad(0x40),easy?6:8,constant(easy?0x6d2750:0x6d22d8),constant(0x6d1ad0),angle,1,34);enemy.word(0x2c,3);enemy.word(0x30,0);enemy.word(0x34,0);}
        if(signed_word(parameters.difficulty)>=2&&signed_word(enemy.word(0xc))>=680){if(enemy.word(0xc)==680)effect(255,0,0);
            if(stage2_midboss_detail::remainder(enemy.word(0xc),180)==179){const auto angle=to_binary64(multiply(random.real_number(constant(0x6d2b10),precision),c(0x6a7708),bits));
                for(std::uint32_t i=0;i<=3;++i){parameters.integers[1]=i;const auto spacing=enemy.word(0x80)?to_binary64(divide(c(0x6d2208),integer(enemy.word(0x80)),bits)):0x7ff0000000000000ull;emit({11,0,{{255,0,0}},2,255,1,41},enemy.quad(0x38),enemy.quad(0x40),enemy.word(0x80),spacing,constant(0x6d1ad8),angle,1,36);}
            }
        }
        parameters.reals[0]=to_binary64(add(binary64(parameters.reals[0]),c(0x6a7888),bits));return;
    }
    if(behavior!=36)throw std::out_of_range("Stage2 boss enemy not implemented");
    enemy.quad(0x38,to_binary64(add(multiply(math.cosine(enemy.quad(0xa8)),c(0x6d1a88),bits),c(0x6d1a60),bits)));
    enemy.quad(0xa8,to_binary64(add(binary64(enemy.quad(0xa8)),c(0x6d2b30),bits)));
    enemy.quad(0x40,ease_option_value(enemy.quad(0x40),constant(0x6d1a90),constant(0x6d1898),bits));
    if(enemy.word(0x80)!=1||stage2_midboss_detail::remainder(enemy.word(0xc),parameters.integers[1]))return;
    constexpr std::array<std::array<std::uint32_t,3>,12> colors{{{{0,0,255}},{{128,0,255}},{{255,0,255}},{{255,0,128}},{{255,0,0}},{{255,128,0}},{{255,255,0}},{{128,255,0}},{{0,255,0}},{{0,255,128}},{{0,255,255}},{{0,128,255}}}};
    for(std::uint32_t ring=0;signed_word(ring)<=signed_word(parameters.integers[2]);++ring){parameters.integers[0]=ring;
        for(std::uint32_t color=0;color<=11;++color){const auto angle=to_binary64(add(multiply(integer(color),c(0x6d2140),bits),binary64(enemy.quad(0xb0)),bits));
            ProjectileEmission input{{11,0,colors[color],2,255,0,41},enemy.quad(0x38),enemy.quad(0x40),1,0,stage.extra_reals[1],angle,1,29};
            emit_projectiles(projectiles,input,player_x,player_y,parameters,random,precision,math);
        }
    }
    enemy.quad(0xb0,to_binary64(add(binary64(enemy.quad(0xb0)),binary64(stage.extra_reals[0]),bits)));
}
}
