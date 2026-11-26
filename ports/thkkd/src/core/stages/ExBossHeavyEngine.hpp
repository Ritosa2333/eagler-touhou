#pragma once
#include "core/stages/ExBossHeavyArithmetic.hpp"
#include "core/stages/ExBossHeavyServices.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/math/GameMath.hpp"
namespace kaikeidou { namespace ex_boss_heavy_detail {
struct Engine{
    ProjectilePools& pool;std::uint32_t index;const ObjectRecord<240>& player;const PatternParameters& p;EffectPool& effects;
    Random& random;RandomPrecision precision;GameMath& math;ExBossHeavyServices& services;Arithmetic m;
    Bullet at(std::uint32_t slot){return {pool,slot*240u};}Bullet b(){return at(index);}
    Number q(std::size_t offset){return m.q(b().quad(offset));}void put(std::size_t offset,Number value){b().quad(offset,m.store(value));}
    std::int64_t age(){return signed_word(b().word(12));}
    void effect(){const auto y=b().quad(0x48),x=b().quad(0x40);const auto color=services.color(b().word(24),b().word(28),b().word(32));allocate_effect(effects,pool.sound_triggers,{10,color,x,y});}
    void initialize(std::uint32_t slot,std::uint32_t kind,std::uint32_t delayed_age=0){auto c=at(slot);c.word(0,1);c.word(12,delayed_age);c.word(16,0);c.word(20,0);c.word(4,kind);c.word(44,kind==9?2u:0u);c.word(48,255);c.word(52,1);if(kind!=9)c.word(60,kind==0?0u:1u);c.quad(112,0x3ff0000000000000ull);c.word(56,slot);ex_boss_heavy_recolor(pool,slot,b().word(24),b().word(28),b().word(32));pool.sound_triggers[41]=1;}
    void vertical_line(){const auto color=services.color(b().word(24),b().word(28),b().word(32));const auto y2=b().word(0x98)+16u,x2=b().word(0x9c)+32u,x1=b().word(0x9c)+32u;services.draw_line(x1,16,x2,y2,color,1);if(signed_word(b().word(0x98))<448)b().word(0x98,b().word(0x98)+12u);}
    void horizontal_line(){const bool positive=signed_word(b().word(0x84))>0;const auto color=services.color(b().word(24),b().word(28),b().word(32));const auto y2=b().word(0x9c)+16u,x2=positive?b().word(0x98)+32u:416u-b().word(0x98),y1=b().word(0x9c)+16u;services.draw_line(positive?32u:416u,y1,x2,y2,color,1);if(signed_word(b().word(0x98))<384)b().word(0x98,b().word(0x98)+12u);}
    void normal170();void normal171();void mode1();void mode2();void mode3();void mode4();void mode5();
    void normal172();void normal173();void normal174();void normal175();void normal176();
};
}}
