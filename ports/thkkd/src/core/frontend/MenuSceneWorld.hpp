// 最小宿主可用的状态所有者；生产也可直接提供自身GameStartEntryRefs。
#pragma once
#include "core/frontend/MenuSceneRuntime.hpp"
namespace kaikeidou {
struct MenuSceneWorld {
 ObjectRecord<240> player;EnemyPool enemies;ProjectilePools projectiles;ItemPool items;EffectPool effects;EffectShake shake;
 OptionsPool options;OptionsContext options_context;PlayerShotPools shots;PlayerBombPool bombs;PlayerShotFireContext fire;
 DialogueState dialogue;DialogueContext dialogue_context;LifecycleContext lifecycle;PatternParameters patterns;EnemyDeathContext death;
 StagePatternContext stage_patterns;Stage3BossTimelineContext extended_patterns;Stage4PatternContext stage4_patterns;
 BossAllocationContext boss_allocation;BossPhaseContext boss_phase;BossPresentationContext boss_presentation;BossStageContext boss_stage;EnemyDamageContext protection;Stage6RuntimeContext runtime;
 MusicPresentationContext music;StageTitleContext title;StageResourceContext resources;BackgroundMeshPool meshes;SaveData savedata;Random random{0};MenuStartContext menu;
 GameplayPresentationDispatchContext gameplay;ReplayPauseEntryContext pause;PlayerBombContext bomb_context;
 GameStartEntryGaps gaps;GameStartEntryReplayContext replay;GameStartEntryMachineFrame frame;MenuSceneRuntimeContext scene;
 GameStartEntryRefs references(){return {player,enemies,projectiles,items,effects,shake,options,options_context,shots,bombs,fire,dialogue,dialogue_context,lifecycle,patterns,death,stage_patterns,extended_patterns,stage4_patterns,boss_allocation,boss_phase,boss_presentation,boss_stage,protection,runtime,music,title,resources,meshes,savedata,random,menu,gameplay,pause,bomb_context,scene.selection.practice_selected,scene.selection.weapon};}
 void prepare_frame(bool replay_variant){frame.stack_address=0x1000000;frame.stack.assign(0x10000,0);frame.registers.esp=0x1008000;frame.registers.ebp=0x1008004;fire.replay_variant=replay_variant;boss_phase.replay_variant=replay_variant;dialogue_context.replay_variant=replay_variant;}
};
}
