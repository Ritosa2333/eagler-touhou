// 旧连续探针局部Context的生产引用组装，关卡函数只由GameplayFrame执行。
#pragma once
#include "core/runtime/GameplayFrameRuntime.hpp"
#include "core/runtime/GameplayHostServices.hpp"
#include "core/stages/ExBossEarlyTimelineRuntimeServices.hpp"
#include "core/stages/ExBossMiddleTimelineRuntimeServices.hpp"
#include "core/stages/ExBossControlRuntimeServices.hpp"
#include "core/stages/ExBossHeavyRuntimeServices.hpp"
#include "core/stages/OmniverseWorldRuntimeServices.hpp"
#include "core/runtime/GameplayHudTailRuntime.hpp"
#include <functional>
namespace kaikeidou {
struct GameplayHostHudTransfer {GameplayHudTailOutcome outcome;};
class GameplayHostPhysicalInput final:public PlayerShotFireInput {
 GameplayHostPlatform& platform_;
public:explicit GameplayHostPhysicalInput(GameplayHostPlatform& p):platform_(p){}std::uint32_t read_counter(std::uint32_t b)override{return platform_.physical(b);}
};
class GameplayHostLogicalInput final:public PlayerShotFireInput,public ItemMotionInput {
 GameplayFrameWorldView& world_;GameplayHostPlatform& platform_;
public:GameplayHostLogicalInput(GameplayFrameWorldView&w,GameplayHostPlatform&p):world_(w),platform_(p){}
 std::uint32_t read_counter(std::uint32_t b)override{if(world_.replay_context.replay_variant&&world_.replay_context.replay_mode){const auto bit=b==11?7u:b;return bit<32?(world_.replay_buttons>>bit)&1u:0u;}return platform_.physical(b);}
};
class GameplayHostCollision final:public CollisionServices {
 unsigned precision_;
public:explicit GameplayHostCollision(unsigned bits):precision_(bits){}
 std::uint32_t special_collision(ObjectRecord<240>& player,ProjectilePools& pools,CollisionContext&)override{return stage5_boss_secondary_contact(pools,player,precision_);}
};
struct GameplayHostContext {
 void render_gameplay(){platform.platform.present_gameplay();}
 void finish_gameplay_render(){platform.platform.finish_gameplay();}

