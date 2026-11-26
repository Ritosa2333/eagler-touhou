// 原5C50E0入口及阶段0..8；完整26阶段分派由主侧组合。
#pragma once
#include "core/stages/ExBossEarlyTimelineNormals.hpp"
#include "core/stages/ExBossEarlyTimelinePhase2.hpp"
#include "core/stages/ExBossEarlyTimelinePhase4.hpp"
#include "core/stages/ExBossEarlyTimelinePhase6.hpp"
#include "core/stages/ExBossEarlyTimelinePhase7.hpp"
#include "core/stages/ExBossEarlyTimelinePhase8.hpp"
namespace kaikeidou {
inline void ex_boss_early_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,EffectPool& effects,const ObjectRecord<240>& player,
    PatternParameters& p,BossPhaseContext& phase,BossAllocationContext& allocation,EnemyDamageContext& protection,BossStageContext& context,
    StagePatternContext& extra,Stage3BossTimelineContext& extended,EnemyDeathContext& death,const std::uint32_t& character,
    Random& random,RandomPrecision precision,GameMath& math,ExBossEarlyTimelineServices& services,PlayerShotFireInput& input,
    const std::array<std::uint32_t,5>& bindings,const PlayerShotFireContext& fire){
    ex_boss_early_timeline_detail::Engine e{timeline,enemies,projectiles,effects,player,p,allocation,protection,context,extra,extended,death,character,random,precision,math,services,input,bindings,fire,phase.boss_index,{{static_cast<unsigned>(precision)}}};
    switch(timeline.subphase){
    case 0:e.phase0();break;case 1:e.normals(1);break;case 2:e.phase2();break;case 3:e.normals(3);break;case 4:e.phase4();break;
    case 5:e.normals(5);break;case 6:e.phase6();break;case 7:e.phase7();break;case 8:e.phase8();break;
    default:if(timeline.subphase<=25)throw std::out_of_range("EX later boss phase is outside early timeline");break;
    }
}
}
