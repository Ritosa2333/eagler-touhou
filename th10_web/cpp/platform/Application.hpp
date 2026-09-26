#pragma once
#include "Startup.hpp"
#include "Title.hpp"
#include "Credits.hpp"
#include "../game/ApplicationFrame.hpp"
#include "../game/ApplicationLoop.hpp"
#include "../game/FrameStatistics.hpp"
#include "../game/Presentation.hpp"
#include "../game/Screenshot.hpp"
namespace th10::browser {
struct Application;
struct AppScreens final:ApplicationEnvironment {
    Application& owner;explicit AppScreens(Application&);
    void enter_lock(ApplicationState&,u32) override;void leave_lock(ApplicationState&,u32) override;
    StartupScreen* create_startup_screen(ApplicationState&) override;
    void destroy_screens(ApplicationState&) override;void create_title() override;
    void destroy_title(TitleMenu*) override;void create_game(i32) override;void destroy_game(GameSession*) override;
    void create_ending() override;void destroy_ending(Ending*) override;void destroy_startup(StartupScreen*) override;void destroy_replay(Replay*) override;
    u32 create_loading_animation(AnmFile&,i32) override;
    u32 begin_thread(CallbackToken,void*,u32,u32&) override;u32 wait_thread(u32,u32) override;
    void close_thread(u32) override;void sleep(u32) override;
};
struct AppFrames final:ApplicationFrameEnvironment {
    Application& owner;explicit AppFrames(Application&);
    void update_audio() override;void update_input() override;i32 process_loading() override;
    i32 transition(ApplicationState&) override;void configure_camera(Camera&) override;
    void set_viewport(void*,const CameraViewport&) override;void clear(u32) override;void flush() override;
};
struct AppLoop final:ApplicationLoopEnvironment {
    Application& owner;explicit AppLoop(Application&);
    Extended time() override;void sleep(u32) override;void flush() override;
    void configure_flat(Camera&) override;void set_viewport(void*,const CameraViewport&) override;
    i32 update() override;void update_audio() override;void stop_loader() override;
    i32 begin_scene(void*) override;void draw() override;
#ifdef TH_NATIVE_PLATFORM
    i32 set_fog_enabled(bool)override;
#else
    i32 render_state(void*,u32,u32) override;
#endif
    void clear_texture(void*) override;void end_scene(void*) override;void present() override;
};
struct AppStatistics final:FrameStatisticsEnvironment {
    Application& owner;explicit AppStatistics(Application&);
    void* allocate(u32) override;Extended time() override;void draw_rate(CommonResources&,const Vec3&,float) override;
};
struct AppPresentation final:PresentationEnvironment {
    Application& owner;explicit AppPresentation(Application&);
    i32 present(void*) override;void reset_device(void*,void*) override;void release_surface(void*) override;
#ifdef TH_NATIVE_PLATFORM
    void configure_graphics()override;
#else
    void render_state(void*,u32,u32) override;void texture_stage(void*,u32,u32,u32) override;void sampler_state(void*,u32,u32,u32) override;
#endif
    void capture_texture(AnmManager&,i32,u32,CaptureRectangle,CaptureRectangle) override;
    void capture_screen(AnmManager&,i32,CaptureRectangle,CaptureRectangle) override;
    void create_directory(const char*) override;bool file_exists(const char*) override;void save_screenshot(ApplicationState&,const char*) override;
};
struct AppScreenshot final:ScreenshotEnvironment {
    Application& owner;explicit AppScreenshot(Application&);
    void sleep(u32) override;void* back_buffer(void*) override;void* allocate(u32) override;void free_bytes(void*) override;
    TextureLock lock_surface(void*) override;void unlock_surface(void*) override;void release_surface(void*) override;
    u32 begin_writer() override;void report(ScreenshotError) override;void open_output(const char*) override;
    void write_output(const u8*,u32,u32&) override;void close_output() override;
};
struct AppConfiguration final:ApplicationConfigEnvironment {
    Application& owner;explicit AppConfiguration(Application&);
    u8* load(const char*,u32*) override;void release(void*) override;i32 save(const char*,const ApplicationConfig&) override;
    void notice(ConfigurationNotice,const char*) override;
};
// One typed owner of the original startup/title/game/results/ending state
// machine. Host imports provide only files, graphics, audio, fonts and clocks.
struct Application final:CallbackReceiver {
    FileSystem& files;Input& input;GameState& state;AnimationEngine& engine;Fonts& fonts;Audio& audio;ScreenEffects& effects;
    ApplicationState& value;Captures captures;GraphicsPresentation parameters{};
    Startup* startup=nullptr;Title* title=nullptr;World* world=nullptr;Credits* credits=nullptr;
    StartupScreen* startup_view=nullptr;TitleMenu* title_view=nullptr;GameSession* session_view=nullptr;Ending* ending_view=nullptr;
    CommonResources* common_view=nullptr;AnmManager* manager;UpdateChain* chain;
    FrameStatistics* statistics=nullptr;UpdateChainEntry* entries[4]{};
    ApplicationLoop clock{};ScreenshotState screenshot{};
    u32 screenshot_handle=0xffffffff,graphics_state=255,loading_ids[3]{},timing_counters[2]{};
    double timing_samples[4]{},frame_duration=0,clock_origin=0;
    double presentation_origin=0;u32 presentation_frames=0;float presentation_fps=0;
    i32 reset_frames=0,loading_pause=0,disable_vsync=0,error=0,notice_code=-1,screenshot_error=0;
    bool stopped=false,writer_pending=false,initialized=false;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // A multiplayer logical frame remains journal-open through its authored
    // 60 Hz Draw.  The display-only high-refresh draw happens after this is
    // finalized and therefore cannot enter deterministic rollback state.
    Netplay::FrameDecision multiplayer_pending_decision{};
    u32 multiplayer_pending_frame=Netplay::INVALID_FRAME;
    u8 multiplayer_frame_skip=0;
    bool multiplayer_frame_open=false;
    bool multiplayer_waiting=false;
    bool multiplayer_generation_pending=false;
    bool multiplayer_replay_scope=false,multiplayer_replay_escape=false;
    u32 multiplayer_replay_seek_target=0,multiplayer_replay_seek_stage=0;
    ApplicationConfig multiplayer_replay_saved_config{};
    u32 multiplayer_rollbacks=0,multiplayer_resimulated_frames=0;
    char multiplayer_failure_detail[256]{};
#endif
    AppScreens screens;AppFrames frames;AppLoop loop;AppStatistics rates;AppPresentation presentation;AppScreenshot screenshots;AppConfiguration config;
    Application(FileSystem&,Input&,GameState&,AnimationEngine&,Fonts&,Audio&,ScreenEffects&);
    ~Application();bool initialize();i32 step(bool scheduled_tick=false);bool presentation_draw(float alpha,bool interpolate,bool world_interpolate=true);void presentation_frame();void save();void shutdown();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    i32 multiplayer_update();
    void multiplayer_fail(const char* reason);
    bool multiplayer_active()const;
    bool multiplayer_pump_network();
    u32 multiplayer_spectator_catchup_budget()const;
    bool multiplayer_resimulate_draw();
    bool multiplayer_finalize_frame();
    bool multiplayer_begin_replay(const char* filename,u32 selected_stage);
    bool multiplayer_replay_seeking()const;
    void multiplayer_finish_replay();
#endif
    void advance_loading();void sync_views();bool ensure_world();void configure_camera(Camera&,bool);
    Extended time();void bind_callbacks(Callbacks&) override;i32 draw_statistics();
#ifndef TH_NATIVE_PLATFORM
    bool invoke(CallbackToken,void*,i32&) override;
#endif
};
}