 using U=std::uint32_t;
 GameStartEntryRefs game;GameplayFrameWorldView* state;GameStartEntryMemory& gameplay_memory;MenuSceneRuntimeContext& menu;
 GameStartEntryMachineFrame& machine;GameplayFramePresentationRegisters& presentation;
 PlayerShotFireContext& firing;LifecycleContext& lifecycle;StageTimeline timeline;Random& random;RandomPrecision precision;
 GameMath& math;PatternParameters& parameters;DialogueContext& dialogue_context;PlayerBombContext& bomb_context;CollisionContext& collision;
 GameplayHostPlatform platform;GameplayHostPresentation graphics;GameplayHostServices death_services;GameplayHostServices& player_services;
 GameplayHostFirst services;GameplayHostPhysicalInput physical;GameplayHostLogicalInput fire_input;GameplayHostLogicalInput& midboss_input;GameplayHostLogicalInput& item_input;
 ReplayGameplayInputCounterAdapter<GameStartEntryMemory> replay_counter;
 GameplayHostTitleReturn title_return;GameplayHostLateTimeline ex_late_services;
 ExBossEarlyTimelineRuntimeServices<GameplayHostServices> ex_boss_services;
 ExBossMiddleTimelineRuntimeServices<GameplayHostServices> ex_middle_services;
 ExBossControlRuntimeServices ex_control_services;ExBossHeavyRuntimeServices<GameplayHostServices> ex_heavy_services;
 OmniverseWorldRuntimeServices<GameplayHostServices,GameplayHostLateTimeline> omni_services;
 GameplayHostEnemies enemy_services;OmniverseBehaviorContext omni_context;
 GameplayHostCollision collision_services;ItemPickupServices pickup_services;PlayerShotHitContext hit;
 GameplayHostServices& title_resources;GameplayHostMeshes mesh_services;GameplayHostBossSprites sprite_services;
 GameplayHostPresentation& title_graphics;GameplayHostPresentation& ending_fade_services;
 bool replay_world=true,boss_world=true,resource_world=true,extended_world=true,full_six_world=true,ex_world=true,omniverse_world=true,action_world=true,dialogue_world=true,presentation_world=true;
 GameplayHudTailOutcome hud_outcome;
 std::function<void()> move_player,present_fps;
 GameplayHostContext(GameStartEntryRefs refs,GameplayFrameWorldView& world,GameStartEntryMemory& memory,GameStartEntryMachineFrame& frame,MenuSceneRuntimeContext& scene,CollisionContext& collisions,MenuScenePlatform& api,GameMath& functions,unsigned bits,GameplayFramePresentationRegisters& registers):
  game(refs),state(&world),gameplay_memory(memory),menu(scene),machine(frame),presentation(registers),firing(refs.fire),lifecycle(refs.lifecycle),random(refs.random),precision(static_cast<RandomPrecision>(bits)),math(functions),parameters(refs.patterns),dialogue_context(refs.dialogue_context),bomb_context(refs.bomb_context),collision(collisions),platform(memory,frame,api),graphics(platform,refs.shake,bits),death_services(refs,world,timeline,platform,graphics,functions,precision,scene),player_services(death_services),services(death_services),physical(platform),fire_input(world,platform),midboss_input(fire_input),item_input(fire_input),replay_counter(memory,refs.fire,physical),title_return(death_services),ex_late_services(death_services,title_return),ex_boss_services(death_services),ex_middle_services(death_services),ex_heavy_services(death_services),omni_services(death_services,ex_late_services),enemy_services(death_services),omni_context{refs.effects,refs.runtime.effect_prefix,refs.shake,refs.player,refs.patterns,refs.boss_stage,refs.resources},collision_services(bits),title_resources(death_services),mesh_services(platform,refs.meshes),sprite_services(platform,graphics,refs.shake,bits),title_graphics(graphics),ending_fade_services(graphics){
  timeline.frame=refs.patterns.game_frame;timeline.part=refs.lifecycle.part;timeline.subphase=refs.lifecycle.subphase;timeline.difficulty=refs.lifecycle.difficulty;
  memory.bind(0xb3e2e8,timeline.frame);memory.bind(0xa77390,timeline.part);memory.bind(0xa733c0,timeline.subphase);memory.bind(0xa7e610,timeline.difficulty);
  bind_replay_gameplay_input(memory,refs.fire,refs.dialogue_context);
  move_player=[this,bits](){update_replay_gameplay_movement(gameplay_memory,physical,firing.replay_variant,bits);};
  present_fps=[this,bits](){gameplay_memory.reconcile();hud_outcome=present_gameplay_hud_tail(gameplay_memory,machine,platform.platform,bits);if(hud_outcome.kind!=GameplayHudTailKind::completed)throw GameplayHostHudTransfer{hud_outcome};presentation.ecx=machine.registers.ecx;presentation.known=(hud_outcome.exact_registers&2u)!=0;const auto average=menu.timing.data.quad(8);const auto ecx=present_menu_fps(menu.timing,menu.title,572,465,machine.registers.ecx,bits,graphics,firing.replay_variant);if((average&0x7fffffffffffffffull)!=0){presentation.ecx=ecx;presentation.known=true;}};
  ex_boss_services.pixel=[this](U x,U y,U ink){platform.call(0x41e7b0,{x,y,ink});};
  ex_middle_services.line=[this](U x1,U y1,U x2,U y2,U ink,U width){platform.call(0x42d190,{x1,y1,x2,y2,ink,width});};
  ex_control_services.circle=[this](U x,U y,U r,U ink,U fill){return platform.call(0x41e460,{x,y,r,ink,fill}).eax;};
  ex_heavy_services.circle=ex_control_services.circle;ex_heavy_services.line=[this](U x1,U y1,U x2,U y2,U ink,U width){return platform.call(0x42d190,{x1,y1,x2,y2,ink,width}).eax;};
  omni_services.line=ex_middle_services.line;omni_services.circle=[this](U x,U y,U r,U ink,U fill){ex_control_services.draw_circle(x,y,r,ink,fill);};
 }
};
}
