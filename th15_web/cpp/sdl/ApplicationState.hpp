#pragma once
#include "Application.hpp"
#include "SceneDisplay.hpp"
#include "FontDevice.hpp"
#include "AudioDevice.hpp"
#include "FileStore.hpp"
#include "../game/Archive.hpp"
#include "../game/Lzss.hpp"
#include "../game/TitleScene.hpp"
#include "../game/EndingScene.hpp"
#include "../game/RunStageFlow.hpp"
#include "../game/ScreenFade.hpp"
#include "../game/AnmSceneEffects.hpp"
#include "../game/Manual.hpp"
#include "../game/ScreenMotionFrame.hpp"
namespace th15::sdl {
struct ApplicationState final:TitleScenePlatform,EndingScenePlatform,StageGameplayServices,SessionGameplayServices,SessionEntryServices,RunConstructionServices,RunCompletionServices,RunStageExitServices,RunPausePlatform,StageDrawServices,AssetSource {
 GraphicsDevice graphics;FontDevice fonts{graphics};AudioDevice audio_device;FileStore files;
 Rng visual;AnmEnvironment environment;AnmManager animations{visual,environment};AnmRenderer renderer{graphics};ScreenViews views{renderer,environment};AsciiText captions{animations,environment};FrameScheduler scheduler;
 Rng loading_random;AnmEnvironment loading_environment;AnmManager loading_animations{loading_random,loading_environment};bool platform_prepared=false;u32 loading_signature=0,loading_prayer=0;unsigned pending_load=0,loading_frames=0;bool loading_startup=false;double loading_debt=0;
 bool loading_waiting()const;bool complete_loading();bool render_loading();bool advance_loading(double);
 GameConfig config;SessionState progress;PlayerLifeSession selection_player;ItemScoreState selection_score;RecordStore records;TitleSelectionSettings selection;TitleAudioSettings volumes;TitleControllerSettings controller;MusicComments comments;TitleKeyboard keyboard_state;
 std::unordered_map<std::string,i32> shared;std::unique_ptr<AnmSceneEffects> scene_effects;std::unique_ptr<SceneDisplay> display;std::array<FrameCallback,2> animation_updates;
 std::unique_ptr<TitleScene> title;std::unique_ptr<EndingScene> ending;std::unique_ptr<RunGameplay> run;std::unique_ptr<SessionReplay> replay;std::unique_ptr<RunSession> session;std::unique_ptr<RunInitialization> initialization;std::unique_ptr<RunStageFlow> flow;std::unique_ptr<RunPause> pause;
 std::vector<std::unique_ptr<ScreenFade>> fades;std::vector<std::unique_ptr<ScreenMotionFrame>> motion;std::unique_ptr<Manual> pause_manual;FrameCallback manual_update;u32 manual_pressed=0,manual_repeated=0;StageCamera camera;std::array<u8,0xa4> live_description{};std::vector<u8> selected_replay,pending_png,pending_animation;std::string pending_animation_name;std::unordered_map<std::string,std::vector<u8>> resource_cache;
 i32 pending_destination=-1,selected_stage=1,pending_page=-1,pending_bank=-1,frame_skip=0,current_destination=13;u32 transition_overlay=0,restart_handle=0,restart_effect_handle=0,frames=0;bool initialized=false,exiting=false,completed_recording=false;std::array<i32,16> projected{};std::string failure;
 std::vector<u8> checkpoint_bytes;
 std::unique_ptr<Lzss> checkpoint_encoder;std::vector<u8> checkpoint_payload;CheckpointHeader checkpoint_header;
 bool pump_checkpoint(u32 budget=131072);bool finish_checkpoint();
 Archive archive;std::vector<u8> archive_bytes;std::unordered_map<std::string,u32> archive_names;
 PlayerTouch touch;u32 controller_buttons=0;i32 numbered_chapter=0;
 // These original title counters live across destruction of the title owner.
 i32 title_demo_idle=0,title_demo_index=0,title_saved_difficulty=0,title_saved_replay_selection=0;
 void remember_title()noexcept{if(title){title_demo_idle=title->frame.demo_idle;title_demo_index=title->frame.demo_index;title_saved_difficulty=title->frame.saved_difficulty;title_saved_replay_selection=title->saved_replay_selection;}}
 bool fail(const std::string&);bool prepare_platform();bool prepare_loading();bool draw_loading(bool signature);bool render_loading(bool signature,unsigned frames);bool initialize(bool);~ApplicationState();bool step(u32,u32,u32,float,bool);bool apply_destination();bool begin_title(i32);bool begin_run();bool begin_ending();void release_run();bool bind_run_display();bool preload_run();bool save_settings();
 StageGameplay* scene()const noexcept{return run?run->scene():nullptr;}const StageAssets* assets()const noexcept{return run?run->scene_assets():nullptr;}const i32* projection();
 bool read(const std::string&,std::vector<u8>&)override;
 bool screen_fade(i32 duration,i32 update_priority,i32 draw_priority,bool covering,bool full_screen);
 bool sound(i32)override;bool text(AnmVm&,const DialogueText&)override;bool hud_text(const HudTextDraw&)override;bool keyboard(TitleKeyboard&)override;bool calendar(i64,ReplayCalendar&)override;bool capture_score_details(PauseScoreDetails&)override;
 bool music(const std::string&)override;bool music(const std::string&,i32)override;bool music_command(i32)override;bool start_title_music()override;bool volume(i32,i32,i32)override;bool save(const TitleControllerSettings&)override;
 bool checkpoint_available(i32,i32,bool&)override;bool checkpoint_stage(i32,i32,i32&)override;bool reset_resume_selection()override;bool prepare_game_music()override;bool begin_transition(u32&)override;bool start_game(i32)override;bool fade_music(float)override;bool transition_size(float,float)override;
 bool request_catalog()override;bool release_catalog()override;bool start_replay(const ReplayStartRequest&)override;bool clear_page()override;bool request_page(i32)override;bool upload_page(i32)override;
 bool read_slot(i32,std::shared_ptr<Replay>&)override;bool prepare_live_replay(bool)override;bool save_slot(i32,const std::array<char,9>&)override;bool release_live_replay()override;
 bool release_transient_animations()override;bool clear_title_overlay()override;bool read_demo(i32,std::shared_ptr<Replay>&)override;bool begin_demo(const ReplayStartRequest&)override;bool queue_title_music(i32,i32,const std::string&)override;bool clear_current_wave()override;bool reset_replay_selection()override;bool return_practice_transition()override;bool title_exit(i32)override;bool fade_out_title_music()override;
 bool audio(i32,float,PlayerShots::SoundAction,bool)override;bool life_hud(i32,i32)override;bool bomb_hud(i32,i32)override;bool game_over()override;bool notice(i32)override;bool popup(const Vec3&,i32,u32)override;bool cancellation_effect(i32,const Vec3&,const Vec3&)override;bool graze_spark(const Vec3&)override;bool graze_flash()override;bool graze_resonance(float)override;bool shake(const ScreenShakeSpec&)override;bool nudge(const ScreenNudgeSpec&)override;
 bool bomb_damage(const DamageQuery&,i32&)override;int enemy_callback(EnemyRuntime&,float)override;bool enemy_additional_damage(EnemyState&,i32,i32&)override;bool enemy_contact(EnemyState&,AnmVm*,i32&,bool&)override;bool enemy_distortion(EnemyState&,float)override;bool enemy_death_callback(EnemyRuntime&)override;bool spell_title(AnmVm&,const std::string&)override;bool spell_sound(i32)override;
 bool queue_music_control(i32,i32)override;bool unlock_music(i32)override;bool complete_stage()override;bool begin_game_over()override;bool initialize_text(AnmVm&,i32,i32)override;bool paint_text(AnmVm&,const DialogueText&)override;bool dialogue_music(bool)override;bool fade_dialogue_music(float)override;bool dialogue_stage_complete()override;bool dialogue_name_banner()override;bool dialogue_sound(i32)override;bool clear_dialogue_field()override;
 i32 dialogue_text_extent(const std::string& value,i32 font)override{return fonts.extent(value,font);}
 bool begin_deformation(i32,i32)override;bool update_deformation(StageScriptState&,float)override;bool synchronize_resources()override;bool retire_reward()override;bool retire_message()override;bool retire_scene_effect()override;bool reset_gui()override;bool stop_sounds()override;bool begin_restart_effect()override;bool begin_restart_overlay()override;bool checkpoint_file(bool)override;
 bool ending_fade()override;bool finish_replay()override;bool destination(SessionDestination)override;bool load_scene(bool&)override;bool activate_scene()override;bool release_background()override;bool load_checkpoint_file(bool&)override;bool restart_overlay(i32)override;bool restart_effect(i32)override;bool prepare_stage_music()override;bool start_stage_music()override;bool start_boss_music()override;bool seek_stage_music(double)override;bool demo_fade()override;bool update_overlays()override;
 bool discard_stage_assets()override;bool return_to_title(bool)override;bool capture_previous_background(StageGameplay&)override;bool prepare_background_transition()override;bool transition_banner()override;bool interrupt_entrance()override;bool retire_restart_overlay()override;bool interrupt_resume_overlay()override;bool restore_pending_progress()override;bool queue_music(i32)override;bool unlock_current_music()override;
 bool prepare_pause_menu()override;bool prepare_checkpoint_storage()override;bool restore_enemy_checkpoint(StageGameplay&)override;bool load_stage_theme(i32)override;bool load_player_theme(i32,bool)override;
 bool prepare_ending(StageGameplay&)override;bool finish_practice()override;bool queue_next_stage()override;bool prepare_next_stage(i32)override;bool stop_checkpoint_worker()override;bool save_records()override;bool clear_session_links()override;bool reset_transition()override;bool disable_pause_callbacks()override;bool queue_exit_music(i32)override;bool reset_audio_slots()override;
 bool seconds(double&)override;bool suspend_music()override;bool finish_audio_requests()override;bool capture_background(StageGameplay&,AnmManager&,u32&,bool)override;bool preserve_current_music(std::string&,double&)override;bool start_game_over_music()override;bool read_replay_slot(i32,std::shared_ptr<Replay>&)override;bool save_named_replay(i32,const std::array<char,9>&)override;bool prepare_replay_save(bool)override;bool open_options(float)override;bool options_finished()const override;bool close_options()override;i32 scene_destination()const override;
 bool resume_looping_sounds()override;bool resume_music()override;bool resume_saved_music(const std::string&,double)override;bool destination(PauseDestination)override;
 bool background_fade(i32,i32,i32)override;
 bool read_message(const std::string&,MessageProgram&)override;bool request_animation(i32,const std::string&)override;bool prepare_animation(AnmResource&)override;bool cancel_animation_request()override;bool loading_overlay()override;bool end_loading_overlay()override;bool prepare_music(const std::string&)override;bool start_music(i32)override;bool fade_music(i32)override;bool shake(i32,i32)override;bool reset_caption_state()override;bool ending_destination(i32)override;
};
}
