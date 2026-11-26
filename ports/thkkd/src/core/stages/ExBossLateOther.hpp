// EX其余后段22个普通弹行为；行为分派值与对象内部字段分开。
#pragma once
#include "core/stages/ExBossLateOtherOrbit.hpp"
#include "core/stages/ExBossLateOtherTransport.hpp"
#include "core/stages/ExBossLateOtherMotion.hpp"
#include "core/stages/ExBossLateOther182.hpp"
#include "core/stages/ExBossLateOtherSpawn.hpp"
namespace kaikeidou {
inline void update_ex_boss_late_other_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,const ObjectRecord<240>& player,const PatternParameters& parameters,const EnemyPool& enemies,const BossPhaseContext& phase,const BossStageContext& context,const StagePatternContext& extra,const Stage3BossTimelineContext& extended,Random& random,RandomPrecision precision,GameMath& math){
    ex_boss_late_other_detail::Engine e{pool,index,player,parameters,enemies,phase,context,extra,extended,random,precision,math,{{static_cast<unsigned>(precision)}}};
    switch(behavior){case 123:e.normal123();break;case 124:e.normal124();break;case 125:e.normal125();break;case 126:e.normal126();break;case 127:e.normal127();break;case 167:e.normal167();break;case 168:e.normal168();break;case 169:e.normal169();break;case 177:e.normal177();break;case 178:e.normal178();break;case 179:e.normal179();break;case 180:e.normal180();break;case 181:e.normal181();break;case 182:e.normal182();break;case 183:e.normal183();break;case 184:e.normal184();break;case 185:e.normal185();break;case 186:e.normal186();break;case 187:e.normal187();break;case 188:e.normal188();break;case 192:e.normal192();break;case 193:e.normal193();break;default:throw std::out_of_range("EX late other behavior outside batch");}
}
}
