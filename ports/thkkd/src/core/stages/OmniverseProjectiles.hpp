// 原普通194..207/209..226与副弹208，复用池的地址别名与既有数学。
#pragma once
#include "core/stages/OmniverseProjectileSimple.hpp"
#include "core/stages/OmniverseProjectileEarly.hpp"
#include "core/stages/OmniverseProjectileTargeting.hpp"
#include "core/stages/OmniverseProjectileChains.hpp"
#include "core/stages/OmniverseProjectileMotion.hpp"
#include "core/stages/OmniverseProjectileSpawners.hpp"
#include "core/stages/OmniverseProjectileBounce.hpp"
#include "core/stages/OmniverseProjectileRingOpening.hpp"
#include "core/stages/OmniverseProjectileRingClosing.hpp"
namespace kaikeidou {
inline void update_omniverse_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,EnemyPool& enemies,OmniverseBehaviorContext& context,Random& random,RandomPrecision precision,GameMath& math,OmniverseBehaviorServices& services){
 omniverse_behavior_detail::Engine e{enemies,pool,context,random,precision,math,services,index,behavior,false,{static_cast<unsigned>(precision)}};
 switch(behavior){case 194:e.projectile194();break;case 195:e.projectile195();break;case 197:e.projectile197();break;case 199:case 200:case 201:e.projectile199_201();break;case 207:e.projectile207();break;case 209:e.projectile209();break;case 212:e.projectile212();break;case 213:e.projectile213();break;case 214:e.projectile214();break;case 216:e.projectile216();break;case 219:e.projectile219();break;case 224:e.projectile224();break;case 225:e.projectile225();break;default:e.simple_projectile();break;}
}
}
