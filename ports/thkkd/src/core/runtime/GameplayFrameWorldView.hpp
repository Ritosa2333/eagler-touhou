// 游戏帧只持启动/菜单已拥有对象的引用；保持既有时间轴和服务的分组接口。
#pragma once
#include "core/runtime/GameStartEntryContext.hpp"
namespace kaikeidou {
struct GameplayFrameWorldView {
 ObjectRecord<240>& player;EnemyPool& enemies;ProjectilePools& projectiles;ItemPool& items;EffectPool& effects;EffectShake& shake;
 OptionsPool& options;OptionsContext& options_context;PlayerShotPools& shots;
 struct Boss {BossAllocationContext& allocation;BossPhaseContext& phase;BossPresentationContext& presentation;BossStageContext& stage;EnemyDeathContext& death;EnemyDamageContext& protection;SaveData& savedata;} boss;
 struct Action {PlayerBombPool& bombs;StagePatternContext& stage;} action;
 struct Background {StageResourceContext& context;BackgroundMeshPool& mesh;} background;
 DialogueState& dialogue;Stage3BossTimelineContext& extended_patterns;Stage4PatternContext& stage4_patterns;Stage6RuntimeContext& stage6;
 ObjectRecord<1200>& ex_vectors;MusicPresentationContext& music;StageTitleContext& title;
 PlayerShotFireContext& replay_context;
 std::uint32_t& fire_lock;std::uint32_t& replay_buttons;bool& replay_playback;
 std::array<std::uint32_t,9>& bindings;std::array<std::uint32_t,16>& input;
 GameplayFrameWorldView(GameStartEntryRefs r,ObjectRecord<1200>& vectors,std::uint32_t& lock,std::uint32_t& buttons,bool& playback,std::array<std::uint32_t,9>& keys,std::array<std::uint32_t,16>& counters):
  player(r.player),enemies(r.enemies),projectiles(r.projectiles),items(r.items),effects(r.effects),shake(r.shake),options(r.options),options_context(r.options_context),shots(r.shots),
  boss{r.boss_allocation,r.boss_phase,r.boss_presentation,r.boss_stage,r.death,r.protection,r.savedata},action{r.bombs,r.stage_patterns},background{r.resources,r.meshes},
  dialogue(r.dialogue),extended_patterns(r.extended_patterns),stage4_patterns(r.stage4_patterns),stage6(r.runtime),ex_vectors(vectors),music(r.music),title(r.title),
  replay_context(r.fire),fire_lock(lock),replay_buttons(buttons),replay_playback(playback),bindings(keys),input(counters){}
 std::uint32_t input_value(std::uint32_t binding)const{
  if(replay_context.replay_variant&&replay_context.replay_mode!=0){const auto bit=binding==11?7u:binding;return bit<32?(replay_buttons>>bit)&1u:0u;}
  return binding<input.size()?input[binding]:0u;
 }
};
}
