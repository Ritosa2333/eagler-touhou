#pragma once
#include "core/stages/ExBossMiddleTimelineServices.hpp"
#include "core/stages/ExBossMiddleTimelineArithmetic.hpp"
#include "core/stages/ExBossMiddleTimelineEmission.hpp"
#include "core/stages/Stage1Timeline.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/BossAllocationContext.hpp"
#include "core/entities/EnemyDamage.hpp"
#include "core/entities/EnemyDeathContext.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/stages/Stage3BossTimelineContext.hpp"
#include "core/stages/Stage5BossTimelineSecondaryContact.hpp"
#include "core/stages/Stage6Float3Rotation.hpp"
#include "core/stages/ExBossMiddleTimelineColoredEffect.hpp"
namespace kaikeidou { namespace ex_boss_middle_timeline_detail {
struct Engine{
    StageTimeline& t;EnemyPool& enemies;ProjectilePools& pool;EffectPool& effects;const ObjectRecord<240>& player;
    PatternParameters& p;BossAllocationContext& allocation;EnemyDamageContext& protection;BossStageContext& context;
    StagePatternContext& extra;Stage3BossTimelineContext& extended;EnemyDeathContext& death;const std::uint32_t& character;
    Random& random;RandomPrecision precision;GameMath& math;ExBossMiddleTimelineServices& services;
    ObjectRecord<1200>& ex_vectors;Effect& effect_prefix;
    std::uint32_t boss;Arithmetic m;
    Enemy& enemy(){return boss==0xffffffffu?context.enemy_prefix:enemies.enemies.at(boss);}
    Bullet bullet(std::uint32_t slot){return {pool,slot*240u};}
    Number q(std::size_t offset){return m.q(enemy().quad(offset));}
    void put(std::size_t offset,Number value){enemy().quad(offset,m.store(value));}
    std::uint32_t& integer(unsigned i){if(i<5)return p.integers[i];if(i==5)return death.pattern_integer_5;return extra.integers_6_to_19.at(i-6);}
    std::uint64_t& real(unsigned i){if(i<2)return p.reals[i];if(i<4)return context.extra_reals[i-2];if(i==4)return extra.real_4;if(i<10)return extended.reals_5_to_9[i-5];if(i<20)return extra.reals_10_to_19[i-10];return extended.reals_20_to_29.at(i-20);}
    std::int64_t frame(){return signed_word(t.frame);}
    void reset(){t.frame=0;p.game_frame=0;}
    void costume(std::uint32_t kind){enemy().word(0x2c,kind);enemy().word(0x30,0);enemy().word(0x34,0);}
    void target(std::uint32_t y=0x6d1a90,std::uint32_t rate=0x6d1ae8){enemy().word(8,17);enemy().quad(0xa8,constant(0x6d1a60));enemy().quad(0xb0,constant(y));enemy().quad(0xb8,constant(rate));enemy().quad(0xc0,constant(rate));}
    void effect(std::uint32_t kind,std::array<std::uint32_t,3> color){const auto y=enemy().quad(0x40),x=enemy().quad(0x38);const auto packed=services.color(color[0],color[1],color[2]);allocate_effect(effects,pool.sound_triggers,{kind,packed,x,y});}
    void effect_at(std::uint32_t kind,std::array<std::uint32_t,3> color,std::uint64_t x,std::uint64_t y){const auto packed=services.color(color[0],color[1],color[2]);allocate_effect(effects,pool.sound_triggers,{kind,packed,x,y});}
    void move_target(std::uint32_t y_base=0x6d1d50,std::uint32_t y_random=0x6d2028){enemy().quad(0xa8,player.quad(0x90));const auto y=random.real_number(constant(y_random),precision);put(0xb0,m.add(m.mul(y,m.c(0x6d1960)),m.c(y_base)));if(compare(m.c(0x6d21f0),q(0xa8))>0)enemy().quad(0xa8,constant(0x6d21f0));else if(compare(m.c(0x6d2fa0),q(0xa8))<0)enemy().quad(0xa8,constant(0x6d2fa0));const auto x=random.real_number(constant(0x6d1aa8),precision);put(0xa8,m.add(m.add(x,m.c(0x6d1b58)),q(0xa8)));}
    std::uint32_t orbit_child(std::uint32_t parent,unsigned i,std::array<std::uint32_t,3> color,unsigned variant,bool second,unsigned delayed=1,unsigned kind=1,std::uint32_t radius_address=0){
        const auto slot=static_cast<std::uint32_t>(allocate_projectile(pool,{kind,2,color,0,255,delayed,41}));if(slot==0xffffffffu)return slot;
        auto b=bullet(slot);b.word(0x78,parent);b.word(0x7c,i);b.quad(0xa0,m.store(m.mul(m.i(b.word(0x7c)),m.c(variant==10?0x6d27f8:0x6d22d8))));
        const auto radius=radius_address?constant(radius_address):variant==5?constant((integer(0)==1)!=second?0x6d1a88:0x6d1b38):constant(0x6d1a88);b.quad(0xa8,radius);
        b.quad(0x60,m.store(m.add(m.q(bullet(b.word(0x78)).quad(0x60)),m.q(b.quad(0xa0)))));
        const auto xparent=bullet(b.word(0x78));const auto cosine=math.cosine(b.quad(0x60));b.quad(0x40,m.store(m.add(m.mul(cosine,m.q(b.quad(0xa8))),m.q(xparent.quad(0x40)))));
        const auto yparent=bullet(b.word(0x78));const auto sine=math.sine(b.quad(0x60));b.quad(0x48,m.store(m.add(m.mul(sine,m.q(b.quad(0xa8))),m.q(yparent.quad(0x48)))));
        b.word(8,118);b.word(0x34,1);return slot;
    }
    void link(std::uint32_t first,std::uint32_t second,unsigned numerator,std::uint32_t divisor,std::array<std::uint32_t,3> color,unsigned delayed=1){
        const auto slot=allocate_projectile(pool,{0,3,color,0,255,delayed,41});if(slot==-1)return;auto b=bullet(static_cast<std::uint32_t>(slot));
        b.word(0x78,first);b.word(0x7c,second);b.quad(0xa0,m.store(m.div(m.i(numerator),m.c(divisor))));b.quad(0x40,constant(0x6d2fa8));b.quad(0x48,constant(0x6d2fa8));b.word(8,119);
    }
    void phase9();void phase9_reimu();void phase9_marisa();void phase9_sanae();
    void phase10();void phase11();void phase11_reimu();void phase11_marisa();void phase11_sanae();
    void phase12();void phase13();void phase14();void phase14_reimu();void phase14_marisa();void phase14_sanae();void phase15();
};
}}
