// 在原发射函数入口之后读取玩家坐标，保留回调修改的共享现场。
#pragma once
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
inline void stage6_boss_timeline_emit(ProjectilePools& pools,const ProjectileEmission& input,const ObjectRecord<240>& player,
 const PatternParameters& parameters,Random& random,RandomPrecision precision,GameMath& math){
 emit_projectiles(pools,input,player.quad(0x90),player.quad(0x98),parameters,random,precision,math);
}
}
