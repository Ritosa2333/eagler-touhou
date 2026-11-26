#pragma once
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
inline void omniverse_timeline_emit(ProjectilePools& pool,const ProjectileEmission& emission,const ObjectRecord<240>& player,const PatternParameters& parameters,Random& random,RandomPrecision precision,GameMath& math){emit_projectiles(pool,emission,player.quad(144),player.quad(152),parameters,random,precision,math);}
}
