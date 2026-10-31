#pragma once
#include "Application.hpp"
#include "SceneDisplay.hpp"
#include "FontDevice.hpp"
#include "AudioDevice.hpp"
#include "FileStore.hpp"
#include "../game/Archive.hpp"
#include "../game/TitleScene.hpp"
#include "../game/EndingScene.hpp"
#include "../game/ApplicationFlow.hpp"
#include "../game/RunInput.hpp"
#include "../game/RunGameplay.hpp"
#include "../game/RunPause.hpp"
#include "../game/ScreenFade.hpp"
#include "../game/FramePerformance.hpp"
namespace th17::sdl {
// Each game scene owns its callback schedule. Title/ending share the global
// ANM draw schedule; stage gameplay owns the complete battle/compositor schedule.
struct ApplicationState final:TitleScenePlatform,EndingScenePlatform,StageGameplayServices,RunPausePlatform,StageDrawServices,AssetSource,ApplicationFlowServices,RunInputServices {
 GraphicsDevice graphics;FontDevice fonts{graphics};AudioDevice audio_device;FileStore files;
 Rng visual;AnmEnvironment environment;AnmManager animations{visual,environment};AnmRenderer renderer{graphics};ScreenViews views{renderer,environment};AsciiText captions{animations,environment};FrameScheduler scheduler;
 GameConfig config;GameRun progress;GameSelection selection;PlayerLifeSession selection_player;ItemScoreState selection_score;RecordStore records;TitleSelectionSettings choices;TitleAudioSettings volumes;TitleControllerSettings controller;MusicComments comments;TrophyCatalog trophy_catalog;TitleSpellMemory spell_memory;TitleKeyboard keyboard_state;
 ApplicationFlow flow{progress,*this};std::unordered_map<std::string,i32> shared;
 std::unique_ptr<AnmSceneEffects> scene_effects;std::unique_ptr<AnmDrawSchedule> global_layers;std::unique_ptr<AsciiDrawSchedule> global_letters;
 std::unique_ptr<TitleScene> title;std::unique_ptr<EndingScene> ending;std::unique_ptr<RunGameplay> run;std::unique_ptr<RunInput> run_input;std::unique_ptr<RunPause> pause;std::unique_ptr<SceneDisplay> display;
 std::shared_ptr<Replay> selected_replay;std::unique_ptr<ReplayRecording> recording;std::array<u8,0xa0> live_description{};
 std::vector<std::unique_ptr<ScreenFade>> fades;std::unique_ptr<Manual> pause_manual;FrameCallback manual_callback;std::array<FrameCallback,4> global_callbacks;
 std::unordered_map<std::string,std::vector<u8>> resource_cache;Archive archive;std::vector<u8> archive_bytes;std::unordered_map<std::string,u32> archive_names;
 StageCamera camera;PlayerTouch touch;GameInput physical_input,retained_input;FramePerformance performance;u32 controller_buttons=0,frames=0,manual_pressed=0,manual_repeated=0,transition_overlay=0,loading_handle=0;u16 stage_seed=0;
 std::vector<u8> pending_png,pending_animation;i32 pending_page=-1,pending_bank=-1,frame_skip=0,preview_index=-1;
 i32 title_demo_idle=0,title_demo_index=0,title_saved_difficulty=0,title_saved_replay_selection=0;
 bool initialized=false,exiting=false,completed_recording=false;std::array<i32,16> projected{};std::string failure;
 AppScene loading_destination=AppScene::Uninitialized;u32 loading_frames=0,loading_duration=0;Rng loading_visual,loading_game;
 void remember_title()noexcept;bool fail(const std::string&);bool initialize(bool);~ApplicationState();bool step(u32,u32,u32,float,bool);const i32* projection();
 FrameScheduler& active_scheduler()noexcept{return scene()?scene()->frame_scheduler():scheduler;}
 StageGameplay* scene()const noexcept{return run?run->scene():nullptr;}StageAssets* assets()const noexcept{return run?run->scene_assets():nullptr;}
 bool save_settings();bool preload_run();bool bind_display(StageGameplay* previous=nullptr);bool draw_global();bool screen_fade(i32,i32,i32,bool,bool,u32=0);void reset_graphics();
 bool advance_flow();bool step_loading();bool draw_loading();
 bool read(const std::string&,std::vector<u8>&)override;
 bool begin_loading(bool&)override;bool shutdown()override;bool shutdown_delay(i32)override;bool release_game()override;bool release_title()override;bool release_ending()override;bool begin_game(bool)override;bool begin_title()override;bool begin_ending()override;
 bool sound(i32)override;bool text(AnmVm&,const TextureText&)override;bool hud_text(const HudTextDraw&)override;bool keyboard(TitleKeyboard&)override;bool calendar(i64,ReplayCalendar&)override;bool capture_score_details(PauseScoreDetails&)override;bool seconds(double&)override;
 // TitleScene dispatches its named menu controllers before reaching the platform.
 bool update_title_menu(TitleScreen,const TitleFrameInput&)override{return true;}bool draw_title_menu(TitleScreen)override{return true;}
 bool music(const std::string&)override;bool music(const std::string&,i32)override;bool music_command(i32)override;bool start_title_music()override;bool volume(i32,i32,i32)override;bool save(const TitleControllerSettings&)override;
 bool fade_menu_music()override;bool show_loading_indicator()override;bool begin_transition(u32&)override;bool start_game(i32)override;i32 checkpoint_key()const override;bool fade_music(float)override;bool transition_size(float,float)override;
 bool request_catalog()override;bool release_catalog()override;bool start_replay(const ReplayStartRequest&)override;bool clear_page()override;bool request_page(i32)override;bool upload_page(i32)override;
 bool read_slot(i32,std::shared_ptr<Replay>&)override;bool prepare_live_replay(bool)override;bool save_slot(i32,const std::array<char,9>&)override;bool release_live_replay()override;
 bool release_transient_animations()override;bool clear_title_overlay()override;bool read_demo(i32,std::shared_ptr<Replay>&)override;bool begin_demo(const DemoRequest&)override;bool queue_title_music(i32,i32,const std::string&)override;bool clear_current_wave()override;bool unlock_title_music()override;bool selection_return_transition()override;bool title_exit(i32)override;bool fade_out_title_music()override;
 bool ending_active()const override;bool ending_complete()const override;bool retire_ending()override;bool preview_ending(i32)override;bool trophy_stage(i32,i32)override;
 bool audio(i32,float,PlayerShots::SoundAction,bool)override;bool popup(const Vec3&,i32,u32)override;bool trophy(i32)override;bool game_over()override;bool finish_practice()override;bool record_play_time()override;bool return_to_title()override;
 bool dialogue_text(AnmVm&,u32,u32,i32,i32,i32,const std::string&)override;bool spell_title(AnmVm&,const std::string&)override;bool music_control(i32,i32)override;bool music_fade(i32)override;bool unlock_music(i32)override;bool transition(const SceneTransition&)override;
 i32 loading_state()const noexcept override;bool retire_loading()override;bool flush_audio()override;bool resume_environment(double)override;bool stage_input_ready(StageGameplay&)override;bool replay_finished()override;
 bool suspend_music()override;bool finish_audio_requests()override;bool capture_background(StageGameplay&,AnmManager&,u32&,bool)override;bool preserve_current_music(std::string&,double&)override;bool start_game_over_music()override;bool read_replay_slot(i32,std::shared_ptr<Replay>&)override;bool save_named_replay(i32,const std::array<char,9>&)override;bool prepare_replay_save(bool)override;
 bool open_options(float)override;bool options_finished()const override;bool close_options()override;i32 scene_destination()const override;bool resume_looping_sounds()override;bool resume_music()override;bool resume_saved_music(const std::string&,double)override;bool reset_audio_slots()override;bool destination(PauseDestination)override;bool background_fade(i32,i32,i32)override;
 bool read_message(const std::string&,MessageResource&)override;bool request_animation(i32,const std::string&)override;bool prepare_animation(AnmResource&)override;bool cancel_animation_request()override;bool loading_overlay()override;bool end_loading_overlay()override;bool prepare_music(const std::string&)override;bool start_music(i32)override;bool fade_music(i32)override;bool shake(i32,i32)override;bool reset_caption_state()override;bool ending_destination(i32)override;
};
}
