#pragma once
#include "RunSession.hpp"
#include "SessionTeardown.hpp"
namespace th15 {
struct RunStageExitServices {
 virtual ~RunStageExitServices()=default;
 virtual bool stop_checkpoint_worker()=0;virtual bool save_records()=0;
 virtual bool clear_session_links()=0;virtual bool reset_transition()=0;
 virtual bool disable_pause_callbacks()=0;
 virtual bool queue_exit_music(i32)=0;virtual bool reset_audio_slots()=0;
};
// Normal clear retains run managers but destroys the outgoing enemy and bomb
// controllers. Consume this after the active scheduler returns. Other exits
// require their own owner factories and are deliberately rejected here.
class RunStageExit final:private SessionTeardownServices {
 RunGameplay& run;RunSession& driver;SessionState& progress;SessionReplay& replay;RunStageExitServices& platform;
 StageGameplay* outgoing=nullptr;bool bomb_owned=false,spell_owned=false,carried=false;
 bool fail(const std::string&);
 bool owns(SessionObject)const noexcept override;
 bool stop_checkpoint_worker()override;bool save_records()override;
 bool clear_session_links()override;bool reset_transition()override;
 bool release(SessionObject)override;bool clear_checkpoint_data()override;
 bool reset_gui_for_stage()override;bool carry_background()override;
 bool disable_callbacks(SessionObject)override;bool reset_items()override;
 bool clear_enemies()override;bool retire_stage_animations()override;
 bool detach_scene_callbacks()override;bool queue_music(i32)override;bool reset_audio_slots()override;
public:
 std::string error;SessionExitPresentation presentation;
 RunStageExit(RunGameplay& r,RunSession& d,SessionState& p,SessionReplay& q,RunStageExitServices& h):run(r),driver(d),progress(p),replay(q),platform(h){}
 bool finish(bool restart_audio=false);
};
}
