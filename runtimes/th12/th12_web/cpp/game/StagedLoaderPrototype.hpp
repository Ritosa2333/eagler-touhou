#pragma once
#include "Types.hpp"
namespace th11::prototype {
enum class Work:u32{Waiting,Ready,Failed};
enum class LoadPhase:u32{Idle,WaitCapture,WaitMinimum,Hud,Scene,Timeline,Ready,Aborted,Failed,Initialize};
struct StageActions {
 virtual ~StageActions()=default;
 virtual bool retire_owner()=0;
 virtual bool begin_worker_wait(bool){return true;}
 virtual bool worker_owns_capture_wait()const{return false;}
 virtual bool worker_wait_ready()const{return true;}
 virtual bool enter_work_and_initialize()=0;
 virtual Work advance_work_and_initialize(){return enter_work_and_initialize()?Work::Ready:Work::Failed;}
 virtual Work prepare_hud()=0;
 virtual Work prepare_scene()=0;
 virtual bool publish_empty_owner()=0;
 virtual Work prepare_timeline()=0;
};
struct StagedLoader {
 LoadPhase phase=LoadPhase::Idle;
 u64 deadline=0,last_sample=0;
 bool initial_stage=false,has_sample=false,initializing=false,preparing_scene=false;
 bool request(StageActions& actions,bool initial=false){
  if(phase!=LoadPhase::Idle&&phase!=LoadPhase::Ready)return false;
  if(!actions.retire_owner()){phase=LoadPhase::Failed;return false;}
  initial_stage=initial;has_sample=false;initializing=false;deadline=0;phase=LoadPhase::WaitCapture;return true;
 }
 // Initialization may complete while the main thread owns an active VM.
 // Do not replace ANM resources or create stage owners at that boundary.
 bool poll_initialization(StageActions& actions,u64 now){
  if(phase==LoadPhase::Aborted||phase==LoadPhase::Failed)return false;
  if(initializing)return true;
  if(phase!=LoadPhase::WaitMinimum&&phase!=LoadPhase::Initialize)return true;
  if(has_sample&&now<last_sample){phase=LoadPhase::Failed;return false;}
  has_sample=true;last_sample=now;
  if(phase==LoadPhase::WaitMinimum){
   if(now<deadline||!actions.worker_wait_ready())return true;
   phase=LoadPhase::Initialize;
  }
  struct Guard {bool& flag;Guard(bool& v):flag(v){flag=true;}~Guard(){flag=false;}} guard(initializing);
  const auto result=actions.advance_work_and_initialize();
  if(result==Work::Failed){phase=LoadPhase::Failed;return false;}
  if(result==Work::Ready)phase=LoadPhase::Hud;
  return true;
 }
 // Complete only an already-entered Scene phase while the Stage is borrowed.
 bool poll_scene(StageActions& actions,u64 now){
  if(phase==LoadPhase::Aborted||phase==LoadPhase::Failed)return false;
  if(preparing_scene||phase!=LoadPhase::Scene)return true;
  if(has_sample&&now<last_sample){phase=LoadPhase::Failed;return false;}
  has_sample=true;last_sample=now;
  struct Guard{bool& flag;Guard(bool& f):flag(f){flag=true;}~Guard(){flag=false;}}guard(preparing_scene);
  const auto result=actions.prepare_scene();
  if(result==Work::Waiting)return true;
  if(result==Work::Failed||!actions.publish_empty_owner()){phase=LoadPhase::Failed;return false;}
  phase=LoadPhase::Timeline;return true;
 }
 bool pump(StageActions& actions,u64 now,i32 first_capture_index,u32 supervisor_flags){
  if(preparing_scene)return true;
  if(phase==LoadPhase::Idle||phase==LoadPhase::Ready)return true;
  if(phase==LoadPhase::Aborted||phase==LoadPhase::Failed)return false;
  if(has_sample&&now<last_sample){phase=LoadPhase::Failed;return false;}
  has_sample=true;last_sample=now;
  for(;;){
   if(phase==LoadPhase::WaitCapture){
    const bool worker_wait=!initial_stage&&actions.worker_owns_capture_wait();
    if(!worker_wait&&first_capture_index>=0){
     if(supervisor_flags&0x180){phase=LoadPhase::Aborted;return false;}
     return true;
    }
    const u64 minimum=initial_stage||worker_wait?0:60000000;
    if(now>~u64(0)-minimum){phase=LoadPhase::Failed;return false;}
    if(!actions.begin_worker_wait(initial_stage)){phase=LoadPhase::Failed;return false;}
    deadline=now+minimum;phase=LoadPhase::WaitMinimum;
   }
   if(phase==LoadPhase::WaitMinimum||phase==LoadPhase::Initialize){
    if(!poll_initialization(actions,now))return false;
    if(phase==LoadPhase::WaitMinimum||phase==LoadPhase::Initialize)return true;
   }
   if(phase==LoadPhase::Hud){
    const auto result=actions.prepare_hud();
    if(result==Work::Waiting)return true;
    if(result==Work::Failed){phase=LoadPhase::Failed;return false;}
    phase=LoadPhase::Scene;
   }
   if(phase==LoadPhase::Scene){
    if(!poll_scene(actions,now))return false;
    if(phase==LoadPhase::Scene)return true;
   }
   if(phase==LoadPhase::Timeline){
    const auto result=actions.prepare_timeline();
    if(result==Work::Waiting)return true;
    if(result==Work::Failed){phase=LoadPhase::Failed;return false;}
    phase=LoadPhase::Ready;return true;
   }
   phase=LoadPhase::Failed;return false;
  }
 }
};
}
