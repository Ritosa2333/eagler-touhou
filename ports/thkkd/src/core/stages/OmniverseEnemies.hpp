// 原敌139..150，年龄由外部调度递增。
#pragma once
#include "core/stages/OmniverseEnemySimple.hpp"
#include "core/stages/OmniverseEnemyEmitters.hpp"
#include "core/stages/OmniverseEnemyBounce.hpp"
#include "core/stages/OmniverseEnemyWaves.hpp"
#include "core/stages/OmniverseEnemyTargets.hpp"
namespace kaikeidou {
inline void update_omniverse_enemy(EnemyPool& enemies,std::uint32_t index,std::uint32_t behavior,ProjectilePools& pool,OmniverseBehaviorContext& context,Random& random,RandomPrecision precision,GameMath& math,OmniverseBehaviorServices& services){
 omniverse_behavior_detail::Engine e{enemies,pool,context,random,precision,math,services,index,behavior,true,{static_cast<unsigned>(precision)}};
 switch(behavior){case 139:e.enemy139();break;case 140:case 145:e.simple_enemy();break;case 141:e.enemy141();break;case 142:e.enemy142();break;case 143:e.enemy143();break;case 144:e.enemy144();break;case 146:case 147:case 148:case 149:case 150:e.enemy146_150();break;default:throw std::out_of_range("Omniverse enemy behavior");}
}
}
