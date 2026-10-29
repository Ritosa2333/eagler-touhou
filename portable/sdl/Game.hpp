// The host around the TH12 core: start-up, resource loading, the original
// title menus, the stage flow, replays, endings and the session's
// input/sound/music/save plumbing - what upstream's site/game.mjs and
// src/stage-loader.mjs did in the browser.
#pragma once
#include "Archive.hpp"
#include "Audio.hpp"
#include "Ending.hpp"
#include "ExeTables.hpp"
#include "Profile.hpp"
#include "Renderer.hpp"
#include "Replay.hpp"
#include "Scene.hpp"
#include "TextRaster.hpp"
#include "TitlePresentation.hpp"
#include "TitleScenes.hpp"
#include "PracticeOverlay.hpp"
#include "FrameClock.hpp"
#include <atomic>
#include <deque>
#include <functional>
#include <optional>
#include <thread>

namespace th12host {
// Core input bits (native held word); menus also read Enter and Retry.
enum Input:u32 {InShot=1,InBomb=2,InFocus=8,InUp=16,InDown=32,InLeft=64,InRight=128,InPause=256,InSkip=512,InEnter=0x80000,InRetry=0x200000};
// One controller sample: Switch buttons by pad-button number (the original's
// joystick buttons, which Key Config assigns) plus directions.
//   0 B  1 A  2 L/ZL  3 R/ZR  4 +  5 X  6 Y
struct PadSample {u32 buttons=0,directions=0,keyboard=0,pressed=0;};

class Game {
public:
    Game();~Game();
    std::string error;
    bool boot(); // files, devices, start-up screen
    // One display frame: up to `ticks` logical ticks with this input, then
    // draw. Returns false to quit.
    bool frame(const PadSample& pad,u32 ticks,double now_ms,float alpha=1);
    double logical_interval()const;
    void present(i32 width,i32 height){gpu.present(width,height);}
    void presented(double ms); // present + swap time of the frame just shown (the log)
    void focus_lost();
    bool snapshot(); // '−': BMP into snapshot/
    void flush_profile(){if(menus)save_profile();}
    void suspend_audio(bool paused){audio.suspend(paused);}
    void refresh_music(){audio.retry_music();}
    void set_motion(int mode,int x,int y){motion_mode=mode;motion_x=x;motion_y=y;}
    const std::string& host_state();
    bool playback(int action,const char* path,int value);
    void practice_key(int code,bool down);
    void clear_inputs(bool pointers){practice_ui.clear(pointers);}
    void mobile_panels(bool enabled,bool visible){practice_ui.mobile(enabled,visible);}
    int practice_pointer(int type,int id,float x,float y,bool desktop=true);
    void practice_wheel(float x,float y,float horizontal,float vertical);
    void shutdown();
#ifdef TH12_MULTIPLAYER
    bool multiplayer_stage(int stage,int character,int shot,int difficulty,int story_character,int story_shot);
    bool multiplayer_ready()const{return menus&&phase==Phase::Menu&&jobs.empty();}
    void multiplayer_start(bool playback,int local_seat,bool visible);
    void multiplayer_finish(bool save_only=false);
    void multiplayer_room();
    void multiplayer_sound(int id,int pan,bool stop){audio.effect(id,pan,stop);}
    void multiplayer_clock_reset(){captured=false;}
#endif
    const char* phase_name()const; // host tests and the log
    Renderer gpu;
    double frame_timing[3]{}; // the last frame's CPU ms: ticks, capture, draw (tools)
    i32 fixed_seed=-1;        // tools: the RNG seed of new games instead of the clock
private:
    enum class Phase {Startup,Menu,Loading};
    struct Session {
        MenuRun run;i32 stage=1,hud_age=0;u32 previous=0,music_serial=0;
        std::shared_ptr<const Replay> replay;u32 cursor=0;         // playback
        std::unique_ptr<ReplayRecorder> recorder;i32 spell_count=0; // recording
        std::optional<std::array<i32,20>> practice_record;
        std::string result_key,card_title;bool clear_recorded=false,result_recorded=false;
        double high_score=0; // the HUD's HI SCORE before this run beats it
    };
    Archive archive;ExeTables exe;Banks banks;TextRaster text;Audio audio;Profile profile;std::unique_ptr<Scene> scene;
    std::unique_ptr<TitleScenes> menus;std::unique_ptr<TitlePresentation> title;
    PracticeOverlay practice_ui;
    std::array<int32_t,20> practice_params{1,1,0,0,0,0,0,0,8,0,8,0,400,10000,0,0,0,0,0,0};
    bool custom_practice=false;
    Phase phase=Phase::Startup;u32 previous_ui=0,previous_buttons=0;bool capture_release=false;
    std::optional<Session> session;std::optional<MenuContext> pending_result;
    std::deque<std::function<bool()>> jobs;std::function<void()> after_jobs;
    std::unordered_map<std::string,std::vector<u8>> files; // bytes handed to the core on every stage load
    std::vector<MusicTrackInfo> music_tracks;
    i32 startup_frames=0;bool startup_ready=false;
    AnimationSampler loading_sampler; // LoadingPresentation's own sampler and display RNG
    std::unique_ptr<MenuAnimation> signature,indicator;std::vector<std::unique_ptr<MenuAnimation>> retiring;u32 loading_seed=0x1234;
    bool held_frame=false; // stage loads draw the last game frame behind the indicator
#ifdef TH12_MULTIPLAYER
    bool mp_playing=false,mp_result=false,mp_replay=false,mp_visible=false;
    int mp_local=0,mp_story_character=0,mp_story_shot=0;
    unsigned mp_restores=0;
    void multiplayer_frame(const PadSample&,u32,double);
#endif
    bool quit=false;       // title Quit
    bool captured=false;   // the scene holds the current game frame
    float presentation_alpha=1;
    u32 slow_frames=0;     // replay slow motion: one tick every fourth frame
    int motion_mode=0,motion_x=0,motion_y=0;
    std::string host_snapshot,pending_replay;int pending_replay_stage=0;
    bool external_paused=false;double external_rate=1,rate_debt=0;
    int seek_target=-1;bool seek_resume=false;std::string replay_file;
    // The ending and staff roll.
    std::unique_ptr<EndingTimeline> ending;AnimationSampler ending_sampler;i32 ending_id=-1;std::vector<std::string> ending_files;
    // The next stage, read and parsed on a loader thread while the stage-clear
    // sequence plays (the original builds the incoming stage as its 30-tick
    // field fade ends, with no loading screen); textures are uploaded one per
    // frame, and next_stage() switches over within the frame.
    struct StageFiles {
        struct Bank {i32 slot=0;bool present=true,uploaded=false;std::shared_ptr<AnmFile> anm;std::vector<u8> bytes;};
        i32 stage=0;std::unordered_map<std::string,std::vector<u8>> files;StdGeometry geometry;std::vector<Bank> banks;std::string error;
    };
    struct Prefetch {StageFiles next;std::thread loader;std::atomic<bool> done{false};~Prefetch();};
    std::unique_ptr<Prefetch> prefetch;
    PresentationCounter picture_counter;float game_fps=0;
    double fps_since=-1;float fps_value=0;u32 fps_frames=0;double last_frame=-1;
    double timing[3]{};u32 timed_frames=0; // CPU ms per frame: ticks, capture, draw
    // The log's frame pacing: present + swap, frames shown late (a missed
    // vsync) and how many followed a frame of over 14 ms of CPU work, catch-up
    // ticks, and the slowest frame's CPU time.
    double present_ms=0,worst_present=0,worst[4]{},last_cpu=0;u32 late_frames=0,late_after_work=0,catch_up=0;bool timed_frame=false;
    bool read(const std::string& name,std::vector<u8>& out);
    bool load_bank(i32 bank,const std::string& name,bool optional=false);
    void queue_stage(i32 stage);
    bool load_core_stage(i32 stage);
    void begin(MenuRun run,std::shared_ptr<const Replay> replay=nullptr);
    // flow: story/Extra replays (not practice ones or the demo) run the core's
    // stage flow, so a clear plays out as in the original; fade_in after a transition.
    bool start_replay_stage(const Replay& replay,const ReplayStage& stage,bool flow,bool fade_in);
    void record_stage();
    void play_replay(const std::string& file,i32 stage,bool demo);
    void next_stage();
    void enter_stage(i32 stage); // the loaded stage takes over (end of next_stage)
    i32 stage_after()const;      // the stage a clear leads to (0: none)
    void watch_prefetch();
    static bool read_stage(Archive& archive,i32 stage,StageFiles& out);
    void install_stage(StageFiles& next);
    void game_tick(u32 held,double now,u32 pulses=0);
    void handle_flow();
    MenuContext result_data(bool clear);
    void record_clear(const MenuContext& result);
    void complete_run(bool clear,bool save_only=false);
    void begin_ending();
    void record_run(const MenuContext& result,const std::string& name);
    void save_replay(i32 slot,const std::string& name,const MenuContext& result);
    void return_title();
    void save_profile();
    void clear_keys();
    void play_music(const std::string& name);
    void actions();
    u32 map_buttons(const PadSample& pad)const;
    void show_indicator();void retire_indicator();
    void tick_loading();
    void draw();
    void draw_loading();
    u32 next_loading_random();
};
}
