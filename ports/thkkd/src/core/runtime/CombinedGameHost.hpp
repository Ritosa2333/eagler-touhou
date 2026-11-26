// 同一状态、内存和活跃MachineFrame连接启动、菜单、暂停与真正游戏帧。
#pragma once
#include "core/runtime/ApplicationStartupRuntime.hpp"
#include "core/runtime/OuterSceneRuntime.hpp"
#include "core/runtime/GameplayHostContext.hpp"
#include <memory>
namespace kaikeidou {
struct CombinedGameHostRefs {
 GameStartEntryRefs game;GameStartEntryGaps& gaps;GameStartEntryReplayContext& replay;GameStartEntryMachineFrame& frame;
 MenuSceneRuntimeContext& menu;CollisionContext& collision;EndingCreditsAssets& ending;MenuBootstrapContext& bootstrap;
 MenuBootstrapGaps& bootstrap_gaps;PauseGameoverContext& pause;ApplicationStartupContext& startup;GameplayHudTailContext& hud;ObjectRecord<1200>& ex_vectors;
};
struct CombinedGameHostPlatform {
 MenuScenePlatform& menu;MenuBootstrapPlatform& bootstrap;PauseGameoverHost pause;EndingCreditsPlatform& ending;
 BossPhaseServices& failure;ApplicationStartupServices& startup;
};
class CombinedGameHost {
 class Forward final:public OuterSceneGameplayServices {
 public:OuterSceneGameplayServices* target=nullptr;
  OuterSceneCallOutcome gameplay_body(GameStartEntryMemory& m,GameStartEntryMachineFrame& f)override{if(!target)throw std::logic_error("Combined gameplay services not connected");try{return target->gameplay_body(m,f);}catch(const GameplayHostHudTransfer& transfer){m.reconcile();return {OuterSceneCallKind::transfer,transfer.outcome.source,transfer.outcome.target,transfer.outcome.exact_registers};}}
  OuterSceneCallOutcome replay_after_gameplay(GameStartEntryMemory& m,GameStartEntryMachineFrame& f)override{if(!target)throw std::logic_error("Combined Replay bridge not connected");return target->replay_after_gameplay(m,f);}
 }forward_;
 CombinedGameHostRefs refs_;CombinedGameHostPlatform host_;bool variant_,playback_=false,sealed_=false;
 GameplayFrameWorldView world_;OuterSceneRuntime outer_;GameplayFramePresentationRegisters presentation_;
 GameplayHostContext context_;GameplayFrameRuntime<GameplayHostContext> gameplay_;ApplicationStartupOutcome startup_outcome_;
public:
 CombinedGameHost(CombinedGameHostRefs refs,CombinedGameHostPlatform host,bool replay_variant,unsigned precision,GameMath& math,const GameStartEntryMemoryExtensions& extra={},ReplayFailureContinuationPolicy policy={}):
  refs_(refs),host_(host),variant_(replay_variant),world_(refs.game,refs.ex_vectors,refs.game.fire.fire_lock,refs.game.fire.replay_buttons,playback_,refs.game.savedata.bindings,refs.menu.logical_input),
  outer_(refs.game,refs.gaps,refs.replay,refs.frame,refs.menu,refs.collision,refs.ending,refs.bootstrap,refs.bootstrap_gaps,refs.pause,replay_variant,precision,math,{host.menu,host.bootstrap,host.pause,host.ending,host.failure,forward_},extra,std::move(policy)),
  context_(refs.game,world_,outer_.memory(),refs.frame,refs.menu,refs.collision,host.menu,math,precision,presentation_),gameplay_(refs.game,context_,presentation_){
  bind_application_startup(outer_.memory(),refs.startup);bind_gameplay_hud_tail(outer_.memory(),refs.hud);outer_.memory().bind(0xb3e3e0,refs.ex_vectors.data);forward_.target=&gameplay_;
  refs.game.fire.replay_variant=replay_variant;refs.game.boss_phase.replay_variant=replay_variant;refs.game.dialogue_context.replay_variant=replay_variant;
 }
 GameStartEntryMemory& memory(){return outer_.memory();}
 void seal(){outer_.seal();sealed_=true;}
 ApplicationStartupOutcome begin_application(const ApplicationStartupPolicy& policy={}){if(!sealed_)throw std::logic_error("Seal combined initial state before startup");startup_outcome_=kaikeidou::begin_application(memory(),refs_.frame,refs_.startup,host_.startup,policy);return startup_outcome_;}
 OuterSceneOutcome step(){if(!sealed_)throw std::logic_error("Seal combined initial state before frame");playback_=refs_.game.fire.replay_variant&&refs_.game.fire.replay_mode!=0;return outer_.step();}
 OuterSceneRuntime& outer(){return outer_;}GameplayHostContext& gameplay_context(){return context_;}
 const GameplayFrameRuntime<GameplayHostContext>& gameplay()const{return gameplay_;}
 const ApplicationStartupOutcome& startup_outcome()const{return startup_outcome_;}
 const GameplayHudTailOutcome& hud_outcome()const{return context_.hud_outcome;}
};
}
