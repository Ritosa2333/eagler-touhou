#pragma once
#include "RunGameplay.hpp"
#include "SessionGameplay.hpp"
#include "SessionActivation.hpp"
#include "SessionReplay.hpp"
namespace th15 {
struct SessionEntryServices {
 virtual ~SessionEntryServices()=default;
 virtual bool discard_stage_assets()=0;virtual bool return_to_title(bool)=0;
 virtual bool capture_previous_background(StageGameplay&)=0;virtual bool prepare_background_transition()=0;virtual bool transition_banner()=0;
 virtual bool interrupt_entrance()=0;virtual bool retire_restart_overlay()=0;virtual bool interrupt_resume_overlay()=0;
 virtual bool restore_pending_progress()=0;virtual bool queue_music(i32)=0;virtual bool unlock_current_music()=0;
};
// Connect the original entrance state machine to real run objects and replay.
// GPU captures, audio and saved-file operations stay explicit platform work.
class RunSession final:private SessionGameplayServices,private SceneActivationServices {
 RunGameplay& run;SessionState& progress;SessionReplay& replay;SessionGameplayServices& platform;SessionEntryServices& entry;
 std::unique_ptr<SessionGameplay> gameplay;std::unique_ptr<SessionActivation> activation;StageCamera camera;i32 scene_destination=0;bool restart_audio=false;bool fail(const std::string&);
 ReplayRunState replay_state();StageGameplay& scene(){return *run.scene();}
 bool ending_fade()override;bool finish_replay()override;
 bool destination(SessionDestination)override;bool load_scene(bool&)override;bool activate_scene()override;bool release_background()override;
 bool load_checkpoint_file(bool&)override;bool restart_overlay(i32)override;bool restart_effect(i32)override;
 bool prepare_stage_music()override;bool start_stage_music()override;bool start_boss_music()override;bool seek_stage_music(double)override;
 bool start_entry_music()override;
 bool demo_fade()override;bool update_overlays()override;
 bool clear_stage_intro()override;bool discard_stage_assets()override;bool return_to_title(bool)override;
 bool initialize_background()override;bool capture_previous_background()override;bool prepare_background_transition()override;bool transition_banner()override;
 bool reset_bullets()override;bool reset_player()override;bool reset_items()override;bool reset_enemies()override;bool reset_lasers()override;
 bool prepare_replay_stage()override;bool start_main_script()override;bool initialize_hud()override;bool enable_game_callbacks()override;bool configure_player_options()override;
 bool interrupt_entrance()override;bool retire_restart_overlay()override;bool interrupt_resume_overlay()override;
 bool restore_pending_progress()override;bool queue_music(i32)override;bool unlock_current_music()override;
public:
 std::string error;
 RunSession(RunGameplay& r,SessionState& p,SessionReplay& q,SessionGameplayServices& h,SessionEntryServices& e):run(r),progress(p),replay(q),platform(h),entry(e){}
 bool attach(const StageCamera&,i32 scene_destination,bool prepare_replay=false,bool restart_audio=false);
 void detach(){activation.reset();gameplay.reset();}
 bool step(const SessionGameplayInput&);SessionGameplay* driver()const noexcept{return gameplay.get();}
};
}
