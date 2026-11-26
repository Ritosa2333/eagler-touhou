// 同一游戏内核的Outer双回调适配；菜单、暂停与游戏持有相同GameStartEntryRefs。
#pragma once
#include "core/runtime/GameplayFrame.hpp"
#include "core/runtime/GameplayFrameWorldView.hpp"
#include "core/runtime/OuterSceneServices.hpp"
#include "core/frontend/ReplayPresentationFrame.hpp"
#include "core/frontend/ReplayGameplayInput.hpp"
#include <optional>
namespace kaikeidou {
struct GameplayFramePresentationRegisters {std::uint32_t ecx=0;bool known=false;};
class GameplayFrameMovement {
 GameStartEntryMemory& memory_;PlayerShotFireInput& physical_;bool replay_;unsigned precision_;
 ReplayGameplayInputOutcome outcome_;
public:
 GameplayFrameMovement(GameStartEntryMemory& memory,PlayerShotFireInput& physical,bool replay,unsigned precision):memory_(memory),physical_(physical),replay_(replay),precision_(precision){}
 void operator()(){outcome_=update_replay_gameplay_movement(memory_,physical_,replay_,precision_);}
 const ReplayGameplayInputOutcome& outcome()const{return outcome_;}
};
// Context.move_player执行实际物理/Replay输入桥；present_fps在白幕前执行原FPS显示并发布实际ECX。
template<class Context>class GameplayFrameRuntime final:public OuterSceneGameplayServices {
 GameStartEntryRefs game_;Context& context_;GameplayFramePresentationRegisters& presentation_;
 ReplayPresentationFrameOutcome replay_outcome_;
 std::uint32_t fade_before_=0;
 std::optional<ReplayGameplayInputTransfer> movement_transfer_;
public:
 GameplayFrameRuntime(GameStartEntryRefs game,Context& context,GameplayFramePresentationRegisters& presentation):game_(game),context_(context),presentation_(presentation){}
 OuterSceneCallOutcome gameplay_body(GameStartEntryMemory& memory,GameStartEntryMachineFrame& frame)override{
  memory.reconcile();presentation_.known=false;context_.timeline.frame=game_.patterns.game_frame;context_.timeline.part=game_.lifecycle.part;
  context_.timeline.subphase=game_.lifecycle.subphase;context_.timeline.difficulty=game_.lifecycle.difficulty;
  movement_transfer_.reset();std::uint32_t fade=0;
  try{fade=execute_gameplay_frame(context_);}catch(const ReplayGameplayInputTransfer& transfer){
   movement_transfer_=transfer;memory.reconcile();return {OuterSceneCallKind::transfer,transfer.source,transfer.target,0};
  }
  fade_before_=fade;
  game_.patterns.game_frame=context_.timeline.frame;game_.lifecycle.part=context_.timeline.part;game_.lifecycle.subphase=context_.timeline.subphase;
  memory.reconcile();
  if(context_.full_six_world&&fade!=0&&fade<0x80000000u){presentation_.ecx=game_.runtime.ending.word(0x20);presentation_.known=true;}
  if(presentation_.known)frame.registers.ecx=presentation_.ecx;
  return {OuterSceneCallKind::completed,0x65343a,0x65343f,presentation_.known?outer_ecx:0u};
 }
 OuterSceneCallOutcome replay_after_gameplay(GameStartEntryMemory& memory,GameStartEntryMachineFrame& frame)override{
  memory.reconcile();replay_outcome_=execute_replay_presentation_frame(memory,frame);memory.reconcile();
  if(replay_outcome_.kind!=ReplayPresentationFrameKind::returned)return {OuterSceneCallKind::transfer,replay_outcome_.source,replay_outcome_.target,0};
  return {OuterSceneCallKind::completed,replay_outcome_.source,replay_outcome_.target,outer_eax|outer_ecx};
 }
 std::uint32_t last_fade_before()const{return fade_before_;}
 const std::optional<ReplayGameplayInputTransfer>& movement_transfer()const{return movement_transfer_;}
 const ReplayPresentationFrameOutcome& replay_outcome()const{return replay_outcome_;}
};
}
