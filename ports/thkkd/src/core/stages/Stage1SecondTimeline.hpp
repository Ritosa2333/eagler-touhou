// 一面part1的完整0x67d7d0..0x67e2cd；Boss分配/配置/演出/退场由同场景服务承接。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/math/GameMath.hpp"

namespace kaikeidou {
namespace stage1_second_detail {
inline std::int64_t signed_word(std::uint32_t value) { return value >> 31 ? std::int64_t(value) - 0x100000000ll : value; }
template<class T> inline T select(std::uint32_t difficulty, const std::array<T,6>& values) { return values[difficulty < 6 ? difficulty : 3]; }
}
inline void stage1_second_segment(StageTimeline& timeline, EnemyPool& enemies, ProjectilePools& projectiles,
    EffectPool& effects, PatternParameters& parameters, BossPhaseContext& phase, BossStageContext& context,
    Random& random, GameMath& math, RandomPrecision precision, BossStageServices& services) {
    using namespace binary_float;
    using stage1_second_detail::signed_word;
    const auto bits = static_cast<unsigned>(precision);
    auto boss = phase.boss_index;
    const auto enemy_at = [&](std::uint32_t index) -> Enemy& { return index==0xffffffffu ? context.enemy_prefix : enemies.enemies[index]; };
    const auto pick = [&](std::array<std::uint32_t,6> values) { return stage1_second_detail::select(parameters.difficulty,values); };
    const auto speed = [&]() { return stage1_second_detail::select(parameters.difficulty,
        std::array<std::uint64_t,6>{{0x3ff199999999999aull,0x3ff6666666666666ull,0x3ffe666666666666ull,0x4000000000000000ull,0,0}}); };
    const auto transition_effect = [&](std::uint64_t y) {
        const auto color = services.color(0,255,0);
        allocate_effect(effects,projectiles.sound_triggers,{12,color,0x4068000000000000ull,y});
        auto& enemy=enemy_at(boss); enemy.word(0x2c,3); enemy.word(0x30,0); enemy.word(0x34,0);
    };
    const auto origin = [&]() { parameters.reals[0]=0x4068000000000000ull; parameters.reals[1]=0x4054000000000000ull; };
    const auto degrees = [&](std::uint64_t value) {
        return to_binary64(divide(multiply(binary64(value),binary64(0x400921fb54442d28ull),bits),integer(180),bits));
    };
    switch(timeline.subphase) {
    case 0:
        if(timeline.frame==1) {
            boss=static_cast<std::uint32_t>(services.allocate_boss(0));
            auto& enemy=enemy_at(boss);
            enemy.word(8,14);enemy.quad(0x38,0x4068000000000000ull);enemy.quad(0x40,0xc03e000000000000ull);
            enemy.quad(0xa8,0x4068000000000000ull);enemy.quad(0xb0,0x4054000000000000ull);
        } else if(timeline.frame==10) services.begin_boss_effect();
        else if(timeline.frame==60) { timeline.subphase=1;timeline.frame=0;parameters.game_frame=0;transition_effect(0x4054000000000000ull); }
        break;
    case 1:
        if(timeline.frame==1) {
            services.configure_phase(0,30,11000,0xfffffff6u);
            auto& enemy=enemy_at(boss);enemy.word(8,15);enemy.word(0xc,0);origin();
            parameters.integers[0]=pick({{40,40,40,40,0,0}});
            parameters.integers[1]=pick({{130,100,70,60,0,0}});
            parameters.integers[2]=pick({{220,160,100,80,0,0}});
            parameters.integers[3]=pick({{310,220,130,100,0,0}});
            parameters.integers[4]=pick({{4,6,12,16,0,0}});
            context.extra_reals[0]=to_binary64(divide(integer(360),add(integer(parameters.integers[4]),integer(1),bits),bits));
            context.extra_reals[1]=speed();
        } else if(timeline.frame==40) enemy_at(boss).word(0x2c,0);
        break;
    case 2:
        if(signed_word(parameters.difficulty)<=1) {
            if(timeline.frame==1) { enemy_at(boss).word(8,14);origin(); }
            else if(timeline.frame==40) transition_effect(0x4054000000000000ull);
            else if(timeline.frame==80) {
                services.configure_phase(0,32,9500,0xfffffff6u);
                auto& enemy=enemy_at(boss);enemy.word(8,16);enemy.word(0xc,150);
                parameters.integers[0]=pick({{4,6,8,8,0,0}});origin();context.extra_reals[1]=speed();enemy.word(0x2c,0);
            }
        } else if(signed_word(timeline.frame)<=32) {
            if(timeline.frame==1) {auto& enemy=enemy_at(boss);enemy.word(8,14);enemy.word(0xc,0);parameters.reals[0]=0x4068000000000000ull;parameters.reals[1]=0x4051800000000000ull;}
            else if(timeline.frame==8) {
                constexpr std::array<std::uint8_t,17> hard{{0xb6,0xc4,0xb7,0xfb,0xa1,0xb8,0xc6,0xe6,0xc5,0xbc,0xba,0xc0,0xb6,0xc4,0xa1,0xb9,0}};
                constexpr std::array<std::uint8_t,21> lunatic{{0xce,0xa3,0xcf,0xd5,0xa1,0xb8,0xb8,0xdf,0xb7,0xe7,0xcf,0xd5,0xb8,0xdf,0xbb,0xd8,0xb1,0xa8,0xa1,0xb9,0}};
                if(parameters.difficulty==2)std::copy(hard.begin(),hard.end(),context.spell_name.begin());
                else if(parameters.difficulty==3)std::copy(lunatic.begin(),lunatic.end(),context.spell_name.begin());
                services.configure_phase(1,40,pick({{0,0,14000,9000,0,0}}),0xffffffffu);
                enemy_at(boss).word(0xc,0);parameters.integers[0]=0;
            } else if(timeline.frame==20) transition_effect(0x4051800000000000ull);
        } else {
            if(timeline.frame==60)enemy_at(boss).word(0x2c,0);
            const auto remainder=signed_word(timeline.frame)%180;
            if(remainder>=60&&remainder<=86) {
                if(remainder==86) {
                    const auto slot=allocate_projectile(projectiles,{2,2,{{255,0,0}},2,255,0,41});
                    if(slot!=-1) {
                        auto& bullet=projectiles.bullets[static_cast<std::size_t>(slot)];bullet.word(8,13);
                        bullet.quad(0x40,enemy_at(boss).quad(0x38));bullet.quad(0x48,enemy_at(boss).quad(0x40));
                        auto angle=to_binary64(add(random.real_number(0x4056800000000000ull,precision),integer(0xffffffd3u),bits));
                        if(random.integer_bits(1,precision,true))angle=to_binary64(add(binary64(angle),integer(180),bits));
                        angle=degrees(angle);
                        bullet.quad(0x50,to_binary64(multiply(math.cosine(angle),integer(15),bits)));
                        bullet.quad(0x58,to_binary64(multiply(math.sine(angle),binary64(0x3fd0000000000000ull),bits)));
                        bullet.word(0xc,86);bullet.word(0x34,1);
                    }
                } else {
                    const auto slot=allocate_projectile(projectiles,{1,1,{{0,0,255}},0,255,0,41});
                    if(slot!=-1) {
                        auto& bullet=projectiles.bullets[static_cast<std::size_t>(slot)];
                        bullet.quad(0x40,enemy_at(boss).quad(0x38));bullet.quad(0x48,enemy_at(boss).quad(0x40));
                        const auto angle=degrees(to_binary64(random.real_number(0x4076700000000000ull,precision)));
                        bullet.quad(0x50,to_binary64(multiply(math.cosine(angle),integer(30),bits)));bullet.quad(0x58,to_binary64(math.sine(angle)));
                        bullet.word(0xc,static_cast<std::uint32_t>(remainder));bullet.word(8,14);
                    }
                }
            }
            if(parameters.difficulty==2) {
                if(remainder==150)for(std::uint32_t i=0;i<=20;++i) {
                    const auto slot=allocate_projectile(projectiles,{0,3,{{255,0,255}},0,255,1,41});
                    if(slot==-1)continue;
                    auto& bullet=projectiles.bullets[static_cast<std::size_t>(slot)];
                    bullet.quad(0x40,enemy_at(boss).quad(0x38));bullet.quad(0x48,enemy_at(boss).quad(0x40));bullet.quad(0x50,0);
                    bullet.quad(0x58,to_binary64(add(multiply(integer(i),binary64(0x3fc1eb851eb851ecull),bits),integer(1),bits)));bullet.word(8,3);
                }
            } else if(signed_word(timeline.frame)%17==0) {
                const auto slot=allocate_projectile(projectiles,{0,3,{{255,0,255}},0,255,0,41});
                if(slot!=-1) {
                    auto& bullet=projectiles.bullets[static_cast<std::size_t>(slot)];bullet.quad(0x40,0x4068000000000000ull);bullet.quad(0x48,0xc034000000000000ull);
                    bullet.quad(0x50,0);bullet.quad(0x58,0x4000000000000000ull);bullet.word(8,3);
                }
            }
        }
        break;
    case 3:
        if(timeline.frame==32) {
            auto& enemy=enemy_at(boss);double x;const auto raw=enemy.quad(0x38);std::memcpy(&x,&raw,8);
            enemy.quad(0xa8,x<192.?0xc059000000000000ull:0x407e400000000000ull);
            enemy.quad(0xb0,0xc064000000000000ull);enemy.quad(0xb8,0x4046800000000000ull);enemy.quad(0xc0,0x4041800000000000ull);
            enemy.word(0xc,0);enemy.word(8,17);
        } else if(timeline.frame==114)services.retire_boss(2);
        else if(timeline.frame==128){++timeline.part;timeline.subphase=0;timeline.frame=0;parameters.game_frame=0;}
        break;
    default:break;
    }
}
}
