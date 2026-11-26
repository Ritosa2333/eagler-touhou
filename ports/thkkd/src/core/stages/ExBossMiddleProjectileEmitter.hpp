// 发射入口保留玩家状态引用，原函数进入后才读取瞄准坐标。
#pragma once
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
inline void ex_boss_middle_projectile_emit(ProjectilePools& pool,const ProjectileEmission& input,const ObjectRecord<240>& player,
 const PatternParameters& parameters,Random& random,RandomPrecision precision,GameMath& math){
 emit_projectiles(pool,input,player.quad(0x90),player.quad(0x98),parameters,random,precision,math);
}
}
