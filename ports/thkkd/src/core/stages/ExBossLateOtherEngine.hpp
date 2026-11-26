#pragma once
#include "core/stages/ExBossLateOtherArithmetic.hpp"
#include "core/stages/ExBossLateOtherInterpolation.hpp"
#include "core/stages/ExBossLateOtherEmission.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/stages/Stage3BossTimelineContext.hpp"
#include "core/entities/EnemyPool.hpp"
namespace kaikeidou { namespace ex_boss_late_other_detail {
struct Engine{
    ProjectilePools& pool;std::uint32_t index;const ObjectRecord<240>& player;const PatternParameters& p;const EnemyPool& enemies;
    const BossPhaseContext& phase;const BossStageContext& context;const StagePatternContext& extra;const Stage3BossTimelineContext& extended;Random& random;RandomPrecision precision;GameMath& math;Arithmetic m;
    Bullet at(std::uint32_t slot){return {pool,slot*240u};}Bullet b(){return at(index);}Number q(std::size_t offset){return m.q(b().quad(offset));}void put(std::size_t offset,Number value){b().quad(offset,m.store(value));}std::int64_t age(){return signed_word(b().word(12));}
    std::uint64_t real(unsigned i)const{if(i<2)return p.reals[i];if(i<4)return context.extra_reals[i-2];if(i==4)return extra.real_4;if(i<10)return extended.reals_5_to_9[i-5];return extra.reals_10_to_19.at(i-10);}
    std::uint32_t enemy_word(std::uint32_t slot,std::uint32_t offset)const{const auto address=0xa87fe8u+slot*248u+offset;std::uint32_t value=0;for(unsigned i=0;i<4;++i){const auto a=address+i;std::uint8_t byte;if(a>=0xa87ef0u&&a<0xa87fe8u)byte=context.enemy_prefix.data[a-0xa87ef0u];else if(a>=0xa87fe8u&&a<0xa8cd68u){const auto n=a-0xa87fe8u;byte=enemies.enemies[n/248u].data[n%248u];}else if(a>=0xa8cd68u&&a<0xa8ce58u)byte=player.data[a-0xa8cd68u];else throw std::out_of_range("EX late other enemy address");value|=std::uint32_t(byte)<<(i*8);}return value;}
    void transport();
    void directed_outside(std::uint32_t low,std::uint32_t xhigh,std::uint32_t yhigh){if(compare(m.i(0),q(0x58))<=0){if(compare(m.c(yhigh),q(0x48))<0)b().word(0,0);}else if(compare(m.c(low),q(0x48))>0)b().word(0,0);if(compare(m.i(0),q(0x50))<=0){if(compare(m.c(xhigh),q(0x40))<0)b().word(0,0);}else if(compare(m.c(low),q(0x40))>0)b().word(0,0);}

    void outside(std::uint32_t low,std::uint32_t xhigh,std::uint32_t yhigh){if(compare(m.c(low),q(0x40))>0||compare(m.c(xhigh),q(0x40))<0||compare(m.c(low),q(0x48))>0||compare(m.c(yhigh),q(0x48))<0)b().word(0,0);}
    void normal123();void normal124();void normal125();void normal126();void normal127();void normal167();void normal168();void normal169();
    void normal177();void normal178();void normal179();void normal180();void normal181();void normal182();void normal183();void normal184();void normal185();void normal186();void normal187();void normal188();void normal192();void normal193();
};
}}
