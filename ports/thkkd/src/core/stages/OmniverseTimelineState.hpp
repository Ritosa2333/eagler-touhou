#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/stages/StageBossServices.hpp"
#include "core/stages/Stage6BossTimelineArithmetic.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/BossAllocationContext.hpp"
#include "core/entities/EnemyDamage.hpp"
#include "core/entities/BossPresentationContext.hpp"
#include "core/player/PlayerLifecycle.hpp"
#include "core/stages/Stage6RuntimeContext.hpp"
#include "core/player/PlayerShotFireContext.hpp"
#include "core/player/PlayerShotPools.hpp"
#include "core/player/ItemPool.hpp"
#include "core/shared/EffectUpdate.hpp"
namespace kaikeidou {
struct OmniverseTimelineState{
 StageTimeline& timeline;EnemyPool& enemies;ProjectilePools& projectiles;EffectPool& effects;ObjectRecord<240>& player;
 const OptionsPool& options;const PlayerShotPools& player_shots;ItemPool& items;PatternParameters& parameters;
 BossPhaseContext& phase;BossAllocationContext& allocation;EnemyDamageContext& protection;BossStageContext& boss;
 StagePatternContext& extra;BossPresentationContext& presentation;LifecycleContext& lifecycle;Stage6RuntimeContext& runtime;EffectShake& shake;
};
struct OmniverseTimelineServices:StageBossServices{
 virtual void music(std::uint32_t track,std::uint32_t mode)=0;
 virtual void draw_line(std::uint32_t x1,std::uint32_t y1,std::uint32_t x2,std::uint32_t y2,std::uint32_t color,std::uint32_t thickness)=0;
 virtual void draw_circle(std::uint32_t x,std::uint32_t y,std::uint32_t radius,std::uint32_t color,std::uint32_t fill)=0;
 virtual void pause_background()=0;virtual void stop_music()=0;
 virtual void return_to_title(bool replay_variant)=0;virtual void initialize_title_visuals()=0;
 virtual std::uint64_t acos_bits(std::uint64_t argument)=0;
};
inline std::uint32_t omniverse_timeline_input(std::uint32_t binding,const PlayerShotFireContext& fire,PlayerShotFireInput& input){
 if(fire.replay_variant&&fire.replay_mode){const auto bit=binding==11?7u:binding;return bit<32?((fire.replay_buttons>>bit)&1u):0u;}return input.read_counter(binding);
}
}
