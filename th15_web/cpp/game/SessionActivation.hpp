#pragma once
#include "SessionState.hpp"
#include "Timer.hpp"
namespace th15 {
struct SceneActivationFrame {bool previous_background=false,return_to_selection=false,restart_audio=false;};
struct SceneActivationServices {
 virtual ~SceneActivationServices()=default;
 virtual bool clear_stage_intro()=0;virtual bool discard_stage_assets()=0;virtual bool return_to_title(bool selection)=0;
 virtual bool initialize_background()=0;virtual bool capture_previous_background()=0;virtual bool prepare_background_transition()=0;virtual bool transition_banner()=0;
 virtual bool reset_bullets()=0;virtual bool reset_player()=0;virtual bool reset_items()=0;virtual bool reset_enemies()=0;virtual bool reset_lasers()=0;
 virtual bool prepare_replay_stage()=0;virtual bool start_main_script()=0;virtual bool initialize_hud()=0;virtual bool enable_game_callbacks()=0;virtual bool configure_player_options()=0;
 virtual bool start_stage_music()=0;virtual bool interrupt_entrance()=0;virtual bool retire_restart_overlay()=0;virtual bool interrupt_resume_overlay()=0;
 virtual bool start_entry_music(){return start_stage_music();}
 virtual bool restore_pending_progress()=0;virtual bool queue_music(i32 kind)=0;virtual bool unlock_current_music()=0;
};
// Scene entrance is separate from manager construction and the main frame
// callback. The shared timer and progress are the same owners used in gameplay.
class SessionActivation {
 SessionState& progress;PlayerLifeSession& player;Timer& age;SceneActivationServices& services;
 bool reset_game();bool check(bool,const char*);
public:
 bool pending_progress=false,loaded=false;std::string error;
 SessionActivation(SessionState& p,PlayerLifeSession& s,Timer& a,SceneActivationServices& h):progress(p),player(s),age(a),services(h){}
 bool load(const SceneActivationFrame&,bool& waiting);bool activate(const SceneActivationFrame&);
};
}
