#pragma once
#include "StageGameplay.hpp"
#include "SessionRuntime.hpp"
#include "ReplayRecording.hpp"
#include "ReplayPlayback.hpp"
namespace th15 {
struct SessionGameplayServices {
 virtual ~SessionGameplayServices()=default;
 virtual bool ending_fade()=0;virtual bool finish_replay()=0;virtual bool destination(SessionDestination)=0;
 virtual bool load_scene(bool& waiting)=0;virtual bool activate_scene()=0;virtual bool release_background()=0;
 virtual bool load_checkpoint_file(bool& restored)=0;virtual bool restart_overlay(i32 script)=0;virtual bool restart_effect(i32 label)=0;
 virtual bool prepare_stage_music()=0;virtual bool start_stage_music()=0;virtual bool start_boss_music()=0;virtual bool seek_stage_music(double seconds)=0;
 virtual bool demo_fade()=0;virtual bool update_overlays()=0;
};
struct SessionGameplayInput {StageGameplayFrame scene;u32 physical_pressed=0;float fps=60;bool background_finished=false,return_to_selection=false;PlayerTouch touch;};
// Attach the actual run controller and input callbacks to the same scheduler
// that owns background, enemies, projectiles, dialogue, HUD and animations.
// Scene factories, persisted files and platform presentation remain explicit.
class SessionGameplay final:private SessionRuntimeServices {
 StageGameplay& scene;SessionState& progress;SessionGameplayServices& services;
 ReplayRecording* recording;ReplayPlayback* playback;std::array<FrameCallback,3> callbacks;
 SessionGameplayInput frame;GameInput live;bool attached=false,input_enabled=true;i32* previous_chapter_owner=nullptr;
 i32 update_runtime();i32 update_input();i32 accelerate_replay();bool fail(const std::string&);
 bool ending_fade()override;bool finish_replay()override;bool destination(SessionDestination)override;
 bool load_scene(bool&)override;bool activate_scene()override;bool release_background()override;
 bool load_checkpoint_file(bool&)override;bool restart_overlay(i32)override;bool restart_effect(i32)override;
 bool prepare_stage_music()override;bool start_stage_music()override;bool start_boss_music()override;bool seek_stage_music(double)override;
 bool demo_fade()override;bool update_score()override;bool chapter_reward(bool)override;bool chapter_checkpoint(i32)override;bool skip_chapter_reward()override;bool update_overlays()override;
public:
 SessionRuntime runtime;std::string error;
 bool auto_focus=false;
 SessionGameplay(StageGameplay&,SessionState&,SessionGameplayServices&,ReplayRecording* recording=nullptr,ReplayPlayback* playback=nullptr);
 ~SessionGameplay();bool step(const SessionGameplayInput&);
 void reset_input()noexcept{live={};}
 void enable_input(bool enabled)noexcept{input_enabled=enabled;}
 bool input_active()const noexcept{return input_enabled;}
 void input_source(ReplayRecording* live_recording,ReplayPlayback* replay)noexcept{recording=live_recording;playback=replay;}
 const GameInput& controls()const noexcept{return playback?playback->input:live;}
};
}
