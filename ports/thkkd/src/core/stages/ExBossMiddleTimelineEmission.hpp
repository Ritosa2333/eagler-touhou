#pragma once
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
template<class RandomSource> inline void ex_boss_middle_timeline_emit(ProjectilePools& pool,const ProjectileEmission& a,const ObjectRecord<240>& player,const PatternParameters& p,RandomSource& random,RandomPrecision precision,GameMath& math){emit_projectiles(pool,a,player.quad(0x90),player.quad(0x98),p,random,precision,math);}
}
