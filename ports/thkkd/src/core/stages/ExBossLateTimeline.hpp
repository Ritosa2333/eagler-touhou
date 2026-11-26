// 原5C50E0入口、阶段16..25及5E2635尾声；行为更新和菜单实现由外层组合。
#pragma once
#include "core/stages/ExBossLateTimelinePhase16.hpp"
#include "core/stages/ExBossLateTimelinePhase16Marisa.hpp"
#include "core/stages/ExBossLateTimelinePhase16Sanae.hpp"
#include "core/stages/ExBossLateTimelinePhase16SanaeMoves.hpp"
#include "core/stages/ExBossLateTimelinePhase16SanaeFinal.hpp"
#include "core/stages/ExBossLateTimelinePhase17.hpp"
#include "core/stages/ExBossLateTimelinePhase17Marisa.hpp"
#include "core/stages/ExBossLateTimelinePhase17Sanae.hpp"
#include "core/stages/ExBossLateTimelinePhase18.hpp"
#include "core/stages/ExBossLateTimelinePhase19.hpp"
#include "core/stages/ExBossLateTimelinePhase20.hpp"
#include "core/stages/ExBossLateTimelinePhase20Reimu.hpp"
#include "core/stages/ExBossLateTimelinePhase20Marisa.hpp"
#include "core/stages/ExBossLateTimelinePhase20Sanae.hpp"
#include "core/stages/ExBossLateTimelineClosing.hpp"
namespace kaikeidou {
inline void ex_boss_late_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,EffectPool& effects,ObjectRecord<240>& player,
    PatternParameters& p,BossPhaseContext& phase,BossAllocationContext& allocation,EnemyDamageContext& protection,BossStageContext& context,
    StagePatternContext& extra,Stage3BossTimelineContext& extended,EnemyDeathContext& death,const std::uint32_t& character,
    Random& random,RandomPrecision precision,GameMath& math,ExBossLateTimelineServices& services,ItemPool& items,BossPresentationContext& presentation,
    StageResourceContext& background,EffectShake& shake,Stage6RuntimeContext& runtime,PlayerShotFireInput& input,
    const std::array<std::uint32_t,5>& bindings,const PlayerShotFireContext& fire){
    ex_boss_late_timeline_detail::Engine e{timeline,enemies,projectiles,effects,player,p,allocation,protection,context,extra,extended,death,character,random,precision,math,services,items,presentation,background,shake,runtime,input,bindings,fire,phase.boss_index,{{static_cast<unsigned>(precision)}}};
    switch(timeline.subphase){case 16:e.phase16();break;case 17:e.phase17();break;case 18:e.phase18();break;case 19:e.phase19();break;case 20:e.phase20();break;case 21:e.phase21();break;case 22:e.phase22();break;case 23:e.phase23();break;case 24:e.phase24();break;case 25:e.phase25();break;
    default:if(timeline.subphase<=25)throw std::out_of_range("EX boss phase outside late timeline");break;}
}
}
