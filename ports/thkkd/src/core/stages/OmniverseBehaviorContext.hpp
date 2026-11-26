#pragma once
#include "core/entities/ProjectilePattern.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/shared/EffectUpdate.hpp"
#include "core/stages/StageResourceContext.hpp"
#include "core/stages/StageBossServices.hpp"
#include "core/math/GameMath.hpp"
namespace kaikeidou {
struct OmniverseBehaviorContext {
 EffectPool& effects;Effect& effect_prefix;EffectShake& shake;ObjectRecord<240>& player;
 PatternParameters& parameters;BossStageContext& boss;StageResourceContext& background;
};
struct OmniverseBehaviorServices:StageBossServices {virtual void kill_enemy(std::uint32_t index)=0;};
}
