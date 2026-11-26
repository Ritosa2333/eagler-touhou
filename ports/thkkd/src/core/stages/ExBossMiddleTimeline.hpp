// 原5C50E0入口、阶段9..15及5E2635尾声；其余阶段由主侧组合。
#pragma once
#include "core/stages/ExBossMiddleTimelinePhase9.hpp"
#include "core/stages/ExBossMiddleTimelinePhase10.hpp"
#include "core/stages/ExBossMiddleTimelinePhase11.hpp"
#include "core/stages/ExBossMiddleTimelinePhase11Marisa.hpp"
#include "core/stages/ExBossMiddleTimelinePhase11Sanae.hpp"
#include "core/stages/ExBossMiddleTimelinePhase12.hpp"
#include "core/stages/ExBossMiddleTimelinePhase13.hpp"
#include "core/stages/ExBossMiddleTimelinePhase14.hpp"
#include "core/stages/ExBossMiddleTimelinePhase15.hpp"
namespace kaikeidou {
inline void ex_boss_middle_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,EffectPool& effects,const ObjectRecord<240>& player,
    PatternParameters& p,BossPhaseContext& phase,BossAllocationContext& allocation,EnemyDamageContext& protection,BossStageContext& context,
    StagePatternContext& extra,Stage3BossTimelineContext& extended,EnemyDeathContext& death,const std::uint32_t& character,
    Random& random,RandomPrecision precision,GameMath& math,ExBossMiddleTimelineServices& services,ObjectRecord<1200>& ex_vectors,Effect& effect_prefix){
    ex_boss_middle_timeline_detail::Engine e{timeline,enemies,projectiles,effects,player,p,allocation,protection,context,extra,extended,death,character,random,precision,math,services,ex_vectors,effect_prefix,phase.boss_index,{{static_cast<unsigned>(precision)}}};
    switch(timeline.subphase){case 9:e.phase9();break;case 10:e.phase10();break;case 11:e.phase11();break;case 12:e.phase12();break;case 13:e.phase13();break;case 14:e.phase14();break;case 15:e.phase15();break;
    default:if(timeline.subphase<=25)throw std::out_of_range("EX boss phase outside middle timeline");break;}
}
}
