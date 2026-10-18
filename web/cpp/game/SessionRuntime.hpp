#pragma once
#include "SessionState.hpp"
#include "FrameScheduler.hpp"
namespace th15 {
struct SessionFrameState {u32 gui_flags=0; i32 gui_age=0,player_life_state=0;bool background_finished=false,return_to_selection=false;u32 pressed=0;};
enum class SessionDestination:i32 {ReturnSelection=2,DemoFinished=4,NormalEnding=15,ExtraEnding=16};
struct SessionRuntimeServices {
 virtual ~SessionRuntimeServices()=default;
 virtual bool ending_fade()=0;virtual bool finish_replay()=0;virtual bool destination(SessionDestination)=0;
 virtual bool load_scene(bool& waiting)=0;virtual bool activate_scene()=0;virtual bool release_background()=0;
 virtual bool load_checkpoint_file(bool& restored)=0;virtual bool restart_overlay(i32 script)=0;
 virtual bool restart_effect(i32 label)=0;virtual bool prepare_stage_music()=0;virtual bool start_stage_music()=0;virtual bool start_boss_music()=0;virtual bool seek_stage_music(double seconds)=0;
 virtual bool demo_fade()=0;virtual bool update_score()=0;
 virtual bool chapter_reward(bool boss)=0;virtual bool chapter_checkpoint(i32 chapter)=0;
 // The purple 43d0b5 hook skips reward code, not checkpoint creation. Called
 // at the branch itself, including chapter zero, to consume each one-shot.
 virtual bool skip_chapter_reward(){return false;}
 virtual bool update_overlays()=0;
};
class SessionRuntime {
 SessionState& progress;PlayerLifeSession& player;SessionRuntimeServices& host;
 bool check(bool,const char*);
public:
 Timer age{0,0,0,0,0};i32 ending_frames=0,requested_chapter=0,frame_skip=0,sampling_mode=0;std::string error;
 SessionRuntime(SessionState& p,PlayerLifeSession& s,SessionRuntimeServices& h):progress(p),player(s),host(h){age.set(0);}
 i32 update(const SessionFrameState&);
};
}
